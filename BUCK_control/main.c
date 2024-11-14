#include <timers.h>
#include <adcs.h>
#include <cmps.h>
#include <pwms.h>
#include <sharedData.h>
#include <claTasks.h>

// Constants for shunt resistors
#define ADC_LSB 0.000805664f
#define K_fbIl 2.2782f

float32 current_L_A3 = 0.0;
extern float32 Vin;
extern float32 Vout;

void ePWM1_Init(void);
extern __interrupt void adca_prerusenie(void);
extern __interrupt void adcc_prerusenie(void);
__interrupt void pwm1_TZ_interrupt(void);

int main(void)
{
    InitSysCtrl();
    DINT;
    IER = 0x0000;
    IFR = 0x0000;
    InitPieCtrl();

    // Allow access to protected registers
    EALLOW;

    // LCA initialization
    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1; // Sets shared access CLA and CPU to memory
    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 1; // Sets RAMLS0 as RAM program for CLA
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 0;  // Sets RAMLS1 as RAM data for CLA
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 0;  // Sets RAMLS2 as RAM data for CLA
#pragma diag_suppres 770    // Warnings elimination
    Cla1Regs.MVECT1 = (uint16_t) &claTask1;
    Cla1Regs.MVECT8 = (uint16_t) &claTask8;
#pragma diag_default 770
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 11;
    Cla1Regs.MCTL.bit.IACKE = 1;
    Cla1Regs.MIER.bit.INT1 = 1;
    Cla1Regs.MIER.bit.INT8 = 1;
    Cla1Regs.MIFRC.bit.INT8 = 1;

    // GPIO settings
    GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;
    GpioCtrlRegs.GPAAMSEL.bit.GPIO23 = 0;
    GpioDataRegs.GPASET.bit.GPIO23 = 1;
    //GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    //GpioDataRegs.GPASET.bit.GPIO2 = 1;

    // Interrupts settings
    PieVectTable.EPWM1_TZ_INT = &pwm1_TZ_interrupt;
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    IER = M_INT2;
    EINT;

    // PWM, ADC and Comparator settings
    Timer0_Init();
    ADCA_Init();
    ADCC_Init();
    CMP4_Init();
    DELAY_US(1000);
    ePWM1_Init();

    // PI regulators initialization
    PIreg_Init(&voltage_PIreg, 60, 0.6, 125, 1);
    voltage_PIreg.ref = 1.2;      // BUCK voltage output

    // Disable access to protected registers
    EDIS;

    // Loop
    while (1)
        ;
}

__interrupt void adca_prerusenie(void)
{
    current_L_A3 = AdcaResultRegs.ADCRESULT0 * ADC_LSB * K_fbIl;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}
__interrupt void adcc_prerusenie(void)
{
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}

__interrupt void pwm1_TZ_interrupt(void)
{
    GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;

    //EALLOW;
    //EPwm1Regs.TZCLR.bit.OST = 1;
    //EPwm1Regs.TZCLR.bit.DCAEVT1 = 1;
    //EPwm1Regs.TZCLR.bit.INT = 1;
    //Cmpss4Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
    PieCtrlRegs.PIEACK.bit.ACK2 = 1;
    //EDIS;
}
