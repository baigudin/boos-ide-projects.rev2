/** 
 * KSZ9031RNX Gigabit Ethernet Transciver driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "driver.ksz.h"
#include "driver.mdio.h"
#include "driver.registers.h"
#include "boos.system.thread.h"

/**
 * The device reset port mask.
 */
#define RESET_PORT_MASK (0x08)

/**
 * Reset pin of MCU port.
 */
sbit reset_ = REG_P1^3;

/**
 * The device physical bus address.
 */
static int8 phyAddr_;

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
int16 kszRead(enum RegKsz regAddr)
{
  uint16 addr = regAddr & 0x1f;
  return mdioRead(phyAddr_, addr);   
}

/**
 * Writes to the device.
 *
 * @param regAddr an register address.
 * @param value   a writing value.
 */
void kszWrite(enum RegKsz regAddr, int16 value)
{
  uint16 addr = regAddr & 0x1f;
  mdioWrite(phyAddr_, addr, value);  
}

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
int8 kszInit(void)  
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
  /* Poll the device for finding its address out */  
  error = BOOS_ERROR;
  for(phyAddr_=1; phyAddr_<8; phyAddr_++)
  {
    val = mdioRead(phyAddr_, REG_KSZ_ID2);
    if( (val & 0xfff0) == 0x1620 )
    {
      error = BOOS_OK;
      break;
    }          
  }
  if(error == BOOS_OK)
  {
    isInitialized_ = 1;  
  }
  return error;
}