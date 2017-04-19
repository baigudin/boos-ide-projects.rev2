/** 
 * TI TMS320F2833x DSP interrupt controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_INTERRUPT_CONTROLLER_HPP_
#define DRIVER_INTERRUPT_CONTROLLER_HPP_

#include "boos.driver.Interrupt.hpp"
#include "driver.Object.hpp"

class InterruptController : public ::Object, public ::Interrupt
{
  typedef ::Object Parent;
  
public:

  /**
   * Constructor of the driver resource.
   *
   * @param handler pointer to user class which implements an interrupt handler interface.
   * @param source  available interrupt source.     
   */   
  InterruptController(::InterruptTask& handler, int32 source) : Parent(),
    handler_ (handler),
    source_  (source),
    driver_  (NULL){
    setConstruct( construct() );
  }
  
  /** 
   * Destructor.
   */                               
  virtual ~InterruptController()
  {
    if( not isConstructed() ) return;
  }
  
  /** 
   * Disables a source interrupt.
   *
   * @return an enable source bit value of a controller before method was called.
   */ 
  virtual bool disable()
  {
    if( not isConstructed() ) return false;    
    return driver_->disable();
  }
  
  /** 
   * Enables a source interrupt.
   *
   * @param status returned status by disable method.
   */    
  virtual void enable(bool status)
  {
    if( not isConstructed() ) return;    
    return driver_->enable(status);
  }
  
  /**
   * Disables all maskable interrupts.
   *
   * @return global interrupts enable bit value before method was called.
   */
  static bool globalDisable()
  {
    return ::driver::Interrupt::globalDisable();
  }
  
  /**
   * Enables all maskable interrupts.
   *
   * @param status the returned status by disable method.
   */
  static void globalEnable(bool status)
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
  static bool init(int32 sourceClock, int32 cpuClock)
  {
    // The driver has been already initialized by BOOS Startup.
    return true;
  }
  
  /**
   * Deinitialization.
   */
  static void deinit()
  {
  }

private:  
  
  /** 
   * Constructor.
   *
   * @return boolean result.
   */  
  bool construct()
  {
    if( not isConstructed() ) return false;  
    ::driver::Interrupt::Resource res;
    res.handler = &handler_;
    res.source = source_;
    driver_ = ::driver::Interrupt::create(res);
    return driver_ == NULL ? false : true;
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  InterruptController(const InterruptController& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  InterruptController& operator =(const InterruptController& obj);    
  
  /**
   * PWM driver initialized falg value.
   */
  static const int32 IS_INITIALIZED = 0x12784565;
  
  
  /**
   * Driver has been initialized successfully (no boot).
   */
  static int32 isInitialized_;    
  
  
  /**
   * A user class which implements an interrupt handler interface.
   */   
  ::InterruptTask& handler_;
  
  /**
   * Available interrupt source.     
   */   
  int32 source_;

  /**
   * HW interrupt driver.     
   */   
  ::api::Interrupt* driver_;

};
  
/**
 * Driver has been initialized successfully (no boot).
 */
int32 InterruptController::isInitialized_;

#endif // DRIVER_INTERRUPT_CONTROLLER_HPP_

