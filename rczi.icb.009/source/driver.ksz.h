/** 
 * KSZ9031RNX Gigabit Ethernet Transciver driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef DRIVER_KSZ_H_
#define DRIVER_KSZ_H_

#include "boos.constants.h"

/**
 * The device register addresses.
 */
enum RegKsz
{
  /**
   * Basic Control.
   */
  REG_KSZ_BMCR = 0x00,

  /**
   * Basic Status.
   */
  REG_KSZ_BMSR = 0x01,
  
  /**
   * PHY Identifier 1.
   */
  REG_KSZ_ID1 = 0x02,
  
  /**
   * PHY Identifier 2.
   */
  REG_KSZ_ID2 = 0x03,
  
  /**
   * Auto-Negotiation Advertisement.
   */
  REG_KSZ_AN_ADV = 0x04,
  
  /**
   * Auto-Negotiation Link Partner Ability.
   */
  REG_KSZ_AN_RX = 0x05,
  
  /**
   * Auto-Negotiation Expansion.
   */
  REG_KSZ_AN_EXP = 0x06,
  
  /**
   * Auto-Negotiation Next Page.
   */
  REG_KSZ_AN_NP = 0x07,
  
  /**
   * Auto-Negotiation Link Partner Next Page Ability.
   */
  REG_KSZ_AN_LPNPA = 0x08,
  
  /**
   * 1000BASE-T Control.
   */
  REG_KSZ_GBC = 0x09,
  
  /**
   * 1000BASE-T Status.
   */
  REG_KSZ_GBS = 0x0A,
  
  /**
   * MMD Access – Control.
   */
  REG_KSZ_MMDAC = 0x0D,
  
  /**
   * MMD Access – Register/Data.
   */
  REG_KSZ_MMDAR = 0x0E,
  
  /**
   * Extended Status.
   */
  REG_KSZ_ES = 0x0F,
  
  /**
   * Remote Loopback.
   */
  REG_KSZ_RL = 0x11,
  
  /**
   * LinkMD Cable Diagnostic.
   */
  REG_KSZ_LCD = 0x12,
  
  /**
   * Digital PMA/PCS Status.
   */
  REG_KSZ_DS = 0x13,
  
  /**
   * RXER Counter.
   */
  REG_KSZ_RXER_COUNT = 0x15,
  
  /**
   * Interrupt Control/Status.
   */
  REG_KSZ_ICS = 0x1B,
  
  /**
   * Auto MDI/MDI-X.
   */
  REG_KSZ_AUTO_MDI = 0x1C,
  
  /**
   * PHY Control.
   */
  REG_KSZ_PHYC = 0x1F
  
};

/**
 * Read from the device.
 *
 * @param regAddr an register address.
 * @return the read resualt.
 */
extern int16 kszRead(enum RegKsz regAddr);

/**
 * Writes to the device.
 *
 * @param regAddr an register address.
 * @param value   a writing value.
 */
extern void kszWrite(enum RegKsz regAddr, int16 value);

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
extern int8 kszInit(void);

#endif /* DRIVER_KSZ_H_ */
