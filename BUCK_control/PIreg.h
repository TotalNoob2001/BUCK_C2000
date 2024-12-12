/*
 * PIreg.h
 *
 *  Created on: 7. 11. 2024
 *      Author: admin
 */

#ifndef PIREG_H_
#define PIREG_H_

#include "f28004x_device.h"
#include "f28004x_examples.h"

typedef struct{
    float32 ref; //ziadana hodnota
    float32 out; //vzstup z reg
    float32 meas; //merana hodnota
    float32 Umax; //horna hranica akcnej veliciny
    float32 Umin; //spodna hranica akcnej veliciny
    float32 Kp; //proporcionalna konstanta
    float32 Ki; //integracna konstanta

    float32 e[2]; //regulacna odchylka;    pole-> e[0] aktualna, e[1] vzorka o jednu
    float32 ui[2];      //integracna cast akcnej veliciny
    float32 up;         //proporcionalna cast
    float32 u;          //akcna velicina u = up + ui
    float32 w;          //anti windup konstanta
}tsPIreg;

extern tsPIreg PIregulatorNapatia;

extern void PIregFunc(tsPIreg *reg);
extern void PIregInit(tsPIreg *reg,float32 Kp, float32 Ki, float32 max, float32 min);

#endif /* PIREG_H_ */
