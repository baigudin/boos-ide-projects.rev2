/** 
 * TI TMS320F2833x DSP Enhanced Pulse Width Modulator abstract controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_PWM_CONTROLLER_HPP_
#define DRIVER_PWM_CONTROLLER_HPP_

#include "driver.Object.hpp"
#include "driver.Pwm.hpp"
#include "driver.PwmRegister.hpp"
#include "driver.System.hpp"
#include "driver.SystemRegister.hpp"
#include "driver.GpioRegister.hpp"
#include "driver.Mutex.hpp"
#include "driver.Interrupt.hpp"
#include "driver.PwmHighResolution.hpp"

class PwmController : public ::Object, 
                      public ::Pwm, 
                      public ::Pwm::DeadBand, 
                      public ::Pwm::Chopper,
                      public ::Pwm::EventTrigger
{
  typedef ::Object                    Parent;
  typedef ::PwmRegister::Tbctl::Val   Tbctl;
  typedef ::PwmRegister::Cmpctl::Val  Cmpctl;
  typedef ::PwmRegister::Aqctl::Val   Aqctl;
  typedef ::PwmRegister::Dbctl::Val   Dbctl;
  
  using   ::Pwm::getDuty;
  using   ::Pwm::Chopper::getDuty;  
  
public:

  /**
   * Constructor of the driver resource.
   *
   * @param clock the PWM internal clock based on the CPU clock in Hz.           
   * @param index an index of a PWM module.
   */   
  PwmController(int32 clock, int32 index) : Parent(),
    #ifdef    DRIVER_HRPWM
    isHiRes_  (false),
    #endif // DRIVER_HRPWM
    regPwm_   (NULL),
    mutex_    (),
    task_     (),
    index_    (index),
    tbclk_    (0),
    deadBand_ (),
    chopper_  (){
    setConstruct( construct(clock) );
  }

  /** 
   * Destructor.
   */                               
  virtual ~PwmController()
  {
    if(!isConstructed()) return;
    if(!mutex_.drv.lock()) return;
    #ifdef DRIVER_HRPWM
    MEP_ScaleFactor[index_+1] = 0;
    #endif // DRIVER_HRPWM
    clockOn(false);
    lock_[index_] = false;
    regPwm_ = NULL;
    index_ = ERROR;
    mutex_.drv.unlock();
  }

  /**
   * Sets a task for generating wave of the PWM module.
   *
   * @param task a new task for generating.
   * @return true if the task has been set successfully.
   */
  virtual bool setTask(const TaskInterface& task)
  {
    if(!isConstructed()) return false;
    if(!mutex_.res.lock()) return false;    
    if(!isTask(task)) return mutex_.res.unlock( false );
    task_ = task;
    return mutex_.res.unlock( true );
  }
  
  /**
   * Stops generating the wave of the PWM module.
   */
  virtual void stop()
  {
    if(!isConstructed()) return;
    if(!mutex_.res.lock()) return;
    regPwm_->tbctl.bit.ctrmode = Tbctl::COUNT_FREEZE;
    mutex_.res.unlock();
  }   
  
  /**
   * Returns the PWM module index.
   *
   * @return argument index passed to create method, or ERROR if error has been occurred.
   */      
  virtual int32 getIndex() const
  {
    return isConstructed() ? index_ : ERROR;
  }    
  
  /**
   * Returns a number of available PWM channels.
   *
   * @return a channels number, or ERROR if error has been occurred.
   */      
  virtual int32 getChannelsNumber() const
  {
    return isConstructed() ? CHANNELS_NUMBER : ERROR;
  }
  
  /**
   * Returns calculated resolution of PWM channel.
   *
   * @param channel available number of channel.   
   * @return the channel resolution in bits, or ERROR if error has been occurred.
   */  
  virtual float32 getResolution(int32 channel) const
  {
    if(!isConstructed() || !isChannel(channel)) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    float32 pwmclk = task_.getFrequency();
    if(pwmclk == ERROR || pwmclk == 0.0f || tbclk_ == 0) return mutex_.res.unlock(ERROR);
    float32 tbclk = static_cast<float32>(tbclk_);
    float32 resol = tbclk / pwmclk;    
    #ifdef DRIVER_HRPWM
    if(isHiRes_ && channel == 0)
    {
      int32 mep = getMep();
      if(mep != ERROR) resol = resol * static_cast<float32>(mep);
    }
    #endif // DRIVER_HRPWM        
    return mutex_.res.unlock( log2(resol) );    
  }
  
  /**
   * Enables the PWM channels in high resolution mode.
   *
   * @return true if high resolution mode is set successfully.
   */        
  virtual bool enableHighResolution()
  {
    return false;
  }
  
  /**
   * Disables the PWM channels in high resolution mode.
   */        
  virtual void disableHighResolution()
  {
    #ifdef DRIVER_HRPWM  
    if(!isConstructed()) return;
    if(!mutex_.res.lock()) return;
    regPwm_->tbphshr.bit.tbphsh = 0;
    regPwm_->cmpahr.bit.cmpahr = 0;
    isHiRes_ = false;
    return mutex_.res.unlock();
    #else  // DRIVER_HRPWM
    return;
    #endif // DRIVER_HRPWM    
  }
  
  /**
   * Returns the PWM clock frequency.
   *
   * @return the clock frequency, or ERROR if error has been occurred.
   */        
  virtual int32 getClockFrequency() const
  {
    return isConstructed() ? tbclk_ : ERROR;
  }
  
  /**
   * Synchronizes the PWM module with a PWM module later in a synchronization chain.
   *
   * @return true if the synchronization has been successfully completed.   
   */      
  virtual bool synchronize()
  {
    if(!isConstructed()) return false;
    if(!mutex_.res.lock()) return false;
    if(!isSynchronizing()) return mutex_.res.unlock(false);
    regPwm_->tbctl.bit.swfsync = 1;
    return mutex_.res.unlock(true);
  }
  
  /**
   * Tests if the PWM module may synchronize next PWM module in a chain.
   *
   * @return true if the module synchronizes the next module.
   */      
  virtual bool isSynchronizing() const
  {
    if(!isConstructed()) return false;
    return regPwm_->tbctl.bit.syncosel == 0 ? true : false;
  }
  
  /**
   * Sets a synchronization event of the PWM module.
   *
   * @param event a synchronization event.
   */      
  virtual void setSynchronization(Sync event)
  {
    if(!isConstructed()) return;
    uint16 value;
    switch(event)
    {
      case INPUT   : value = 0; break;
      case PERIOD  : value = 1; break;
      case EDGE    : value = 2; break;
      case DISABLE : value = 3; break;
      default      : return;
    }  
    regPwm_->tbctl.bit.syncosel = value;
  }

  /**
   * Returns the synchronization event of the PWM module.   
   *
   * @return the synchronization event.
   */      
  virtual Sync getSynchronization() const
  {
    if(!isConstructed()) return DISABLE;
    switch(regPwm_->tbctl.bit.syncosel)
    {
      case  0: return INPUT;
      case  1: return PERIOD;
      case  2: return EDGE;
      case  3: return DISABLE;
      default: return DISABLE;
    }  
  }
  
  /**
   * Sets the PWM module phase shift time.
   *
   * @param time the PWN signals shift time during synchronization in ns.
   */      
  virtual void setPhase(int32 time)
  {
    if(time < 0) return;  
    if(!isConstructed()) return;
    if(!mutex_.res.lock()) return;
    #ifdef DRIVER_HRPWM    
    // Lock up the phase shift for future backlog
    if(isHiRes_) return mutex_.res.unlock();
    #endif
    int32 tbprd = 1000000000 / tbclk_;
    int32 value = time / tbprd;
    regPwm_->tbphs.val = value <= 0xffff ? value : 0xffff;
    return mutex_.res.unlock();
  }

  /**
   * Returns the PWM module phase shift time.
   *
   * @return the PWN signals shift time during synchronization in ns.
   */      
  virtual int32 getPhase() const
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    int32 tbprd = 1000000000 / tbclk_;
    int32 value = regPwm_->tbphs.val * tbprd;
    return mutex_.res.unlock(value);
  }
  
  /**
   * Returns the PWM module phase enabling key.
   *
   * @param the phase enabling key.     
   */      
  virtual ::Pwm::Toggle& getPhaseKey()
  {
    return isConstructed() ? key_[PHASE_KEY_INDEX] : key_[ILLEGAL_KEY_INDEX];  
  }  
  
  /**
   * Sets the PWM module counter direction after a synchronization.
   *
   * @param value new direction value.
   */      
  virtual void setPhaseDirection(Count count)
  {
    if(!isConstructed()) return;
    uint16 value;
    switch(count)
    {
      case DOWN : value = 0; break;
      case UP   : value = 1; break;
      default   : return;
    }  
    regPwm_->tbctl.bit.phsdir = value;  
  }

  /**
   * Returns the PWM module counter direction after a synchronization.
   *
   * @return the direction value.
   */      
  virtual Count getPhaseDirection() const
  {
    if(!isConstructed()) return UPDOWN;
    switch(regPwm_->tbctl.bit.phsdir)
    {
      case  0: return DOWN;
      case  1: return UP;
      default: return UPDOWN;
    }    
  }
  
  /**
   * Tests if the PWM module has dead-band submodule.
   *
   * @return true if the module has dead-band submodule.
   */      
  virtual bool isDeadBanded() const
  {
    return true;
  }
  
  /**
   * Returns a dead-band submodule interface.
   *
   * @return reference to the dead-band submodule interface.
   */      
  virtual ::Pwm::DeadBand& getDeadBand()
  {
    return *this;
  }
  
  /**
   * Sets rising-edge delay.
   *
   * @param time delay time in ns.
   */  
  virtual void setRisingDelay(float32 time)
  {
    if(time < 0) return;    
    if(!isConstructed()) return;
    if(!mutex_.res.lock()) return;    
    float32 tbclk = static_cast<float32>(tbclk_);
    float32 value = time / (1000000000.0f / tbclk) + 0.5f;    
    deadBand_.rising = time;    
    regPwm_->dbred.bit.del = static_cast<uint16>(value);
    mutex_.res.unlock();    
  }
    
  /**
   * Returns the PWM signal rising-edge delay.
   *
   * @param set flag allows getting the set or calculated value.
   * @return the rising-edge delay in ns, or ERROR if error has been occurred.     
   */  
  virtual float32 getRisingDelay(bool set)
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;        
    float32 value;
    if(set)
    {
      value = deadBand_.rising;
    }
    else
    {
      uint16 count = regPwm_->dbred.bit.del;
      float32 tbclk = static_cast<float32>(tbclk_);      
      float32 tbprd = 1000000000.0f / tbclk;
      value = tbprd * static_cast<float32>(count);
    }
    return mutex_.res.unlock(value);
  }
  
  /**
   * Sets falling-edge delay.
   *
   * @param time delay time in ns.
   */   
  virtual void setFallingDelay(float32 time)
  {
    if(time < 0) return;  
    if(!isConstructed()) return;  
    if(!mutex_.res.lock()) return;
    float32 tbclk = static_cast<float32>(tbclk_);
    float32 value = time / (1000000000.0f / tbclk) + 0.5f;    
    deadBand_.falling = time;    
    regPwm_->dbfed.bit.del = static_cast<uint16>(value);
    mutex_.res.unlock();
  } 
    
  /**
   * Returns the PWM signal falling-edge delay.
   *
   * @param set flag allows getting the set or calculated value.
   * @return the falling-edge delay in ns, or ERROR if error has been occurred.     
   */  
  virtual float32 getFallingDelay(bool set)
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    float32 value;
    if(set)
    {
      value = deadBand_.falling;
    }
    else
    {
      uint16 count = regPwm_->dbfed.bit.del;
      float32 tbclk = static_cast<float32>(tbclk_);      
      float32 tbprd = 1000000000.0f / tbclk;
      value = tbprd * static_cast<float32>(count);
    }
    return mutex_.res.unlock(value);
  }
  
  /**
   * Retruns in-key for a channel.
   *
   * @param channel available dead-band signal channel number.
   * @param the toggling key.   
   */  
  virtual ::Pwm::Toggle& getInput(int32 channel)
  {
    if(!isConstructed()) return key_[ILLEGAL_KEY_INDEX];
    switch(channel)
    {
      case  0: return key_[4];
      case  1: return key_[5];
      default: return key_[ILLEGAL_KEY_INDEX];
    }
  }

  /**
   * Retruns out-key for a channel.
   *
   * @param channel available dead-band signal channel number.
   * @param the toggling key.      
   */  
  virtual ::Pwm::Toggle& getOutput(int32 channel)
  {
    if(!isConstructed()) return key_[ILLEGAL_KEY_INDEX];
    switch(channel)
    {
      case  0: return key_[1];
      case  1: return key_[0];
      default: return key_[ILLEGAL_KEY_INDEX];
    }
  }

  /**
   * Retruns polarity-key for a channel.
   *
   * @param channel available dead-band signal channel number.
   * @param the toggling key.      
   */  
  virtual ::Pwm::Toggle& getPolarity(int32 channel)
  {
    if(!isConstructed()) return key_[ILLEGAL_KEY_INDEX];
    switch(channel)
    {
      case  0: return key_[2];
      case  1: return key_[3];
      default: return key_[ILLEGAL_KEY_INDEX];
    }
  }
  
  /**
   * Tests if the PWM module has chopper submodule.
   *
   * @return true if the module has chopper submodule.
   */      
  virtual bool isChopped() const
  {
    return true;
  }
  
  /**
   * Returns a chopper submodule interface.
   *
   * @return reference to the chopper submodule interface.
   */      
  virtual ::Pwm::Chopper& getChopper()
  {
    return *this;
  }
  
  /**
   * Sets new frequency of chopping signal.
   *
   * @param frequency new chopping signal frequency in Hz.
   */
  virtual void setFrequency(int32 frequency)
  {
    if(frequency < 0) return; 
    if(!isConstructed()) return;  
    if(!mutex_.res.lock()) return;
    uint16 bit;
    int32 div = sysclk_ / 8 / frequency;
    // Calculate dividers for chopping clock frequency
    if(0 <= div && div < 2) 
    {
      bit = 0;
    }
    else if(2 <= div && div < 3) 
    {
      bit = 1;
    }
    else if(3 <= div && div < 4) 
    {
      bit = 2;
    }    
    else if(4 <= div && div < 5)
    {
      bit = 3;
    }
    else if(5 <= div && div < 6) 
    {
      bit = 4;
    }
    else if(6 <= div && div < 7) 
    {
      bit = 5;
    }    
    else if(7 <= div && div < 8) 
    {
      bit = 6;
    }
    else
    {
      bit = 7;
    }    
    regPwm_->pcctl.bit.chpfreq = bit;
    chopper_.freq = frequency;
    mutex_.res.unlock();  
  }
  
  /**
   * Returns the frequency of chopping signal.
   *
   * @param set flag allows getting the set or calculated value.     
   * @return the chopping signal frequency in Hz, or ERROR if error has been occurred.
   */
  virtual int32 getFrequency(bool set) const
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    int32 freq;
    if(set)
    {
      freq = chopper_.freq;
    }
    else
    {
      int32 freq = sysclk_ / 8;
      switch(regPwm_->pcctl.bit.chpfreq)
      {
        case  0: freq = freq / 1;  break;
        case  1: freq = freq / 2;  break;
        case  2: freq = freq / 3;  break;
        case  3: freq = freq / 4;  break;
        case  4: freq = freq / 5;  break;
        case  5: freq = freq / 6;  break;
        case  6: freq = freq / 7;  break;
        case  7: freq = freq / 8;  break;        
        default: freq = ERROR; break;
      }
    }
    return mutex_.res.unlock(freq);  
  }
  
  /**
   * Sets the chopper duty.
   *
   * @param duty a chopping duty value in percentages.
   */        
  virtual void setDuty(float32 duty)
  {
    const int32 DUTY_12_MAX = (12 + 25) >> 1;
    const int32 DUTY_25_MAX = (25 + 37) >> 1;
    const int32 DUTY_37_MAX = (37 + 50) >> 1;
    const int32 DUTY_50_MAX = (50 + 62) >> 1;
    const int32 DUTY_62_MAX = (62 + 75) >> 1;    
    const int32 DUTY_75_MAX = (75 + 85) >> 1;        
    const int32 DUTY_85_MAX = (85 +100) >> 1;            
    if(duty < 0.0f || duty > 100.0f) return;  
    if(!isConstructed()) return;  
    if(!mutex_.res.lock()) return;
    uint16 bit;
    int32 val = static_cast<int32>(duty + 0.5f);
    // Set 12.5% duty cycle
    if(0 <= val && val < DUTY_12_MAX)
    {
      bit = 0;
    }
    // Set 25.0% duty cycle    
    else if(DUTY_12_MAX <= val && val < DUTY_25_MAX) 
    {
      bit = 1;
    }
    // Set 37.5% duty cycle        
    else if(DUTY_25_MAX <= val && val < DUTY_37_MAX) 
    {
      bit = 2;
    }
    // Set 50.0% duty cycle        
    else if(DUTY_37_MAX <= val && val < DUTY_50_MAX) 
    {
      bit = 3;
    }
    // Set 62.5% duty cycle            
    else if(DUTY_50_MAX <= val && val < DUTY_62_MAX) 
    {
      bit = 4;
    }
    // Set 75.0% duty cycle
    else if(DUTY_62_MAX <= val && val < DUTY_75_MAX) 
    {
      bit = 5;
    }
    // Set 87.5% duty cycle    
    else if(DUTY_75_MAX <= val && val < DUTY_85_MAX) 
    {
      bit = 6;
    }    
    // Set 87.5% duty cycle as well    
    else
    {
      bit = 6;
    }
    regPwm_->pcctl.bit.chpduty = bit;
    chopper_.duty = duty;
    mutex_.res.unlock();  
  }
  
  /**
   * Returns the chopper duty.
   *
   * @param set flag allows getting the set or calculated value.
   * @return the chopping duty value in percentages, or ERROR if error has been occurred.
   */        
  virtual float32 getDuty(bool set) const
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    float32 duty;
    if(set)
    {
      duty = chopper_.duty;
    }
    else
    {
      switch(regPwm_->pcctl.bit.chpduty)
      {
        case  0: duty = 12.5f;  break;
        case  1: duty = 25.0f;  break;
        case  2: duty = 37.5f;  break;
        case  3: duty = 50.0f;  break;
        case  4: duty = 62.5f;  break;
        case  5: duty = 75.0f;  break;
        case  6: duty = 87.5f;  break;
        default: duty = ERROR; break;
      }
    }
    return mutex_.res.unlock(duty);  
  }
  
  /**
   * Sets new width of the first pulse.
   *
   * @param time new first pulse width in ns.
   */
  virtual void setFirstPulse(int32 time)
  {
    if(time < 0) return;  
    if(!isConstructed()) return;  
    if(!mutex_.res.lock()) return;
    uint16 bit;
    uint32 mult = time / (8000 / (sysclk_ / 1000000));
    if(mult == 0) bit = 0x0;
    else if(mult > 16) bit = 0xf;
    else bit = mult - 1;
    regPwm_->pcctl.bit.oshtwth = bit;
    chopper_.pulse = time;    
    mutex_.res.unlock();  
  }   
  
  /**
   * Returns the width of the first pulse.
   *
   * @param set flag allows getting the set or calculated value.     
   * @return the first pulse width in ns, or ERROR if error has been occurred.
   */
  virtual int32 getFirstPulse(bool set) const
  {
    if(!isConstructed()) return ERROR;
    if(!mutex_.res.lock()) return ERROR;
    int32 pulse;
    if(set)
    {
      pulse = chopper_.pulse;
    }
    else
    {
      int32 mult = regPwm_->pcctl.bit.oshtwth + 1;
      int32 wide = 8000 / (sysclk_ / 1000000);
      pulse = mult * wide;
    }
    return mutex_.res.unlock(pulse);  
  }
  
  /**
   * Retruns the chopper enable key.
   *
   * @param an enable key.
   */  
  virtual ::Pwm::Toggle& getChopping()
  {
    return isConstructed() ? key_[CHOPPER_KEY_INDEX] : key_[ILLEGAL_KEY_INDEX];
  }
  
  /**
   * Tests if the PWM module has chopper submodule.
   *
   * @return true if the module has chopper submodule.
   */      
  virtual bool isTriggered() const
  {
    return true;
  }
  
  /**
   * Returns a chopper submodule interface.
   *
   * @return reference to the chopper submodule interface.
   */      
  virtual ::Pwm::EventTrigger& getTrigger()
  {
    return *this;
  }
  
  /**
   * Sets triggering event.
   *
   * @param issue an trigger issuing source.
   * @param event an trigger received event.
   * @return true if the event has been set successfully.
   */  
  virtual bool setEvent(int32 issue, int32 event)
  {
    if(!isConstructed()) return false;
    if(!mutex_.res.lock()) return false;
    uint16 bit;
    bool res = false;
    switch(issue)
    {
      case Pwm::ADC_SOCA:
      {
        // Reset time-base events
        regPwm_->etsel.bit.socaen = 0;    
        regPwm_->etsel.bit.socasel = 0;
        // Set time-base events        
        bit = getEnableEvenBits(event);
        if( bit == 0xffff ) break;
        regPwm_->etsel.bit.socasel = bit;
        // Set one the socasel event selected need to be occurred
        regPwm_->etps.bit.socaprd = 1;
        // Enable event
        regPwm_->etsel.bit.socaen = 1;        
        res = true;
      }
      break;
      case Pwm::ADC_SOCB:
      {
        // Reset time-base events
        regPwm_->etsel.bit.socben = 0;    
        regPwm_->etsel.bit.socbsel = 0;
        // Set time-base events        
        bit = getEnableEvenBits(event);
        if( bit == 0xffff ) break;
        regPwm_->etsel.bit.socbsel = bit;
        // Set one the socasel event selected need to be occurred
        regPwm_->etps.bit.socbprd = 1;
        // Enable event
        regPwm_->etsel.bit.socben = 1;        
        res = true;
      }
      break;      
    }
    return mutex_.drv.unlock(res);    
  }
  
  /**
   * Resets triggering event.
   *
   * @param issue an trigger issuing source.
   */  
  virtual void resetEvent(int32 issue)
  {
    if(!isConstructed()) return;
    if(!mutex_.res.lock()) return;
    switch(issue)
    {
      case Pwm::ADC_SOCA:
      {
        // Reset time-base events
        regPwm_->etsel.bit.socaen = 0;    
        regPwm_->etsel.bit.socasel = 0;
      }
      break;
      case Pwm::ADC_SOCB:
      {
        // Reset time-base events
        regPwm_->etsel.bit.socben = 0;    
        regPwm_->etsel.bit.socbsel = 0;
      }
      break;      
    }
    return mutex_.drv.unlock();    
  }
  
  /**
   * Initialization.
   *
   * @param sourceClock the CPU oscillator source clock in Hz.   
   * @return true if no errors.
   */
  static bool init(int32 sourceClock)
  {
    isInitialized_ = 0;
    for(int32 i=0; i<RESOURCES_NUMBER; i++) lock_[i] = false;
    // Create the driver Mutex
    drvMutex_ = new Mutex();
    if(drvMutex_ == NULL || !drvMutex_->isConstructed() ) return false;
    // Create the CPU registers maps
    regSys_ = new (SystemRegister::ADDRESS) SystemRegister();
    regGpio_ = new (GpioRegister::ADDRESS) GpioRegister();    
    sysclk_ = getCpuClock(sourceClock);
    if(sysclk_ <= 0) return false;
    // Set all ePWMs pins mixed with GPIO[0-11] pins as ePWMs
    System::eallow();
    // Enable the internal pullup on the specified pins.
    regGpio_->gpapud.val &= 0xfffff000;
    // Configure the GPIO pins as ePWMs output
    regGpio_->gpamux1.val &= 0xff000000;  
    regGpio_->gpamux1.val |= 0x00555555;      
    System::dallow();
    #ifdef DRIVER_HRPWM
    uint32 addr;
    // Default TI SFO Library varaibles initialization
    for(int32 i=0; i<PWM_CH; i++) 
    {
      MEP_ScaleFactor[i] = 0;
      switch(i)
      {
        case  1: addr = PwmRegister::ADDRESS0; break;
        case  2: addr = PwmRegister::ADDRESS1; break;
        case  3: addr = PwmRegister::ADDRESS2; break;
        case  4: addr = PwmRegister::ADDRESS3; break;
        case  5: addr = PwmRegister::ADDRESS4; break;
        case  6: addr = PwmRegister::ADDRESS5; break;        
        default: addr = 0;
      }       
      ePWM[i] = reinterpret_cast<EPWM_REGS*>(addr);
    }
    #endif // DRIVER_HRPWM
    isInitialized_ = IS_INITIALIZED;
    return true;
  }
  
  /**
   * Deinitialization.
   */
  static void deinit()
  {
    sysclk_ = 0;
    regSys_ = NULL;
    regGpio_ = NULL;
    isInitialized_ = 0;
    if(drvMutex_ != NULL) delete drvMutex_;
    for(int32 i=0; i<RESOURCES_NUMBER; i++) lock_[i] = false;
  }
  
protected:  

  /**
   * Returns max available frequency of the PWM module.
   *
   * @return max frequency, or ERROR if error occurred.   
   */        
  virtual int32 maxFrequency() const = 0;  
  
  /**
   * Returns min available frequency of the PWM module.
   *
   * @return min frequency, or ERROR if error occurred.
   */
  virtual int32 minFrequency() const = 0;
  
  /**
   * Sets received event.
   *
   * @param event an trigger received event.
   * @return true if the event has been set successfully.
   */  
  virtual uint16 getEnableEvenBits(int32 event) = 0;

  /**
   * Tests task has correct values
   *
   * @param task a task.
   * @return true if the task is correct.
   */
  virtual bool isTask(const TaskInterface& task) const
  {
    float32 duty;  
    int32 maxFreq, minFreq, freq;
    if(task.getChannelsNumber() > CHANNELS_NUMBER) return false;
    maxFreq = maxFrequency();
    if(maxFreq == ERROR) return false;    
    minFreq = minFrequency();
    if(minFreq == ERROR) return false;    
    freq = task.getFrequency();
    if(freq == ERROR) return false;
    if(freq < minFreq || freq > maxFreq) return false;
    for(int32 i=0; i<CHANNELS_NUMBER; i++)
    {
      duty = task.getDuty(i);
      if(duty == ERROR) return false;
      if(duty < 0 || duty > 100) return false;
    }
    return true;    
  }
  
  /**
   * Tests if passed value is available channel number.
   *
   * The method may be used for determining a channel is available for PWM or DB
   *
   * @param channel a number of channel.
   * @return true if channel number is available.
   */    
  virtual bool isChannel(int32 channel) const
  {
    return 0 <= channel && channel < CHANNELS_NUMBER ? true : false;
  }

  #ifdef DRIVER_HRPWM
  
  /**
   * Starts generating the tasked wave of the PWM module in high resolution.
   */
  virtual void startHighResolution()
  {
    if(not isHiRes_) return;
    uint16 cmphr;
    // Set timer period    
    int32 pwmclk = task_.getFrequency();    
    // Rounding up real count of timer-base clocks which wishes to be loaded
    int32 count = tbclk_ / pwmclk + (tbclk_ % pwmclk != 0 ? 1 : 0);
    float32 prd = static_cast<float32>( count );
    float32 mep = static_cast<float32>( getMep() );
    float32 duty = task_.getDuty(0);      
    if(mep != ERROR || duty == ERROR)
    {
      float32 value = duty * prd / 100.0f;
      value = value - static_cast<float32>( static_cast<int32>(value) );
      value = value * mep + 1.5;
      cmphr = static_cast<uint16>(value);
      // Write values to the registers in iterrupts disable mode
      // for possibility to call the method in an interrupt handler
      bool is = disable();      
      regPwm_->cmpahr.bit.cmpahr = cmphr;       
      enable(is);
      // TODO: Here the HRPWM capability is enable, and updating has to be.
      // But, the TI SFO_MepEn_V5 fuction always returns SFO_INCOMPLETE state.
      // Thus, when the problem is solved, the step will be uncommented, and,
      // also probably, a background task is a BOOS thread will be written
      // for dynamical updating the MEP.
      // mep = static_cast<float32>( updatedMep() );      
    }
  }
  
  /** 
   * Returns MEP Scale Factor of the PWM channel.
   *
   * @return new MEP Scale Factor, or ERROR if error has been occurred.
   */    
  virtual int32 getMep() const
  {
    // This is important to lock driver mutex for locking all resources access
    if(!mutex_.drv.lock()) return ERROR;    
    int16 mep = MEP_ScaleFactor[0];
    return (0 < mep && mep <= MAX_MEP) ? mutex_.drv.unlock(mep) : mutex_.drv.unlock(ERROR);
  }

  /** 
   * Returns and updates MEP Scale Factor of the PWM channel.
   *
   * This is important, the method must be called in the driver mutex locked code section,
   * cause it updates TI SFO Library global variables which might be used concurrently.
   *
   * @return new MEP Scale Factor, or ERROR if error has been occurred.
   */    
  virtual int32 updatedMep()  
  {
    int16 mep, num;
    num = index_ + 1;    
    volatile int16 error = SFO_INCOMPLETE;
    // This is important to lock driver mutex for locking all resources access
    if(!mutex_.drv.lock()) return ERROR;    
    // If HRPWM capability is disabled
    if(regPwm_->cmpahr.bit.cmpahr == 0)
    {
      // Initialize MEP_ScaleFactor varaible
      while(error == SFO_INCOMPLETE) 
      {
        error = SFO_MepDis_V5(num);
      }
    }
    else
    {
      // Initialize MEP_ScaleFactor varaible
      while(error == SFO_INCOMPLETE) 
      {
        error = SFO_MepEn_V5(num);
      }
    }
    mep = MEP_ScaleFactor[num];
    if(error != SFO_COMPLETE) return mutex_.drv.unlock(ERROR);
    if(mep < 0 || mep > MAX_MEP) return mutex_.drv.unlock(ERROR);
    // Initialize MEP_ScaleFactor[0] with a typical MEP seed value for SFO_MepEn_V5 
    MEP_ScaleFactor[0] = mep;
    return mutex_.drv.unlock(mep);
  }

  #endif // DRIVER_HRPWM     
  
  /** 
   * Disables a source interrupt.
   *
   * @return an enable source bit value of a controller before method was called.
   */ 
  virtual bool disable() const
  {
    return false;
  }
  
  /** 
   * Enables a source interrupt.
   *
   * @param status returned status by disable method.
   */    
  virtual void enable(bool status) const
  {
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  static bool isNan(float32 num)
  {
    // So, here is I will think that I'm the smartest guy and want 
    // getting real value of memory cell of passed float number
    int32 cell = *reinterpret_cast<int32*>(&num) & FLOAT32_EXPONENT;
    return cell == FLOAT32_EXPONENT ? true : false;
  } 
  
private:

  /** 
   * Constructor.
   *
   * @param clock the PWM internal clock based on the CPU clock in Hz.           
   * @return boolean result.
   */  
  bool construct(int32 clock)
  {
    Toggle* key;  
    uint32 addr;
    if(isInitialized_ != IS_INITIALIZED) return false;
    if(!mutex_.drv.isConstructed()) return false;    
    if(!mutex_.res.isConstructed()) return false;
    if(!mutex_.drv.lock()) return false;    
    switch(index_)
    {
      case  0: addr = PwmRegister::ADDRESS0; break;
      case  1: addr = PwmRegister::ADDRESS1; break;
      case  2: addr = PwmRegister::ADDRESS2; break;
      case  3: addr = PwmRegister::ADDRESS3; break;
      case  4: addr = PwmRegister::ADDRESS4; break;
      case  5: addr = PwmRegister::ADDRESS5; break;        
      default: return mutex_.drv.unlock(false);
    }    
    if(lock_[index_] == true) return mutex_.drv.unlock(false);
    regPwm_ = new (addr) PwmRegister();
    // Dead-band keys initialization
    for(int32 i=0; i<DEAD_BAND_KEYS_NUMBER; i++)
    {
      // Index of this loop is value of DBCTL register shift for current key
      key = new (&key_[i]) Toggle(&regPwm_->dbctl.val, i, &mutex_.res);
      if(key == NULL || !key->isConstructed()) return mutex_.drv.unlock(false);
    }
    // Choper key initialization
    key = new (&key_[CHOPPER_KEY_INDEX]) Toggle(&regPwm_->pcctl.val, 0, &mutex_.res);
    if(key == NULL || !key->isConstructed()) return mutex_.drv.unlock(false);
    // Phase key initialization
    key = new (&key_[PHASE_KEY_INDEX]) Toggle(&regPwm_->tbctl.val, 2, &mutex_.res);
    if(key == NULL || !key->isConstructed()) return mutex_.drv.unlock(false);
    // Calculate dividers for TBCLK
    if(clock < 0) return mutex_.drv.unlock(false);
    int32 div, div0, div1;
    tbclk_ = sysclk_;    
    div = tbclk_ / clock;
    if(div < 1)
    {
      div0 = 0;
    }
    else if(1 <= div && div < 2) 
    {
      div0 = 1;
    }
    else if(2 <= div && div < 4) 
    {
      div0 = 2;
    }
    else if(4 <= div && div < 8) 
    {
      div0 = 4;
    }    
    else if(8 <= div && div < 16) 
    {
      div0 = 8;
    }
    else if(16 <= div && div < 32) 
    {
      div0 = 16;
    }
    else if(32 <= div && div < 64) 
    {
      div0 = 32;
    }    
    else if(64 <= div && div < 128) 
    {
      div0 = 64;
    }
    else
    {
      div0 = 128;
    }    
    if(div0 == 0) return mutex_.drv.unlock(false); 
    tbclk_ = tbclk_ / div0;
    div = tbclk_ / clock; 
    if(div < 1)
    {
      div1 = 0;
    }
    else if(1 <= div && div < 2) 
    {
      div1 = 1;
    }
    else if(2 <= div && div < 4) 
    {
      div1 = 2;
    }
    else if(4 <= div && div < 6) 
    {
      div1 = 4;
    }    
    else if(6 <= div && div < 8) 
    {
      div1 = 6;
    }
    else if(8 <= div && div < 10) 
    {
      div1 = 8;
    }
    else if(10 <= div && div < 12) 
    {
      div1 = 10;
    }    
    else if(12 <= div && div < 14) 
    {
      div1 = 12;
    }
    else
    {
      div1 = 14;
    }    
    if(div1 == 0) return mutex_.drv.unlock(false); 
    tbclk_ = tbclk_ / div1;    
    switch(div0)
    {
      case   1: div0 = 0; break;
      case   2: div0 = 1; break;
      case   4: div0 = 2; break;
      case   8: div0 = 3; break;
      case  16: div0 = 4; break;
      case  32: div0 = 5; break;
      case  64: div0 = 6; break;
      case 128: div0 = 7; break;
      default : return mutex_.drv.unlock(false);
    }
    switch(div1)
    {
      case  1: div1 = 0; break;
      case  2: div1 = 1; break;
      case  4: div1 = 2; break;
      case  6: div1 = 3; break;
      case  8: div1 = 4; break;
      case 10: div1 = 5; break;
      case 12: div1 = 6; break;
      case 14: div1 = 7; break;
      default: return mutex_.drv.unlock(false);
    }    
    lock_[index_] = true;
    // Set strat settings
    clockOn(true);    
    // Set TBCLK frequency dividers
    regPwm_->tbctl.bit.clkdiv = div0;
    regPwm_->tbctl.bit.hspclkdiv = div1;    
    // Set EPWMxSYNCO signal source is enabled
    regPwm_->tbctl.bit.syncosel = 0;    
    // Set free run of the ePWM
    regPwm_->tbctl.bit.freesoft = 2;
    // Use TBPRD shadow register
    regPwm_->tbctl.bit.prdld = Tbctl::SHADOW;    
    // Set TBCNT is loaded with the phase
    regPwm_->tbctl.bit.phsen = 1;    
    // Set timer counter
    regPwm_->tbctr.val = 0;
    // Set phase phase
    regPwm_->tbphs.val = 0;    
    // Use CMPA and CMPB shadow registers
    regPwm_->cmpctl.bit.shdwamode = Cmpctl::SHADOW;
    regPwm_->cmpctl.bit.shdwbmode = Cmpctl::SHADOW;  
    // Set CMPA and CMPB are loaded when CTR equals to zero
    regPwm_->cmpctl.bit.loadamode = Cmpctl::LOAD_CTR_EQ_ZERO;
    regPwm_->cmpctl.bit.loadbmode = Cmpctl::LOAD_CTR_EQ_ZERO;     
    #ifdef DRIVER_HRPWM
    mutex_.drv.unlock();
    return updatedMep() != ERROR ? true : false;
    #else  // DRIVER_HRPWM            
    return mutex_.drv.unlock(true);
    #endif // DRIVER_HRPWM
  }
  
  /** 
   * Turns the PWM clock on.
   */    
  void clockOn(bool value)
  {
    uint16 bit = value ? 1 : 0;
    System::eallow();
    switch(index_)
    {
      case  0: regSys_->pclkcr1.bit.epwm1enclk = bit; break; 
      case  1: regSys_->pclkcr1.bit.epwm2enclk = bit; break; 
      case  2: regSys_->pclkcr1.bit.epwm3enclk = bit; break; 
      case  3: regSys_->pclkcr1.bit.epwm4enclk = bit; break; 
      case  4: regSys_->pclkcr1.bit.epwm5enclk = bit; break; 
      case  5: regSys_->pclkcr1.bit.epwm6enclk = bit; break; 
      default: break; 
    }
    System::dallow();
  }
  
  /** 
   * Returns SYSCLK based on OSCCLK.
   *     
   * @param sourceClock source clock in Hz.    
   * @return CPU clock in Hz.
   */  
  static int32 getCpuClock(int32 sourceClock)
  {
    int32 sysclk, m, d;
    if(regSys_ == NULL) return ERROR;
    // Test the oscillator is not off
    if(regSys_->pllsts.bit.oscoff == 1) return false;
    // Test the PLL is set correctly
    if(regSys_->pllsts.bit.plloff == 1 && regSys_->pllcr.bit.div > 0) return false;
    // Calculate the CPU frequency
    m = regSys_->pllcr.bit.div != 0 ? regSys_->pllcr.bit.div : 1;
    switch(regSys_->pllsts.bit.divsel)
    {
      case  0: 
      case  1: d = 4; break;
      case  2: d = 2; break;       
      default: return false;
    }
    sysclk = sourceClock / d * m;
    return sysclk > 0 ? sysclk : ERROR;
  }
  
  /**
   * Returns calculated value of logarithm function.
   *
   * @param x an argument value.
   * @return the logarithm value.
   */ 
  static float32 log2(float32 x)
  {
    const float LOG2E = 1.44269504088896340736f;
    union Float
    {
      Float(){}
      Float(float32 v){val = v;}          
     ~Float(){}    
      float32 val;
      struct
      {
        uint32 mant : 23;
        uint32 exp  : 8;
        uint32 sign : 1;  
      } bit;
    } f = x;
    int32 exp = f.bit.exp - 127;
    f.bit.sign = 0;
    f.bit.exp = 127;
    return (f.val - 1.0f) * LOG2E + exp;
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  PwmController(const PwmController& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  PwmController& operator =(const PwmController& obj);    
  
  /**
   * Mutexs of the driver and the resource.
   */
  struct Mutexs
  {
    /** 
     * Constructor.
     */    
    Mutexs() :
      drv       (*drvMutex_),
      res       (){
    }
    
    /** 
     * Destructor.
     */    
   ~Mutexs(){}
    
    /**
     * The driver Mutex.
     */  
    Mutex& drv;
    
    /**
     * The resource Mutex.
     */      
    mutable Mutex res;

  };
  
  /**
   * Dead-Band values.
   */
  struct DeadBand
  {
    /** 
     * Constructor.
     */    
    DeadBand() :
      rising  (0.0f),
      falling (0.0f){
    }
    
    /** 
     * Destructor.
     */    
   ~DeadBand(){}
    
    /**
     * Rising edge delay.
     */  
    float32 rising;
    
    /**
     * Falling edge delay.
     */      
    float32 falling;

  };  
  
  /**
   * Chopper values.
   */
  struct Chopper
  {
    /** 
     * Constructor.
     */    
    Chopper() :
      freq  (ERROR),
      duty  (ERROR),
      pulse (ERROR){
    }
    
    /** 
     * Destructor.
     */    
   ~Chopper(){}
   
    /**
     * Chopping frequency in Hz.
     */  
    int32 freq;   
    
    /**
     * Chopping duty in percentages.
     */  
    float32 duty;

    /**
     * One-shot pulse width in ns.
     */      
    int32 pulse;

  };    
  
  /**
   * Dead-Band keys controller.
   */
  class Toggle : public ::Object, public ::Pwm::Toggle
  {
    typedef ::Object Parent;

  public:
  
    /** 
     * Constructor.
     */    
    Toggle() : Parent(),
      reg_   (NULL),
      mut_   (NULL),
      shift_ (0){
      setConstruct( false );
    }  

    /** 
     * Constructor.
     */    
    Toggle(uint16* reg, int32 shift, Mutex* mut) : Parent(),
      reg_   (reg),
      mut_   (mut),
      shift_ (shift){
      setConstruct( construct() );
    }

    /** 
     * Destructor.
     */    
    virtual ~Toggle()
    {
    }

    /** 
     * Disables the key.
     *
     * @return the key bit value before the method was called.
     */ 
    virtual bool disable()
    {
      if(!isConstructed()) return false;
      if(!mut_->lock()) return false;
      uint16& reg = *reg_;
      uint16 value = reg & 0x1 << shift_;      
      bool bit = value == 0 ? false : true;
      reg &= ~(0x1 << shift_);
      return mut_->unlock(bit);
    }

    /**
     * Enables the key.
     *
     * @param status returned status by disable method, or true for directly turning the key on.
     */    
    virtual void enable(bool status=true)
    {
      if(!isConstructed()) return;    
      if(status == false ) return;
      if(!mut_->lock()) return;
      uint16& reg = *reg_;
      reg |= 0x1 << shift_;
      return mut_->unlock();      
    }
    
    /**
     * Operator new.
     *
     * @param size unused.
     * @param ptr  address of memory.
     * @return address of memory.
     */     
    void* operator new(uint32, Toggle* ptr)
    {
      return reinterpret_cast<void*>(ptr);
    }     

  private:
  
    /** 
     * Constructor.
     *
     * @param clock the PWM internal clock based on the CPU clock in Hz.           
     * @return boolean result.
     */  
    bool construct()
    {
      if(!Parent::isConstructed()) return false;    
      if(!mut_->isConstructed()) return false;      
      return true;
    }
  
    /**
     * Locking mutex.
     */
    Mutex* mut_;      

    /**
     * Controlling bit shift.
     */
    int32 shift_;
    
    /**
     * PWM registers.
     */
    uint16* reg_;
    
  };
  
protected:
  
  /**
   * Number of PWM modules.
   */
  static const int32 RESOURCES_NUMBER = 6;
  
  /**
   * Number of PWM modules.
   */
  static const int32 CHANNELS_NUMBER = 2;
  
  /**
   * Number of PWM dead-band keys.
   */
  static const int32 KEYS_NUMBER = 9;  
  
  /**
   * Number of PWM dead-band keys.
   */
  static const int32 DEAD_BAND_KEYS_NUMBER = 6;
  
  /**
   * PWM chopper enable key index.
   */
  static const int32 CHOPPER_KEY_INDEX = 6;
  
  /**
   * PWM phase enable key index.
   */
  static const int32 PHASE_KEY_INDEX = 7;  

  /**
   * Illegal key index.
   */
  static const int32 ILLEGAL_KEY_INDEX = 8;

  /**
   * PWM driver initialized falg value.
   */
  static const int32 IS_INITIALIZED = 0xab569814;
  
  /**
   * The sing mask of float 32-bit number.
   */  
  static const int32 FLOAT32_SING = 0x80000000;
  
  /**
   * The exponent mask of float 32-bit number.
   */   
  static const int32 FLOAT32_EXPONENT = 0x7f800000;
  
  /**
   * The mantissa mask of float 32-bit number.
   */     
  static const int32 FLOAT32_MANTISSA = 0x007fffff;  

  #ifdef DRIVER_HRPWM
  
  /**
   * Max Micro Edge Positioning (MEP) step size at SYSCLKOUT = 60–150 MHz.
   */     
  static const int16 MAX_MEP = 310;
  
  #endif // DRIVER_HRPWM  

  /**
   * Locked PWM flags (no boot).
   */
  static bool lock_[RESOURCES_NUMBER];
  
  /**
   * CPU clock in Hz (no boot).
   */
  static int32 sysclk_;
  
  /**
   * System Control Registers (no boot).
   */  
  static SystemRegister* regSys_;
  
  /**
   * GPIO Registers (no boot).
   */  
  static GpioRegister* regGpio_;

  /**
   * Mutex of this driver (no boot).
   */  
  static Mutex* drvMutex_;
  
  /**
   * Driver has been initialized successfully (no boot).
   */
  static int32 isInitialized_;
  
  #ifdef DRIVER_HRPWM
  
  /**
   * HR PWM enabled.
   */
  bool isHiRes_;  
  
  #endif // DRIVER_HRPWM
  
  /**
   * HW registers.
   */
  PwmRegister* regPwm_;    
  
  /**
   * The driver and the resource mutexs.
   */  
  Mutexs mutex_;

  /**
   * The current executing task.
   */  
  Task<CHANNELS_NUMBER> task_;

  /**
   * The index of ePWM module.
   */
  int32 index_; 

  /**
   * Timer-Base clock in Hz.
   */
  int32 tbclk_;
  
  /**
   * Dead-band values.
   */  
  ::PwmController::DeadBand deadBand_;  

  /**
   * Chopper values.
   */  
  ::PwmController::Chopper chopper_;    
  
  /**
   * Submodule keys.
   *
   * Keys 0-5 must be always Dead-Band keys.
   * Key 6 is chopper key.
   * Key 7 is illegal key.
   */  
  Toggle key_[KEYS_NUMBER];
 
};

/**
 * Locked PWM flags (no boot).
 */
bool PwmController::lock_[PwmController::RESOURCES_NUMBER];

/**
 * CPU clock in Hz (no boot).
 */
int32 PwmController::sysclk_;

/**
 * System Control Registers (no boot).
 */  
SystemRegister* PwmController::regSys_;

/**
 * GPIO Registers (no boot).
 */  
GpioRegister* PwmController::regGpio_;

/**
 * Mutex of this driver (no boot).
 */  
Mutex* PwmController::drvMutex_;
  
/**
 * Driver has been initialized successfully (no boot).
 */
int32 PwmController::isInitialized_;

#endif // DRIVER_PWM_CONTROLLER_HPP_
