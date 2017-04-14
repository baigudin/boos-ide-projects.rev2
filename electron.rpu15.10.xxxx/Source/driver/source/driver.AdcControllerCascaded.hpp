/** 
 * TI TMS320F2833x DSP Simultaneous Cascaded Analog-to-Digital Converter controller.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_ADC_CONTROLLER_CASCADED_HPP_
#define DRIVER_ADC_CONTROLLER_CASCADED_HPP_

#include "driver.AdcController.hpp"

class AdcControllerCascaded : public ::AdcController
{
  typedef ::AdcController  Parent;
  
public:

  /**
   * Constructor of the driver resource.
   *
   * @param clock the desiring ADC clock frequency in Hz.   
   */   
  AdcControllerCascaded(int32 clock) : Parent(clock)
  {
    setConstruct( construct() );
  }

  /** 
   * Destructor.
   */                               
  virtual ~AdcControllerCascaded()
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
    return Adc::SIMULTANEOUS_CASCADED;  
  }
 
  /**
   * Returns a sequences number of the ADC.
   *
   * @return the sequences number.
   */
  virtual int32 getSequencesNumber() const
  {
    return SEQUENCES_NUMBER;
  }
  
  /**
   * Returns a ADC sequencer.
   *
   * @return the ADC sequencer resource.
   */  
  virtual ::Adc::Sequence& getSequence(int32 index)
  {
    if( not isConstructed() ) return seq_[ILLEGAL_SEQ_INDEX];
    return 0 <= index && index < SEQUENCES_NUMBER ? seq_[index] : seq_[ILLEGAL_SEQ_INDEX];
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
    if( not mutex_.drv.lock() ) return false;    
    bool res = false;
    do{
      // Set simultaneous sampling mode
      regAdc_->adctrl3.bit.smodeSel = 1;    
      // Cascaded mode. SEQ1 and SEQ2 operate as a single 16-state sequencer
      regAdc_->adctrl1.bit.seqCasc = 1;
      // Sequences initialization
      bool isSeq = true;
      for(int32 i=0; i<SEQUENCES_NUMBER; i++)
      {
        // Index of this loop is value of DBCTL register shift for current key
        SequenceController* seq = new (&seq_[i]) SequenceController(mutex_);
        if(seq != NULL && seq->isConstructed()) continue;
        isSeq = false;
        break;
      }
      if( not isSeq ) break;
      // Completing
      res = true;
    }while(false);
    return mutex_.drv.unlock(res);    
  }
  
  /**
   * Copy constructor.
   *
   * @param obj reference to source object.
   */
  AdcControllerCascaded(const AdcControllerCascaded& obj);

  /**
   * Assignment operator.
   *
   * @param obj reference to source object.
   * @return reference to this object.   
   */
  AdcControllerCascaded& operator =(const AdcControllerCascaded& obj);    
  
  /**
   * The ADC Sequence.
   */
  class SequenceController : public ::AdcController::SequenceController
  {
    typedef ::AdcController::SequenceController Parent;  
  
  public:
  
    /**
     * Constructor.
     */   
    SequenceController() : Parent(),
      task_ (NULL){
      setConstruct( false );
    }
    
  
    /**
     * Constructor.
     */   
    SequenceController(Mutexs& mutex) : Parent(mutex),
      task_ (NULL){
      setConstruct( construct() );
    }
    
    /** 
     * Destructor.
     */                               
    virtual ~SequenceController()
    {
    }
  
    /**
     * Sets a sampling task of the ADC module.
     *
     * @param task a new task for sampling.
     * @return true if the task has been set successfully.
     */
    virtual bool setTask(const TaskInterface& task)
    {
      if( not isConstructed() ) return false;
      if( not mutex_->res.lock() ) return false;     
      bool res = false;
      do{
        if( not registerTask(task) ) break;
        res = true;
      }while(false);      
      return mutex_->res.unlock(res);        
    }
    
    /**
     * Starts the sampling task of the ADC module.
     *
     * @return true if the task has been set successfully.
     */
    virtual bool startTask()
    {
      if( not isConstructed() ) return false;
      if( not mutex_->res.lock() ) return false;     
      bool res = false;
      do{
        if( task_ == NULL ) break;
        regAdc_->adctrl2.bit.socSeq1 = 1;        
        res = true;
      }while(false);      
      return mutex_->res.unlock(res);        
    }
    
    /**
     * Waits a sampling result.
     *
     * @return true if the result has been sampled successfully.
     */
    virtual bool waitResult()
    {
      return false;
    }
    
    /**
     * Operator new.
     *
     * @param size unused.
     * @param ptr  address of memory.
     * @return address of memory.
     */     
    void* operator new(uint32, SequenceController* ptr)
    {
      return reinterpret_cast<void*>(ptr);
    }     
    
  private:
  
    /**
     * Registers a task.
     *
     * @param task a task.
     * @return true if the task has been registered.
     */
    bool registerTask(const TaskInterface& task)
    {
      int32 number = task.getChannelsNumber();
      if(number < 1 || number > CHANNELS_NUMBER) return false;
      const int32* channel = task.getChannels();
      for(int32 i=0; i<number; i++)
      {
        int32 chn = channel[i];
        if(0 <= chn && chn <= 7)
        {
          registerChannel(i, channel[i]);
          continue;
        }
        return false;
      }
      regAdc_->adcmaxconv.bit.maxConv1 = number - 1;
      task_ = &task;      
      return true;    
    }
    
    /** 
     * Registers a channel.
     */  
    void registerChannel(int32 index, int32 channel)
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
     * Constructor.
     *
     * @return boolean result.
     */  
    bool construct()
    { 
      if( not isConstructed() ) return false;
      return true;
    }
    
    /**
     * Number of ADC module channels.
     */
    static const int32 CHANNELS_NUMBER = 8;

    /**
     * The sampling task.
     */    
    const TaskInterface* task_;
      
  };
  
  /**
   * Number of ADC module sequences.
   */
  static const int32 SEQUENCES_NUMBER = 1;
  
  /**
   * Illegal sequencer index.
   */
  static const int32 ILLEGAL_SEQ_INDEX = 1;    
  
  /**
   * Sequence controller.
   */  
  SequenceController seq_[SEQUENCES_NUMBER + 1];
     
};
#endif // DRIVER_ADC_CONTROLLER_CASCADED_HPP_

