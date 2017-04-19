/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pll.hpp" 
#include "driver.Pwm.hpp"
#include "driver.Adc.hpp"
#include "driver.Interrupt.hpp"
#include "driver.FullBridge.hpp"

/**
 * The channels number of each PWM.
 */
const int32 PWM_CHANNELS = 2;

/**
 * The number of blocks of internal circle buffer.
 */
const int32 ADC_BLOCKS = 2;

/**
 * The number of sequences of sampling channels.
 */
const int32 ADC_SEQUENCES = 4;

/**
 * The number of sampling channels.
 */
const int32 ADC_CHANNELS = 1;

/**
 * The number of results in a channel.
 */
const int32 ADC_RESULTS = 2;

/**
 * The ADC task class.
 */
typedef Adc::Task<ADC_BLOCKS, ADC_SEQUENCES, ADC_CHANNELS, ADC_RESULTS> AdcTask;

/**
 * Starts new PWM task.
 *
 * @param pwm       a PWM module. 
 * @param frequency task frequency in Hz.
 * @param duty0     first channel duty in percentages.
 * @param duty1     second channel duty in percentages.
 * @return true if the task has been started successfully.
 */   
static bool setPwmTask(Pwm& pwm, int32 frequency, float32 duty0, float32 duty1)
{
  Pwm::TaskData<PWM_CHANNELS> data = {frequency, duty0, duty1};
  Pwm::Task<PWM_CHANNELS> task = data;
  return pwm.setTask(task);  
}

/**
 * Samples the singnal generating by PWM.
 *
 * @param adc a sampling ADC module.
 * @param pwm a generating PWM module.
 * @return true if the task has been started successfully.
 */   
static void sample(Adc& adc, Pwm& pwm)
{
  int32 index, result, current, voltage;
  // Set ADCA0 channel sampling
  int32 channel[1] = {Adc::A0B0};
  // Set PWM Event Trigger settings
  if( not pwm.isTriggered() ) return;
  ::Pwm::EventTrigger& et = pwm.getTrigger();
  // Send SOCA to ADC to take current (I) 
  if( not et.setEvent(Pwm::ADC_SOCA, Pwm::CTR_ZERO) ) return;
  // Send SOCB to ADC to take voltage (V)   
  if( not et.setEvent(Pwm::ADC_SOCB, Pwm::CTR_PRD) ) return;
  // Create 2 elements circle bufer for sampling
  // 1 simultaneous channels, which will be sampled 3 times
  AdcTask task(channel);
  // Test the number of ADC sequences
  if(adc.getSequencesNumber() != 1) return;
  // Get the first ADC sequencer
  Adc::Sequence& seq = adc.getSequence(0);
  if( not seq.setTask(task) ) return;
  // PWM triggering ADC conversions for all 3 sequences
  if( not seq.setTrigger(Adc::PWM_SOCA) ) return;
  if( not seq.setTrigger(Adc::PWM_SOCB) ) return;
  volatile bool exec = true;
  while(exec)
  {
    index = seq.wait();
    if(index == -1) break;
    for(int32 s=0; s<ADC_SEQUENCES; s++)
    {
      // Read the resual of corresponding sequence
      //
      // NOTE: The PWM has been started and SOCA and SOCB have been triggering ADC.  
      // SOCA is the current trigger, SOCB is the voltage trigger. 
      // Which trigger at the first pass triggers ADC is not obvious. 
      // If the first trigger had been SOCA, the zero case would be the current result; 
      // otherwise it would be the voltage result.
      switch (s & 0x1)
      {
        case 0:
        {
          current = result = task[index][s][0][0];
          if(current == -1) break;
          // Do somethings with the current result
          asm(" nop");          
        }
        break;
        case 1:
        {
          voltage = result = task[index][s][0][0];        
          if(voltage == -1) break;
          // Do somethings with the voltage result
          asm(" nop");          
        }
        break;
      }
      if(result != -1) continue;
      exec = false;
      break;
    }
    // Below this is ONLY A DEBUG HACK
    int32* addr = const_cast<int32*>(task.getFull());
    for(int32 s=0; s<ADC_SEQUENCES; s++)
    {
      for(int32 c=0; c<ADC_CHANNELS; c++)  
      {
        for(int32 r=0; r<ADC_RESULTS; r++)      
        {
          *addr = 0;
          addr++;
        }
      }
    }
    // Below this is NOT the debug hack
    // Free processed task buffer
    task.setFullIsFree(); 
  }
  // Stop PWM triggering ADC conversions for all 3 sequences  
  seq.resetTrigger(Adc::PWM_SOCA);
  seq.resetTrigger(Adc::PWM_SOCB);  
}

/**
 * User program entry.
 *
 * @return error code or zero.
 */   
int _main()
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
  // Initialize the Interrupt driver
  if( not Interrupt::init(oscclk, sysclk) ) return -1;  
  // Initialize the PWM driver
  if( not Pwm::init(oscclk) ) return -1;
  // Initialize the DRV8432 Dual Full-Bridge driver
  if( not FullBridge::init() ) return -1;  
  // Initialize the ADC driver
  if( not Adc::init(oscclk) ) return -1;  
  // Enable global interrupts
  Interrupt::globalEnable();
  // Create a PWM resources
  pwm[0] = Pwm::create(sysclk, 1, Pwm::UPDOWN);
  pwm[1] = Pwm::create(sysclk, 2, Pwm::UP);
  // Create ACD ADCINA0 channel resource and desire setting ADCCLK 25 MHz
  adc = Adc::create(25000000, Adc::SIMULTANEOUS_CASCADED);
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
    // Set new task for PWM 1 (A and B channels) at 42 KHz
    res &= setPwmTask(*pwm[0], 42000, 50.0f, 50.0f);    
    // Set new task for PWM 2 (C channel) at 200 KHz in HR mode
    pwm[1]->enableHighResolution();          
    res &= setPwmTask(*pwm[1], 200000, 50.0f, 0.0f);
    res &= pwm[0]->start();
    res &= pwm[1]->start();
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
      // Execute ADC sampling
      sample(*adc, *pwm[0]);
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

