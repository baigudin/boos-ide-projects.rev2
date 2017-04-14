/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pll.hpp" 
#include "driver.Pwm.hpp"
#include "driver.Adc.hpp"

/**
 * The channels number of each PWM.
 */
const int32 PWM_CHANNELS = 2;

/**
 * The channels number of ADC.
 */
const int32 ADC_CHANNELS = 2;

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
  // Set task for A_I || A_U and C_I || C_U
  Adc::TaskData<ADC_CHANNELS> data = {{Adc::A3B3, Adc::A2B2}};
  Adc::Task<ADC_CHANNELS> task(data);
  // Test the number of ADC sequences
  if(adc.getSequencesNumber() != 1) return;
  // Get the first ADC sequencer
  Adc::Sequence& seq = adc.getSequence(0);
  if( not seq.setTask(task) ) return;
  if( not seq.startTask() ) return;  
  if( not seq.waitResult() ) return;
  asm(" nop");
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
  Pwm* pwm = NULL;
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
