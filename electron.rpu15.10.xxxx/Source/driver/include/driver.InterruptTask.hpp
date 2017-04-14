/**
 * Interrupt Task for interrupt service routine calling.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */ 
#ifndef DRIVER_INTERRUPT_TASK_HPP_
#define DRIVER_INTERRUPT_TASK_HPP_

#include "driver.Types.hpp"

class InterruptTask
{
  
public:

  /** 
   * Destructor.
   */
  virtual ~InterruptTask(){}
  
  /**
   * The method with self context.
   */  
  virtual void handler() = 0;

};
#endif // DRIVER_INTERRUPT_TASK_HPP_
