/**
 * Interrupt Task for interrupt service routine calling.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */ 
#ifndef DRIVER_INTERRUPT_TASK_HPP_
#define DRIVER_INTERRUPT_TASK_HPP_

#include "boos.api.Task.hpp"
#include "driver.Types.hpp"

class InterruptTask : public ::api::Task
{
  
public:

  /**
   * Constructor.
   */   
  InterruptTask(){}

  /** 
   * Destructor.
   */
  virtual ~InterruptTask(){}
  
  /**
   * The method with self context.
   */  
  virtual void handler() = 0;
  
  // The declaration below is only for interfacing this interface to the BOOS Interrupt Driver.
  // These might be deleted if the BOOS Core operating system is not used.
  
  /**
   * The method with self context.
   */  
  virtual void main()
  {
    handler();
  }
  
  /**
   * Tests if this object has been constructed.
   *
   * @return true if object has been constructed successfully.
   */    
  virtual bool isConstructed() const
  {
    return true;
  }   
  
  /**
   * Returns size of interrupt stack.
   *
   * The method returns size of interrupt stack in bytes which should be allocated for the task.
   *
   * @return stack size in bytes.
   */  
  virtual int32 stackSize() const
  {
    return 0x800;
  }
    
private:    

  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  InterruptTask(const InterruptTask& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.     
   */
  InterruptTask& operator =(const InterruptTask& obj);    

};
#endif // DRIVER_INTERRUPT_TASK_HPP_

