/** 
 * KSZ9031RNX Gigabit Ethernet Transciver driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef DRIVER_LED_H_
#define DRIVER_LED_H_

#include "boos.Constants.h"

/**
 * The led selection.
 */
enum Led
{
  /**
   * 1000Base-X led.
   */
  LED_X = 0x00,

  /**
   * 1000Base-T led.
   */
  LED_T = 0x01
  
};

/**
 * Switchs a led.
 *
 * @param led an chosen led.
 * @param on  true turns on, false turns off.
 */
extern void ledSwitch(enum Led led, int8 on);

/**
 * Tests if a led is turned on.
 *
 * @param led an chosen led.
 * @return true if a led is turned on.
 */
extern int8 ledIsSwitchedOn(enum Led led);

/**
 * Toggles a led.
 *
 * @param led an chosen led.
 */
extern void ledToggle(enum Led led);

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
extern int8 ledInit(void);

#endif /* DRIVER_LED_H_ */
