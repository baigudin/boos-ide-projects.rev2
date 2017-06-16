/** 
 * User program entry.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#include "boos.main.h"
#include "boos.driver.interrupt.h"
#include "boos.driver.comparator.h"
#include "boos.driver.timer.h"
#include "boos.system.thread.h"
#include "driver.led.h"
#include "driver.mdio.h"
#include "driver.max.h"
#include "driver.ksz.h"
#include "driver.registers.h"

/**
 * Error code blink time in milliseconds. 
 */
#define ERROR_CODE_BLINK_TIME (500)

/**
 * The board state.
 */
typedef struct _Board
{
  struct _Resource
  {
    int8 cmp[2];
    int8 inr[2];    
  } res;
} Board;

/**
 * The board state.
 */
static Board board_;

/**
 * Interrupt handler of CP 0.
 *
 * @param out an output comparator value.
 */
static void handlerComparator0(int8 out)
{
  if(out == 0)
  {
    ledToggle(LED_X, 0);
  }
  else
  {
    ledToggle(LED_X, 1);    
  }
}

/**
 * Interrupt handler of INT 0.
 */
static void handlerInterrupt0(void)
{
  uint16 ir = maxRead(REG_MAX_IR);  
}

/**
 * Interrupt handler of CP 1.
 *
 * @param out an output comparator value.
 */
static void handlerComparator1(int8 out)
{
}

/**
 * Interrupt handler of INT 1.
 */
static void handlerInterrupt1(void)
{
}

/**
 * Configures MAX24287.
 *
 * @return error code or zero.
 */
static int8 maxConfig(void)
{
  int8 error = BOOS_OK;
  
  maxWrite(REG_MAX_GPIOCR1, 0x6 << 12  /* 3 GPO1: Output real-time link status, 0=link down, 1=link up */
                          | 0x1 << 9   /* 6 GPO2: Output CRS (carrier sense) status */
                          | 0x1 << 6   /* GPIO1: Drive logic 0 */
                          | 0x1 << 3   /* GPIO2: Drive logic 0 */
                          | 0x1 << 0   /* GPIO3: Drive logic 0 */
  );
  maxWrite(REG_MAX_GPIOCR2, 0x0 << 13  /* Set latched status bit when input goes low */
                          | 0x0 << 12  /* Set latched status bit when input goes low */  
                          | 0x1 << 9   /* GPIO7: Drive logic 0 */  
                          | 0x1 << 6   /* GPIO6: Drive logic 0 */  
                          | 0x1 << 3   /* GPIO5: Drive logic 0 */  
                          | 0x1 << 0   /* GPIO4: Drive logic 0 */
  );
  maxWrite(REG_MAX_PCSCR,   0x0 << 14  /* PCS link timer has normal timeout */
                          | 0x0 << 13  /* Enable PCS receive running disparity */  
                          | 0x0 << 12  /* Enable PCS transmit running disparity */  
                          | 0x0 << 6   /* Restart auto-negotiation after 5 seconds */  
                          | 0x0 << 4   /* SGMII PCS mode selected */  
                          | 0x0 << 1   /* Disable terminal loopback */
                          | 0x1 << 0   /* Enable comma alignment */
  );                        
  maxWrite(REG_MAX_GMIICR,  0x2 << 14  /* 1000 Mbps GMII RGMII-1000 */
                          | 0x0 << 13  /* TBI with one 125MHz receive clock (RXCLK pin) */
                          | 0x0 << 12  /* MII-DCE (MAX24287 on PHY side of MII, both RXCLK and TXCLK are outputs) */
                          | 0x1 << 11  /* RGMII or RTBI bus mode */
                          | 0x0 << 10  /* TXCLK pin is high impedance */
                          | 0x1 << 7   /* Write as 1 */                          
                          | 0x0 << 3   /* Noninverted */
                          | 0x0 << 0   /* Disable remote loopback */
  );

  do{
    /* Create comparison P0.2 on CP+ with VDD/2 on CP- */
    board_.res.cmp[0] = comparatorCreate(&handlerComparator0, 0, 0xD, 0x1);
    if(board_.res.cmp[0] == 0)
    {
      error = BOOS_ERROR;      
      break;
    }
    comparatorIntEnable(board_.res.cmp[0], 1);
    
    /* Create interrupts for status links */    
    board_.res.inr[0] = interruptCreate(&handlerInterrupt0, 0);
    if(board_.res.inr[0] == 0)
    {
      error = BOOS_ERROR;
      break;
    }
    
    REG_TCON &= 0xFC;
    REG_TCON |= 0x0 << 1  /* External Interrupt 0 flag is cleared */  
             |  0x1 << 0; /* External Interrupt 0 is edge triggered */   
    
    REG_IT01CF &= 0xFC;
    REG_IT01CF |= 0x1 << 3  /* INT0 input is active high */
               |  0x1 << 0; /* INT0 is P0.1 */        
    
    interruptEnable(board_.res.inr[0], 1);        

  }while(0);
  return error;
}

/**
 * Configures KSZ9031RNX.
 *
 * @return error code or zero.
 */
static int8 kszConfig(void)
{
  int8 error = BOOS_OK;  

  /* TODO */
  return error;    
  
  kszWrite(REG_KSZ_MMD_OMSO, 0x0 << 15  /* Override strap-in for RGMII to advertise all capabilities */
                           | 0x0 << 14  /* Override strap-in for RGMII to advertise all capabilities except 1000-T half-duplex */
                           | 0x0 << 13  /* Override strap-in for RGMII to advertise 1000-T full and half-duplex only */
                           | 0x1 << 12  /* Override strap-in for RGMII to advertise 1000-T full-duplex only */
                           | 0x0 << 10  /* Disable PME output (Pin 38) */
                           | 0x0 << 8   /* Disable PME output (Pin 17) */  
                           | 0x0 << 7   /* Not override strap-in for chip power-down mode */
                           | 0x0 << 4   /* Not override strap-in for NAND Tree mode  */ 
                           | 0x1 << 0   /* Rserved */   
  );  
  kszWrite(REG_KSZ_MMD_CNT,  0x1 << 4   /* Tri-color dual-LED mode */
                           | 0x0 << 1   /* CLK125_EN strap-in is disabled */ 
  );

  do{
    /* Create comparison P1.6 on CP+ with VDD/2 on CP- */
    board_.res.cmp[1] = comparatorCreate(&handlerComparator1, 1, 0xD, 0x7);
    if(board_.res.cmp[1] == 0)
    {
      error = BOOS_ERROR;      
      break;
    }
    comparatorIntEnable(board_.res.cmp[1], 1);
    
    /* Create interrupts for status links */    
    board_.res.inr[1] = interruptCreate(&handlerInterrupt1, 2);
    if(board_.res.inr[1] == 0)
    {
      error = BOOS_ERROR;
      break;
    }
    
    REG_TCON &= 0xF3;
    REG_TCON |=  0x0 << 3  /* External Interrupt 1 flag is cleared */
               | 0x1 << 2; /* External Interrupt 1 is edge triggered */
    
    REG_IT01CF &= 0xF3;    
    REG_IT01CF |= 0x0 << 7  /* INT1 input is active low */
               |  0x0 << 4; /* INT1 is P0.0 */
    
    interruptEnable(board_.res.inr[1], 1);     

  }while(0);
  return error;
}

/**
 * Executes machine states.
 */
void machine(void)
{
  volatile int8 exe = 1;
  while(exe)
  {
  };
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
    error = maxConfig();    
    if(error != BOOS_OK){ break; } 
    
    /* Stage 2 */
    stage++;    
    error = kszConfig();    
    if(error != BOOS_OK){ break; }

    /* Stage complete */    
    stage = 0;
    machine();
    
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
