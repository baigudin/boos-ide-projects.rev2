/** 
 * Pulse Width Modulator driver factory.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_PWM_HPP_
#define DRIVER_PWM_HPP_

#include "driver.Types.hpp"

class Pwm
{

public:

  /**
   * Error defenition.
   */
  static const int32 ERROR = -1;

  /**
   * The PWM counting policy.
   */
  enum Count
  {
    /**
     * The PWM will count up and after down.
     */
    UPDOWN = 0,
      
    /**
     * The PWM will count up.
     */
    UP = 1,
    
    /**
     * The PWM will count down.
     */
    DOWN = 2
    
  }; 
  
  /**
   * The PWM synchronization events.
   */  
  enum Sync
  {
    /**
     * The PWM synchronizes next PWM by prev PWM impulse.
     */
    INPUT = 0,
      
    /**
     * The PWM synchronizes next PWM if new period is occurred.
     */
    PERIOD = 1,
    
    /**
     * The PWM synchronizes next PWM if an edge of pulse is occurred.
     */
    EDGE = 2,
    
    /**
     * The PWM does not synchronize next PWM.
     */
    DISABLE = 3

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
     * Returns a channels number of a task.
     *
     * @return the channels number, or ERROR if error has been occurred.
     */
    virtual int32 getChannelsNumber() const = 0;

    /**
     * Returns the frequency of a task.
     *
     * @return the task frequency in Hz, or ERROR if error has been occurred.
     */
    virtual int32 getFrequency() const = 0;    

    /**
     * Returns the channel duty of a task.
     *
     * @param channel a channel number.          
     * @return the task channel duty in percentages, or ERROR if error has been occurred.
     */    
    virtual float32 getDuty(int32 channel) const = 0;
    
  };
  
  /**
   * The PWM task.
   *
   * @param CHANNELS a number of PWM channels.
   */
  template <int32 CHANNELS>  
  struct TaskData
  {
    /**
     * The next step frequency in Hz.
     */
    int32 frequency;
    
    /**
     * The next step duty in percentages.
     */    
    float32 duty[CHANNELS]; 

  };

  /**
   * The PWM switching task.
   *
   * @param CHANNELS a number of PWM channels.
   */
  template <int32 CHANNELS>  
  class Task : public TaskInterface
  {
  
  public:  
  
    /**
     * Constructor.
     */  
    Task()
    {
      task_.frequency = ERROR;
      for(int32 i=0; i<CHANNELS; i++) task_.duty[i] = ERROR;
    }    

    /**
     * Constructor.
     */  
    Task(const TaskData<CHANNELS>& obj)
    {
      task_.frequency = obj.frequency;
      for(int32 i=0; i<CHANNELS; i++) task_.duty[i] = obj.duty[i];
    }      
  
    /**
     * Constructor.
     */      
    Task(const Task<CHANNELS>& obj)
    {
      task_.frequency = obj.task_.frequency;
      for(int32 i=0; i<CHANNELS; i++) task_.duty[i] = obj.task_.duty[i];
    }  
    
    /**
     * Constructor.
     */      
    Task(const TaskInterface& obj)
    {
      task_.frequency = obj.getFrequency();
      for(int32 i=0; i<CHANNELS; i++) task_.duty[i] = obj.getDuty(i);
    }    

    /**
     * Destructor.
     */  
    virtual ~Task(){}
    
    /**
     * Returns a channels number of a task.
     *
     * @return the channels number, or ERROR if error has been occurred.
     */
    virtual int32 getChannelsNumber() const
    {
      return CHANNELS;
    }

    /**
     * Returns the frequency of a task.
     *
     * @return the task frequency in Hz, or ERROR if error has been occurred.
     */
    virtual int32 getFrequency() const
    {
      return task_.frequency;
    }

    /**
     * Returns the channel duty of a task.
     *
     * @param channel a channel number.          
     * @return the task channel duty in percentages, or ERROR if error has been occurred.
     */    
    virtual float32 getDuty(int32 channel) const
    {
      return channel < CHANNELS ? task_.duty[channel] : ERROR;    
    }
    
    /** 
     * Assignment operator.
     *
     * @param obj reference to source task.
     * @return reference to this object.     
     */     
    Task& operator =(const TaskData<CHANNELS>& obj)
    {
      task_.frequency = obj.frequency;
      for(int32 i=0; i<CHANNELS; i++) task_.duty[i] = obj.duty[i];
      return *this;
    }    
    
    /** 
     * Assignment operator.
     *
     * @param obj reference to source task.
     * @return reference to this object.     
     */     
    Task& operator =(const TaskInterface& obj)
    {
      task_.frequency = obj.getFrequency();
      for(int32 i=0; i<CHANNELS; i++) task_.duty[i] = obj.getDuty(i);
      return *this;
    }      
         
  private:

    /**
     * The task.
     */    
    TaskData<CHANNELS> task_;
    
  };
  
  /**
   * Some toggling keys interface.
   */  
  class Toggle
  {

  public:
    
    /** 
     * Destructor.
     */                               
    virtual ~Toggle(){}
    
    /** 
     * Disables the key.
     *
     * @return the key bit value before the method was called.
     */ 
    virtual bool disable() = 0;
    
    /** 
     * Enables the key.
     *
     * The true passed argument directly turns the key on, 
     * and the false does nothing, the key stays in the current state.
     * 
     * @param status returned status by disable method, or true for directly turning the key on.
     */    
    virtual void enable(bool status=true) = 0;

  };
   
  /**
   * The PWM Dead-Band Generator Submodule.
   */  
  class DeadBand
  {
  
  public:
  
    /**
     * Destructor.
     */  
    virtual ~DeadBand(){}
    
    /**
     * Sets rising-edge delay.
     *
     * The method sets new rising-edge delay.     
     * If passing argument value is more than hardware can contain, 
     * maximum hardware value will be set, but no calculated return value 
     * will always equal this setting value.
     *
     * @param time delay time in ns.
     */  
    virtual void setRisingDelay(float32 time) = 0;    
    
    /**
     * Returns the PWM signal rising-edge delay.
     *
     * The method returns PWM signal rising-edge delay of current task. 
     * This delay might be returned as passed value in the set method
     * or calculated basing on current values of PWM module registers 
     * if the passed argument is set to true.
     *
     * @param set flag allows getting the set or calculated value.
     * @return the rising-edge delay in ns, or ERROR if error has been occurred.     
     */  
    virtual float32 getRisingDelay(bool set=false) = 0;    
    
    /**
     * Sets falling-edge delay.
     *
     * The method sets new falling-edge delay.
     * If passing argument value is more than hardware can contain, 
     * maximum hardware value will be set, but no calculated return value 
     * will always equal this setting value.
     *
     * @param time delay time in ns.
     */  
    virtual void setFallingDelay(float32 time) = 0;
    
    /**
     * Returns the PWM signal falling-edge delay.
     *
     * The method returns PWM signal falling-edge delay of current task. 
     * This delay might be returned as passed value in the set method
     * or calculated basing on current values of PWM module registers 
     * if the argument is set to true.
     *
     * @param set flag allows getting the set or calculated value.
     * @return the falling-edge delay in ns, or ERROR if error has been occurred.     
     */  
    virtual float32 getFallingDelay(bool set=false) = 0;  
    
    /**
     * Retruns in-key for a channel.
     *
     * @param channel available dead-band signal channel number.
     * @param the toggling key.
     */  
    virtual ::Pwm::Toggle& getInput(int32 channel) = 0;

    /**
     * Retruns out-key for a channel.
     *
     * @param channel available dead-band signal channel number.
     * @param the toggling key.     
     */  
    virtual ::Pwm::Toggle& getOutput(int32 channel) = 0;

    /**
     * Retruns polarity-key for a channel.
     *
     * @param channel available dead-band signal channel number.
     * @param the toggling key.     
     */  
    virtual ::Pwm::Toggle& getPolarity(int32 channel) = 0;
    
  };
  
  /**
   * The PWM Chopper Submodule.
   */  
  class Chopper
  {
  
  public:
  
    /**
     * Destructor.
     */  
    virtual ~Chopper(){}
    
    /**
     * Sets new frequency of chopping signal.
     *
     * The method sets chopping signal frequency. If the passed frequency is 
     * less or more than PWM Chopper Submodule can generate, minimal or maximal 
     * available frequency must be set.
     *
     * @param frequency new chopping signal frequency in Hz.
     */
    virtual void setFrequency(int32 frequency) = 0;    
    
    /**
     * Returns the frequency of chopping signal.
     *
     * The method returns the chopping signal frequency. 
     * This frequency might be returned as passed value in the set method 
     * or calculated basing on current values of PWM module registers 
     * if the argument is set to true.
     *
     * @param set flag allows getting the set or calculated value.     
     * @return the chopping signal frequency in Hz, or ERROR if error has been occurred.
     */
    virtual int32 getFrequency(bool set=false) const = 0;
    
    /**
     * Sets the chopper duty.
     *
     * The method sets chopping signal duty. If the passed duty is 
     * less or more than PWM Chopper Submodule can set, minimal or maximal 
     * available duty must be set.
     *
     * @param duty a chopping duty value in percentages.
     */        
    virtual void setDuty(float32 duty) = 0;    
    
    /**
     * Returns the chopper duty.
     *
     * The method returns the chopping signal duty. 
     * This duty might be returned as passed value in the set method 
     * or calculated basing on current values of PWM module registers 
     * if the argument is set to true.
     *
     * @param set flag allows getting the set or calculated value.
     * @return the chopping duty value in percentages, or ERROR if error has been occurred.
     */        
    virtual float32 getDuty(bool set=false) const = 0;
    
    /**
     * Sets new width of the first pulse.
     *
     * The method sets width of the first pulse. If the passed time of the width is 
     * less or more than PWM Chopper Submodule can set, minimal or maximal 
     * available width must be set.
     *
     * @param time new first pulse width in ns.
     */
    virtual void setFirstPulse(int32 time) = 0;    
    
    /**
     * Returns the width of the first pulse.
     *
     * The method returns the width of the first pulse. 
     * This width might be returned as passed value in the set method 
     * or calculated basing on current values of PWM module registers 
     * if the argument is set to true.
     *     
     * @param set flag allows getting the set or calculated value.     
     * @return the first pulse width in ns, or ERROR if error has been occurred.
     */
    virtual int32 getFirstPulse(bool set=false) const = 0;
    
    /**
     * Retruns the chopper enable key.
     *
     * @param an enable key.
     */  
    virtual ::Pwm::Toggle& getChopping() = 0;
    
  };

  /** 
   * Destructor.
   */                               
  virtual ~Pwm(){}
  
  /**
   * Starts generating the tasking wave of the PWM module.
   *
   * Note: The method has to copy the passed task into internal structure data.
   * Thus, when the method has been called, the passing task object might be deleted.
   *
   * @param task a new task for generating.
   * @return true if the task has been started successfully.
   */
  virtual bool start(const TaskInterface& task) = 0;
  
  /**
   * Stops generating the wave of the PWM module.
   */      
  virtual void stop() = 0;   
  
  /**
   * Returns the PWM module index.
   *
   * @return argument index passed to create method, or ERROR if error has been occurred.
   */      
  virtual int32 getIndex() const = 0;

  /**
   * Returns a number of available PWM channels.
   *
   * @return a channels number, or ERROR if error has been occurred.
   */
  virtual int32 getChannelsNumber() const = 0;
  
  /**
   * Returns calculated resolution of PWM channel.
   *
   * @param channel available number of channel.   
   * @return the channel resolution in bits, or ERROR if error has been occurred.
   */  
  virtual float32 getResolution(int32 channel) const = 0;

  /**
   * Enables the PWM channels in high resolution mode.
   *
   * The method only enables high resolution mode and does not restart stared task.
   * Thus, only the next started task will be executed in high resolution.
   *
   * @return true if high resolution mode is set successfully.
   */        
  virtual bool enableHighResolution() = 0;
  
  /**
   * Disables the PWM channels in high resolution mode.
   */        
  virtual void disableHighResolution() = 0;
  
  /**
   * Returns the PWM module counting policy.
   *
   * @return the argument count passed to create method.
   */        
  virtual Count getCountPolicy() const = 0;  
  
  /**
   * Returns the PWM signal frequency of currently assigned task.
   *
   * The method returns PWM signal frequency of current task. 
   * This frequency might be returned as value passed in the task 
   * or calculated basing on current values of PWM module registers 
   * if the argument is set to true.
   *
   * @param set flag allows getting the set or calculated value.
   * @return the signal frequency in Hz, or ERROR if error has been occurred.
   */        
  virtual int32 getSignalFrequency(bool set=false) const = 0;  
  
  /**
   * Returns the PWM clock frequency.
   *
   * The method returns PWM clock frequency, which was calculated and set 
   * while the object was constructing. This frequency based on 
   * real dividers of concrete PWM module and may be differed form 
   * the value passed in the create method if needful dividers were not found.
   *
   * @return the clock frequency in Hz, or ERROR if error has been occurred.
   */        
  virtual int32 getClockFrequency() const = 0;  
  
  /**
   * Returns the PWM module channel duty of currently assigned task.
   *
   * @param channel available number of channel.
   * @param set     flag allows getting the set or calculated value.
   * @return the channel duty in percentages, or ERROR if error has been occurred.
   */        
  virtual float32 getDuty(int32 channel, bool set=false) const = 0;
  
  /**
   * Synchronizes the PWM module with a PWM module later in a synchronization chain.
   *
   * @return true if the synchronization has been successfully completed.
   */      
  virtual bool synchronize() = 0;
  
  /**
   * Tests if the PWM module may synchronize next PWM module in a chain.
   *
   * @return true if the module synchronizes the next module.
   */      
  virtual bool isSynchronizing() const = 0;
  
  /**
   * Sets a synchronization event of the PWM module.
   *
   * @param event a synchronization event.
   */      
  virtual void setSynchronization(Sync event) = 0;

  /**
   * Returns the synchronization event of the PWM module.   
   *
   * @return the synchronization event.
   */      
  virtual Sync getSynchronization() const = 0; 
  
  /**
   * Sets the PWM module phase shift time.
   *
   * @param time the PWN signals shift time during synchronization in ns.
   */      
  virtual void setPhase(int32 time) = 0;

  /**
   * Returns the PWM module phase shift time.
   *
   * @return the PWN signals shift time during synchronization in ns.
   */      
  virtual int32 getPhase() const = 0;
  
  /**
   * Returns the PWM module phase enabling key.
   *
   * @param the phase enabling key.     
   */      
  virtual ::Pwm::Toggle& getPhaseKey() = 0;
  
  /**
   * Sets the PWM module counter direction after a synchronization.
   *
   * The method should be used when the PWM has been configured in 
   * the up-down-count mode. And UPDOWN count mode will be ignored.
   *
   * @param count new direction value.
   */      
  virtual void setPhaseDirection(Count count) = 0;

  /**
   * Returns the PWM module counter direction after a synchronization.
   *
   * @return the direction value.
   */      
  virtual Count getPhaseDirection() const = 0;  
  
  /**
   * Tests if the PWM module has dead-band submodule.
   *
   * @return true if the module has dead-band submodule.
   */      
  virtual bool isDeadBanded() const = 0;
  
  /**
   * Returns a dead-band submodule interface.
   *
   * @return reference to the dead-band submodule interface.
   */      
  virtual ::Pwm::DeadBand& getDeadBand() = 0;
  
  /**
   * Tests if the PWM module has chopper submodule.
   *
   * @return true if the module has chopper submodule.
   */      
  virtual bool isChopped() const = 0;
  
  /**
   * Returns a chopper submodule interface.
   *
   * @return reference to the chopper submodule interface.
   */      
  virtual ::Pwm::Chopper& getChopper() = 0;  
  
  /**
   * Returns the driver resource interface.
   *
   * @param clock the desiring PWM clock frequency in Hz.      
   * @param number a number of a PWM module.
   * @param count a counting policy of the PWM module behavior.   
   * @return the PWM module driver interface, or null if error has been occurred.
   */
  static ::Pwm* create(int32 clock, int32 number, Count count);
   
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
#endif // DRIVER_PWM_HPP_
