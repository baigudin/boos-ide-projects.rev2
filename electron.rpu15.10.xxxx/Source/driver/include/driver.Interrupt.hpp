/**
 * Interrupt controller driver factory. 
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */  
#ifndef DRIVER_INTERRUPT_HPP_
#define DRIVER_INTERRUPT_HPP_

#include "driver.InterruptTask.hpp"

class Interrupt
{

public:

  /** 
   * Destructor.
   */                               
  virtual ~Interrupt(){}
  
  /** 
   * Disables a source interrupt.
   *
   * @return an enable source bit value of a controller before method was called.
   */ 
  virtual bool disable() = 0;
  
  /** 
   * Enables a source interrupt.
   *
   * @param status returned status by disable method.
   */    
  virtual void enable(bool status=true) = 0;
  
  /** 
   * Enables a controller.
   *
   * @param status returned status by disable method.
   * @param ret    value which will be returned.
   * @return given value.
   */
  template<typename Type>
  Type enable(bool status, Type ret)
  {
    enable(status);
    return ret;
  }
 
  /**
   * Returns the interrupt interface of a target processor.
   *
   * @param handler pointer to user class which implements an interrupt handler interface.
   * @param source  available interrupt source.     
   * @return target processor interrupt interface.
   */
  static ::Interrupt* create(::InterruptTask& handler, int32 source);
  
  /**
   * Disables all maskable interrupts.
   *
   * @return global interrupts enable bit value before method was called.
   */
  static bool globalDisable();
  
  /**
   * Enables all maskable interrupts.
   *
   * @param status the returned status by disable method.
   */
  static void globalEnable(bool status=true);
  
  /**
   * Enables all maskable interrupts.
   *
   * @param status the returned status by disable method.
   * @param ret    value which will be returned.     
   * @return given value.     
   */
  template<class Type>
  static inline bool globalEnable(bool status, Type ret)
  {
    globalEnable(status);
    return ret;   
  }
  
  /**
   * Initializes the driver.
   *
   * @param sourceClock source clock in Hz.    
   * @param cpuClock    requesting CPU clock in Hz.
   * @return true if no errors are occurred.
   */
  static bool init(int32 sourceClock, int32 cpuClock);
  
  /**
   * Deinitializes the driver.
   */
  static void deinit();
  
};
#endif // BOOS_TARGET_INTERRUPT_HPP_
