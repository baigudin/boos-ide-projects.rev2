/** 
 * KSZ9031RNX Gigabit Ethernet Transciver driver.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef DRIVER_KSZ_H_
#define DRIVER_KSZ_H_

#include "boos.Constants.h"

/**
 * The device register addresses.
 */
enum RegKsz
{
  /**
   * Basic Control.
   */
  REG_KSZ_BMCR = 0x0000u,

  /**
   * Basic Status.
   */
  REG_KSZ_BMSR = 0x0001u,
  
  /**
   * PHY Identifier 1.
   */
  REG_KSZ_ID1 = 0x0002u,
  
  /**
   * PHY Identifier 2.
   */
  REG_KSZ_ID2 = 0x0003u,
  
  /**
   * Auto-Negotiation Advertisement.
   */
  REG_KSZ_AN_ADV = 0x0004u,
  
  /**
   * Auto-Negotiation Link Partner Ability.
   */
  REG_KSZ_AN_RX = 0x0005u,
  
  /**
   * Auto-Negotiation Expansion.
   */
  REG_KSZ_AN_EXP = 0x0006u,
  
  /**
   * Auto-Negotiation Next Page.
   */
  REG_KSZ_AN_NP = 0x0007u,
  
  /**
   * Auto-Negotiation Link Partner Next Page Ability.
   */
  REG_KSZ_AN_LPNPA = 0x0008u,
  
  /**
   * 1000BASE-T Control.
   */
  REG_KSZ_GBC = 0x0009u,
  
  /**
   * 1000BASE-T Status.
   */
  REG_KSZ_GBS = 0x000Au,
  
  /**
   * MMD Access - Control.
   */
  REG_KSZ_MMD_CTL = 0x000Du,
  
  /**
   * MMD Access - Register/Data.
   */
  REG_KSZ_MMD_RD = 0x000Eu,
  
  /**
   * Extended Status.
   */
  REG_KSZ_ES = 0x000Fu,
  
  /**
   * Remote Loopback.
   */
  REG_KSZ_RL = 0x0011u,
  
  /**
   * LinkMD Cable Diagnostic.
   */
  REG_KSZ_LCD = 0x0012u,
  
  /**
   * Digital PMA/PCS Status.
   */
  REG_KSZ_DS = 0x0013u,
  
  /**
   * RXER Counter.
   */
  REG_KSZ_RXER_COUNT = 0x0015u,
  
  /**
   * Interrupt Control/Status.
   */
  REG_KSZ_ICS = 0x001Bu,
  
  /**
   * Auto MDI/MDI-X.
   */
  REG_KSZ_AUTO_MDI = 0x001Cu,
  
  /**
   * PHY Control.
   */
  REG_KSZ_PHYC = 0x001Fu,
  
  /**
   * AN FLP Burst Transmit - LO.
   */
  REG_KSZ_MMD_AN_FLP_BT_LO = 0x8003u,  
  
  /**
   * AN FLP Burst Transmit - HI.
   */
  REG_KSZ_MMD_AN_FLP_BT_HI = 0x8004u,
  
  /**
   * 1000BASE-T Link-Up Time Control.
   */
  REG_KSZ_MMD_1000T_LUTC = 0x815Au,

  /**
   * Common Control.
   */
  REG_KSZ_MMD_CC = 0x8200u,
  
  /**
   * Strap Status.
   */
  REG_KSZ_MMD_SS = 0x8201u,
  
  /**
   * Operation Mode Strap Override.
   */
  REG_KSZ_MMD_OMSO = 0x8202u,
  
  /**
   * Operation Mode Strap Status.
   */
  REG_KSZ_MMD_OMSS = 0x8203u,
  
  /**
   * RGMII Control Signal Pad Skew.
   */
  REG_KSZ_MMD_RGMII_CNT_PS = 0x8204u,
  
  /**
   * RGMII RX Data Pad Skew.
   */
  REG_KSZ_MMD_RGMII_RXD_PS = 0x8205u,
  
  /**
   * RGMII TX Data Pad Skew.
   */
  REG_KSZ_MMD_RGMII_TXD_PS = 0x8206u,
  
  /**
   * GMII Clock Pad Skew.
   */
  REG_KSZ_MMD_GMII_CLK_PS = 0x8208u,
  
  /**
   * Wake-On-LAN - Control.
   */
  REG_KSZ_MMD_WOL_CTL = 0x8210u,
  
  /**
   * Wake-On-LAN - Magic Packet, MAC-DA-0.
   */
  REG_KSZ_MMD_WOL_MAC0 = 0x8211u,
  
  /**
   * Wake-On-LAN - Magic Packet, MAC-DA-1.
   */
  REG_KSZ_MMD_WOL_MAC1 = 0x8212u,
  
  /**
   * Wake-On-LAN - Magic Packet, MAC-DA-2.
   */
  REG_KSZ_MMD_WOL_MAC2 = 0x8213u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 0, Expected CRC 0.
   */
  REG_KSZ_MMD_WOL_T0C0 = 0x8214u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 0, Expected CRC 1.
   */
  REG_KSZ_MMD_WOL_T0C1 = 0x8215u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 1, Expected CRC 0.
   */
  REG_KSZ_MMD_WOL_T1C0 = 0x8216u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 1, Expected CRC 1.
   */
  REG_KSZ_MMD_WOL_T1C1 = 0x8217u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 2, Expected CRC 0.
   */
  REG_KSZ_MMD_WOL_T2C0 = 0x8218u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 2, Expected CRC 1.
   */
  REG_KSZ_MMD_WOL_T2C1 = 0x8219u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 3, Expected CRC 0.
   */
  REG_KSZ_MMD_WOL_T3C0 = 0x821Au,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 3, Expected CRC 1.
   */
  REG_KSZ_MMD_WOL_T3C1 = 0x821Bu,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 0, Mask 0.
   */
  REG_KSZ_MMD_WOL_T0M0 = 0x821Cu,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 0, Mask 1.
   */
  REG_KSZ_MMD_WOL_T0M1 = 0x821Du,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 0, Mask 2.
   */
  REG_KSZ_MMD_WOL_T0M2 = 0x821Eu,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 0, Mask 3.
   */
  REG_KSZ_MMD_WOL_T0M3 = 0x821Fu,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 1, Mask 0.
   */
  REG_KSZ_MMD_WOL_T1M0 = 0x8220u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 1, Mask 1.
   */
  REG_KSZ_MMD_WOL_T1M1 = 0x8221u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 1, Mask 2.
   */
  REG_KSZ_MMD_WOL_T1M2 = 0x8222u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 1, Mask 3.
   */
  REG_KSZ_MMD_WOL_T1M3 = 0x8223u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 2, Mask 0.
   */
  REG_KSZ_MMD_WOL_T2M0 = 0x8224u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 2, Mask 1.
   */
  REG_KSZ_MMD_WOL_T2M1 = 0x8225u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 2, Mask 2.
   */
  REG_KSZ_MMD_WOL_T2M2 = 0x8226u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 2, Mask 3.
   */
  REG_KSZ_MMD_WOL_T2M3 = 0x8227u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 3, Mask 0.
   */
  REG_KSZ_MMD_WOL_T3M0 = 0x8228u,
   
  /**
   * Wake-On-LAN - Customized Packet, Type 3, Mask 1.
   */
  REG_KSZ_MMD_WOL_T3M1 = 0x8229u,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 3, Mask 2.
   */
  REG_KSZ_MMD_WOL_T3M2 = 0x822Au,
  
  /**
   * Wake-On-LAN - Customized Packet, Type 3, Mask 3.
   */
  REG_KSZ_MMD_WOL_T3M3 = 0x822Bu,
  
  /**
   * Analog Control 4.
   */
  REG_KSZ_MMD_ALG_CTL4 = 0x9C04u,

  /**
   * EDPD Control.
   */
  REG_KSZ_MMD_EDPD_CTL = 0x9C23u  
  
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
