/** 
 * TI TMS320F2833x DSP Sequential Analog-to-Digital Converter controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_ADC_CONTROLLER_HPP_
#define DRIVER_ADC_CONTROLLER_HPP_

#include "driver.Object.hpp"
#include "driver.Adc.hpp"
#include "driver.AdcRegister.hpp"
#include "driver.System.hpp"
#include "driver.SystemRegister.hpp"
#include "driver.GpioRegister.hpp"
#include "driver.Mutex.hpp"
#include "driver.Interrupt.hpp"

class AdcController : public ::Object, public ::Adc
{
  typedef ::Object Parent;
  
public:

  /**
   * Constructor of the driver resource.
   *
   * @param clock the desiring ADC clock frequency in Hz.
   */   
  AdcController(int32 clock) : Parent(),
    regAdc_     (NULL),
    index_      (0),
    hspclk_     (0),
    adcclk_     (0),
    mutex_      (){
    setConstruct( construct(clock) );
  }

  /** 
   * Destructor.
   */                               
  virtual ~AdcController()
  {
    if( not isConstructed() ) return;
    if( not mutex_.drv.lock() ) return;
    // Disable ADC clock 
    regSys_->pclkcr0.bit.adcenclk = 0;    
    // Power down the bandgap and reference circuitry inside the analog core
    regAdc_->adctrl3.bit.adcbgrfdn = 0x0;
    // Power down the the analog circuitry inside the analog core
    regAdc_->adctrl3.bit.adcpwdn = 0x0;      
    regAdc_ = NULL;
    mutex_.drv.unlock();
  }
  
  /**
   * Returns ADC resolution.
   *
   * @return the ADC resolution in bits, or ERROR if error has been occurred.
   */  
  virtual int32 getResolution() const
  {
    return 12;
  }
  
  /**
   * Returns the ADC clock frequency.
   *
   * @return the clock frequency, or ERROR if error has been occurred.
   */        
  virtual int32 getClockFrequency() const
  {
    return isConstructed() ? adcclk_ : ERROR;
  }
  
  /**
   * Initializes the driver.
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
    if(drvMutex_ == NULL ||  not drvMutex_->isConstructed() ) return false;
    // Create register maps
    regSys_ = new (SystemRegister::ADDRESS) SystemRegister();
    // Calculate SYSCLK
    sysclk_ = getCpuClock(sourceClock);
    if(sysclk_ <= 0) return false;
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
    isInitialized_ = 0;
    if(drvMutex_ != NULL) delete drvMutex_;
    for(int32 i=0; i<RESOURCES_NUMBER; i++) lock_[i] = false;
  }

private:  
  
  /** 
   * Constructor.
   *
   * @param clock the desiring ADC clock frequency in Hz.   
   * @return boolean result.
   */  
  bool construct(int32 clock)
  {
    bool res;  
    if(isInitialized_ != IS_INITIALIZED) return false;
    if( not mutex_.drv.isConstructed() ) return false;    
    if( not mutex_.res.isConstructed() ) return false;
    if( not mutex_.drv.lock() ) return false;    
    res = false;
    do{
      if(lock_[index_] == true) break;
      if(index_ < 0 || index_ >= RESOURCES_NUMBER) break;
      // Calculate High-Speed Peripheral Clock Prescaler (HISPCP) divider    
      uint16 hsp, adp;
      int32 div;
      div = sysclk_ / clock;
      if(div < 1)
      {
         break;
      }
      else if(1 <= div && div < 2) 
      {
        div = 1;
        hsp = 0;
      }
      else if(2 <= div && div < 4) 
      {
        div = 2;
        hsp = 1;      
      }
      else if(4 <= div && div < 6) 
      {
        div = 4;
        hsp = 2;      
      }    
      else if(6 <= div && div < 8) 
      {
        div = 6;
        hsp = 3;      
      }
      else if(8 <= div && div < 10) 
      {
        div = 8;
        hsp = 4;      
      }
      else if(10 <= div && div < 12) 
      {
        div = 10;
        hsp = 5;      
      }    
      else if(12 <= div && div < 14)
      {
        div = 12;
        hsp = 6;      
      }
      else
      {
        div = 14;
        hsp = 7;      
      }
      hspclk_ = sysclk_ / div;
      // Calculate ADC Core clock divider    
      div = hspclk_ / clock;
      if(div < 1) break;
      adp = div >> 1 & 0xf;
      adcclk_ = hspclk_ / div;
      // Create ADC register
      regAdc_ = new (AdcRegister::ADDRESS) AdcRegister();      
      System::eallow();
      // Enable ADC clock
      regSys_->pclkcr0.bit.adcenclk = 1;
      // Calibrate the ADC here is IMPORTANT
      calibrate();
      // Set ADC HISPCP
      regSys_->hispcp.bit.hspclk = hsp;
      // Emulation suspend is ignored
      regAdc_->adctrl1.bit.susmod = 0;
      // Power up the bandgap and reference circuitry inside the analog core
      regAdc_->adctrl3.bit.adcbgrfdn = 0x3;
      // Power up the the analog circuitry inside the analog core
      regAdc_->adctrl3.bit.adcpwdn  = 0x1; 
      sleep(5);
      // Set ADC Core    
      regAdc_->adctrl3.bit.adcclkps = adp;
      // Set ADC Core clock prescaler always is dividing to 1
      regAdc_->adctrl1.bit.cps = 0;
      // Set SOC pulse width is 1 ADCLK periods
      regAdc_->adctrl1.bit.acqPs = 0x0;
      System::dallow();
      lock_[index_] = true;    
      res = true;
    }while(false);
    return mutex_.drv.unlock(res);
  }
  
  /** 
   * Causes to sleep.
   *     
   * @param millis a time to sleep in milliseconds..    
   */  
  static void sleep(int32 millis)
  {  
    int32 max = 0x29f1 * millis;
    for(int32 i=0; i<max; i++);
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
   * Copies device specific calibration data into ADCREFSEL and ADCOFFTRIM registers.
   *
   * Note: FAILURE TO INITIALIZE THESE REGISTERS WILL CAUSE THE ADC TO FUNCTION OUT OF SPECIFICATION.
   * Because TI reserved OTP memory is secure, the ADC_Cal() routine must be called from
   * secure memory or called from non-secure memory after the Code Security Module is
   * unlocked. If the system is reset or the ADC module is reset using Bit 14 (RESET) from the
   * ADC Control Register 1, the routine must be repeated.
   */  
  static void calibrate();
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  AdcController(const AdcController& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  AdcController& operator =(const AdcController& obj);    
  
protected:    

  /**
   * Mutexs of the driver and the resource.
   */
  struct Mutexs
  {
    /** 
     * Constructor.
     */    
    Mutexs() :
      drv (*drvMutex_),
      res (*drvMutex_){
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
    Mutex& res;

  };
  
  /**
   * The ADC Sequence.
   */
  class SequenceController : public ::Object, public ::Adc::Sequence, public ::InterruptTask
  {
    typedef ::Object Parent;  
  
  public:
  
    /**
     * Available interrupt sources.
     */
    enum Source 
    {
      ADC_SEQ1INT = 0x0000,
      ADC_SEQ2INT = 0x0010,
      ADC_ADCINT  = 0x0050
    };
  
    /**
     * Constructor.
     */   
    SequenceController() : Parent(),
      regAdc_ (NULL),
      mutex_  (NULL),
      int_    (NULL){
      setConstruct( false );
    }  
  
    /**
     * Constructor.
     */   
    SequenceController(Mutexs& mutex) : Parent(),
      regAdc_ (NULL),
      mutex_  (&mutex){
      setConstruct( construct() );
    }
    
    /** 
     * Destructor.
     */                               
    virtual ~SequenceController()
    {
    }
    
    /**
     * Tests if this object has been constructed.
     *
     * @return true if object has been constructed successfully.
     */    
    virtual bool isConstructed() const
    {
      return this->Parent::isConstructed();
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
      // Create ADC register
      regAdc_ = new (AdcRegister::ADDRESS) AdcRegister();
      regAdcDma_ = new (AdcDmaRegister::ADDRESS) AdcDmaRegister();            
      return true;
    }     
    
  protected:
  
    /**
     * Analog-to-Digital Converter Control Registers.
     */  
    AdcRegister* regAdc_;

    /**
     * Analog-to-Digital Converter Control Registers.
     */      
    AdcDmaRegister* regAdcDma_;
    
    /**
     * The driver and the resource mutexs.
     */  
    Mutexs* mutex_;
    
    /**
     * The interrupt driver.
     */  
    Interrupt* int_;    
  };  

  /**
   * Number of ADC modules.
   */
  static const int32 RESOURCES_NUMBER = 1;
  
  /**
   * PWM driver initialized falg value.
   */
  static const int32 IS_INITIALIZED = 0x12784565;
  
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
   * Mutex of this driver (no boot).
   */  
  static Mutex* drvMutex_;
  
  /**
   * Driver has been initialized successfully (no boot).
   */
  static int32 isInitialized_;
  
  /**
   * Analog-to-Digital Converter Control Registers.
   */  
  AdcRegister* regAdc_;  

  /**
   * The index of ADC resource.
   */
  int32 index_; 
  
  /**
   * High-speed peripheral clock in Hz (no boot).
   */
  int32 hspclk_;  
  
  /**
   * The ADC resource clock in Hz (no boot).
   */
  int32 adcclk_;
  
  /**
   * The driver and the resource mutexs.
   */  
  Mutexs mutex_;  

};

/**
 * Locked PWM flags (no boot).
 */
bool AdcController::lock_[AdcController::RESOURCES_NUMBER];

/**
 * CPU clock in Hz (no boot).
 */
int32 AdcController::sysclk_;

/**
 * System Control Registers (no boot).
 */  
SystemRegister* AdcController::regSys_;

/**
 * Mutex of this driver (no boot).
 */  
Mutex* AdcController::drvMutex_;
  
/**
 * Driver has been initialized successfully (no boot).
 */
int32 AdcController::isInitialized_;

#endif // DRIVER_ADC_CONTROLLER_HPP_

