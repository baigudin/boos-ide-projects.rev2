/** 
 * Full-Bridge driver factory.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_FULL_BRIDGE_HPP_
#define DRIVER_PLL_HPP_

#include "driver.Types.hpp"

class FullBridge
{

public:

  /**
   * Initializes the driver.
   *
   * @return true if no errors are occurred.
   */
  static bool init();
  
  /**
   * Deinitializes the driver.
   */
  static void deinit();
  
};
#endif // DRIVER_FULL_BRIDGE_HPP_
