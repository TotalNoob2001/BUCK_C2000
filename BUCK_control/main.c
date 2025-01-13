

/**
 * main.c
 */
#include "f28004x_device.h"
#include "f28004x_examples.h"
#include "cmps.h"
#include "ClaTasks.h"
#include "Sci.h"


#define k_fbIl 2.2782f

float32 CurrentLA3= 0.0;




//deklaracia preruseni

extern __interrupt void ADC_A_prerusenie(void);
extern __interrupt void ADC_C_prerusenie(void);
__interrupt void Pwm1_TZ_Interrupt(void);


//deklaracie inicializacnych funkcii
void ePWM1_Init(void);
void ADCA_Init(void);
void ADCC_Init(void);

extern float32 Vin;
extern float32 Vout;

Uint16 rxData;
Uint16 data;
Uint16 pocetTxZnakov;


char string1[] = {'N','A','Z','D','A','R',' ','\0'};
char string2[] = {'C','A','U',' ','\0'};
char string3[] = {'T','e','x','a','s',' ','I','n','s','t','r','u','m','e','n','t','s','\0'};




int main(void)
{
    InitSysCtrl();

    DINT;
    IER = 0x0000;
    IFR = 0x0000;

    InitPieCtrl();

    EALLOW;


    PieVectTable.EPWM1_TZ_INT = &Pwm1_TZ_Interrupt;
    PieVectTable.SCIA_RX_INT = &Sci_Rx_prerusenie;          // na adresu vektora prerusenia SCIA_RX_INT ulozi adresu obsluzneho
        // podprogramu Sci_Rx_prerusenie
    PieVectTable.SCIA_TX_INT = &Sci_Tx_prerusenie;          // na adresu vektora prerusenia SCIA_TX_INT ulozi adresu obsluzneho
    EDIS;


    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;      // povoli prerusenie INT9.1 (SCIA_RX_INT) na urovni PIE modulu
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    IER = M_INT2| M_INT9;
    EINT;

    // --- Inicializacia CLA ------------------------------------------------

    EALLOW;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;    // nastavi zdielany pristup (CPU a CLA) k pamati RAMLS0
    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;    // nastavi zdielany pristup (CPU a CLA) k pamati RAMLS1
    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;    // nastavi zdielany pristup (CPU a CLA) k pamati RAMLS2

    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 1;    // nastavi RAMLS0 ako program RAM pre CLA
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 0;    // nastavi RAMLS1 ako data RAM pre CLA
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 0;    // nastavi RAMLS2 ako data RAM pre CLA

    #pragma diag_suppress 770
    Cla1Regs.MVECT1 = (Uint16) &ClaTask1;     // do registra MVECT1 zapise adruse ClaTasku1
    Cla1Regs.MVECT8 = (Uint16) &ClaTask8;     // do registra MVECT8 zapise adruse ClaTasku8
    #pragma diag_default 770

    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 11;    // spustac Task-u 1 bude ADC_C_INT1 (ziadost o prerusenie od ADC C)
    Cla1Regs.MCTL.bit.IACKE = 1;        // povoli softverove spustanie ClaTask-ov
    Cla1Regs.MIER.bit.INT1 = 1;         // povoli Task1
    Cla1Regs.MIER.bit.INT8 = 1;         // povoli Task8
    EDIS;





    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;    // nastavi Gpio28 ako Rx pin periferie SCI A
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;    // nastavi Gpio29 ako Tx pin periferie SCI A
    GpioCtrlRegs.GPAMUX1.bit.GPIO0=1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1=1;
    GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;  // nastavi GPIO23 ako vystup
    GpioCtrlRegs.GPAAMSEL.bit.GPIO23 = 0;
    GpioDataRegs.GPASET.bit.GPIO23 = 1;
  //  GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // nastavi GPIO2 ako vystup pre rezistor R=2;
  //  GpioDataRegs.GPASET.bit.GPIO2 = 1;

    EDIS;

    EALLOW;
    Cla1Regs.MIFRC.bit.INT8 = 1;    // softverovo spustime ClaTask8, ktory vykona inicializaciu prvok struktury PI regulatora
    EDIS;

    ADCA_Init();
    ADCC_Init();
    CMP4_Init();
    Sci_Init();
    DELAY_US(1000);
    ePWM1_Init();

    EPwm1Regs.TZCLR.bit.OST=1;

    while(1)
    {
        Sci_SendString(string1);
        DELAY_US(1000000);
        Sci_SendString_ISR(string3, &stavSci);
        DELAY_US(1000000);
    }


	return 0;
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
    AdccRegs.ADCINTSEL1N2.bit.INT1CONT = 1; // pouzivatel musi mazat INTFLG, aby mohli byt generovane
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
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;  // zmaze flag ADCINT1
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;        // znovupovoli prerusenia na linke INT1 na urovni CPU
}



