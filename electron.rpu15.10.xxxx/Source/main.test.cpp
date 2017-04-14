/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pll.hpp" 
#include "driver.Pwm.hpp"

/**
 * The channels number of each PWM.
 */
const int32 CHANNELS = 2;

/**
 * The PWM task table.
 */
static const Pwm::Task<CHANNELS> table[] = {
  {  200000, 75.6f, 25.1f },
  { 1000000, 10.0f, 20.0f } 
};

/**
 * The number of task table rows.
 */
const int32 TASK_NUMBER = sizeof(table) / sizeof(Pwm::Task<CHANNELS>);

/**
 * Starts new PWM task.
 *
 * @param pwm       a PWM module. 
 * @param frequency task frequency in Hz.
 * @param duty0     first channel duty in percentages.
 * @param duty1     second channel duty in percentages.
 * @return true if the task has been started successfully.
 */   
static bool startTask(Pwm& pwm, int32 frequency, float32 duty0, float32 duty1)
{
  Pwm::Task<CHANNELS> task = {frequency, duty0, duty1};
  Pwm::TaskNode<CHANNELS> taskNode = task;
  return pwm.start(taskNode);  
}

/**
 * Delays the PWM first channel signal by using Dead-Band module.
 *
 * @param pwm   a PWM module. 
 * @param delay a PWM first channel signal delay time in ns.  
 * @return real delayed time in ns.
 */   
static float32 delayFirstChannel(Pwm& pwm, float32 delay)
{
  if(!pwm.isDeadBanded()) return false;
  Pwm::DeadBand& db = pwm.getDeadBand();
  // Turn S1 on
  db.getOutput(0).enable();
  // Turn S4 off
  db.getInput(0).disable();
  // Set 20 us delay of rising-edge
  db.setRisingDelay(delay);
  // Return delayed 
  return db.getRisingDelay();
}

/**
 * Inverts the PWM second channel signal by using Dead-Band module.
 *
 * @param pwm a PWM module. 
 * @return true if the signal has been inverted successfully.
 */   
static bool invertSecondChannel(Pwm& pwm)
{
  if(!pwm.isDeadBanded()) return false;
  Pwm::DeadBand& db = pwm.getDeadBand();
  // Turn S0 on
  db.getOutput(1).enable();
  // Turn S5 on
  db.getInput(1).enable();
  // Turn S3 on
  db.getPolarity(1).enable();
  return true;
}

/**
 * Normalizes the PWM second channel signal by using Dead-Band module.
 *
 * @param pwm a PWM module. 
 * @return true if the signal has been normalized successfully.
 */   
static bool normalSecondChannel(Pwm& pwm)
{
  if(!pwm.isDeadBanded()) return false;
  Pwm::DeadBand& db = pwm.getDeadBand();
  // Turn S0 off
  db.getOutput(1).disable();
  // Turn S5 off
  db.getInput(1).disable();
  // Turn S3 off
  db.getPolarity(1).disable();   
  return true;
}

/**
 * Chops the PWM channel signals by using Chopper module.
 *
 * @param pwm a PWM module. 
 * @return true if the signal has been chopped successfully.
 */   
static bool chopAllChannels(Pwm& pwm)
{
  int32 ires;
  float32 fres;
  if(!pwm.isChopped()) return false;
  Pwm::Chopper& chp = pwm.getChopper();
  chp.getChopping().enable();
  // Set min first pulse width  
  chp.setFrequency(2343750);
  ires = chp.getFrequency();
  // Set duty 
  chp.setDuty(50.0f);
  fres = chp.getDuty();
  // Set min first pulse width
  chp.setFirstPulse(53);  
  // Set max first pulse width
  chp.setFirstPulse(848);  
  ires = chp.getFirstPulse();
  return fres == Pwm::ERROR || ires == Pwm::ERROR ? false : true;
}

/**
 * Test the HRPWM channel
 *
 * @param pwm a PWM module. 
 */   
static void testHighResolution(Pwm& pwm)
{
  Pwm::DeadBand& db = pwm.getDeadBand();
  // Enable high resolution for the PWM channel.
  // Note: One PWM module has only one High Resolution PWM channel,
  //       which is channel one. So, we enable it.
  pwm.enableHighResolution();
  // Start the task executing
  startTask(pwm, 1250000, 40.5f, 10.0f);
  // Disable signal inversion of both Dead-Band module channels
  db.getPolarity(0).disable();  
  db.getPolarity(1).disable();
  // Switch the first PWM channel to the both output channels of Dead-Band module.
  // Therefore, Chopper module, which has been placed after Dead-Band module, will get
  // absolutely identical signal.
  db.getInput(0).disable();
  db.getInput(1).disable(); 
  db.getOutput(0).enable();   
  db.getOutput(1).enable();
  // In this point, using 500 MHz oscilloscope we can watch that the signal has different duty
  // This is HRPWM module work. The signal in the first channel is more wide than the signal
  // in the second channel.
  asm(" nop");
  // So, after this method will be executed, the signal will be real identical on oscilloscope screen.
  pwm.disableHighResolution();  
}

/**
 * User program entry.
 *
 * @return error code or zero.
 */   
int main()
{
  int32 ires = 0;
  float32 fres = 0.0f;
  Pwm* pwm[2];
  // The CPU is being clocked by 30 MHz oscillator
  const int32 oscclk = 30000000;
  // The CPU requests having 150 MHz internal frequency
  const int32 sysclk = 150000000;  
  // Initializes the PLL driver
  if( not Pll::init(oscclk, sysclk) ) return -1;
  // Initializes the PWM driver
  if( not Pwm::init(oscclk) ) return -1;
  // Create a PWM resources
  pwm[0] = Pwm::create(150000000, 1, Pwm::UP);
  pwm[1] = Pwm::create(150000000, 2, Pwm::UP);
  if(pwm[0] != NULL || pwm[1] != NULL)
  {
    // Test HRPWM
    testHighResolution(*pwm[0]);
    // Start new task
    startTask(*pwm[0], 10000, 75.1f, 10.5f);    
    // Start new task    
    startTask(*pwm[1], 10000, 75.1f, 10.5f);
    if(pwm[0]->isSynchronizing())
    {
      // Enable phase synchronization
      pwm[0]->getPhaseKey().enable();
      pwm[1]->getPhaseKey().enable();      
      // The PWM synchronizes next PWM by prev input PWM impulse
      pwm[0]->setSynchronization(Pwm::INPUT);
      pwm[1]->setSynchronization(Pwm::INPUT);      
      // Synchronize the PWM with next PWM
      pwm[0]->synchronize();
      // Set 5 us shift time for the PWM
      pwm[1]->setPhase(5000);
      // Get the PWM shift time
      ires = pwm[1]->getPhase();      
      // Synchronize the PWM with next PWM
      pwm[0]->synchronize();
    }
    // Get the Resolution of PWM in Hz
    fres = pwm[0]->getResolution(0);
    fres = pwm[0]->getResolution(1);
    // Get the clock frequency of PWM in Hz
    ires = pwm[0]->getClockFrequency();
    // Get the real signal frequency of PWM in Hz    
    ires = pwm[0]->getSignalFrequency();
    // Get the set signal frequency of PWM in Hz
    ires = pwm[0]->getSignalFrequency(true);    
    // Get the real duty of PWM channel 0
    fres = pwm[0]->getDuty(0);
    // Get the set duty of PWM channel 0
    fres = pwm[0]->getDuty(0, true);
    // Get the real duty of PWM channel 1    
    fres = pwm[0]->getDuty(1);            
    // Get the set duty of PWM channel 1
    fres = pwm[0]->getDuty(1, true);    
    // Test generations
    startTask(*pwm[0], 20000, 50.0f, 25.0f);    
    invertSecondChannel(*pwm[0]);
    normalSecondChannel(*pwm[0]);
    fres = delayFirstChannel(*pwm[0], 5000.0f);
    chopAllChannels(*pwm[0]);
    // Synchronize PWM with next PWM, which is PWM 2
    pwm[0]->synchronize();
    // Stop generating the PWM wave if it is being generated
    pwm[0]->stop();
  }
  // Delete the PWM resource
  delete pwm[0];
  // Delete the PWM resource
  delete pwm[1];
  // Deinitializes the PWM driver
  Pwm::deinit();  
  // Deinitializes the PLL driver
  Pll::deinit();
  // Return something for no errors, no warnings
  return ires || fres ? 0 : 1;
}
