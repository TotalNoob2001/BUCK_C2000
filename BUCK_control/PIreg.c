/*
 * PIreg.c
 *
 *  Created on: 7. 11. 2024
 *      Author: admin
 */

#include "PIreg.h"

tsPIreg PIregulatorNapatia;


void PIregFunc(tsPIreg *reg)
{
    //vypocet reg odchylky
    reg->e[0] = reg->ref - reg->meas;
    //vypocet proporcionalnej zlozky
    reg->up = reg->Kp * reg->e[0];
    //vypocet integracnej zlozky
    reg->ui[0] = (reg->e[0]+reg->e[1])*reg->Ki * reg->w + reg->ui[1];
    //vypocet akcnej veliciny
    reg->u = reg->ui[0]+reg->up;

    //po skonceni vypoctov treba posunut vzorky o 1
    reg->e[1] = reg->e[0];
    reg->ui[1] = reg->ui[0];

    reg->out = __fsat(reg->u , reg->Umax , reg->Umin);
    //najrychlajsi sposob saturacie -menej vypoctovych cyklov
    if(reg->u == reg->out)
        {
            //vystup regulatora NIE JE saturovany
            reg->w = 1.0;
        }
        else
        {
            // vystup regulatora JE saturovany
            reg->w = 0.0;
        }

}
void PIregInit(tsPIreg *reg,float32 Kp, float32 Ki, float32 max, float32 min)
{
    reg->Umax = max;
    reg->Umin = min;
    reg->Kp = Kp;
    reg->Ki = Ki;

    reg->w = 1.0;
    reg->e[1] = 0;
    reg->ui[1] = 0;
}
