/** 
 * TI TMS320F2833x DSP Enhanced Pulse Width Modulator up-down counting controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_PWM_CONTROLLER_UP_DOWN_HPP_
#define DRIVER_PWM_CONTROLLER_UP_DOWN_HPP_

#include "driver.PwmController.hpp"

class PwmControllerUpDown : public ::PwmController
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
  PwmControllerUpDown(int32 clock, int32 index) : Parent(clock, index)
  {
    setConstruct( construct() );
  }

  /** 
   * Destructor.
   */                               
  virtual ~PwmControllerUpDown()
  {
  }
  
  /**
   * Starts generating the tasked wave of the PWM module.
   *
   * @return true if the task has been started successfully.
   */
  virtual bool start()
  {
    if(!isConstructed()) return false;
    if(!mutex_.res.lock()) return false;    
    if(!isTask(task_)) return mutex_.res.unlock( false );  
    uint16 prd, cmp[CHANNELS_NUMBER];
    // Set timer period
    int32 pwmclk = task_.getFrequency();
    // Rounding up real count of timer-base clocks which wishes to be loaded
    uint32 count = tbclk_ / pwmclk + (tbclk_ % pwmclk != 0 ? 1 : 0);
    // Real count of timer-base clocks which will be loaded
    prd = (count / 2) & 0x0000ffff;
    for(int32 i=0; i<CHANNELS_NUMBER; i++)
    {
      float32 duty = task_.getDuty(i);
      if(duty == ERROR) duty = 0.0f;
      float32 dead = 100.0f - duty;
      float32 value = static_cast<float32>(prd) * dead / 100.0f + 0.5f;
      cmp[i] = static_cast<uint32>(value) & 0x0000ffff;
    }
    // Write values to the registers in iterrupts disable mode
    // for possibility to call the method in an interrupt handler
    bool is = disable();
    regPwm_->tbprd.val = prd;
    for(int32 i=0; i<CHANNELS_NUMBER; i++) regPwm_->cmp[i].val = cmp[i];    
    enable(is);
    return mutex_.res.unlock( true );
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
      freq = tbclk_ / ( 2 * prd );
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
      float32 prd = static_cast<float32>(regPwm_->tbprd.val);
      float32 cmp = static_cast<float32>(regPwm_->cmp[channel].val);
      enable(is);
      duty = ((prd - cmp) * 100.0f) / prd;
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
    return Pwm::UPDOWN;
  }    
  
protected:
  
  /**
   * Returns max available frequency of the PWM module.
   *
   * @return max frequency, or ERROR if error occurred.   
   */        
  virtual int32 maxFrequency() const
  {
    if(!isConstructed()) return ERROR;
    return tbclk_ / 2;
  }
  
  /**
   * Returns min available frequency of the PWM module.
   *
   * @return min frequency, or ERROR if error occurred.
   */
  virtual int32 minFrequency() const
  {
    if(!isConstructed()) return ERROR;        
    return tbclk_ / 0xffff / 2 + 1;
  }
  
  /**
   * Sets received event.
   *
   * @param event an trigger received event.
   * @return true if the event has been set successfully.
   */  
  virtual uint16 getEnableEvenBits(int32 event)
  {
    uint16 bit = 0xffff;
    switch(event)
    {
      default: break;        
      // Time-base counter equal to zero.
      case CTR_ZERO: bit = 1; break;
      // Time-base counter equal to period.
      case CTR_PRD: bit = 2; break;
      // Time-base counter equal to CMPA when the timer is incrementing.
      case CTRU_CMPA: bit = 4; break;
      // Time-base counter equal to CMPA when the timer is decrementing.
      case CTRD_CMPA: bit = 5; break;
      // Time-base counter equal to CMPB when the timer is incrementing.
      case CTRU_CMPB: bit = 6; break;
      //Time-base counter equal to CMPB when the timer is decrementing.
      case CTRD_CMPB: bit = 7; break;
    }
    return bit;
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
    // Set UPDOWN counting mode
    regPwm_->tbctl.bit.ctrmode = Tbctl::COUNT_UPDOWN;    
    for(int32 i=0; i<CHANNELS_NUMBER; i++)
    {
      regPwm_->aqctl[i].val = 0x0000;    
      switch(i)
      {
        case 0:
        {
          regPwm_->aqctl[i].bit.cau = Aqctl::SET;
          regPwm_->aqctl[i].bit.cad = Aqctl::CLEAR;
        }
        break;
        case 1:
        {
          regPwm_->aqctl[i].bit.cbu = Aqctl::SET;
          regPwm_->aqctl[i].bit.cbd = Aqctl::CLEAR;
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
  PwmControllerUpDown(const PwmControllerUpDown& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  PwmControllerUpDown& operator =(const PwmControllerUpDown& obj);    

};
#endif // DRIVER_PWM_CONTROLLER_UP_DOWN_HPP_

