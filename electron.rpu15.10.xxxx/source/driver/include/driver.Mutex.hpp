/**
 * Mutex class.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_MUTEX_HPP_
#define DRIVER_MUTEX_HPP_

#include "driver.Types.hpp"

class Mutex
{

public:

  /** 
   * Constructor.
   */    
  Mutex()
  {
  }
  
  /** 
   * Destructor.
   */      
  virtual ~Mutex()
  {
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
   * Locks the mutex.
   *
   * @return true if the semaphore is lock successfully.
   */      
  virtual bool lock()
  {
    return true;
  }
  
  /**
   * Unlocks the mutex.
   */      
  virtual void unlock()
  {
  }
  
  /** 
   * Unlocks the mutex and returns passed parameter.
   *
   * @param ret a value which will be returned.
   * @return passed value.
   */
  template<typename Type>
  Type unlock(Type ret)
  {
    unlock();
    return ret;
  }  

private:
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  Mutex(const Mutex& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  Mutex& operator =(const Mutex& obj);      

};
#endif // DRIVER_MUTEX_HPP_
