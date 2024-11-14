#include <pwms.h>

void ePWM1_Init(void)
{
    EALLOW;
    // GPIO settings
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;

    // Clock setting
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;

    // Period and compare
    EPwm1Regs.TBPRD = 250;
    EPwm1Regs.CMPA.bit.CMPA = 125;
    EPwm1Regs.CMPB.bit.CMPB = 125;

    // Action qualifier
    EPwm1Regs.AQCTLA.bit.ZRO = 2;
    EPwm1Regs.AQCTLA.bit.CAU = 1;
    EPwm1Regs.AQCTLA.bit.CAD = 2;
    EPwm1Regs.AQCTLB.bit.PRD = 2;
    EPwm1Regs.AQCTLB.bit.CAU = 2;
    EPwm1Regs.AQCTLB.bit.CAD = 1;

    // Dead band
    EPwm1Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm1Regs.DBCTL.bit.POLSEL = 2;
    EPwm1Regs.DBCTL.bit.IN_MODE = 0;
    EPwm1Regs.DBRED.bit.DBRED = 10;
    EPwm1Regs.DBFED.bit.DBFED = 10;

    // ADC launching setting
    EPwm1Regs.ETSEL.bit.SOCASEL = 2;
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;
    EPwm1Regs.ETPS.bit.SOCAPRD = 2;

    // Turn on PWM in up-down mode
    EPwm1Regs.TBCTL.bit.CTRMODE = 2;

    // Trip zone
    EPwm1Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm1Regs.TZCTL.bit.TZA = 2;
    EPwm1Regs.TZCTL.bit.TZB = 2;
    EPwm1Regs.TZSEL.bit.OSHT6 = 0;
    EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3;
    EPwm1Regs.TZDCSEL.bit.DCAEVT1 = 1;
    EPwm1Regs.TZEINT.bit.OST = 1;
    EPwm1Regs.TZCLR.bit.OST = 1;
    EDIS;
}
