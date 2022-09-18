/*-----------------------------------------------------------------------------
REG51F380.H

Header file for Silabs C8051F380/1/2/3/4/5/6 and C8051F387 devices.
Copyright (c) 2011 ARM Ltd and ARM Germnay GmbH.
All rights reserved.
-----------------------------------------------------------------------------*/


#ifndef __REG51F380_H__
#define __REG51F380_H__

#include <simul_main.hpp>
#define main simul_main

static void _O_FirstExecutionConfig_REG51F38C_function (void)
{
	_SIMUL_SET_PORTNUM(4);
    _SIMUL_HIDE_PORT(0, 0xFF);
    _SIMUL_HIDE_PORT(3, 0x7F);
}
static simul::base::FirstExecutionClass* _O_FirstExecutionConfig_REG51F38C_class(new simul::base::FirstExecutionClass(_O_FirstExecutionConfig_REG51F38C_function));

/* SFRPage 0x00 and SFRPage 0x0F Registers */
#define P0 simul::core::GetPort(0)      /* Port 0 Latch                             */
#define SP GET_BAD   /* Stack Pointer                            */
#define DPL GET_BAD  /* Data Pointer Low                         */
#define DPH GET_BAD  /* Data Pointer High                        */
#define EMI0TC GET_BAD /* External Memory Interface Timing         */
#define EMI0CF GET_BAD /* External Memory Interface Configuration  */
#define OSCLCN GET_BAD /* Internal Low-Frequency Oscillator Control*/
#define PCON GET_BAD /* Power Control                            */
//sfr TCON        = 0x88;          /* Timer/Counter Control                    */
//sfr TMOD        = 0x89;          /* Timer/Counter Mode                       */
//sfr TL0         = 0x8A;          /* Timer/Counter 0 Low                      */
//sfr TL1         = 0x8B;          /* Timer/Counter 1 Low                      */
//sfr TH0         = 0x8C;          /* Timer/Counter 0 High                     */
//sfr TH1         = 0x8D;          /* Timer/Counter 1 High                     */
//sfr CKCON       = 0x8E;          /* Clock Control                            */
//sfr PSCTL       = 0x8F;          /* Program Store R/W Control                */
#define P1 simul::core::GetPort(1) /* Port 1 Latch                             */
//sfr TMR3CN      = 0x91;          /* Timer/Counter 3 Control         (page 0) */
//sfr TMR3RLL     = 0x92;          /* Timer/Counter 3 Reload Low      (page 0) */
//sfr TMR3RLH     = 0x93;          /* Timer/Counter 3 Reload High     (page 0) */
//sfr TMR3L       = 0x94;          /* Timer/Counter 3 Low                      */
//sfr TMR3H       = 0x95;          /* Timer/Counter 3 High                     */
#define USB0ADR GET_BAD /* USB0 Indirect Address Register           */
#define USB0DAT GET_BAD /* USB0 Data Register                       */
//sfr TMR4CN      = 0x91;          /* Timer/Counter 4 Control         (page F) */
//sfr TMR4RLL     = 0x92;          /* Timer/Counter 4 Reload Low      (page F) */
//sfr TMR4RLH     = 0x93;          /* Timer/Counter 4 Reload High     (page F) */
//sfr SCON0       = 0x98;          /* UART0 Control                            */
//sfr SBUF0       = 0x99;          /* UART0 Data Buffer                        */
//sfr CPT1CN      = 0x9A;          /* Comparator 1 Control                     */
//sfr CPT0CN      = 0x9B;          /* Comparator 0 Control                     */
//sfr CPT1MD      = 0x9C;          /* Comparator 1 Mode                        */
//sfr CPT0MD      = 0x9D;          /* Comparator 0 Mode                        */
//sfr CPT1MX      = 0x9E;          /* Comparator 1 Mux                         */
//sfr CPT0MX      = 0x9F;          /* Comparator 0 Mux                         */
#define P2 simul::core::GetPort(2)      /* Port 2 Latch                             */
//sfr SPI0CFG     = 0xA1;        /* SPI0 Configuration                       */
//sfr SPI0CKR     = 0xA2;        /* SPI0 Clock rate control                  */
//sfr SPI0DAT     = 0xA3;        /* SPI0 Data Buffer                         */
#define P0MDOUT simul::core::GetPortType(0) /* Port 0 Output Mode                       */
#define P1MDOUT simul::core::GetPortType(1) /* Port 1 Output Mode                       */
#define P2MDOUT simul::core::GetPortType(2) /* Port 2 Output Mode                       */
#define P3MDOUT simul::core::GetPortType(3) /* Port 3 Output Mode                       */
//sfr IE          = 0xA8;          /* Interrupt Enable                         */
#define CLKSEL _SIMUL_GET_BAD          /* Clock Select                             */
//sfr EMI0CN      = 0xAA;          /* EMIF control                             */
//sfr SBCON1      = 0xAC;          /* UART1 Baud Rate Generator Control        */
//sfr P4MDOUT     = 0xAE;          /* Port 4 Mode                              */
//sfr PFE0CN      = 0xAF;          /* Prefetch Engine Control                  */
//sfr P3          = 0xB0;          /* Port 3 Latch                             */
//sfr OSCXCN      = 0xB1;          /* External Oscillator Control              */
#define OSCICN _SIMUL_GET_BAD          /* Internal Oscillator Control              */
//sfr OSCICL      = 0xB3;          /* Internal Oscillator Calibration          */
//sfr SBRLL1      = 0xB4;          /* UART1 Baud Rate Generator Low            */
//sfr SBRLH1      = 0xB5;          /* UART1 Baud Rate Generator High           */
//sfr FLSCL       = 0xB6;          /* Flash Scale                              */
//sfr FLKEY       = 0xB7;          /* Flash Lock and Key                       */
//sfr IP          = 0xB8;          /* Interrupt Priority                       */
//sfr CLKMUL      = 0xB9;          /* Clock Multiplier                (page 0) */
//sfr AMX0N       = 0xBA;          /* AMUX0 Negative Channel Select            */
//sfr AMX0P       = 0xBB;          /* AMUX0 Positive Channel Select            */
//sfr ADC0CF      = 0xBC;          /* ADC0 Configuration              (page 0) */
//sfr ADC0L       = 0xBD;          /* ADC0 Low                                 */
//sfr ADC0H       = 0xBE;          /* ADC0 High                                */
//sfr SFRPAGE     = 0xBF;          /* SFR Page Select                          */
//sfr SMBTC       = 0xB9;          /* SMBus0/1 Timing Control         (page F) */
//sfr SMB0CN      = 0xC0;          /* SMBus0 Control                  (page 0) */
//sfr SMB0CF      = 0xC1;          /* SMBus0 Configuration            (page 0) */
//sfr SMB0DAT     = 0xC2;          /* SMBus0 Data                     (page 0) */
//sfr ADC0GTL     = 0xC3;          /* ADC0 Greater-Than Compare Low            */
//sfr ADC0GTH     = 0xC4;          /* ADC0 Greater-Than Compare High           */
//sfr ADC0LTL     = 0xC5;          /* ADC0 Less-Than Compare Word Low          */
//sfr ADC0LTH     = 0xC6;          /* ADC0 Less-Than Compare Word High         */
//sfr P4          = 0xC7;          /* Port 4 Latch                             */
//sfr SMB1CN      = 0xC0;          /* SMBus1 Control                  (page F) */
//sfr SMB1CF      = 0xC1;          /* SMBus1 Configuration            (page F) */
//sfr SMB1DAT     = 0xC2;          /* SMBus1 Data                     (page F) */
//sfr TMR2CN      = 0xC8;          /* Timer/Counter 2 Control         (page 0) */
//sfr REG01CN     = 0xC9;          /* Regulator Control               (page 0) */
//sfr TMR2RLL     = 0xCA;          /* Timer/Counter 2 Reload Low      (page 0) */
//sfr TMR2RLH     = 0xCB;          /* Timer/Counter 2 Reload High     (page 0) */
//sfr TMR2L       = 0xCC;          /* Timer/Counter 2 Low             (page 0) */
//sfr TMR2H       = 0xCD;          /* Timer/Counter 2 High            (page 0) */
//sfr SMB0ADM     = 0xCE;          /* SMBus0 Address Mask             (page 0) */
//sfr SMB0ADR     = 0xCF;          /* SMBus0 Address                  (page 0) */
//sfr TMR5CN      = 0xC8;          /* Timer/Counter 5 Control         (page F) */
//sfr TMR5RLL     = 0xCA;          /* Timer/Counter 5 Reload Low      (page F) */
//sfr TMR5RLH     = 0xCB;          /* Timer/Counter 5 Reload High     (page F) */
//sfr TMR5L       = 0xCC;          /* Timer/Counter 5 Low             (page F) */
//sfr TMR5H       = 0xCD;          /* Timer/Counter 5 High            (page F) */
//sfr SMB1ADM     = 0xCE;          /* SMBus1 Address Mask             (page F) */
//sfr SMB1ADR     = 0xCF;          /* SMBus1 Address                  (page F) */
//sfr PSW         = 0xD0;          /* Program Status Word                      */
//sfr REF0CN      = 0xD1;          /* Voltage Reference Control                */
//sfr SCON1       = 0xD2;          /* UART1 Control                            */
//sfr SBUF1       = 0xD3;          /* UART1 Data Buffer                        */
#define P0SKIP _SIMUL_GET_BAD          /* Port 0 Skip                              */
#define P1SKIP _SIMUL_GET_BAD          /* Port 1 Skip                              */
#define P2SKIP _SIMUL_GET_BAD          /* Port 2 Skip                              */
//sfr USB0XCN     = 0xD7;          /* USB0 Transceiver Control                 */
#define PCA0CN _SIMUL_GET_BAD          /* PCA0 Control                             */
#define PCA0MD _SIMUL_GET_BAD          /* PCA0 Mode                                */
#define PCA0CPM0 _SIMUL_GET_BAD          /* PCA Module 0 Mode Register               */
#define PCA0CPM1 _SIMUL_GET_BAD          /* PCA Module 1 Mode Register               */
#define PCA0CPM2 _SIMUL_GET_BAD          /* PCA Module 2 Mode Register               */
#define PCA0CPM3 _SIMUL_GET_BAD          /* PCA Module 3 Mode Register               */
#define PCA0CPM4 _SIMUL_GET_BAD          /* PCA Module 4 Mode Register               */
//sfr P3SKIP      = 0xDF;          /* Port 3 Skip                              */
//sfr ACC         = 0xE0;          /* Accumulator                              */
#define XBR0 _SIMUL_GET_BAD          /* Port I/O Crossbar Control 0              */
#define XBR1 _SIMUL_GET_BAD          /* Port I/O Crossbar Control 1              */
//sfr XBR2        = 0xE3;          /* Port I/O Crossbar Control 2              */
//sfr IT01CF      = 0xE4;          /* INT0/INT1 Configuration         (page 0) */
//sfr SMOD1       = 0xE5;          /* UART1 Mode                               */
//sfr EIE1        = 0xE6;          /* Extended Interrupt Enable 2              */
//sfr EIE2        = 0xE7;          /* Extended Interrupt Enable 2              */
//sfr CKCON1      = 0xE4;          /* Clock Control 1                 (page F) */
//sfr ADC0CN      = 0xE8;          /* ADC0 Control                             */
//sfr PCA0CPL1    = 0xE9;          /* PCA0 Capture 2 Low                       */
//sfr PCA0CPH1    = 0xEA;          /* PCA0 Capture 2 High                      */
//sfr PCA0CPL2    = 0xEB;          /* PCA0 Capture 3 Low                       */
//sfr PCA0CPH2    = 0xEC;          /* PCA0 Capture 3 High                      */
//sfr PCA0CPL3    = 0xED;          /* PCA0 Capture 4 Low                       */
//sfr PCA0CPH3    = 0xEE;          /* PCA0 Capture 4 High                      */
//sfr RSTSRC      = 0xEF;          /* Reset Source Configuration/Status        */
//sfr B           = 0xF0;          /* B Register                               */
#define P0MDIN _SIMUL_GET_BAD          /* Port 0 Input Mode                        */
#define P1MDIN _SIMUL_GET_BAD          /* Port 1 Input Mode                        */
#define P2MDIN _SIMUL_GET_BAD          /* Port 2 Input Mode                        */
#define P3MDIN _SIMUL_GET_BAD          /* Port 3 Input Mode                        */
#define P4MDIN _SIMUL_GET_BAD          /* Port 4 Input Mode                        */
///External Interrupt Priority 1

#endif                                 /* #define __REG51F380_H__            */
