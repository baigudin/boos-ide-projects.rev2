/** 
 * KSZ9031RNX Gigabit Ethernet Transciver driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef DRIVER_LED_H_
#define DRIVER_LED_H_

#include "boos.constants.h"

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
 * @param led an chose led.
 * @param on  true turns on, false turns off.
 */
extern void ledToggle(enum Led led, int8 on);

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
extern int8 ledInit(void);

#endif /* DRIVER_LED_H_ */
