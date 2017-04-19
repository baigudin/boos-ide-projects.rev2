/** 
 * TI TMS320F2833x DSP Sequential Analog-to-Digital Converter controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_ADC_CONTROLLER_SEQUENTIAL_HPP_
#define DRIVER_ADC_CONTROLLER_SEQUENTIAL_HPP_

#include "driver.AdcController.hpp"

class AdcControllerSequential : public ::AdcController
{
  typedef ::AdcController  Parent;
  
public:

  /**
   * Constructor of the driver resource.
   *
   * @param clock the desiring ADC clock frequency in Hz.   
   */   
  AdcControllerSequential(int32 clock) : Parent(clock)
  {
    setConstruct( construct() );
  }

  /** 
   * Destructor.
   */                               
  virtual ~AdcControllerSequential()
  {
    if( not isConstructed() ) return;
  }
  
  /**
   * Returns the ADC module mode.
   *
   * @return the argument mode passed to create method.
   */        
  virtual Mode getMode() const
  {
    return Adc::SEQUENTIAL;  
  }

private:  

  /** 
   * Constructor.
   *
   * @return boolean result.
   */  
  bool construct()
  {
    if( not Parent::isConstructed() ) return false;  
    bool res = false;
    do{
      // Set sequential sampling mode
      regAdc_->adctrl3.bit.smodeSel = 0;    
      // To uncomment when the mode has been written
      // res = true;
    }while(false);
    return mutex_.drv.unlock(res);    
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  AdcControllerSequential(const AdcControllerSequential& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  AdcControllerSequential& operator =(const AdcControllerSequential& obj);    
  

};
#endif // DRIVER_ADC_CONTROLLER_SEQUENTIAL_HPP_

