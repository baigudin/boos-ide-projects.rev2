/** 
 * Management Data Input/Output.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef DRIVER_MDIO_H_
#define DRIVER_MDIO_H_

#include "boos.constants.h"

/**
 * Read from a PHY device.
 *
 * @param phyAddr an device address. 
 * @param regAddr an register address.
 * @return the read resualt.
 */
extern int16 mdioRead(int8 phyAddr, int8 regAddr);

/**
 * Writes to a PHY device.
 *
 * @param phyAddr an device address. 
 * @param regAddr an register address.
 * @param value   a writing value.
 */
extern void mdioWrite(int8 phyAddr, int8 regAddr, int16 value);

/**
 * Initializes the driver.
 *
 * @return error code or else zero if no errors have been occurred.
 */   
extern int8 mdioInit(void);

#endif /* DRIVER_MDIO_H_ */
