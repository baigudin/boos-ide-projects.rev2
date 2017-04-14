/** 
 * Interrupt controller driver factory. 
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "boos.driver.Interrupt.hpp"
#include "driver.Interrupt.hpp"

/**
 * Returns the interrupt interface of a target processor.
 *
 * @param handler pointer to user class which implements an interrupt handler interface.
 * @param source  available interrupt source.     
 * @return target processor interrupt interface.
 */
::Interrupt* Interrupt::create(::InterruptTask& handler, int32 source)
{
  //::driver::Interrupt::Resource res;
  //res.handler = &handler;
  //res.source = source;
  // TODO interrupt controller class
  //return ::driver::Interrupt::create(res);
  return NULL;
}

/**
 * Disables all maskable interrupts.
 *
 * @return global interrupts enable bit value before method was called.
 */
bool Interrupt::globalDisable()
{
  return ::driver::Interrupt::globalDisable();
}

/**
 * Enables all maskable interrupts.
 *
 * @param status the returned status by disable method.
 */
void Interrupt::globalEnable(bool status)
{
  ::driver::Interrupt::globalEnable(status);
}

/**
 * Initializes the driver.
 *
 * @param sourceClock source clock in Hz.    
 * @param cpuClock    requesting CPU clock in Hz.
 * @return true if no errors are occurred.
 */
bool Interrupt::init(int32 sourceClock, int32 cpuClock)
{
  return true;
}

/**
 * Deinitializes the driver.
 */
void Interrupt::deinit()
{
}

