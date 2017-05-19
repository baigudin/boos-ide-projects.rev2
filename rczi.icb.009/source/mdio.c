/** 
 * Management Data Input/Output.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "mdio.h"
#include "registers.h"

/**
 * MDIO input/output data port 
 */
sbit MDIO = REG_P1^4;

/**
 * MDIO interface synchronization port 
 */
sbit MDC = REG_P1^5;

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
      MDIO = 1;
    }
    else
    {
      MDIO = 0;      
    }
    /* Set a clock to MDC */    
    MDC = 1;
    delay();    
    MDC = 0;    
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
    MDC = 1;
    delay();    
    MDC = 0;    
    delay();    
    val <<= 1;    
    val |= MDIO;  
  }
  return val;
}

/**
 * Returns an timer resource.
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