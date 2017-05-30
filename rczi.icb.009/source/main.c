/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "boos.main.h"
#include "boos.driver.interrupt.h"
#include "boos.driver.timer.h"
#include "boos.system.thread.h"
#include "driver.led.h"
#include "driver.mdio.h"
#include "driver.max.h"
#include "driver.ksz.h"

/**
 * Error code blink time in milliseconds. 
 */
#define ERROR_CODE_BLINK_TIME (500)

/**
 * The application.
 *
 * @return error code or zero.
 */
static int8 application(void)
{
  return -1;
}

/**
 * User program entry.
 *
 * @return error code or zero.
 */
int8 mainStart(void)
{
  int8 i, stage, error;   
  stage = 0;
  error = BOOS_OK;
  do{
    /* Stage 1 */
    stage++;
    error = ledInit();    
    if(error != BOOS_OK)
    { 
      break; 
    }    
    /* Stage 2 */
    stage++;    
    error = mdioInit();    
    if(error != BOOS_OK)
    { 
      break; 
    }
    /* Stage 3 */
    stage++;    
    error = maxInit();    
    if(error != BOOS_OK)
    { 
      break; 
    }    
    /* Stage 4 */
    stage++;    
    error = kszInit();    
    if(error != BOOS_OK)
    { 
      break; 
    }        
    /* Start application */
    stage++;    
    error = application();
    break;
  }while(0);
  /* Blink an error code */
  while(error != BOOS_OK)
  {
    ledToggle(LED_X, 1);
    for(i=0; i<stage; i++)
    {
      ledToggle(LED_T, 1);
      threadSleep(ERROR_CODE_BLINK_TIME);
      ledToggle(LED_T, 0);
      if(i + 1 != stage)
      {
        threadSleep(ERROR_CODE_BLINK_TIME);
      }
    }
    ledToggle(LED_X, 0);
    threadSleep(ERROR_CODE_BLINK_TIME << 1);
  }
  return error;
}
