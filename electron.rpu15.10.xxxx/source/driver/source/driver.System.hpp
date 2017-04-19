/** 
 * Access to the resources of TI TMS320F2833x DSP family .
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_SYSTEM_HPP_
#define DRIVER_SYSTEM_HPP_

#include "driver.Types.hpp"

class System
{

public:

  /**
   * Enables access to protected space.
   */
  static void eallow();
  
  /**
   * Disables access to protected space.
   */
  static void dallow();  

};
#endif // DRIVER_SYSTEM_HPP_

