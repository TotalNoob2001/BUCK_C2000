#ifndef SCI_H_
#define SCI_H_

#include <f28004x_device.h>
#include <f28004x_examples.h>

typedef enum {
    busy,
    free
}teStatusSCI;

extern teStatusSCI statusSCI;

extern __interrupt void SCI_Rx_interrupt(void);
extern __interrupt void SCI_Tx_interrupt(void);

extern void SCI_Init(void);
extern uint16_t SCI_SendString(char *str);
extern uint16_t SCI_SendString_ISR(char *string, teStatusSCI *status);

#endif /* SCI_H_ */
