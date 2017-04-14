/**
 * System memory configuration for TI TMS320F28335/F28333/F28235 DSCs.
 *
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2017 Sergey Baigudin
 * @license   http://baigudin.software/license/
 * @link      http://baigudin.software
 */
-heap  0x1000
-stack 0x1000

MEMORY
{
  /** Program Memory */
PAGE 0:    

  RAM         : origin = 0x008000, length = 0x004000   
  FLASHH      : origin = 0x300000, length = 0x008000
  FLASHG      : origin = 0x308000, length = 0x008000
  FLASHF      : origin = 0x310000, length = 0x008000
  FLASHE      : origin = 0x318000, length = 0x008000
  FLASHD      : origin = 0x320000, length = 0x008000
  FLASHC      : origin = 0x328000, length = 0x008000
  FLASHA      : origin = 0x338000, length = 0x007F80
  ADC_CAL     : origin = 0x380080, length = 0x000009    
  BROM        : origin = 0x3fffc0, length = 0x000040

  /** Data Memory */
PAGE 1:

  M0          : origin = 0x000000, length = 0x000400
  M1          : origin = 0x000400, length = 0x000400
  PIE         : origin = 0x000d00, length = 0x000100     
  RAM         : origin = 0x00c000, length = 0x003000
  HEAP        : origin = 0x00f000, length = 0x001000
  FLASHB      : origin = 0x330000, length = 0x008000  
}

SECTIONS
{
  /** Hardware interrupts */    
  .pie       : > PIE,     PAGE = 1, type = NOLOAD
  .reset     : > BROM,    PAGE = 0, TYPE = DSECT   

  /** Allocate program areas */
  .special   : {
                 ./Debug/driver.Adc.obj            (.text)
                 ./Debug/driver.Adc.ll.obj         (.text)
                 ./Debug/driver.FullBridge.obj     (.text)
                 ./Debug/driver.Interrupt.obj      (.text)
                 ./Debug/driver.Pll.obj            (.text)
                 ./Debug/driver.Pwm.obj            (.text)
                 ./Debug/driver.System.ll.obj      (.text)
                 ./Debug/main.rpu15.10.4810.obj    (.text)
                 ./Debug/boos.Board.obj            (.text)                                  
               } 
               > RAM,     PAGE = 0
  .text      : > FLASHA,  PAGE = 0                  
  .cinit     : > FLASHA,  PAGE = 0
  .pinit     : > FLASHA,  PAGE = 0
               

  /** Initalized data sections */
  .data      : > FLASHB,  PAGE = 1  
  .econst    : > FLASHB,  PAGE = 1

  /** Uninitalized data sections */
  .stack     : > RAM,     PAGE = 1, fill = 0xffff
  .bss       : > RAM,     PAGE = 1, fill = 0xffff      
  .sysmem    : > RAM,     PAGE = 1, fill = 0xffff      
  .ebss      : > RAM,     PAGE = 1, fill = 0xffff
  .esysmem   : > RAM,     PAGE = 1, fill = 0xffff
  
   /** ADC calibration section is IMPORTANT for ADC */
   .adc_cal : load = ADC_CAL, PAGE = 0, TYPE = NOLOAD  
}
