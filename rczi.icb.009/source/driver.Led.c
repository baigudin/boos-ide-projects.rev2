/** 
 * The board LED driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "driver.Led.h"
#include "driver.Registers.h"

/**
 * The 1000-X led port mask.
 */
#define LED_X_PORT_MASK (0x01)

/**
 * The 1000-T led port mask.
 */
#define LED_T_PORT_MASK (0x02)

/**
 * The 1000-X led port.
 */
sbit ledX_ = REG_P1^0;

/**
 * The 1000-T led port.
 */
sbit ledT_ = REG_P1^1;

/**
 * The driver has been initialized successfully.
 */
static int8 isInitialized_;

/**
 * Switchs a led.
 *
 * @param led an chosen led.
 * @param on  true turns on, false turns off.
 */
void ledSwitch(enum Led led, int8 on)
{
  if(isInitialized_)
  {
    switch(led)
    {
      case LED_X:
      {
        ledX_ = on == 0 ? 0 : 1;
      }
      break;
      case LED_T:
      {
        ledT_ = on == 0 ? 0 : 1;      
      }
      break;
      default:
      {
      }
      break;    
    }
  }
}

/**
 * Tests if a led is turned on.
 *
 * @param led an chosen led.
 * @return true if a led is turned on.
 */
int8 ledIsSwitchedOn(enum Led led)
{
  int8 state;
  if(isInitialized_)
  {
    switch(led)
    {
      case LED_X:
      {
        state = ledX_ == 0 ? 0 : 1;
      }
      break;
      case LED_T:
      {
        state = ledT_ == 0 ? 0 : 1;        
      }
      break;
      default:
      {
        state = 0;
      }
      break;    
    }
  }
  else
  {
    state = 0;    
  }
  return state;  
}

/**
 * Toggles a led.
 *
 * @param led an chosen led.
 */
void ledToggle(enum Led led)
{
  if( ledIsSwitchedOn(led) )
  {
    ledSwitch(led, 0);
  }
  else
  {
    ledSwitch(led, 1);
  }
}

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
int8 ledInit(void)  
{
  isInitialized_ = 0;
  /* Set the reset port is push-pull */
  REG_P1MDOUT |= LED_X_PORT_MASK | LED_T_PORT_MASK;
  /* Set reset port is skipped by the Crossbar and used for GPIO */
  REG_P1SKIP |= LED_X_PORT_MASK | LED_T_PORT_MASK;
  /* Enable Crossbar for using port pins as digital outputs */
  REG_XBR2 |= 0x40;
  ledX_ = 0;
  ledT_ = 0;
  isInitialized_ = 1;  
  return BOOS_OK;
}