/**
 * System memory configuration for TI TMS320F2833x DSCs.
 *
 * @author    Sergey Baigudin, baigudin@mail.ru
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

  HWI0         : origin = 0x000000, length = 0x000040
  CODE         : origin = 0x008000, length = 0x006000
  ADC_CAL      : origin = 0x380080, length = 0x000009    
  HWI1         : origin = 0x3fffc0, length = 0x000040

  /** Data Memory */
PAGE 1:

  RAM1         : origin = 0x000400, length = 0x001000
  DATA         : origin = 0x00d000, length = 0x003000     
}

SECTIONS
{
   /** Hardware interrupts */    
   .hwi        : > HWI0,    PAGE = 0, TYPE = DSECT
   .reset      : > HWI1,    PAGE = 0, TYPE = DSECT   

   /** Allocate program areas */
   .cinit      : > CODE,    PAGE = 0
   .pinit      : > CODE,    PAGE = 0
   .text       : > CODE,    PAGE = 0

   /** Initalized data sections */
   .cio        : > DATA,    PAGE = 1
   .econst     : > DATA,    PAGE = 1
   .switch     : > DATA,    PAGE = 1     

   /** Uninitalized data sections */
   .bss        : > DATA,    PAGE = 1
   .stack      : > DATA,    PAGE = 1   
   .ebss       : > DATA,    PAGE = 1
   .esysmem    : > DATA,    PAGE = 1
   
   /** ADC calibration section is IMPORTANT for ADC */
   .adc_cal : load = ADC_CAL, PAGE = 0, TYPE = NOLOAD   
}
