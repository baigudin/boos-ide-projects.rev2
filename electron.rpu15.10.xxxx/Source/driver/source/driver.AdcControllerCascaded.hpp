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
      regAdc_->ctrl3.bit.smodeSel = 1;    
      // Cascaded mode. SEQ1 and SEQ2 operate as a single 16-state sequencer
      regAdc_->ctrl1.bit.seqCasc = 1;
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
      task_            (NULL),
      result000_       (NULL),
      sequencesLeft_   (0),      
      sequencesNumber_ (0),      
      channelsNumber_  (0),
      resultsNumber_   (0),
      sampleNumber_    (0){      
      setConstruct( false );
    }
  
    /**
     * Constructor.
     */   
    SequenceController(Mutexs& mutex) : Parent(mutex),
      task_            (NULL),
      result000_       (NULL),      
      sequencesLeft_   (0),      
      sequencesNumber_ (0),      
      channelsNumber_  (0),      
      resultsNumber_   (0),
      sampleNumber_    (0){
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
    virtual bool setTask(TaskInterface& task)
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
     * Triggers software start of conversion sequence.
     *
     * @return true if the trigger has been successful.
     */
    virtual bool trigger()
    {
      if( not isConstructed() ) return false;
      if( not mutex_->res.lock() ) return false;     
      bool res = false;
      do{
        if( task_ == NULL ) break;
        // Start task
        regAdc_->ctrl2.bit.socSeq1 = 1;        
        res = true;
      }while(false);      
      return mutex_->res.unlock(res);        
    }
  
    /**
     * The method with self context.
     */  
    virtual void handler()
    {
      do{
        if(task_ == NULL) break;
        if(sequencesLeft_ == 0) 
        {
          result000_ = task_->getFree();
          if(result000_ == NULL) break;
          sequencesLeft_ = sequencesNumber_;
        }
        register int32 index = (sequencesNumber_ - sequencesLeft_) * sampleNumber_;
        register int32* result = &result000_[index];
        for(int32 i=0; i<sampleNumber_; i++) 
          result[i] = regAdcDma_->result[i].val;
        if( --sequencesLeft_ == 0 )
          task_->setFreeIsFull();
      }while(false);
      // Reset sequencer to state CONV00
      regAdc_->ctrl2.bit.rstSeq1 = 1;
      // Clears the SEQ1 interrupt flag bit
      regAdc_->st.bit.intSeq1Clr = 1;
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
    bool registerTask(TaskInterface& task)
    {
      // Allow to set task only once
      if(task_ != NULL ) return false;
      sequencesNumber_ = task.getSequencesNumber();      
      channelsNumber_ = task.getChannelsNumber();      
      resultsNumber_ = task.getResultsNumber();
      if(channelsNumber_ < 1 || 8 < channelsNumber_) return false;
      if(resultsNumber_ != 2) return false; 
      sampleNumber_ = channelsNumber_ * resultsNumber_;     
      const int32* channel = task.getChannels();
      for(int32 i=0; i<channelsNumber_; i++)
      {
        int32 chn = channel[i];
        if(0 <= chn && chn <= 7)
        {
          registerChannel(i, channel[i]);
          continue;
        }
        return false;
      }
      regAdc_->maxconv.bit.maxConv1 = channelsNumber_ - 1;
      task_ = &task;
      return true;
    }
    
    /** 
     * Registers a channel.
     */  
    void registerChannel(int32 index, int32 channel)
    {
      int32 seq = (index >> 2) & 0x1;
      AdcRegister::Chselseq* reg = &regAdc_->chselseq[seq];
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
      // Create the ADC interrupt source resource
      int_ = Interrupt::create(*this, ADC_SEQ1INT);
      int_->enable();
      // Interrupt request by INT_SEQ1 is enabled
      regAdc_->ctrl2.bit.intEnaSeq1 = 1; 
      // INT_SEQ1 is set at the end of every SEQ1 sequence
      regAdc_->ctrl2.bit.intModSeq1 = 0;
      return int_ == NULL ? false : true;
    }

    /**
     * The sampling task.
     */
    TaskInterface* task_;

    /**
     * The first result value of the conversion.
     */
    int32* result000_;
    
    /**
     * The sequences number.
     */
    int32 sequencesLeft_;
    
    /**
     * The sequences number.
     */
    int32 sequencesNumber_;
    
    /**
     * The channels number for sampling.
     */
    int32 channelsNumber_;
    
    /**
     * The results number of a channel.
     */
    int32 resultsNumber_;
    
    /**
     * The samples number of a sampling.
     */
    int32 sampleNumber_;
              
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

