/*
 * Sci.h
 *
 *  Created on: May 9, 2023
 *      Author: AB17
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


//! \brief  Funkcia pre vyslanie stringu ukonceneho specialnym znakom \0
//! \param[in]  str pointer na string
//! \return pocet vyslanych znakov
extern Uint16 Sci_SendString(char *str);

extern Uint16 Sci_SendString_ISR(char *string, teStavSeriovky *stav);

#endif /* SCI_H_ */
