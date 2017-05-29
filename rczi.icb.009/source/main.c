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
#include "driver.mdio.h"
#include "driver.max.h"


/**
 * User program entry.
 *
 * @return error code or zero.
 */
int8 mainStart()
{
  int8 error = BOOS_OK;
  do{
    /* Stage 1 */
    error = mdioInit();    
    if(error != BOOS_OK){ break; }
    /* Stage 2 */
    error = maxInit();    
    if(error != BOOS_OK){ break; }    
    /* Stage complete */
  }while(0);
  /* Start main program */
  if(error == BOOS_OK)
  {
    
  }
  return error;
}
