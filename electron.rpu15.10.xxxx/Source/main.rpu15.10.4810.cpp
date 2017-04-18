/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pll.hpp" 
#include "driver.Interrupt.hpp"
#include "driver.Pwm.hpp"
#include "driver.Adc.hpp"

/**
 * The channels number of each PWM.
 */
const int32 PWM_CHANNELS = 2;

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
  (void) startPwmTask;
  Pwm::TaskData<PWM_CHANNELS> data = {frequency, duty0, duty1};
  Pwm::Task<PWM_CHANNELS> task = data;
  return pwm.start(task);  
}

/**
 * Executes ADC task.
 *
 * @param adc       a ADC module. 
 * @return true if the task has been started successfully.
 */   
static void executeAdcTask(Adc& adc)
{
  int32 index;
  // Set Ia || Ua and Ic || Uc
  int32 channel[2] = {Adc::A3B3, Adc::A2B2};
  // Create 5 elements circle bufer for sampling
  // 2 simultaneous channels, which will be sampled 3 times
  Adc::Task<5,3,2,2> task(channel);
  // Test the number of ADC sequences
  if(adc.getSequencesNumber() != 1) return;
  // Get the first ADC sequencer
  Adc::Sequence& seq = adc.getSequence(0);
  if( not seq.setTask(task) ) return;
  // Create result buffer for 2 results of 2 channels
  int32 result[2][2];
  
  /**
   * The first way, but probably the fastest the ugliest, to get the result
   */
  // Software triggering ADC conversions for all 3 sequences
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  index = task.getFullIndex();
  if(index == -1) return;
  const int32 (&buf)[3][2][2] = task[index];
  for(int32 s=0; s<3; s++)
  {
    for(int32 c=0; c<2; c++)  
    {
      // Result A3 if 'c' equals 0
      // Result A2 if 'c' equals 1      
      result[c][0] = buf[s][c][0];
      // Result B3 if 'c' equals 0
      // Result B2 if 'c' equals 0      
      result[c][1] = buf[s][c][1];
      if(result[c][0] == -1 || result[c][1] == -1) return;            
    }
  }
  // Free processed task buffer
  task.setFullIsFree();
  
  /**
   * The second way to get the result
   */
  // Software triggering ADC conversions for all 3 sequences
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  index = task.getFullIndex();
  if(index == -1) return;
  for(int32 s=0; s<3; s++)
  {
    for(int32 c=0; c<2; c++)  
    {
      // Result A3 if 'c' equals 0
      // Result A2 if 'c' equals 1      
      result[c][0] = task[index][s][c][0];
      // Result B3 if 'c' equals 0
      // Result B2 if 'c' equals 0      
      result[c][1] = task[index][s][c][1]; 
      if(result[c][0] == -1 || result[c][1] == -1) return;           
    }
  }
  // Free processed task buffer
  task.setFullIsFree();
  
  /**
   * The third way to get the result
   */
  // Software triggering ADC conversions for all 3 sequences
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  if( not seq.trigger() ) return;      for(int32 i=0; i<0xfffff; i++);
  int32* addr = task.getFull();
  if(addr == NULL) return;  
  for(int32 s=0; s<3; s++)
  {
    for(int32 c=0; c<2; c++)  
    {
      // Result A3 if 'c' equals 0
      // Result A2 if 'c' equals 1      
      result[c][0] = *addr;
      addr++;
      // Result B3 if 'c' equals 0
      // Result B2 if 'c' equals 0      
      result[c][1] = *addr;      
      addr++;
      if(result[c][0] == -1 || result[c][1] == -1) return;
    }
  }
  // Free processed task buffer
  task.setFullIsFree();
}

/**
 * User program entry.
 *
 * @return error code or zero.
 */   
int mainBoard()
{
  volatile bool exe = true;
  bool res = false;
  Pwm* pwm = NULL;
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
  // Initialize the ADC driver
  if( not Adc::init(oscclk) ) return -1;  
  // Enable global interrupts
  Interrupt::globalEnable();
  // Create a PWM resources
  pwm = Pwm::create(sysclk, 1, Pwm::UP);
  // Create ACD ADCINA0 channel resource and desire setting ADCCLK 25 MHz
  adc = Adc::create(25000000, Adc::SIMULTANEOUS_CASCADED);
  if(pwm != NULL && adc != NULL)
  {
    res = true;    
    // Waiting some complete actions
    while( exe )
    {
      executeAdcTask(*adc);
    }
  }
  // Delete the ADC resource
  delete adc;  
  // Delete the PWM resource
  delete pwm;
  // Deinitializes the ADC driver
  Adc::deinit();  
  // Deinitializes the PWM driver
  Pwm::deinit();  
  // Deinitializes the PLL driver
  Pll::deinit();
  // Return something for no errors, no warnings
  return res ? 0 : 1;
}
