/** 
 * Dual Full-Bridge PWM Motor Driver factory. 
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.FullBridge.hpp"
#include "driver.System.hpp"
#include "driver.SystemRegister.hpp"
#include "driver.GpioRegister.hpp"

/**
 * Initializes the driver.
 *
 * @return true if no errors.
 */
bool FullBridge::init()
{
  GpioRegister* regGpio = new (GpioRegister::ADDRESS) GpioRegister();    
  System::eallow();
  // Set no reset for AB channel
  regGpio->gpapud.bit.gpio7 = 0;
  regGpio->gpamux1.bit.gpio7 = 0;
	regGpio->gpadir.bit.gpio7 = 1;
  regGpio->gpaset.bit.gpio7 = 1;  
  // Set no reset for CD channel
  regGpio->gpapud.bit.gpio9 = 0;
  regGpio->gpamux1.bit.gpio9 = 0;
	regGpio->gpadir.bit.gpio9 = 1;
  regGpio->gpaset.bit.gpio9 = 1;
  System::dallow();
  return true;
}

/**
 * Deinitializes the driver.
 */
void FullBridge::deinit()
{
  GpioRegister* regGpio = new (GpioRegister::ADDRESS) GpioRegister();    
  System::eallow();
  // Set reset for AB channel
  regGpio->gpaclear.bit.gpio7 = 1;
  // Set reset for CD channel
  regGpio->gpaclear.bit.gpio9 = 1;
  System::dallow();
}
