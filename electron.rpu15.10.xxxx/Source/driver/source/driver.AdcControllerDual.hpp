/** 
 * TI TMS320F2833x DSP Simultaneous Dual Analog-to-Digital Converter controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_ADC_CONTROLLER_DUAL_HPP_
#define DRIVER_ADC_CONTROLLER_DUAL_HPP_

#include "driver.AdcController.hpp"

class AdcControllerDual : public ::AdcController
{
  typedef ::AdcController  Parent;
  
public:

  /**
   * Constructor of the driver resource.
   *
   * @param clock the desiring ADC clock frequency in Hz.   
   */   
  AdcControllerDual(int32 clock) : Parent(clock)
  {
    setConstruct( construct() );
  }

  /** 
   * Destructor.
   */                               
  virtual ~AdcControllerDual()
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
    return Adc::SIMULTANEOUS_DUAL;  
  }

private:  

  /** 
   * Set channel A.
   */  
  void setChannelA(int32 index, int32 channel)
  {
    int32 seq = (index >> 2) & 0x1;
    AdcRegister::Adcchselseq* reg = &regAdc_->adcchselseq[seq];
    switch(index & 0x3)
    {
      case  0: reg->bit.conv0 = channel & 0xf; break;
      case  1: reg->bit.conv1 = channel & 0xf; break;
      case  2: reg->bit.conv2 = channel & 0xf; break;
      case  3: reg->bit.conv3 = channel & 0xf; break;
      default: break;
    }
  }
  
  /** 
   * Set channel B.
   */  
  void setChannelB(int32 index, int32 channel)
  {
    int32 seq = ((index >> 2) & 0x1) + 2;
    AdcRegister::Adcchselseq* reg = &regAdc_->adcchselseq[seq];
    switch(index & 0x3)
    {
      case  0: reg->bit.conv0 = channel & 0xf; break;
      case  1: reg->bit.conv1 = channel & 0xf; break;
      case  2: reg->bit.conv2 = channel & 0xf; break;
      case  3: reg->bit.conv3 = channel & 0xf; break;
      default: break;
    }
  }

  /** 
   * Constructor.
   *
   * @return boolean result.
   */  
  bool construct()
  {
    if( not Parent::isConstructed() ) return false;
    if( not mutex_.drv.lock() ) return false;    
    bool res = false;
    do{
      // Set simultaneous sampling mode
      regAdc_->adctrl3.bit.smodeSel = 1;    
      // Dual-sequencer mode. SEQ1 and SEQ2 operate as two 8-state sequencers
      regAdc_->adctrl1.bit.seqCasc = 0;
      res = true;
    }while(false);
    return mutex_.drv.unlock(res);    
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  AdcControllerDual(const AdcControllerDual& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  AdcControllerDual& operator =(const AdcControllerDual& obj);    
  

};
#endif // DRIVER_ADC_CONTROLLER_DUAL_HPP_

