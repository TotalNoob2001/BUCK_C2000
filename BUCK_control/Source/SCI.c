#include <SCI.h>

char RxBuffer[100];
char *str;
Uint16 RxIndex = 0;
teStatusSCI statusSCI = free;

__interrupt void SCI_Rx_interrupt(void)
{
    RxBuffer[RxIndex] = SciaRegs.SCIRXBUF.bit.SAR;
    RxIndex++;
    if (RxIndex >= 100)
        RxIndex = 0;

    PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

__interrupt void SCI_Tx_interrupt(void)
{
    static uint16_t i = 1;
    statusSCI = busy;
    if (*(str + 1) != '\0')
    {
        SciaRegs.SCITXBUF.bit.TXDT = *(str + i);
        i++;
    }
    else
    {
        SciaRegs.SCICTL2.bit.TXINTENA = 0;
        statusSCI = free;
        i = 1;
    }

    PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

Uint16 SCI_SendString_ISR(char *string, teStatusSCI *status)
{
    while (*status == busy)
    {

    }
    *status = busy;
    str = string;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCITXBUF.bit.TXDT = *str;
}

Uint16 SCI_SendString(char *str)
{
    uint16_t i = 0;
    uint16_t timeout = 0;

    while (*(str + 1) != '\0')
    {
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

void SCI_Init(void){
    SciaRegs.SCICTL1.bit.SWRESET = 0;

    SciaRegs.SCICCR.bit.STOPBITS = 0;
    SciaRegs.SCICCR.bit.PARITYENA = 0;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0;
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
    SciaRegs.SCICCR.bit.SCICHAR = 7;

    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.RXENA = 1;

    // BRR = (25MHz / (9600 * 8)) - 1 = 325
    SciaRegs.SCIHBAUD.bit.BAUD = 0x01;
    SciaRegs.SCILBAUD.bit.BAUD = 0x45;

    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    SciaRegs.SCICTL1.bit.SWRESET = 1;
}
