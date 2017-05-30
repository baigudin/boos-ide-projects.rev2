/** 
 * MAX24287 1Gbps parallel-to-serial MII converter driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef DRIVER_MAX_H_
#define DRIVER_MAX_H_

#include "boos.constants.h"

/**
 * The device register addresses.
 */
enum RegMax
{
  /**
   * BMCR (RW)
   */
  REG_MAX_BMCR = 0x00,  
  
  /**
   * BMSR (RO)
   */
  REG_MAX_BMSR = 0x01,
  
  /**
   * ID1 (RO)
   */
  REG_MAX_ID1 = 0x02,
  
  /**
   * ID2 (RO)
   */
  REG_MAX_ID2 = 0x03,
  
  /**
   * AN_ADV (RW)
   */
  REG_MAX_AN_ADV = 0x04,
  
  /**
   * AN_RX (RO)
   */
  REG_MAX_AN_RX = 0x05,
  
  /**
   * AN_EXP (RO)
   */
  REG_MAX_AN_EXP = 0x06,
  
  /**
   * EXT_STAT (RO)
   */
  REG_MAX_EXT_STAT = 0x0f,
  
  /**
   * JIT_DIAG (RW)
   */
  REG_MAX_JIT_DIAG = 0x10,
  
  /**
   * PCSCR (RW)
   */
  REG_MAX_PCSCR = 0x11,
  
  /**
   * GMIICR (RW)
   */
  REG_MAX_GMIICR = 0x12,
  
  /**
   * CR (RW)
   */
  REG_MAX_CR = 0x13,
  
  /**
   * IR (RW)
   */
  REG_MAX_IR = 0x14,
  
  /**
   * ID (RO)
   */
  REG_MAX_ID = 0x30,
  
  /**
   * GPIOCR1 (RW)
   */
  REG_MAX_GPIOCR1 = 0x31,
  
  /**
   * GPIOCR2 (RW)
   */
  REG_MAX_GPIOCR2 = 0x32,
  
  /**
   * GPIOSR (RO)
   */
  REG_MAX_GPIOSR = 0x33,
  
  /**
   * PTPCR1 (RW)
   */
  REG_MAX_PTPCR1 = 0x70
  
};

/**
 * Read from the device.
 *
 * @param regAddr an register address.
 * @return the read resualt.
 */
extern int16 maxRead(enum RegMax regAddr);

/**
 * Writes to the device.
 *
 * @param regAddr an register address.
 * @param value   a writing value.
 */
extern void maxWrite(enum RegMax regAddr, int16 value);

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
extern int8 maxInit(void);

#endif /* DRIVER_MAX_H_ */
