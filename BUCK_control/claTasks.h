/*
 * ClaTasks.h
 *
 *  Created on: 14. 11. 2024
 *      Author: admin
 */

#ifndef CLATASKS_H_
#define CLATASKS_H_

#define ADC_lsb 0.0008056640625f
#define K_fbVin 4.030903f
#define K_fbVoutAlt 3.06122f

typedef struct {
    // --- Vonkajsie veliciny PI reg - tzn. ktore nastavujeme ------------
    float32 ref;    // ziadana hodnota
    float32 out;    // vystup z PI regulatora
    float32 meas;   // merana hodnota riadenej veliciny
    float32 Umax;   // horna hranica akcnej veliciny (horna saturacia)
    float32 Umin;   // spodna hranica akcnej veliciny (spodna saturacia)
    float32 Kp;     // proporcionalna konstanta PI regulatora
    float32 Ki;     // integracna konstanta PI regulatora
    // -------------------------------------------------------------------

    // --- Vnutorne veliciny PI regulatora - tzn. nemusia nas zaujimat ---
    float32 e[2];   // regulacna odchylka
    float32 ui[2];  // integracna cast akcnej veliciny
    float32 up;     // propocionalna cast akcnej veliciny
    float32 u;      // akcna velicina u = up + ui
    float32 w;      // anti-windup konstanta
    // -------------------------------------------------------------------
} tsPIreg;

extern tsPIreg PIregulatorNapatia;

extern void PIregFunc(tsPIreg *reg);
extern void PIregInit(tsPIreg *reg, float32 Kp, float32 Ki, float32 max, float32 min);

extern __interrupt void ClaTask1(void);
extern __interrupt void ClaTask8(void);

#endif /* CLATASKS_H_ */
