/** 
 * Root class of all driver classes.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_OBJECT_HPP_
#define DRIVER_OBJECT_HPP_

class Object
{
  
public:

  /** 
   * Constructor.
   */  
  Object() : 
    isConstructed_ (true){
  }
  
  /** 
   * Destructor.
   */    
  virtual ~Object()
  {
    isConstructed_ = false;
  }  
  
  /**
   * Tests if this object has been constructed.
   *
   * @return true if object has been constructed successfully.
   */    
  virtual bool isConstructed() const
  {
    return isConstructed_;
  }

protected:

  /**
   * Sets the object constructed flag.
   *
   * @param flag constructed flag.
   */      
  virtual void setConstruct(bool flag)
  {
    if(isConstructed_ == true) isConstructed_ = flag;
  }

private:
  
  /** 
   * Object constructed flag.
   */  
  bool isConstructed_;

};
#endif // DRIVER_OBJECT_HPP_
