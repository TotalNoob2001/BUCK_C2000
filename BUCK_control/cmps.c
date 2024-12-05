/*
 * cmps.c
 *
 *  Created on: 31. 10. 2024
 *      Author: admin
 */

#include "cmps.h"

void CMP4_Init(void)
{
    EALLOW;
    AnalogSubsysRegs.CMPHPMXSEL.bit.CMP4HPMXSEL = 1;
    OutputXbarRegs.OUTPUT6MUX0TO15CFG.bit.MUX6 = 0;
    OutputXbarRegs.OUTPUT6MUXENABLE.bit.MUX6 = 1;
    EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX6 = 0;
    EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX6 = 1;

    Cmpss4Regs.COMPCTL.bit.COMPHSOURCE = 0;
    Cmpss4Regs.COMPCTL.bit.COMPHINV = 1;
    Cmpss4Regs.COMPCTL.bit.CTRIPOUTHSEL = 1;
    Cmpss4Regs.COMPCTL.bit.CTRIPHSEL = 1;

    Cmpss4Regs.DACHVALS.bit.DACVAL = 2048;
    //Uout=3V => Uoutfb=0,9802V ->
    //12bit dac vref=3,3V LSB=0,000805664V
    //DacVal=Uoutfb/LSB=1241 zaokruhlene
    //na cele cislo uint_16
    Cmpss4Regs.COMPCTL.bit.COMPDACE = 1;
    EDIS;

}


