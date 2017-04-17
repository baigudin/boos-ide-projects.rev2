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
     * Returns a channels number for sampling.
     *
     * @return the channels number, or ERROR if error has been occurred.
     */
    virtual int32 getChannelsNumber() const = 0;
    
    /**
     * Returns an array of sampling channel numbers.
     *
     * @return the channel numbers array, or NULL if error has been occurred.
     */
    virtual int32* getChannels() = 0;
    
    /**
     * Returns an array of sampling results.
     *
     * @return the results array, or NULL if error has been occurred.
     */
    virtual int32* getResults() = 0;
    
    
  }; 
  
  /**
   * The ADC task.
   *
   * @param CHANNELS a number of channels in sequence.
   * @param PATHES     
   */
  template <int32 CHANNELS>
  struct TaskData
  {
    /**
     * The list of sampling channels.
     */    
    int32 channel[CHANNELS]; 
    
    /**
     * The result of sampled channels.
     */    
    int32 result[CHANNELS << 1];     

  };  
  
  /**
   * The ADC task.
   *
   * @param CHANNELS a number of channels in sequence.
   */
  template <int32 CHANNELS>  
  class Task : public TaskInterface
  {
  
  public:  

    /**
     * Constructor.
     */  
    Task(TaskData<CHANNELS>& data) :
      data_ (&data){
    }      

    /**
     * Destructor.
     */  
    virtual ~Task(){}
    
    /**
     * Returns a channels number for sampling.
     *
     * @return the channels number, or ERROR if error has been occurred.
     */
    virtual int32 getChannelsNumber() const
    {
      return CHANNELS;    
    }
    
    /**
     * Returns an array of sampling channel numbers.
     *
     * @return the channel numbers array, or NULL if error has been occurred.
     */
    virtual int32* getChannels()
    {
      return data_->channel;
    }    
    
    /**
     * Returns an array of sampling results.
     *
     * @return the results array, or NULL if error has been occurred.
     */
    virtual int32* getResults()
    {
      return data_->result;
    }    
    
  private:

    /**
     * The task.
     */    
    TaskData<CHANNELS>* data_;
    
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
     * Starts the sampling task of the ADC module.
     *
     * @return true if the task has been set successfully.
     */
    virtual bool startTask() = 0;
    
    /**
     * Waits a sampling result.
     *
     * @return true if the result has been sampled successfully.
     */
    virtual bool waitResult() = 0;
    
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
