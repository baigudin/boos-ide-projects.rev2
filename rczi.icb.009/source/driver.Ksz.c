/** 
 * KSZ9031RNX Gigabit Ethernet Transciver driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "driver.Ksz.h"
#include "driver.Mdio.h"
#include "driver.Registers.h"
#include "boos.system.Thread.h"

/**
 * The device reset port mask.
 */
#define RESET_PORT_MASK (0x08)

/**
 * Access to PHY or MMD register mask.
 */
#define REG_PHY_MMD_MASK (0x8000)

/**
 * PHY register address mask.
 */
#define REG_PHY_RA_MASK  (0x001F)

/**
 * MMD device address mask.
 */
#define REG_MMD_DA_MASK  (0x7F00)

/**
 * MMD register address mask.
 */
#define REG_MMD_RA_MASK  (0x001F)

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
  int16 value;
  uint16 da, ra;
  if(isInitialized_)
  {  
    /* Accessing to standard registers  */  
    if( (regAddr & REG_PHY_MMD_MASK) == 0 )
    {
      ra = regAddr & REG_PHY_RA_MASK;
      value = mdioRead(phyAddr_, ra);  
    }
    /* Accessing to MDIO Manageable device */
    else
    {
      da = (regAddr & REG_MMD_DA_MASK) >> 8;    
      ra = (regAddr & REG_MMD_RA_MASK) >> 0;
      /* Set up register address for MMD */
      mdioWrite(phyAddr_, REG_KSZ_MMD_CTL, da);
      /* Select Register 10h of MMD - Device Address 2h. */
      mdioWrite(phyAddr_, REG_KSZ_MMD_RD, ra);
      /* Select register data for MMD - Device Address 2h, Register 10h. */
      mdioWrite(phyAddr_, REG_KSZ_MMD_CTL, da | 0x4000);
      /* Write value 0001h to MMD - Device Address 2h, Register 10h */
      value = mdioRead(phyAddr_, REG_KSZ_MMD_RD);
    }  
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
void kszWrite(enum RegKsz regAddr, int16 value)
{
  uint16 da, ra;
  if(isInitialized_)
  { 
    /* Accessing to standard registers  */  
    if( (regAddr & REG_PHY_MMD_MASK) == 0 )
    {
      ra = regAddr & REG_PHY_RA_MASK;
      mdioWrite(phyAddr_, ra, value);  
    }
    /* Accessing to MDIO Manageable device */
    else
    {
      da = (regAddr & REG_MMD_DA_MASK) >> 8;    
      ra = (regAddr & REG_MMD_RA_MASK) >> 0;
      /* Set up register address for MMD */
      mdioWrite(phyAddr_, REG_KSZ_MMD_CTL, da);
      /* Select Register 10h of MMD - Device Address 2h. */
      mdioWrite(phyAddr_, REG_KSZ_MMD_RD, ra);
      /* Select register data for MMD - Device Address 2h, Register 10h. */
      mdioWrite(phyAddr_, REG_KSZ_MMD_CTL, da | 0x4000);
      /* Write value 0001h to MMD - Device Address 2h, Register 10h */
      mdioWrite(phyAddr_, REG_KSZ_MMD_RD, value);
    }
  }
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