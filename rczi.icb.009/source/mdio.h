/** 
 * Management Data Input/Output.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef MDIO_H_
#define MDIO_H_

#include "boos.types.h"

/**
 * Returns an timer resource.
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

#endif /* MDIO_H_ */
