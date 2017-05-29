/** 
 * Management Data Input/Output.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "driver.mdio.h"
#include "driver.registers.h"

/**
 * MDIO port mask.
 */
#define MDIO_PORT_MASK (0x10)

/**
 * MDC port mask.
 */
#define MDC_PORT_MASK (0x20)

/**
 * MDIO input/output data port 
 */
sbit mdio_ = REG_P1^4;

/**
 * MDIO interface synchronization port 
 */
sbit mdc_ = REG_P1^5;

/**
 * Delays the bus.
 *
 * The function delays Management Data Clock (MDC), 
 * which has a maximum clock rate of 2.5 MHz and no minimum rate.
 */
static void delay(void)
{
}

/**
 * Outputs a value to the bus.
 *
 * @param val an outputing value.
 * @param num an number of outputing bits.
 */
static void output(uint32 val, int8 num)
{
  for(val <<= (32 - num); num; val <<= 1, num--)
  {
    /* Set the value bit to MDO */
    if(val & 0x80000000)
    {
      mdio_ = 1;
    }
    else
    {
      mdio_ = 0;      
    }
    /* Set a clock to MDC */    
    mdc_ = 1;
    delay();    
    mdc_ = 0;    
    delay();    
  }
}

/**
 * Inputs a value from the bus.
 *
 * @param val an outputing value.
 * @param num an number of outputing bits.
 */
static uint16 input(void)
{
  int8 i;
  uint16 val = 0;
  for(i=0; i<16; i++)
  {
    mdc_ = 1;
    delay();    
    mdc_ = 0;    
    delay();    
    val <<= 1;    
    val |= mdio_;  
  }
  return val;
}

/**
 * Read from a PHY device.
 *
 * @param phyAddr an device address. 
 * @param regAddr an register address.
 * @return the read resualt.
 */
int16 mdioRead(int8 phyAddr, int8 regAddr)
{
  int16 val;
  output(0xffffffff, 32);
  output(0x6, 4);
  output(phyAddr, 5);
  output(regAddr, 5);
  output(1, 1);  
  val = input();
  output(1, 1);
  return val;  
}

/**
 * Writes to a PHY device.
 *
 * @param phyAddr an device address. 
 * @param regAddr an register address.
 * @param value   a writing value.
 */
void mdioWrite(int8 phyAddr, int8 regAddr, int16 value)
{
  output(0xffffffff, 32);
  output(0x5, 4);
  output(phyAddr, 5);
  output(regAddr, 5);
  output(0x2, 2);
  output(value, 16);
  output(1, 1);
}

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
int8 mdioInit(void)  
{
  /* Set default values of data and clock bus lines */
  mdio_ = 1;
  mdc_ = 0;
  /* Set MDIO port is open-drain */
  REG_P1MDOUT &= ~MDIO_PORT_MASK;      
  /* Set MDC port is push-pull */
  REG_P1MDOUT |= MDC_PORT_MASK;    
  /* Set P1.4 and P1.5 are skipped by the Crossbar and used for GPIO */
  REG_P1SKIP |= MDIO_PORT_MASK | MDC_PORT_MASK;
  /* Enable Crossbar for making MDIO is bidirectional line */  
  REG_XBR2 |= 0x40;
  return BOOS_OK;
}
