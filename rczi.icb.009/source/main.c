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
 * Configures MAX24287.
 *
 * @return error code or zero.
 */
static int8 maxConfig(void)
{
  maxWrite(REG_MAX_GPIOCR1, 0x3 << 12  /* GPO1: Interrupt output, active low */
                          | 0x6 << 9   /* GPO2: Output real-time link status, 0=link down, 1=link up */
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
  return BOOS_OK;
}

/**
 * Configures MAX24287.
 *
 * @return error code or zero.
 */
static int8 kszConfig(void)
{
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
  return BOOS_OK;
}

/**
 * Interrupt handler.
 */
static void handlerLinkActiveMax(void)
{
}

/**
 * Interrupt handler.
 */
static void handlerLinkActiveKsz(void)
{
}

/**
 * Interrupt handler.
 */
static void handlerLinkStatusMax(void)
{
}

/**
 * Interrupt handler.
 */
static void handlerLinkStatusKsz(void)
{
}

/**
 * Configures the board.
 *
 * @return error code or zero.
 */
static int8 boardConfig(void)
{
  int8 error = BOOS_OK;
  do{
    /* Create interrupts for active links */
    board_.res.cmp[0] = comparatorCreate(handlerLinkActiveMax, 0);
    if(board_.res.cmp[0] == 0)
    {
      error = BOOS_ERROR;      
      break;
    }
    board_.res.cmp[1] = comparatorCreate(handlerLinkActiveKsz, 1);
    if(board_.res.cmp[1] == 0)
    {
      error = BOOS_ERROR;      
      break;
    }
    /* Create interrupts for status links */    
    board_.res.inr[0] = interruptCreate(handlerLinkStatusMax, 0);
    if(board_.res.inr[0] == 0)
    {
      break;
    }
    board_.res.inr[1] = interruptCreate(handlerLinkStatusKsz, 2);
    if(board_.res.inr[1] == 0)
    {
      break;
    }
    comparatorSetInput(board_.res.cmp[0], 0xD, 0x1);
    comparatorSetInput(board_.res.cmp[0], 0xD, 0x7);    
    /* Enable interrupts */
    comparatorIntEnable(board_.res.cmp[0], 1);
    comparatorIntEnable(board_.res.cmp[1], 1);
    interruptEnable(board_.res.inr[0], 1);        
    interruptEnable(board_.res.inr[1], 1);    
  }while(0);
  return error;  
}

/**
 * Execut user program.
 */
void executing(void)
{
  volatile int8 exe = 1;
  while(exe){};
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
    
    /* Stage 5 */
    stage++;    
    error = maxConfig();    
    if(error != BOOS_OK){ break; } 
    
    /* Stage 6 */
    stage++;    
    error = kszConfig();    
    if(error != BOOS_OK){ break; }
    
    /* Stage 7 */
    stage++;    
    error = boardConfig();    
    if(error != BOOS_OK){ break; }    

    /* Stage complete */    
    stage = 0;    
    executing();
    
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
