/**
 * Silicon Labs C8051F9x registers.
 *
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @link      http://baigudin.software
 */
#ifndef REGISTERS_H_
#define REGISTERS_H_

/**
 * Registers byte addresses
 */
sfr REG_P0          = 0x80;        /* Port 0 Latch                           */
sfr REG_SP          = 0x81;        /* Stack Pointer                          */
sfr REG_DPL         = 0x82;        /* Data Pointer - Low byte                */
sfr REG_DPH         = 0x83;        /* Data Pointer - High byte               */
sfr REG_SPI1CFG     = 0x84;        /*                                        */
sfr REG_SPI1CKR     = 0x85;        /*                                        */
sfr REG_TOFFL       = 0x85;        /*                                        */
sfr REG_SPI1DAT     = 0x86;        /*                                        */
sfr REG_TOFFH       = 0x86;        /*                                        */
sfr REG_PCON        = 0x87;        /* Power Control                          */
sfr REG_TCON        = 0x88;        /* Timer Control                          */
sfr REG_TMOD        = 0x89;        /* Timer Mode                             */
sfr REG_TL0         = 0x8A;        /* Timer 0 - Low byte                     */
sfr REG_TL1         = 0x8B;        /* Timer 1 - Low byte                     */
sfr REG_TH0         = 0x8C;        /* Timer 0 - High byte                    */
sfr REG_TH1         = 0x8D;        /* Timer 1 - High byte                    */
sfr REG_CKCON       = 0x8E;        /* Clock Control                          */
sfr REG_PSCTL       = 0x8F;        /* Program Store R/W Control              */
sfr REG_P1          = 0x90;        /* Port 1 Latch                           */
sfr REG_TMR3CN      = 0x91;        /*                                        */
sfr REG_CRC0DAT     = 0x91;        /*                                        */
sfr REG_TMR3RLL     = 0x92;        /*                                        */
sfr REG_CRC0CN      = 0x92;        /*                                        */
sfr REG_TMR3RLH     = 0x93;        /*                                        */
sfr REG_CRC0IN      = 0x93;        /*                                        */
sfr REG_TMR3L       = 0x94;        /*                                        */
sfr REG_CRC0FLIP    = 0x94;        /*                                        */
sfr REG_TMR3H       = 0x95;        /*                                        */
sfr REG_DC0CF       = 0x96;        /*                                        */
sfr REG_CRC0AUTO    = 0x96;        /*                                        */
sfr REG_DC0CN       = 0x97;        /*                                        */
sfr REG_CRC0CNT     = 0x97;        /*                                        */
sfr REG_SCON0       = 0x98;        /* UART0 Control                          */
sfr REG_SBUF0       = 0x99;        /* UART0 Buffer                           */
sfr REG_CPT1CN      = 0x9A;        /*                                        */
sfr REG_CPT0CN      = 0x9B;        /*                                        */
sfr REG_CPT1MD      = 0x9C;        /*                                        */
sfr REG_CPT0MD      = 0x9D;        /*                                        */
sfr REG_CPT1MX      = 0x9E;        /*                                        */
sfr REG_CPT0MX      = 0x9F;        /*                                        */
sfr REG_P2          = 0xA0;        /* Port 2 Latch                           */
sfr REG_SPI0CFG     = 0xA1;        /*                                        */
sfr REG_SPI0CKR     = 0xA2;        /*                                        */
sfr REG_SPI0DAT     = 0xA3;        /*                                        */
sfr REG_P0MDOUT     = 0xA4;        /*                                        */
sfr REG_P1MDOUT     = 0xA5;        /*                                        */
sfr REG_P2MDOUT     = 0xA6;        /*                                        */
sfr REG_SFRPAGE     = 0xA7;        /*                                        */
sfr REG_IE          = 0xA8;        /* Interrupt Enable                       */
sfr REG_CLKSEL      = 0xA9;        /* Clock Select                           */
sfr REG_EMI0CN      = 0xAA;        /*                                        */
sfr REG_EMI0CF      = 0xAB;        /*                                        */
sfr REG_RTC0ADR     = 0xAC;        /*                                        */
sfr REG_P0DRV       = 0xAC;        /*                                        */
sfr REG_RTC0DAT     = 0xAD;        /*                                        */
sfr REG_P1DRV       = 0xAD;        /*                                        */
sfr REG_RTC0KEY     = 0xAE;        /*                                        */
sfr REG_P2DRV       = 0xAE;        /*                                        */
sfr REG_EMI0TC      = 0xAF;        /*                                        */
sfr REG_OSCIFIN     = 0xB0;        /* Internal Fine Oscillator Calibration   */
sfr REG_OSCXCN      = 0xB1;        /* External Oscillator Control            */
sfr REG_OSCICN      = 0xB2;        /* Internal Oscillator Control            */
sfr REG_OSCICL      = 0xB3;        /* Internal Oscillator Calibration        */
sfr REG_PMU0CF      = 0xB5;        /*                                        */
sfr REG_FLSCL       = 0xB6;        /*                                        */
sfr REG_FLKEY       = 0xB7;        /* Flash Lock & Key                       */
sfr REG_IP          = 0xB8;        /* Interrupt Priority                     */
sfr REG_IREF0CN     = 0xB9;        /*                                        */
sfr REG_ADC0PWR     = 0xB9;        /*                                        */
sfr REG_ADC0AC      = 0xBA;        /*                                        */
sfr REG_ADC0MX      = 0xBB;        /* ADC0 Mux Channel Selection             */
sfr REG_ADC0CF      = 0xBC;        /* ADC0 CONFIGURATION                     */
sfr REG_ADC0L       = 0xBD;        /* ADC0 LSB Result                        */
sfr REG_ADC0TK      = 0xBD;        /*                                        */
sfr REG_ADC0H       = 0xBE;        /*                                        */
sfr REG_P1MASK      = 0xBF;        /*                                        */
sfr REG_SMB0CN      = 0xC0;        /*                                        */
sfr REG_SMB0CF      = 0xC1;        /*                                        */
sfr REG_SMB0DAT     = 0xC2;        /*                                        */
sfr REG_ADC0GTL     = 0xC3;        /* ADC0 Greater-Than Compare Low          */
sfr REG_ADC0GTH     = 0xC4;        /* ADC0 Greater-Than Compare High         */
sfr REG_ADC0LTL     = 0xC5;        /* ADC0 Less-Than Compare Word Low        */
sfr REG_ADC0LTH     = 0xC6;        /* ADC0 Less-Than Compare Word High       */
sfr REG_P0MASK      = 0xC7;        /* Port 1 Mask                            */
sfr REG_TMR2CN      = 0xC8;        /* Timer 2 Control                        */
sfr REG_REG0CN      = 0xC9;        /* Regulator Control                      */
sfr REG_TMR2RLL     = 0xCA;        /* Timer 2 Reload Low                     */
sfr REG_TMR2RLH     = 0xCB;        /* Timer 2 Reload High                    */
sfr REG_TMR2L       = 0xCC;        /* Timer 2 Low Byte                       */
sfr REG_TMR2H       = 0xCD;        /* Timer 2 High Byte                      */
sfr REG_PCA0CPM5    = 0xCE;        /*                                        */
sfr REG_P1MAT       = 0xCF;        /* Port1 Match                            */
sfr REG_PSW         = 0xD0;        /* Program Status Word                    */
sfr REG_REF0CN      = 0xD1;        /* Voltage Reference 0 Control            */
sfr REG_PCA0CPL5    = 0xD2;        /*                                        */
sfr REG_PCA0CPH5    = 0xD3;        /*                                        */
sfr REG_P0SKIP      = 0xD4;        /* Port 0 Skip                            */
sfr REG_P1SKIP      = 0xD5;        /* Port 1 Skip                            */
sfr REG_P0MAT       = 0xD7;        /* Port 0 Match                           */
sfr REG_PCA0CN      = 0xD8;        /* PCA0 Control                           */
sfr REG_PCA0MD      = 0xD9;        /* PCA0 Mode                              */
sfr REG_PCA0CPM0    = 0xDA;        /* PCA0 Module 0 Mode                     */
sfr REG_PCA0CPM1    = 0xDB;        /* PCA0 Module 1 Mode                     */
sfr REG_PCA0CPM2    = 0xDC;        /* PCA0 Module 2 Mode                     */
sfr REG_PCA0CPM3    = 0xDD;        /* PCA0 Module 3 Mode                     */
sfr REG_PCA0CPM4    = 0xDE;        /* PCA0 Module 4 Mode                     */
sfr REG_PCA0PWM     = 0xDF;        /*                                        */
sfr REG_ACC         = 0xE0;        /* Accumulator                            */
sfr REG_XBR0        = 0xE1;        /*                                        */
sfr REG_XBR1        = 0xE2;        /*                                        */
sfr REG_XBR2        = 0xE3;        /*                                        */
sfr REG_IT01CF      = 0xE4;        /*                                        */
sfr REG_EIE1        = 0xE6;        /*                                        */
sfr REG_EIE2        = 0xE7;        /*                                        */
sfr REG_ADC0CN      = 0xE8;        /*                                        */
sfr REG_PCA0CPL1    = 0xE9;        /*                                        */
sfr REG_PCA0CPH1    = 0xEA;        /*                                        */
sfr REG_PCA0CPL2    = 0xEB;        /*                                        */
sfr REG_PCA0CPH2    = 0xEC;        /*                                        */
sfr REG_PCA0CPL3    = 0xED;        /*                                        */
sfr REG_PCA0CPH3    = 0xEE;        /*                                        */
sfr REG_RSTSRC      = 0xEF;        /*                                        */
sfr REG_B           = 0xF0;        /* B Register                             */
sfr REG_P0MDIN      = 0xF1;        /* Port 0 Input Mode                      */
sfr REG_P1MDIN      = 0xF2;        /* Port 1 Input Mode                      */
sfr REG_P2MDIN      = 0xF3;        /* Port 2 Input Mode                      */
sfr REG_SMB0ADR     = 0xF4;        /*                                        */
sfr REG_SMB0ADM     = 0xF5;        /*                                        */
sfr REG_EIP1        = 0xF6;        /* Extended Interrupt Priority 1          */
sfr REG_EIP2        = 0xF7;        /* Extended Interrupt Priority 2          */
sfr REG_SPI0CN      = 0xF8;        /* SPI0 Control                           */
sfr REG_PCA0L       = 0xF9;        /* PCA0 Counter Low Byte                  */
sfr REG_PCA0H       = 0xFA;        /* PCA0 Counter High Byte                 */
sfr REG_PCA0CPL0    = 0xFB;        /* PCA Module 0 Capture/Compare Low Byte  */
sfr REG_PCA0CPH0    = 0xFC;        /* PCA Module 0 Capture/Compare High Byte */
sfr REG_PCA0CPL4    = 0xFD;        /*                                        */
sfr REG_PCA0CPH4    = 0xFE;        /*                                        */
sfr REG_VDM0CN      = 0xFF;        /*                                        */

/**
 * Registers bit definitions
 */
/* TCON */
sbit REG_TCON_BIT_IT0 = REG_TCON^0;  /* External Interrupt 0 Type            */
sbit REG_TCON_BIT_IE0 = REG_TCON^1;  /* External Interrupt 0 Edge Flag       */
sbit REG_TCON_BIT_IT1 = REG_TCON^2;  /* External Interrupt 1 Type            */
sbit REG_TCON_BIT_IE1 = REG_TCON^3;  /* External Interrupt 1 Edge Flag       */
sbit REG_TCON_BIT_TR0 = REG_TCON^4;  /* Timer 0 On/Off Control               */
sbit REG_TCON_BIT_TF0 = REG_TCON^5;  /* Timer 0 Overflow Flag                */
sbit REG_TCON_BIT_TR1 = REG_TCON^6;  /* Timer 1 On/Off Control               */
sbit REG_TCON_BIT_TF1 = REG_TCON^7;  /* Timer 1 Overflow Flag                */

/* SCON0 */
sbit REG_SCON0_BIT_RI  = REG_SCON0^0;
sbit REG_SCON0_BIT_TI  = REG_SCON0^1;
sbit REG_SCON0_BIT_RB8 = REG_SCON0^2;
sbit REG_SCON0_BIT_TB8 = REG_SCON0^3;
sbit REG_SCON0_BIT_REN = REG_SCON0^4;
sbit REG_SCON0_BIT_SM3 = REG_SCON0^5;
sbit REG_SCON0_BIT_SM1 = REG_SCON0^6;
sbit REG_SCON0_BIT_SM0 = REG_SCON0^7;

/* IE */
sbit REG_IE_BIT_EX0 = REG_IE^0;
sbit REG_IE_BIT_ET0 = REG_IE^1;
sbit REG_IE_BIT_EX1 = REG_IE^2;
sbit REG_IE_BIT_ET1 = REG_IE^3;
sbit REG_IE_BIT_ES  = REG_IE^4;
sbit REG_IE_BIT_EA  = REG_IE^7;

/* IP */
sbit REG_IP_BIT_PX0 = REG_IP^0;
sbit REG_IP_BIT_PT0 = REG_IP^1;
sbit REG_IP_BIT_PX1 = REG_IP^2;
sbit REG_IP_BIT_PT1 = REG_IP^3;
sbit REG_IP_BIT_PS  = REG_IP^4;

/* PSW */
sbit REG_PSW_BIT_P   = REG_PSW^0;
sbit REG_PSW_BIT_OV  = REG_PSW^2;
sbit REG_PSW_BIT_RS0 = REG_PSW^3;
sbit REG_PSW_BIT_RS1 = REG_PSW^4;
sbit REG_PSW_BIT_F0  = REG_PSW^5;
sbit REG_PSW_BIT_AC  = REG_PSW^6;
sbit REG_PSW_BIT_CY  = REG_PSW^7;

#endif /* BOOS_DRIVER_REGISTERS_H_ */

