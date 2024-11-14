#include <timers.h>
#include <adcs.h>
#include <cmps.h>
#include <pwms.h>
#include <PI_reg.h>

// Constants for shunt resistors
#define ADC_LSB 0.000805664f
#define K_fbVin 4.030903f
#define K_fbVout 2.02977f
#define K_fbVoutAlt 3.06122f
#define K_fbIl 2.2782f

Uint16 dutyPWM1 = 25;
float32 current_L_A3 = 0.0;     // ADC result for inductor current
float32 voltage_Vout_C3 = 0.0;  // ADC result for output voltage
float32 voltage_Vin_C5 = 0.0;   // ADC result for input voltage

void ePWM1_Init(void);
__interrupt void adca_prerusenie(void);
__interrupt void adcc_prerusenie(void);
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

    // GPIO settings
    GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;
    GpioCtrlRegs.GPAAMSEL.bit.GPIO23 = 0;
    GpioDataRegs.GPASET.bit.GPIO23 = 1;
    //GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    //GpioDataRegs.GPASET.bit.GPIO2 = 1;

    // Interrupts settings
    PieVectTable.ADCA1_INT = &adca_prerusenie;
    PieVectTable.ADCC1_INT = &adcc_prerusenie;
    PieVectTable.EPWM1_TZ_INT = &pwm1_TZ_interrupt;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx3 = 1;
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    IER = M_INT1 | M_INT2;
    EINT;

    // PWM, ADC and Comparator settings
    Timer0_Init();
    ADCA_Init();
    ADCC_Init();
    CMP4_Init();
    ePWM1_Init();

    // PI regulators initialization
    PIreg_Init(&voltage_PIreg, 60, 0.6, 125, 1);
    voltage_PIreg.ref = 1.2;      // BUCK voltage output

    // Disable access to protected registers
    EDIS;

    // Loop
    while (1);
}

__interrupt void adca_prerusenie(void)
{
    current_L_A3 = AdcaResultRegs.ADCRESULT0 * ADC_LSB * K_fbIl;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}
__interrupt void adcc_prerusenie(void)
{
    voltage_Vout_C3 = AdccResultRegs.ADCRESULT0 * ADC_LSB * K_fbVout;
    voltage_Vin_C5 = AdccResultRegs.ADCRESULT1 * ADC_LSB * K_fbVin;

    voltage_PIreg.meas = voltage_Vout_C3;
    PIreg_Func(&voltage_PIreg);
    EPwm1Regs.CMPA.bit.CMPA = voltage_PIreg.out;

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
