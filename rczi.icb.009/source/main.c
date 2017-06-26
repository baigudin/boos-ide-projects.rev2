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
 * Interrupt handler of CP0.
 * 
 * The handler serves MAX24287 chip output real-time link status.
 * It is initialized to get interrupt by rising-edge and falling-edge of
 * a signal, which goes out from GPIO1 pin of the chip and goes in to P0.2
 * of the CPU. Having gotten zero value as the agrument of the function, which is 
 * a result of comparation of the signal and VDD divided by 2, the function
 * interprets that as the link is down, otherwise the link is up.
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
 * The handler serves KSZ9031RNX chip output real-time link status in Single-LED Mode.
 * It is initialized to get interrupt by rising-edge and falling-edge of
 * a signal, which goes out from LED2 pin of the chip and goes in to P1.6
 * of the CPU. Having gotten zero value as the agrument of the function, which is 
 * a result of comparation of the signal and VDD divided by 2, the function
 * interprets that as the link is up, otherwise the link is down.
 *
 * @param out an output comparator value.
 */
static void handlerComparator1(int8 out)
{
  int16 bs = kszRead(REG_KSZ_BMSR);
  if(out == 0)
  {
    ledToggle(LED_T, 1);        
  }
  else
  {
    ledToggle(LED_T, 0);    
  }  
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
  int8 error;
  int16 value;
  error = BOOS_OK;
  
  /* Set outputs of GPO1 and GPO2 */
  maxWrite(REG_MAX_GPIOCR1, 0x6 << 12  /* GPO1: Output real-time link status, 0=link down, 1=link up */
                          | 0x1 << 9   /* GPO2: Output CRS (carrier sense) status */
                          | 0x1 << 6   /* GPIO1: Drive logic 0 */
                          | 0x1 << 3   /* GPIO2: Drive logic 0 */
                          | 0x1 << 0   /* GPIO3: Drive logic 0 */
  );
  
  /* Set values of GPIO4 to GPIO7 */
  maxWrite(REG_MAX_GPIOCR2, 0x0 << 13  /* Set latched status bit when input goes low */
                          | 0x0 << 12  /* Set latched status bit when input goes low */  
                          | 0x1 << 9   /* GPIO7: Drive logic 0 */  
                          | 0x1 << 6   /* GPIO6: Drive logic 0 */  
                          | 0x1 << 3   /* GPIO5: Drive logic 0 */  
                          | 0x1 << 0   /* GPIO4: Drive logic 0 */
  );
  
  /* Set RGMII-1000 parallel interface mode */  
  maxWrite(REG_MAX_GMIICR,  0x2 << 14  /* 1000 Mbps GMII RGMII-1000 */
                          | 0x0 << 13  /* TBI with one 125MHz receive clock (RXCLK pin) */
                          | 0x0 << 12  /* MII-DCE (MAX24287 on PHY side of MII, both RXCLK and TXCLK are outputs) */
                          | 0x1 << 11  /* RGMII or RTBI bus mode */
                          | 0x0 << 10  /* TXCLK pin is high impedance */
                          | 0x1 << 7   /* Write as 1 */                          
                          | 0x0 << 3   /* Noninverted */
                          | 0x0 << 0   /* Disable remote loopback */
  );  
  
  /* Set 1000BASE-X serial interface mode */
  maxWrite(REG_MAX_PCSCR,   0x0 << 14  /* PCS link timer has normal timeout */
                          | 0x0 << 13  /* Enable PCS receive running disparity */  
                          | 0x0 << 12  /* Enable PCS transmit running disparity */  
                          | 0x0 << 6   /* Restart auto-negotiation after 5 seconds */  
                          | 0x0 << 4   /* 1000BASE-X PCS mode selected */  
                          | 0x0 << 1   /* Disable terminal loopback */
                          | 0x1 << 0   /* Enable comma alignment */
  ); 
  
  /* Set 1000BASE-X auto-negotiation TX advertisement */
  maxWrite(REG_MAX_AN_ADV,  0x0 << 15  /* Next Page capability is not supported */
                          | 0x0 << 12  /* No Error, Link OK */  
                          | 0x0 << 7   /* No Pause */  
                          | 0x0 << 6   /* Half duplex is not supported */
                          | 0x1 << 5   /* Advertise full duplex capability */
  );

  /* Enable auto-negotiation */
  maxWrite(REG_MAX_BMCR, 0x0 << 15  /* No reset */
                       | 0x0 << 14  /* Loopback diagnostic */
                       | 0x1 << 12  /* Enable auto-negotiation process */ 
                       | 0x1 << 9   /* Restart auto-negotiation */ 
                       | 0x0 << 7   /* No collision test */ 
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
    
    /* Create interrupts for the link activity */    
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
  int8 error;
  int16 value;
  
  error = BOOS_OK;
  
  /* Make an erratum recommendation to correct long link-up time due to default 
     FLP interval setting. Change 8ms default FLP burst-to-burst timing to
     16ms IEEE Standard specified timing after the device power-up/reset */
  
  /* Set Burst Transmit 16 ms period as the typical by writing to device 0 register 4 and 3 */
  kszWrite(REG_KSZ_MMD_AN_FLP_BT_HI, 0x0006 << 0);  /* Select 16 ms interval timing */  
  kszWrite(REG_KSZ_MMD_AN_FLP_BT_LO, 0x1A80 << 0);  /* Select 16 ms interval timing */
  
  /* Enable auto-negotiation */
  kszWrite(REG_KSZ_BMCR, 0x0 << 15  /* No reset */
                       | 0x0 << 14  /* Loopback: Normal operation*/
                       | 0x0 << 13  /* Speed Select (LSB): 10 Mbps if auto-negotiation is disabled */
                       | 0x1 << 12  /* Enable auto-negotiation process */ 
                       | 0x0 << 11  /* Power-Down: Normal operation  */
                       | 0x0 << 10  /* Isolate: Normal operation */ 
                       | 0x1 << 9   /* Restart auto-negotiation */ 
                       | 0x0 << 8   /* Duplex Mode: Full-duplex */ 
                       | 0x0 << 6   /* Speed Select (MSB): 10 Mbps if auto-negotiation is disabled */
  );
  
  /* Override strap-in options for the device */

  /* Common Control (device 2 register 0) */  
  kszWrite(REG_KSZ_MMD_CC, 0x1 << 4   /* Single-LED mode */
                         | 0x0 << 1   /* CLK125_EN strap-in is disabled */ 
  );      
  
  /* Operation Mode Strap Override (device 2 register 2) */
  kszWrite(REG_KSZ_MMD_OMSO, 0x0 << 15  /* Advertise all capabilities */
                           | 0x0 << 14  /* Advertise all capabilities except 1000-T half-duplex */
                           | 0x0 << 13  /* Advertise 1000-T full and half-duplex only */
                           | 0x1 << 12  /* Advertise 1000-T full-duplex only */
                           | 0x0 << 10  /* Disable PME output (Pin 38) */
                           | 0x0 << 8   /* Disable PME output (Pin 17) */  
                           | 0x0 << 7   /* Not override strap-in for chip power-down mode */
                           | 0x0 << 4   /* Not override strap-in for NAND Tree mode */ 
                           | 0x1 << 0   /* The bit is described as reserved, but it must be set to one
                                           for overriding the straped options */
  );   

  /* Make software reset of the device */
  value = kszRead(REG_KSZ_BMCR);
  kszWrite(REG_KSZ_BMCR, 0x1 << 15  /* Reset*/
                       | value
  );
  
  /* Sleep as defined in IEEE 802.3 */
  threadSleep(500);  
  
  /* Set max available delay of RXCLK by RGMII Clock Pad Skew (device 2 register 8) */
  kszWrite(REG_KSZ_MMD_GMII_CLK_PS, 0x0f << 5   /* RGMII GTX_CLK input +0.00 ns additional delay */
                                  | 0x1f << 0   /* RGMII RX_CLK output +0.96 ns additional delay */ 
  );

  /* Reduce link-up time, therefore accessing to 1000BASE-T Link-Up Time Control 
     register will be available only when Basic Control register is set to 0x2100 
     to disable auto-negotiation and force 100BASE-TX mode */

  /* Set 0x2100 to Basic Control */
  kszWrite(REG_KSZ_BMCR, 0x0 << 15  /* No reset */
                       | 0x0 << 14  /* Loopback: Normal operation*/
                       | 0x1 << 13  /* Speed Select (LSB): 100 Mbps if auto-negotiation is disabled */
                       | 0x0 << 12  /* Disable auto-negotiation process */ 
                       | 0x0 << 11  /* Power-Down: Normal operation  */
                       | 0x0 << 10  /* Isolate: Normal operation */ 
                       | 0x0 << 9   /* Restart auto-negotiation */ 
                       | 0x1 << 8   /* Duplex Mode: Full-duplex */ 
                       | 0x0 << 6   /* Speed Select (MSB): 100 Mbps if auto-negotiation is disabled */
  ); 
  
  /* Reduce link-up time by 1000BASE-T Link-Up Time Control (device 1 register 5Ah) */
  kszWrite(REG_KSZ_MMD_1000T_LUTC,  0x3 << 1); /* Optional setting to reduce link-up time */
  
  /* Enable auto-negotiation */
  kszWrite(REG_KSZ_BMCR, 0x0 << 15  /* No reset */
                       | 0x0 << 14  /* Loopback: Normal operation*/
                       | 0x0 << 13  /* Speed Select (LSB): 1000 Mbps if auto-negotiation is disabled */
                       | 0x1 << 12  /* Enable auto-negotiation process */ 
                       | 0x0 << 11  /* Power-Down: Normal operation  */
                       | 0x0 << 10  /* Isolate: Normal operation */ 
                       | 0x1 << 9   /* Restart auto-negotiation */ 
                       | 0x1 << 8   /* Duplex Mode: Full-duplex */ 
                       | 0x1 << 6   /* Speed Select (MSB): 1000 Mbps if auto-negotiation is disabled */
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
    
    /* Create interrupts for the link activity */
    board_.res.inr[1] = interruptCreate(&handlerInterrupt1, 2);
    if(board_.res.inr[1] == 0)
    {
      error = BOOS_ERROR;
      break;
    }
    
    REG_TCON &= 0xF3;
    REG_TCON |= 0x0 << 3  /* External Interrupt 1 flag is cleared */
             |  0x1 << 2; /* External Interrupt 1 is edge triggered */
    
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
