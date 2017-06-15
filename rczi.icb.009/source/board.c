/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "boos.board.h"
#include "driver.led.h"
#include "driver.mdio.h"
#include "driver.max.h"
#include "driver.ksz.h"

/**
 * Initializes a processor board.
 *
 * @return error code or else zero if no errors have been occurred.
 */ 
int8 boardInit(void)
{
  int8 stage, error;   
  stage = 0;
  error = BOOS_OK;
  do{
    /* Stage 1 */
    stage++;
    error = ledInit();    
    if(error != BOOS_OK){ break; } 
    
    /* Stage 2 */
    stage++;    
    error = mdioInit();    
    if(error != BOOS_OK){ break; } 
    
    /* Stage 3 */
    stage++;    
    error = maxInit();    
    if(error != BOOS_OK){ break; } 
    
    /* Stage 4 */
    stage++;    
    error = kszInit();    
    if(error != BOOS_OK){ break; } 
       
  }while(0);
  return error;
}
