/*
 * claTasks.h
 *
 *  Created on: 14 Nov 2024
 *      Author: Erik
 */

#ifndef CLATASKS_H_
#define CLATASKS_H_

#include <f28004x_device.h>
#include <f28004x_examples.h>

#define ADC_LSB 0.000805664f
#define K_fbVin 4.030903f
#define K_fbVout 2.02977f
#define K_fbVoutAlt 3.06122f

typedef struct {
    // Outer/global variables
    float32 ref;        // Requested value
    float32 out;        // Regulator output
    float32 meas;       // Measured value of active variable
    float32 Umax;       // Upper barrier
    float32 Umin;       // Lower barrier
    float32 Kp;         // Proportional gain
    float32 Ki;         // Integral gain
    // Inner/local variables
    float32 e[2];       // Regulator error
    float32 up;         // Proportional part of active variable
    float32 ui[2];      // Integrated part of active variable
    float32 u;          // Active variable u = up + ui
    float32 w;          // Anti-windup constant
} ts_PIreg;

extern ts_PIreg voltage_PIreg;

extern void PIreg_Func(ts_PIreg *reg);
extern void PIreg_Init(ts_PIreg *reg, float32 Kp, float32 Ki, float32 max, float32 min);

extern __interrupt void claTask1(void);
extern __interrupt void claTask8(void);

#endif /* CLATASKS_H_ */
