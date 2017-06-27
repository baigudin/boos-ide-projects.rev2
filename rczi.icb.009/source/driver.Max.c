/** 
 * MAX24287 1Gbps parallel-to-serial MII converter driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "driver.Max.h"
#include "driver.Mdio.h"
#include "driver.Registers.h"
#include "boos.system.Thread.h"

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
 * Mask of register address.
 */
#define REG_ADDR_MASK (0x001f)

/**
 * Mask of register page.
 */
#define REG_PAGE_MASK (0x0003)

/**
 * Mask of register address.
 */
#define REG_PAGE_SHIFT (5)

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
  int16 value;
  uint16 addr, page;
  if(isInitialized_)
  {   
    addr = regAddr & REG_ADDR_MASK;
    page = regAddr >> REG_PAGE_SHIFT & REG_PAGE_MASK;
    if(page != page_)
    {
      mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, page | 0x0010);
      page_ = page;
    }
    value = mdioRead(PHY_ADDR, addr);  
  }
  else
  {
    value = -1;
  }
  return value;
}

/**
 * Writes to the device.
 *
 * @param regAddr an register address.
 * @param value   a writing value.
 */
void maxWrite(enum RegMax regAddr, int16 value)
{
  uint16 addr, page; 
  if(isInitialized_)
  { 
    addr = regAddr & REG_ADDR_MASK;    
    page = regAddr >> REG_PAGE_SHIFT & REG_PAGE_MASK;    
    if(page != page_)
    {
      mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, page | 0x0010);
      page_ = page;
    }
    mdioWrite(PHY_ADDR, addr, value);
  }
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
  /* The device ID checking */
  if(error == BOOS_OK)
  {
    mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, 0x0011);    
    val = mdioRead(PHY_ADDR, REG_MAX_ID & REG_ADDR_MASK);
    /* Revision A */
    if((val & 0x0fff) == 0x0edf)
    {
      /* Set zero register page */
      mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, 0x0010);
    }
    /* Revision B */    
    else if((val & 0x0fff) == 0x0ee0)
    {
      /* Make the startup procedure */
      /* Selects MDIO page 2 */
      mdioWrite(PHY_ADDR, REG_MAX_PAGESEL , 0x0012);      
      /* Write 0x4004 to PTPCR1 to power-down the receive CDR */
      mdioWrite(PHY_ADDR, REG_MAX_PTPCR1 & REG_ADDR_MASK, 0x4004);            
      /* Wait 1ms */
      threadSleep(1);
      /* Write 0x4000 to PTPCR1 to power-up the receive CDR */
      mdioWrite(PHY_ADDR, REG_MAX_PTPCR1 & REG_ADDR_MASK, 0x4000);
      /* Set the BMCR.DP_RST bit to reset the datapath */
      mdioWrite(PHY_ADDR, REG_MAX_PAGESEL, 0x0010);
      mdioWrite(PHY_ADDR, REG_MAX_BMCR & REG_ADDR_MASK, 0x8000);
    }  
    else
    {
      error = BOOS_ERROR;      
    }
  }
  if(error == BOOS_OK)
  {
    page_ = 0;    
    isInitialized_ = 1;  
  }  
  return error;
}