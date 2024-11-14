#include <adcs.h>

void ADCA_Init(void)
{
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 2;
    EDIS;

    SetVREF(ADC_ADCA, ADC_EXTERNAL, ADC_VREF3P3);

    EALLOW;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0;
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 0;

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5;
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 19;
}

void ADCC_Init(void)
{
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdccRegs.ADCCTL2.bit.PRESCALE = 2;
    EDIS;

    SetVREF(ADC_ADCC, ADC_EXTERNAL, ADC_VREF3P3);

    EALLOW;
    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 1;
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdccRegs.ADCINTSEL1N2.bit.INT1CONT = 1;

    AdccRegs.ADCSOC0CTL.bit.CHSEL = 3;
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 5;
    AdccRegs.ADCSOC0CTL.bit.ACQPS = 19;
    AdccRegs.ADCSOC1CTL.bit.CHSEL = 5;
    AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 5;
    AdccRegs.ADCSOC1CTL.bit.ACQPS = 19;
}
