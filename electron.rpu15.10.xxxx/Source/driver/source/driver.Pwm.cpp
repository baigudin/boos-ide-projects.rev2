/** 
 * Pulse Width Modulator driver factory.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pwm.hpp"
#include "driver.PwmControllerUpDown.hpp"
#include "driver.PwmControllerUp.hpp"
#include "driver.PwmControllerDown.hpp"

/**
 * Returns the driver resource interface.
 *
 * @param clock  the PWM internal clock based on the CPU clock in Hz.    
 * @param number a number of a PWM module.
 * @param count  a counting policy of the PWM module behavior.   
 * @return the PWM module driver interface, or null if error has been occurred.
 */
::Pwm* Pwm::create(int32 clock, int32 number, Count count)
{
  PwmController* res;
  int32 index = number - 1;
  switch(count)
  {
    case Pwm::UPDOWN : res = new PwmControllerUpDown(clock, index); break;
    case Pwm::UP     : res = new PwmControllerUp(clock, index);     break;
    case Pwm::DOWN   : res = new PwmControllerDown(clock, index);   break;
    default          : res = NULL;                                  break;
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
bool Pwm::init(int32 sourceClock)
{
  return PwmController::init(sourceClock);
}

/**
 * Deinitializes the driver.
 */
void Pwm::deinit()
{
  PwmController::deinit();
}

/**
 * Locked PWM flags (no boot).
 */
bool PwmController::lock_[PwmController::RESOURCES_NUMBER];

/**
 * CPU clock in Hz (no boot).
 */
int32 PwmController::sysclk_;

/**
 * System Control Registers (no boot).
 */  
SystemRegister* PwmController::regSys_;

/**
 * GPIO Registers (no boot).
 */  
GpioRegister* PwmController::regGpio_;

/**
 * Mutex of this driver (no boot).
 */  
Mutex* PwmController::drvMutex_;
  
/**
 * Driver has been initialized successfully (no boot).
 */
int32 PwmController::isInitialized_;

