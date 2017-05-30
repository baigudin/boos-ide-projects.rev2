/** 
 * MAX24287 1Gbps parallel-to-serial MII converter driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "driver.max.h"
#include "driver.mdio.h"
#include "driver.registers.h"
#include "boos.system.thread.h"

/**
 * The device reset port mask.
 */
#define RESET_PORT_MASK (0x04)

/**
 * The device physical bus address.
 */
#define PHY_ADDR (0x04)

/**
 * PAGESEL (RW)
 */
#define REG_MAX_PAGESEL (0x1f)

/**
 * Reset pin of MCU port.
 */
sbit reset_ = REG_P1^2;

/**
 * Current page of the device registers.
 */
static int8 page_;

/**
 * The driver has been initialized successfully.
 */
static int8 isInitialized_;

/**
 * Read from the device.
 *
 * @param regAddr an register address.
 * @return the read resualt.
 */
int16 maxRead(enum RegMax regAddr)
{
  uint16 addr = regAddr & 0x1f;
  uint16 page = regAddr >> 5 & 0x3;
  if(page != page_)
  {
    mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, page | 0x10);
    page_ = page;
  }
  return mdioRead(PHY_ADDR, addr);  
}

/**
 * Writes to the device.
 *
 * @param regAddr an register address.
 * @param value   a writing value.
 */
void maxWrite(enum RegMax regAddr, int16 value)
{
  uint16 addr = regAddr & 0x1f;
  uint16 page = regAddr >> 5 & 0x3;
  if(page != page_)
  {
    mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, page | 0x10);
    page_ = page;
  }
  mdioWrite(PHY_ADDR, addr, value);
}

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
int8 maxInit(void)  
{
  int16 val;  
  int8 error = BOOS_OK;  
  isInitialized_ = 0;    
  /* Set the reset port is push-pull */
  REG_P1MDOUT |= RESET_PORT_MASK;
  /* Set reset port is skipped by the Crossbar and used for GPIO */
  REG_P1SKIP |= RESET_PORT_MASK;
  /* Enable Crossbar for using port pins as digital outputs */
  REG_XBR2 |= 0x40;
  /* Reset the device */
  reset_ = 1;
  threadSleep(10);
  reset_ = 0; 
  /* Read-write test */
  for(val=3; val>=0; val--)
  {
    mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, val);
    if(mdioRead(PHY_ADDR, REG_MAX_PAGESEL) != val)
    {
      error = BOOS_ERROR;
      break;
    }      
  }
  /* Set zero register page */
  mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, 0x10);
  page_ = 0;  
  /* The device ID checking */
  if(error == BOOS_OK)
  {
    val = maxRead(REG_MAX_ID);
    if(val & 0x0fff != 0x0ee0)
    {
      error = BOOS_ERROR;
    }  
  }
  if(error == BOOS_OK)
  {
    isInitialized_ = 1;  
  }  
  return error;
}