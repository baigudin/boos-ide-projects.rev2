/** 
 * Analog-to-Digital Converter factory.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Adc.hpp"
//#include "driver.AdcControllerSequential.hpp"
#include "driver.AdcControllerCascaded.hpp"
//#include "driver.AdcControllerDual.hpp"

/**
 * Returns the driver resource interface.
 *
 * @param clock the desiring ADC clock frequency in Hz.
 * @param mode  the ADC sampling mode.
 * @return the ADC module driver interface, or null if error has been occurred.
 */
::Adc* Adc::create(int32 clock, Mode mode)
{
  AdcController* res;
  switch(mode)
  {
  //case Adc::SEQUENTIAL            : res = new AdcControllerSequential(clock); break;
    case Adc::SIMULTANEOUS_CASCADED : res = new AdcControllerCascaded(clock);   break;
  //case Adc::SIMULTANEOUS_DUAL     : res = new AdcControllerDual(clock);       break;
    default: res = NULL; break;
  }
  if(res == NULL) return NULL;
  if(res->isConstructed()) return res;
  delete res;
  return NULL;    
}

/**
 * Initializes the driver.
 *
 * @param sourceClock the CPU oscillator source clock in Hz.
 * @return true if no errors.
 */
bool Adc::init(int32 sourceClock)  
{
  return AdcController::init(sourceClock);
}

/**
 * Deinitializes the driver.
 */
void Adc::deinit()
{
  AdcController::deinit();
}

/**
 * Locked PWM flags (no boot).
 */
bool AdcController::lock_[AdcController::RESOURCES_NUMBER];

/**
 * CPU clock in Hz (no boot).
 */
int32 AdcController::sysclk_;

/**
 * System Control Registers (no boot).
 */  
SystemRegister* AdcController::regSys_;

/**
 * Mutex of this driver (no boot).
 */  
Mutex* AdcController::drvMutex_;
  
/**
 * Driver has been initialized successfully (no boot).
 */
int32 AdcController::isInitialized_;


