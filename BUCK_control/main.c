

/**
 * main.c
 */
#include "f28004x_device.h"
#include "f28004x_examples.h"
#include "cmps.h"
#include "PIreg.h"

#define ADC_lsb 0.000805664f
#define K_fbVin 4.030903f
#define K_fbVout 2.02977f    //DAC val 1835 @3V trip zone
#define K_fbVoutAlt 3.06122f //DAC val 1241 @3V trip zone
//skontrolovat osadenie 680R rezistorov podla toho K_fbVout/K_fbVoutAlt
#define k_fbIl 2.2782f

float32 CurrentLA3= 0.0;
float32 VoltageOutC3= 0.0;
float32 VoltageInC5= 0.0;

Uint16 dutyPWM1 = 25;

//deklaracia preruseni

__interrupt void ADC_A_prerusenie(void);
__interrupt void ADC_C_prerusenie(void);
__interrupt void Pwm1_TZ_Interrupt(void);


//deklaracie inicializacnych funkcii
void ePWM1_Init(void);
void ADCA_Init(void);
void ADCC_Init(void);

int main(void)
{
    InitSysCtrl();

    DINT;
    IER = 0x0000;
    IFR = 0x0000;

    InitPieCtrl();

    EALLOW;

    PieVectTable.ADCA1_INT = &ADC_A_prerusenie;
    PieVectTable.ADCC1_INT = &ADC_C_prerusenie;
    PieVectTable.EPWM1_TZ_INT = &Pwm1_TZ_Interrupt;

    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx3 = 1;
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    IER = M_INT1| M_INT2;
    EINT;

    EALLOW;

    GpioCtrlRegs.GPAMUX1.bit.GPIO0=1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1=1;
    GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;  // nastavi GPIO23 ako vystup
    GpioCtrlRegs.GPAAMSEL.bit.GPIO23 = 0;
    GpioDataRegs.GPASET.bit.GPIO23 = 1;
  //  GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // nastavi GPIO2 ako vystup pre rezistor R=2;
  //  GpioDataRegs.GPASET.bit.GPIO2 = 1;

    EDIS;

    PIregInit(&PIregulatorNapatia, 65, 7, 125, 1); //kp ki max min
    PIregulatorNapatia.ref=2.0;

    ADCA_Init();
    ADCC_Init();
    CMP4_Init();
    ePWM1_Init();

    EPwm1Regs.TZCLR.bit.OST=1;

    while(1)
    {

    }
}

void ePWM1_Init(void)
{
   EPwm1Regs.TBCTL.bit.CLKDIV=0;
   EPwm1Regs.TBCTL.bit.HSPCLKDIV=0;
   EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;

   EPwm1Regs.TBPRD=250;
   EPwm1Regs.CMPA.bit.CMPA = 125;

   EPwm1Regs.AQCTLA.bit.CAU=1;
   EPwm1Regs.AQCTLA.bit.CAD = 2;

   EPwm1Regs.AQCTLB.bit.CAU=2;
   EPwm1Regs.AQCTLB.bit.CAD = 1;

   EPwm1Regs.DBCTL.bit.OUT_MODE = 3;   // povoli Dead Band modul, t.j. povoli DBRED aj DBFED
   EPwm1Regs.DBCTL.bit.POLSEL = 2;     // neguje vystup z DBFED a neneguje vystup z DBRED
   EPwm1Regs.DBCTL.bit.IN_MODE = 0;

   EPwm1Regs.DBRED.bit.DBRED = 10;      // nastavuje oneskorenie nabeznej hrany na 50ns pri fsysclk = 100MHz
   EPwm1Regs.DBFED.bit.DBFED = 10;      // nastavuje oneskorenie dobeznej hrany na 50ns pri fsysclk = 100MHz

   // Nastavenie spustania AD prevodu SOCA
   EPwm1Regs.ETSEL.bit.SOCASEL = 2;
   EPwm1Regs.ETSEL.bit.SOCAEN = 1;
   EPwm1Regs.ETPS.bit.SOCAPRD = 2;


   EPwm1Regs.TBCTL.bit.CTRMODE = 2;

   //TripZone
   EALLOW;
   EPwm1Regs.TZSEL.bit.DCAEVT1 = 1;
   EPwm1Regs.TZCTL.bit.TZA = 2;        // nastavi na vystupe ePWM1A uroven Low
   EPwm1Regs.TZCTL.bit.TZB = 2;        // nastavi na vystupe ePWM1B uroven Low

   EPwm1Regs.TZSEL.bit.OSHT6 = 0;
   EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3;    // pripoji signal TRIPIN4 na DCAH
   EPwm1Regs.TZDCSEL.bit.DCAEVT1 = 1;          // udalost DCAEVT1 nastane pri dobeznej hrane signalu DCAH

   EPwm1Regs.TZEINT.bit.OST = 1;
   EDIS;
}

__interrupt void Pwm1_TZ_Interrupt (void)
{
    //EPwm1Regs.TZCLR.bit.OST=1;
    //EPwm1Regs.TZCLR.bit.DCAEVT1=1;
    //EPwm1Regs.TZCLR.bit.INT=1;
    //Cmpss4Regs.COMPSTSCLR.bit.HLATCHCLR=1;
    GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;
    PieCtrlRegs.PIEACK.bit.ACK2 = 1;
}

void ADCA_Init(void)
{ //50MHz
    EALLOW;
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
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

    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    EDIS;
}

void ADCC_Init(void)
{
    EALLOW;
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;   // generuje pulz ziadosti o prerusenie 1 cyklus ADCCLK pred ukoncenim prevodu
    AdccRegs.ADCCTL2.bit.PRESCALE = 2;      // nastavi preddelicku hodin, t.j. ADCCLK = fsysclk/2 = 100MHz/2 = 50MHz
    EDIS;

    SetVREF(ADC_ADCC, ADC_EXTERNAL, ADC_VREF3P3);

    EALLOW;
    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 1;  // EOC1 bude generovat ziadost o prerusenie
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;    // povoli generovanie ziadosti o prerusenie
    AdccRegs.ADCINTSEL1N2.bit.INT1CONT = 0; // pouzivatel musi mazat INTFLG, aby mohli byt generovane
                                            // dalsie ziadosti o prerusenie

    AdccRegs.ADCSOC0CTL.bit.CHSEL = 3;      // SOC0 bude prevadzat vstupny kanal C3
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 5;    // PWM1 SOCA bude spustat prevod SOC0
    AdccRegs.ADCSOC0CTL.bit.ACQPS = 19;     // vzorkovacie okno velkosti (19+1)*ADCLK = 20/50MHz = 400ns

    AdccRegs.ADCSOC1CTL.bit.CHSEL = 5;      // SOC1 bude prevadzat vstupny kanal C5
    AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 5;    // PWM1 SOCA bude spustat prevod SOC1
    AdccRegs.ADCSOC1CTL.bit.ACQPS = 19;     // vzorkovacie okno velkosti (19+1)*ADCLK = 20/50MHz = 400ns


    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;      // zapne napajanie analogovej casti ADC
    EDIS;
}

__interrupt void ADC_A_prerusenie(void)
{
    CurrentLA3= AdcaResultRegs.ADCRESULT0 * ADC_lsb * k_fbIl;



    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;  // zmaze flag ADCINT1
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;        // znovupovoli prerusenia na linke INT1 na urovni CPU
}

__interrupt void ADC_C_prerusenie(void)
{
    VoltageOutC3 = AdccResultRegs.ADCRESULT0 * ADC_lsb*K_fbVoutAlt;
    VoltageInC5 = AdccResultRegs.ADCRESULT1 * ADC_lsb*K_fbVin;

    PIregulatorNapatia.meas = VoltageOutC3;
    PIregFunc(&PIregulatorNapatia);
    EPwm1Regs.CMPA.bit.CMPA = PIregulatorNapatia.out;

    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;  // zmaze flag ADCINT1
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;        // znovupovoli prerusenia na linke INT1 na urovni CPU
}



