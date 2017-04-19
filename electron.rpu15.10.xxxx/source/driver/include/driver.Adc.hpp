/** 
 * Analog-to-Digital Converter factory.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_ADC_HPP_
#define DRIVER_ADC_HPP_

#include "driver.Types.hpp"

class Adc
{

public:

  /**
   * Error defenition.
   */
  static const int32 ERROR = -1;

  /**
   * The ADC sampling modes.
   */
  enum Mode
  {
    /**
     * The ADC sequential sampling mode.
     */
    SEQUENTIAL = 0,
    
    /**
     * The ADC simultaneous sampling mode by dual-sequencer.
     */
    SIMULTANEOUS_CASCADED = 1,    
      
    /**
     * The ADC simultaneous sampling mode by single-sequencer.
     */
    SIMULTANEOUS_DUAL = 2
    
  };
  
  /**
   *  Triggers as sources for the start of conversion sequence.
   */
  enum Trigger
  {
    /**
     * PWM SOCA start.
     */
    PWM_SOCA = 0,
    
    /**
     * PWM SOCB start.
     */
    PWM_SOCB = 1
    
  };

  /**
   * The TMS320F2833x ADC sequential channel numbers.
   */
  enum ChannelSequential
  {
    A0 = 0, A1 = 1, A2 = 2,  A3 = 3,  A4 = 4,  A5 = 5,  A6 = 6,  A7 = 7,
    B0 = 8, B1 = 9, B2 = 10, B3 = 11, B4 = 12, B5 = 13, B6 = 14, B7 = 15
  };
  
  /**
   * The TMS320F2833x ADC simultaneous channel numbers.
   */
  enum ChannelSimultaneous
  {
    A0B0 = 0, A1B1 = 1, A2B2 = 2, A3B3 = 3, A4B4 = 4, A5B5 = 5, A6B6 = 6, A7B7 = 7
  };
  
  /**
   * The PWM task interface.
   */  
  class TaskInterface
  {
  
  public:
  
    /**
     * Destructor.
     */  
    virtual ~TaskInterface(){}
    
    /**
     * Returns the number of sequences of sampling channels.
     *
     * @return the sequences number.
     */
    virtual int32 getSequencesNumber() const = 0;
    
    /**
     * Returns the number of sampling channels.
     *
     * @return the channels number.
     */
    virtual int32 getChannelsNumber() const = 0;
    
    /**
     * Returns the number of results in a channel
     *
     * @return the results number.
     */
    virtual int32 getResultsNumber() const = 0;
    
    /**
     * Returns an array of sampling channel numbers.
     *
     * @return the channel numbers array, or NULL if error has been occurred.
     */
    virtual const int32* getChannels() const = 0;

    /**
     * Returns a pointer to the first resualt of a free block.
     *
     * @return the free block first resualt, or NULL if no free has been.
     */
    virtual const int32* getFree() const = 0;
    
    /**
     * Returns a pointer to the first resualt of a full block.
     *
     * @return the full block first resualt, or NULL if no full has been.
     */
    virtual const int32* getFull() const = 0;
    
    /**
     * Sets first free block is full.
     */
    virtual void setFreeIsFull() = 0;
    
    /**
     * Sets first full block is free.
     */
    virtual void setFullIsFree() = 0;
    
    /**
     * Returns an index of the first free block.
     *
     * @return the first free block index, or -1 if no free has been.
     */
    virtual int32 getFreeIndex() const = 0;
    
    /**
     * Returns an index of the first full block.
     *
     * @return the first full block index, or -1 if no full has been.
     */
    virtual int32 getFullIndex() const = 0;

  }; 
  
  /**
   * The ADC task.
   *
   * @param BLOCKS    a number of blocks of internal circle buffer.
   * @param SEQUENCES a number of sequences of sampling channels.
   * @param CHANNELS  a number of sampling channels.
   * @param RESULTS   a number of results in a channel.
   */
  template <int32 BLOCKS, int32 SEQUENCES, int32 CHANNELS, int32 RESULTS>
  class Task : public TaskInterface
  {
  
  public:  

    /**
     * Constructor.
     *
     * @param channel an array of sampling channel numbers.
     */  
    Task(int32* channel) :
      free_     (0),
      full_     (0),
      isFilled_ (false){
      // Copy the channel array
      for(int32 i=0; i<CHANNELS; i++) 
        channel_[i] = channel[i];
      // Initialize default value of results
      for(int32 b=0; b<BLOCKS; b++) 
        for(int32 s=0; s<SEQUENCES; s++) 
          for(int32 c=0; c<CHANNELS; c++)               
            for(int32 r=0; r<RESULTS; r++) 
              result_[b][s][c][r] = 0;
      // Initialize illegal value of results
      for(int32 s=0; s<SEQUENCES; s++) 
        for(int32 c=0; c<CHANNELS; c++)               
          for(int32 r=0; r<RESULTS; r++) 
            illegal_[s][c][r] = -1;
    }      

    /**
     * Destructor.
     */  
    virtual ~Task(){}

    /**
     * Returns the number of sequences of sampling channels.
     *
     * @return the sequences number.
     */
    virtual int32 getSequencesNumber() const
    {
      return SEQUENCES;
    }
    
    /**
     * Returns the number of sampling channels.
     *
     * @return the channels number.
     */
    virtual int32 getChannelsNumber() const
    {
      return CHANNELS;    
    }
    
    /**
     * Returns the number of results in a channel
     *
     * @return the results number.
     */
    virtual int32 getResultsNumber() const
    {
      return RESULTS;
    }    
    
    /**
     * Returns an array of sampling channel numbers.
     *
     * @return the channel numbers array, or NULL if error has been occurred.
     */
    virtual const int32* getChannels() const
    {
      return channel_;
    }

    /**
     * Returns a pointer to the first resualt of a free block.
     *
     * @return the free block first resualt, or NULL if no free has been.
     */
    virtual const int32* getFree() const
    {
      return not isFilled_ ? &result_[free_][0][0][0] : NULL;
    }
    
    /**
     * Returns a pointer to the first resualt of a full block.
     *
     * @return the full block first resualt, or NULL if no full has been.
     */
    virtual const int32* getFull() const
    {
      return isFilled_ || full_ != free_ ? &result_[full_][0][0][0] : NULL;
    }
    
    /**
     * Sets first free block is full.
     */
    virtual void setFreeIsFull()
    {
      if( isFilled_ ) return;
      int32 free = free_ + 1;
      if(free == BLOCKS) free = 0;
      if(free == full_) isFilled_ = true;
      free_ = free;
    }
    
    /**
     * Sets first full block is free.
     */
    virtual void setFullIsFree()
    {
      if( not isFilled_ && full_ == free_ ) return;
      int32 full = full_ + 1;
      if(full == BLOCKS) full = 0;
      isFilled_ = false;
      full_ = full;
    }
    
    /**
     * Returns an index of the first free block.
     *
     * @return the first free block index, or -1 if no free has been.
     */
    virtual int32 getFreeIndex() const
    {
      return not isFilled_ ? free_ : -1;
      
    }    
    
    /**
     * Returns an index of the first full block.
     *
     * @return the first full block index, or -1 if no full has been.
     */
    virtual int32 getFullIndex() const
    {
      return isFilled_ || full_ != free_ ? full_ : -1;      
    }
    
    /**
     * Returns an array of sampled results.
     *
     * @return the results array, or NULL if error has been occurred.
     */
    const int32 (&operator[](int32 index) const)[SEQUENCES][CHANNELS][RESULTS]
    {
      return index < BLOCKS ? result_[index] : illegal_;
    }    
    
  private:
  
    /**
     * The list of sampling channels.
     */    
    int32 channel_[CHANNELS]; 
    
    /**
     * The result of sampled channels.
     */    
    int32 result_[BLOCKS][SEQUENCES][CHANNELS][RESULTS];
    
    /**
     * The illegal result of sampled channels.
     */    
    int32 illegal_[SEQUENCES][CHANNELS][RESULTS];    
    
    /**
     * First free block.
     */    
    int32 free_;
    
    /**
     * First filled block.
     */    
    int32 full_;
    
    /**
     * First filled block.
     */    
    bool isFilled_;    
    
  };
  
  /**
   * The ADC Sequence.
   */
  class Sequence
  {
  
  public:
  
    /**
     * Sets a sampling task of the ADC module.
     *
     * @param task a new task for sampling.
     * @return true if the task has been set successfully.
     */
    virtual bool setTask(TaskInterface& task) = 0;
    
    /**
     * Waits while sampling of task sequences will be completed.
     *
     * @return the index of completed task sequences block, or ERROR if error has been occurred.
     */
    virtual int32 wait() = 0;
    
    /**
     * Triggers software start of conversion sequence.
     *
     * @return true if the trigger has been successful.
     */
    virtual bool trigger() = 0;
    
    /**
     * Sets a trigger as source to start of conversion sequence.
     *
     * @param source a source for starting.
     * @return true if the trigger has been successful.
     */
    virtual bool setTrigger(int32 source) = 0;
    
    /**
     * Resets a trigger as source to start of conversion sequence.
     *
     * @param source a source for starting.
     */
    virtual void resetTrigger(int32 source) = 0;
     
  };
  
  /**
   * Returns a sequences number of the ADC.
   *
   * @return the sequences number.
   */
  virtual int32 getSequencesNumber() const = 0;  
  
  /**
   * Returns a ADC sequencer.
   *
   * @return the ADC sequencer resource.
   */  
  virtual ::Adc::Sequence& getSequence(int32 index) = 0;  
  
  /**
   * Returns ADC resolution.
   *
   * @return the ADC resolution in bits, or ERROR if error has been occurred.
   */  
  virtual int32 getResolution() const = 0;
  
  /**
   * Returns the ADC module mode.
   *
   * @return the argument mode passed to create method.
   */        
  virtual Mode getMode() const = 0;  
  
  /**
   * Returns the ADC clock frequency.
   *
   * The method returns ADC clock frequency, which was calculated and set 
   * while the object was constructing. This frequency based on 
   * real dividers of ADC module and may be differed form 
   * the value passed in the create method if needful dividers were not found.
   *
   * @return the clock frequency in Hz, or ERROR if error has been occurred.
   */        
  virtual int32 getClockFrequency() const = 0;   

  /**
   * Returns the driver resource interface.
   *
   * @param clock the desiring ADC clock frequency in Hz.
   * @param mode  the ADC sampling mode.
   * @return the ADC module driver interface, or null if error has been occurred.
   */
  static ::Adc* create(int32 clock, Mode mode);
   
  /**
   * Initializes the driver.
   *
   * Note: The CPU PLL Block must be initialized before
   * the driver initialization will be called. Otherwise,
   * the driver will have incorrectly calculated CPU frequency
   * if the PLL Block has been initialized after that.
   *
   * @param sourceClock the CPU oscillator source clock in Hz.
   * @return true if no errors.
   */   
  static bool init(int32 sourceClock);
  
  /**
   * Deinitializes the driver.
   */
  static void deinit();
  
};
#endif // DRIVER_ADC_HPP_
