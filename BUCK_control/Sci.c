/*
 * Sci.c
 *
 *  Created on: 28. 11. 2024
 *      Author: admin
 */
#include "Sci.h"

char rxBuffer[3];
Uint16 rxIndex = 0;
char *str;

teStavSeriovky stavSci = free;

__interrupt void Sci_Rx_prerusenie(void)
{
	rxBuffer[2] = rxBuffer[1];
	rxBuffer[1] = rxBuffer[0];
	rxBuffer[0] = SciaRegs.SCIRXBUF.bit.SAR;
	Sci_SendString(rxBuffer);
	if (rxBuffer[2] == 'z' && rxBuffer[1] == 'a' && rxBuffer[0] == 'p')
		GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;

	if (rxBuffer[2] == 'v' && rxBuffer[1] == 'y' && rxBuffer[0] == 'p')
		GpioDataRegs.GPBSET.bit.GPIO34 = 1;

	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

__interrupt void Sci_Tx_prerusenie(void)
{
	static Uint16 i = 1;
	stavSci = busy;
	if (*(str + i) != '\0')
	{

		SciaRegs.SCITXBUF.bit.TXDT = *(str + i);
		i++;
	}
	else
	{
		i = 1;
		SciaRegs.SCICTL2.bit.TXINTENA = 0; // zakaze prerusenie od vysielania
		stavSci = free;
	}
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

Uint16 Sci_SendString_ISR(char *string, teStavSeriovky *stav)
{
	while (*stav == busy)
	{
		;
	}
	*stav = busy;
	str = string;
	SciaRegs.SCICTL2.bit.TXINTENA = 1;
	SciaRegs.SCITXBUF.bit.TXDT = *str;
}

Uint16 Sci_SendString(char *str)
{
	Uint16 i = 0;
	Uint16 timeout = 0;
	while (*(str + i) != '\0')
	{
		// Cakanie pokial je vysielaci buffer prazdny
		while ((SciaRegs.SCICTL2.bit.TXRDY == 0) && (timeout < 10000))
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
	SciaRegs.SCICCR.bit.STOPBITS = 0; // ramec bude obsahovat jeden stop bit
	SciaRegs.SCICCR.bit.PARITYENA = 0;     // zakaze pouzitie paritneho bitu
	SciaRegs.SCICCR.bit.LOOPBKENA = 0;      // zakaze loopback rezim
	SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;  // zakaze adresny rezim
	SciaRegs.SCICCR.bit.SCICHAR = 7;

	SciaRegs.SCICTL1.bit.TXENA = 1;     // povoli vysielanie cez SCI A
	SciaRegs.SCICTL1.bit.RXENA = 1;     // povoli prijem cez SCI A

	// Nastavenie baudovej (komunikacnej) rychlosti na 9600baud
	// BRR = (LSPCLK / (SCI_Baudrate*8)) - 1 = (25MHz / (9600 * 8)) - 1 = 325 = 0x0145
	SciaRegs.SCIHBAUD.bit.BAUD = 0x01;
	SciaRegs.SCILBAUD.bit.BAUD = 0x45;

	SciaRegs.SCICTL2.bit.TXINTENA = 1;  // povoli prerusenie od vysielania
	SciaRegs.SCICTL2.bit.RXBKINTENA = 1;  // povoli prerusenie od prijmu

	SciaRegs.SCICTL1.bit.SWRESET = 1;
}

