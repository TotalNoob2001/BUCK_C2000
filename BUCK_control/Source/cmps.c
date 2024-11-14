#include <cmps.h>

void CMP4_Init(void){
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

    Cmpss4Regs.DACHVALS.bit.DACVAL = 1835;
    Cmpss4Regs.COMPCTL.bit.COMPDACE = 1;
    EDIS;
}
