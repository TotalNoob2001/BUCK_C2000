/*
 * Sci.c
 *
 *  Created on: May 9, 2023
 *      Author: AB17
 */

#include "Sci.h"

char rxBuffer[100];
Uint16 rxIndex = 0;
char *str;

teStavSeriovky stavSci = free;

__interrupt void Sci_Rx_prerusenie(void)
{
    rxBuffer[rxIndex] = SciaRegs.SCIRXBUF.bit.SAR;
    rxIndex++;

    if(rxIndex >= 100)
    {
        rxIndex = 0;
    }

    PieCtrlRegs.PIEACK.bit.ACK9 = 1;    // znovupovoli generovanie ziadosti o prerusenie na linke 9 na urovni CPU
}


__interrupt void Sci_Tx_prerusenie(void)
{
    static Uint16 i = 1;
    stavSci = busy;

    if(*(str + i) != '\0')
    {

        SciaRegs.SCITXBUF.bit.TXDT = *(str + i);
        i++;
    }
    else
    {
        i = 1;
        SciaRegs.SCICTL2.bit.TXINTENA = 0;  // zakaze prerusenie od vysielania
        stavSci = free;
    }

    PieCtrlRegs.PIEACK.bit.ACK9 = 1;    // znovupovoli generovanie ziadosti o prerusenie na linke 9 na urovni CPU
}


Uint16 Sci_SendString_ISR(char *string, teStavSeriovky *stav)
{
    while(*stav == busy)
    {
        ;
    }
    *stav = busy;

    str = string;                       // pointer str bude ukazovat na rovnaku adresu ako pointer string
    SciaRegs.SCICTL2.bit.TXINTENA = 1;  // povoli prerusenie od vysielania
    SciaRegs.SCITXBUF.bit.TXDT = *str;
}


Uint16 Sci_SendString(char *str)
{
    Uint16 i = 0;
    Uint16 timeout = 0;

    while(*(str + i) != '\0')
    {
        // Cakanie pokial je vysielaci buffer prazdny
        while((SciaRegs.SCICTL2.bit.TXRDY == 0) && (timeout < 10000))
        {
            timeout++;
        }

        SciaRegs.SCITXBUF.bit.TXDT = *(str + i);
        i++;
        timeout = 0;
    }

    return i;
}


void Sci_Init(void)
{
    SciaRegs.SCICTL1.bit.SWRESET = 0;

    SciaRegs.SCICCR.bit.STOPBITS = 0;       // ramec bude obsahovat jeden stop bit
    SciaRegs.SCICCR.bit.PARITYENA = 0;      // zakaze pouzitie paritneho bitu
    SciaRegs.SCICCR.bit.LOOPBKENA = 0;      // zakaze loopback rezim
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;  // zakaze adresny rezim
    SciaRegs.SCICCR.bit.SCICHAR = 7;        // Sci ramec bude obsahovat 8 datovych bitov

    SciaRegs.SCICTL1.bit.TXENA = 1;     // povoli vysielanie cez SCI A
    SciaRegs.SCICTL1.bit.RXENA = 1;     // povoli prijem cez SCI A

    // Nastavenie baudovej (komunikacnej) rychlosti na 9600baud
    // BRR = (LSPCLK / (SCI_Baudrate*8)) - 1 = (25MHz / (9600 * 8)) - 1 = 325 = 0x145
    SciaRegs.SCIHBAUD.bit.BAUD = 0x01;
    SciaRegs.SCILBAUD.bit.BAUD = 0x45;

    // Povolenie preruseni
    SciaRegs.SCICTL2.bit.TXINTENA = 1;  // povoli prerusenie od vysielania
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;// povoli prerusenie od prijmu

    SciaRegs.SCICTL1.bit.SWRESET = 1;
}

