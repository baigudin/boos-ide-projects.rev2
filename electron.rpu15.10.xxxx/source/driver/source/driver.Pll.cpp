/** 
 * Phase-Locked Loop Controller driver factory. 
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#include "driver.Pll.hpp"
#include "driver.System.hpp"
#include "driver.SystemRegister.hpp"

/**
 * Initializes the driver.
 *
 * @param sourceClock source clock in Hz.    
 * @param cpuClock    requesting CPU clock in Hz.
 * @return true if no errors.
 */
bool Pll::init(int32 sourceClock, int32 cpuClock)
{
  SystemRegister* regSys = new (SystemRegister::ADDRESS) SystemRegister();
  int32 sel = 0x2;
  int32 mul = cpuClock * 2 / sourceClock;
  // Output frequency of the PLL (VCOCLK) does not exceed 300 MHz
  if(sourceClock * mul > 300000000) return false;
  // Multipliers more than 10 are reserved
  if(mul > 10) return false;
  // Test the oscillator is not off
  if(regSys->pllsts.bit.oscoff == 1) return false;
  // Test the PLL is set correctly
  if(regSys->pllsts.bit.plloff == 1 && regSys->pllcr.bit.div > 0) return false;
  // Start changing procedure
  if(regSys->pllsts.bit.mclksts == 1) return false;    
  System::eallow();    
  // Disable watchdog
  uint16 val = regSys->wdcr.val;
  val &= ~(SystemRegister::WDCR_M_WDDIS|SystemRegister::WDCR_M_WDCHK);
  val |=   SystemRegister::WDCR_V_WDDIS|SystemRegister::WDCR_V_WDCHK;
  regSys->wdcr.val = val;
  // Set division to four
  if(regSys->pllsts.bit.divsel != 0) regSys->pllsts.bit.divsel = 0;
  // Disable failed oscillator detect logic
  regSys->pllsts.bit.mclkoff = 1;
  // Set new PLL
  regSys->pllcr.bit.div = mul;
  // Test PLL is locked
  while(regSys->pllsts.bit.plllocks != 1);
  // Enable failed oscillator detect logic
  regSys->pllsts.bit.mclkoff = 0;
  // Set divider
  regSys->pllsts.bit.divsel = sel;
  System::dallow();
  return true;
}

/**
 * Deinitializes the driver.
 */
void Pll::deinit()
{
}
