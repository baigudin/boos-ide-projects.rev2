/** 
 * TI TMS320F2833x DSP High-Resolution Pulse Width Modulator definition.
 *
 * The definition enables compiling the driver with HR supporting, 
 * and requires TI SFO Library v5 (SFO_TI_Build_V5B_fpu.lib). 
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 */
#ifndef DRIVER_PWM_HIGH_RESOLUTION_HPP_
#define DRIVER_PWM_HIGH_RESOLUTION_HPP_

/**
 * High-Resolution driver bulid definition
 */
#ifndef DRIVER_HRPWM
#define DRIVER_HRPWM
#endif//DRIVER_HRPWM

#ifdef  DRIVER_HRPWM

#include "SFO_V5.h"

#if PWM_CH != 7
#error "The HRPWM driver supports only 6 HRPWM channels. PWM_CH has be defined with 7."
#endif

/**
 * TI SFO Library global variables
 */
#ifndef TI_GLOBAL_HRPWM_VARIABLES_SET
#define TI_GLOBAL_HRPWM_VARIABLES_SET

struct EPWM_REGS;

/**
 * Defining number of HRPWM channels used
 */
int MEP_ScaleFactor[PWM_CH];

/**
 * Declaring elements required
 */
volatile struct EPWM_REGS* ePWM[PWM_CH];

#endif // TI_GLOBAL_HRPWM_VARIABLES_SET
#endif // DRIVER_HRPWM
#endif // DRIVER_PWM_HIGH_RESOLUTION_HPP_

