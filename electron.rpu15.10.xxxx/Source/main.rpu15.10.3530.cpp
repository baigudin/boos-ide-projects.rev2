/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pll.hpp" 
#include "driver.Pwm.hpp"
#include "driver.Adc.hpp"
#include "driver.FullBridge.hpp"

/**
 * The channels number of each PWM.
 */
const int32 PWM_CHANNELS = 2;

/**
 * The channels number of ADC.
 */
const int32 ADC_CHANNELS = 1;

/**
 * Starts new PWM task.
 *
 * @param pwm       a PWM module. 
 * @param frequency task frequency in Hz.
 * @param duty0     first channel duty in percentages.
 * @param duty1     second channel duty in percentages.
 * @return true if the task has been started successfully.
 */   
static bool startPwmTask(Pwm& pwm, int32 frequency, float32 duty0, float32 duty1)
{
  Pwm::Task<PWM_CHANNELS> task = {frequency, duty0, duty1};
  Pwm::TaskNode<PWM_CHANNELS> taskNode = task;
  return pwm.start(taskNode);  
}

/**
 * Executes ADC task.
 *
 * @param adc       a ADC module. 
 * @return true if the task has been started successfully.
 */   
static bool executeAdcTask(Adc& adc)
{
  int32 result;
  Adc::Task<ADC_CHANNELS> task = {3, {Adc::A0} };
  Adc::TaskNode<ADC_CHANNELS> taskNode = task;
  if( not adc.setTask(taskNode) ) return false;
  if( not adc.startTask() ) return false;  
  result = adc.resultTask(0, 0);
  result = adc.resultTask(1, 0);  
  result = adc.resultTask(2, 0);    
  return result != Adc::ERROR ? true : false;  
}


/**
 * User program entry.
 *
 * @return error code or zero.
 */   
int main()
{
  volatile bool exe = true;
  bool res = false;
  Pwm* pwm[2] = {NULL, NULL};
  Adc* adc = NULL;  
  // The CPU is being clocked by 30 MHz oscillator
  const int32 oscclk = 30000000;
  // The CPU requests having 150 MHz internal frequency
  const int32 sysclk = 150000000;  
  // Initialize the PLL driver
  if( not Pll::init(oscclk, sysclk) ) return -1;
  // Initialize the PWM driver
  if( not Pwm::init(oscclk) ) return -1;
  // Initialize the ADC driver
  if( not Adc::init(oscclk) ) return -1;  
  // Initialize the DRV8432 Dual Full-Bridge driver
  if( not FullBridge::init() ) return -1;
  // Create a PWM resources
  pwm[0] = Pwm::create(sysclk, 1, Pwm::UP);
  pwm[1] = Pwm::create(sysclk, 2, Pwm::UP);
  // Create ACD ADCINA0 channel resource and desire setting ADCCLK 25 MHz
  adc = Adc::create(25000000, Adc::DUAL);
  if(pwm[0] != NULL && pwm[1] != NULL && adc != NULL)
  {
    res = true;    
    // Enable phase synchronization for accessing the synchronize method
    pwm[0]->getPhaseKey().enable();
    pwm[1]->getPhaseKey().enable();      
    // The PWM synchronizes next PWM by prev input PWM impulse
    pwm[0]->setSynchronization(Pwm::INPUT);
    pwm[1]->setSynchronization(Pwm::DISABLE);      
    // Test all features of the PWMs
    res &= pwm[0]->isSynchronizing();
    res &= pwm[0]->isDeadBanded();    
    res &= pwm[1]->isDeadBanded();
    // Start new task for PWM 1 (A and B channels) at 42 KHz
    res &= startPwmTask(*pwm[0], 42000, 50.0f, 50.0f);    
    // Start new task for PWM 2 (C channel) at 200 KHz in HR mode
    pwm[1]->enableHighResolution();          
    res &= startPwmTask(*pwm[1], 200000, 50.0f, 0.0f);
    if(res)
    {
      Pwm::DeadBand* db[2];
      db[0] = &pwm[0]->getDeadBand();          
      db[1] = &pwm[1]->getDeadBand();                
      // Set Dead-Band feature of PWM 1
      db[0]->getInput(0).disable();
      db[0]->getPolarity(0).disable();                  
      db[0]->getOutput(0).enable();     
      db[0]->getInput(1).disable(); 
      db[0]->getPolarity(1).enable();      
      db[0]->getOutput(1).enable();      
      // Set Dead-Band feature of PWM 2
      db[1]->getInput(0).disable();
      db[1]->getPolarity(0).disable();        
      db[1]->getOutput(0).disable();   
      // Synchronize the PWM 1 with the PWM 2
      pwm[0]->synchronize();      
      // Waiting some complete actions
      while( exe )
      {
        executeAdcTask(*adc);
      }
    }
    // Stop generating the PWM wave if it is being generated
    pwm[0]->stop();
    pwm[1]->stop();    
  }
  // Delete the ADC resource
  delete adc;  
  // Delete the PWM resource
  delete pwm[1];  
  // Delete the PWM resource
  delete pwm[0];
  // Deinitializes the ADC driver
  Adc::deinit();  
  // Deinitializes the PWM driver
  Pwm::deinit();  
  // Deinitializes the PLL driver
  Pll::deinit();
  // Return something for no errors, no warnings
  return res ? 0 : 1;
}

