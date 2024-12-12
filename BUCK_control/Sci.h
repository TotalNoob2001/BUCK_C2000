/*
 * Sci.h
 *
 *  Created on: 28. 11. 2024
 *      Author: admin
 */

#ifndef SCI_H_
#define SCI_H_

#include "f28004x_device.h"

typedef enum {
    busy,
    free
} teStavSeriovky;

extern teStavSeriovky stavSci;

extern __interrupt void Sci_Rx_prerusenie(void);
extern __interrupt void Sci_Tx_prerusenie(void);

extern void Sci_Init(void);

extern Uint16 Sci_SendString(char *str);

extern Uint16 Sci_SendString_ISR(char *string, teStavSeriovky *stav);
#endif /* SCI_H_ */
