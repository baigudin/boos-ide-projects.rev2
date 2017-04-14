/** 
 * Phase-Locked Loop Controller driver factory.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_PLL_HPP_
#define DRIVER_PLL_HPP_

#include "driver.Types.hpp"

class Pll
{

public:

  /**
   * Initializes the driver.
   *
   * @param sourceClock source clock in Hz.    
   * @param cpuClock    requesting CPU clock in Hz.
   * @return true if no errors are occurred.
   */
  static bool init(int32 sourceClock, int32 cpuClock);
  
  /**
   * Deinitializes the driver.
   */
  static void deinit();
  
};
#endif // DRIVER_PLL_HPP_
