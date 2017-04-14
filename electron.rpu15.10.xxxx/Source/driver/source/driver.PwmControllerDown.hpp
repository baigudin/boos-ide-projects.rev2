/** 
 * TI TMS320F2833x DSP Enhanced Pulse Width Modulator down counting controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_PWM_CONTROLLER_DOWN_HPP_
#define DRIVER_PWM_CONTROLLER_DOWN_HPP_

#include "driver.PwmController.hpp"

class PwmControllerDown : public ::PwmController
{
  typedef ::PwmController             Parent;
  typedef ::PwmRegister::Tbctl::Val   Tbctl;
  typedef ::PwmRegister::Cmpctl::Val  Cmpctl;
  typedef ::PwmRegister::Aqctl::Val   Aqctl;
  typedef ::PwmRegister::Dbctl::Val   Dbctl;
  
  using   ::PwmController::getDuty;  

public:

  /**
   * Constructor of the driver resource.
   *
   * @param clock the PWM internal clock based on the CPU clock in Hz.   
   * @param index an index of a PWM module.
   */   
  PwmControllerDown(int32 clock, int32 index) : Parent(clock, index)
  {
    setConstruct( construct() );
  }

  /** 
   * Destructor.
   */                               
  virtual ~PwmControllerDown()
  {
  }
  
  /**
   * Returns the PWM signal frequency of currently assigned task.
   *
   * @param set flag allows getting the set or calculated value.
   * @return the signal frequency, or ERROR if error has been occurred.
   */    
  virtual int32 getSignalFrequency(bool set) const
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    int32 freq;
    if(set)
    {
      freq = task_.getFrequency();
    }
    else
    {
      int32 prd = regPwm_->tbprd.val;
      freq = tbclk_ / ( prd + 1 );
    }
    return mutex_.res.unlock(freq);
  }
  
  /**
   * Returns the PWM module channel duty of currently assigned task.
   *
   * @param channel available number of channel.
   * @param set     flag allows getting the set or calculated value.
   * @return the channel duty in percentages, or ERROR  if error has been occurred.
   */        
  virtual float32 getDuty(int32 channel, bool set) const  
  {
    if(!isConstructed() || !isChannel(channel)) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    float32 duty;
    if(set) 
    {
      duty = task_.getDuty(channel);
    }
    else
    {
      bool is = disable();
      float32 prd = static_cast<float32>(regPwm_->tbprd.val) + 1.0f;
      float32 cmp = static_cast<float32>(regPwm_->cmp[channel].val);
      enable(is);
      duty = 100.0f - ((prd - cmp) * 100.0f) / prd;
    }
    return mutex_.res.unlock(duty);
  }
  
  /**
   * Returns the PWM module counting policy.
   *
   * @return the argument count passed to create method.
   */        
  virtual Count getCountPolicy() const
  {
    return Pwm::DOWN;
  }
  
  #ifdef DRIVER_HRPWM  
  
  /**
   * Enables the PWM channels in high resolution mode.
   *
   * @return true if high resolution mode is set successfully.
   */        
  virtual bool enableHighResolution()
  {
    if(!isConstructed()) return false;
    // TI highly recommends that TBCLK=SYSCLKOUT
    if(tbclk_ != sysclk_) return false;
    // TI says that min SYSCLKOUT must be 60 MHz
    if(sysclk_ < 60000000) return false;
    if(!mutex_.res.lock()) return false;
    System::eallow();    
    // Set CMPAHR to controls the micro edge position
    regPwm_->hrcnfg.bit.ctlmode = 0x0;
    // Control rising edge position
    regPwm_->hrcnfg.bit.edgmode = 0x1;
    // Set CMPAHR is loaded when CTR equals to zero and match the CMPA selection
    regPwm_->hrcnfg.bit.hrload = 0x0;
    System::dallow();    
    isHiRes_ = true;
    return mutex_.res.unlock(true);
  }
  
  #endif // DRIVER_HRPWM     
  
protected:

  /**
   * Starts generating the wave of the PWM module.
   *
   * @return true if the PWM has been started successfully.
   */
  virtual bool startTask()
  {
    uint16 prd, cmp[CHANNELS_NUMBER];
    // Set timer period
    int32 pwmclk = task_.getFrequency();
    if(pwmclk == ERROR) return false;
    // Rounding up real count of timer-base clocks which wishes to be loaded
    uint32 count = tbclk_ / pwmclk + (tbclk_ % pwmclk != 0 ? 1 : 0);
    // Real count of timer-base clocks which will be loaded
    prd = (count - 1) & 0x0000ffff;
    for(int32 i=0; i<CHANNELS_NUMBER; i++)
    {
      float32 duty = task_.getDuty(i);
      if(duty == ERROR) duty = 0.0f;
      float32 value = static_cast<float32>(count) * duty / 100.0f;    
      cmp[i] = static_cast<uint32>(value) & 0x0000ffff;
    }
    // Write values to the registers in iterrupts disable mode
    // for possibility to call the method in an interrupt handler
    bool is = disable();
    regPwm_->tbprd.val = prd;
    for(int32 i=0; i<CHANNELS_NUMBER; i++) regPwm_->cmp[i].val = cmp[i];    
    enable(is);
    #ifdef DRIVER_HRPWM
    startTaskHR();
    #endif // DRIVER_HRPWM        
    return true;
  } 
  
  /**
   * Returns max available frequency of the PWM module.
   *
   * @return max frequency, or ERROR if error occurred.   
   */        
  virtual int32 maxFrequency() const
  {
    if(!isConstructed()) return ERROR;
    return tbclk_;
  }
  
  /**
   * Returns min available frequency of the PWM module.
   *
   * @return min frequency, or ERROR if error occurred.
   */
  virtual int32 minFrequency() const
  {
    if(!isConstructed()) return ERROR;        
    return tbclk_ / 0xffff + 1;
  }
  
private:

  /** 
   * Constructor.
   *
   * @return boolean result.
   */  
  bool construct()
  {
    if(!Parent::isConstructed()) return false;
    System::eallow();
    // Stop the time-base clock within any enabled ePWM module
    regSys_->pclkcr0.bit.tbclksync = 0;
    // Set UP counting mode
    regPwm_->tbctl.bit.ctrmode = Tbctl::COUNT_DOWN;    
    for(int32 i=0; i<CHANNELS_NUMBER; i++)
    {
      regPwm_->aqctl[i].val = 0x0000;    
      switch(i)
      {
        case 0:
        {
          regPwm_->aqctl[i].bit.cad = Aqctl::SET;
          regPwm_->aqctl[i].bit.prd = Aqctl::CLEAR;
        }
        break;
        case 1:
        {
          regPwm_->aqctl[i].bit.cbd = Aqctl::SET;
          regPwm_->aqctl[i].bit.prd = Aqctl::CLEAR;
        }
        break;
        default: break;
      }
    }
    regSys_->pclkcr0.bit.tbclksync = 1;
    System::dallow();    
    return true;
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  PwmControllerDown(const PwmControllerDown& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  PwmControllerDown& operator =(const PwmControllerDown& obj);    

};
#endif // DRIVER_PWM_CONTROLLER_DOWN_HPP_
