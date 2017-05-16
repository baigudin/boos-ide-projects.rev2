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
#include "registers.h"

/**
 * Green led port.
 */
sbit GLED = REG_P1^0;

/**
 * Yellow led port.
 */
sbit YLED = REG_P1^1;

/**
 * MAX24287 
 */
sbit RSTM = REG_P1^2;

/**
 * KSZ9031 
 */
sbit RSTK = REG_P1^3;

/**
 * MDIO input/output data port 
 */
sbit MDIO = REG_P1^4;

/**
 * MDIO interface synchronization port 
 */
sbit MDC = REG_P1^5;

/**
 * User program entry.
 *
 * @return error code or zero.
 */
int8 userMain()
{
  /* Set default states of P0 and P1 ports */
  REG_P0 = 0x87;
  REG_P1 = 0x5f;
  /* From P1.0 till P1.3 and P1.5 output are push-pull */
  REG_P1MDOUT = 0x2F;
  /* From P0.0 till P0.7 and P1.0 till P1.6 are
     skipped by the Crossbar and used for GPIO */
  REG_P0SKIP = 0xFF;
  REG_P1SKIP = 0x7F;
  /* Crossbar enabled */  
  REG_XBR2 = 0x40;
  /* Enable Comparators */
  REG_CPT0CN = 0x8F;
  REG_CPT1CN = 0x8F;
  threadSleep(1);
  /* Setup the comparator 0 will interrupt the processor 
     when P0.2 signal level will be changed */
  REG_CPT0MX = 0xD1;
  REG_CPT0MD = 0xB0;
  /* Setup the comparator 1 will interrupt the processor 
     when P1.6 signal level will be changed */  
  REG_CPT1MX = 0xD7;
  REG_CPT1MD = 0xB0;
  /* Setup INT0 and INT1 will interrupt the processor 
     when rising or falling edge on P0.1 or P0.0 accordingly */
  REG_TCON = 0x05;
  REG_IT01CF = 0x09;
  /* Setup the thmers 0 and 1 are 16-bit width and
     clocked by SYSCLK divided 48 */
  REG_TMOD = 0x11;
  REG_CKCON = 0x2;
  threadSleep(50);
  /* Complite setuping of MAX24287 */  
  RSTM = 0;
  
  GLED = 1;
  GLED = 0;  
  
  YLED = 1;  
  YLED = 0;    
  
  return 0;  
}
