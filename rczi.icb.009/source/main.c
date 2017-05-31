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
  /* Set MAX24287 configuration */
  maxWrite(REG_MAX_GPIOCR1, 0x3 << 12  /* GPO1: Interrupt output, active low */
                          | 0x6 << 9   /* GPO2: Output real-time link status, 0=link down, 1=link up */
                          | 0x1 << 6   /* GPIO1: Drive logic 0 */
                          | 0x1 << 3   /* GPIO2: Drive logic 0 */
                          | 0x1 << 0); /* GPIO3: Drive logic 0 */
  
  maxWrite(REG_MAX_GPIOCR2, 0x0 << 13  /* Set latched status bit when input goes low */
                          | 0x0 << 12  /* Set latched status bit when input goes low */  
                          | 0x1 << 9   /* GPIO7: Drive logic 0 */  
                          | 0x1 << 6   /* GPIO6: Drive logic 0 */  
                          | 0x1 << 3   /* GPIO5: Drive logic 0 */  
                          | 0x1 << 0); /* GPIO4: Drive logic 0 */
  
  maxWrite(REG_MAX_PCSCR,   0x0 << 14  /* PCS link timer has normal timeout */
                          | 0x0 << 13  /* Enable PCS receive running disparity */  
                          | 0x0 << 12  /* Enable PCS transmit running disparity */  
                          | 0x0 << 6   /* Restart auto-negotiation after 5 seconds */  
                          | 0x0 << 4   /* SGMII PCS mode selected */  
                          | 0x0 << 1   /* Disable terminal loopback */
                          | 0x1 << 0); /* Enable comma alignment */
                          
  maxWrite(REG_MAX_GMIICR,  0x2 << 14  /* 1000 Mbps GMII RGMII-1000 */
                          | 0x0 << 13  /* TBI with one 125MHz receive clock (RXCLK pin) */
                          | 0x0 << 12  /* MII-DCE (MAX24287 on PHY side of MII, both RXCLK and TXCLK are outputs) */
                          | 0x1 << 11  /* RGMII or RTBI bus mode */
                          | 0x0 << 10  /* TXCLK pin is high impedance */
                          | 0x1 << 7   /* Write as 1 */                          
                          | 0x0 << 3   /* Noninverted */
                          | 0x0 << 0); /* Disable remote loopback */
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
