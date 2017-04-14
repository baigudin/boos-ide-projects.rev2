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
  
  /**
   * Starts the sampling task of the ADC module.
   *
   * @return true if the task has been started successfully.
   */
  virtual bool startTask()
  {
    if( not isConstructed() ) return false;
    if( not mutex_.res.lock() ) return false; 
    bool res = false;
    do{
      res = true;
    }while(false);
    return mutex_.res.unlock(res);        
  }
  
protected:

  /**
   * Registers a task.
   *
   * @param task a task.
   * @return true if the task has been registered.
   */
  virtual bool regTask(const TaskNodeInterface& task)
  {
    int32 chn[2];   
    int32 num = task.getChannelsNumber();
    for(int32 a=0, b=1, i=0; a<num; a+=2, b+=2, i++)
    {
      chn[0] = task.getChannel(a);
      chn[1] = task.getChannel(b);
      setChannelA(i, chn[0]);
      setChannelB(i, chn[1]);      
    }
    regAdc_->adcmaxconv.bit.maxConv1 = 0;
    regAdc_->adcmaxconv.bit.maxConv2 = 0;
    return true;
  }   
      
  /**
   * Tests task has correct values
   *
   * @param task a task.
   * @return true if the task is correct.
   */
  virtual bool isTask(const TaskNodeInterface& task) const
  {
    int32 chn[2];
    int32 num = task.getChannelsNumber();
    if(num & 0x1) return false;
    if(num < 0x1) return false;    
    if(num > 0x10) return false;
    for(int32 a=0, b=1; a<num; a+=2, b+=2)
    {
      chn[0] = task.getChannel(a);
      chn[1] = task.getChannel(b);
      if(0 <= chn[0] && chn[0] <= 7)
        if(8 <= chn[1] && chn[1] <= 15)
          continue;
      return false;
    }
    return true;    
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

