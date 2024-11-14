#include <PI_reg.h>

ts_PIreg voltage_PIreg;

void PIreg_Func(ts_PIreg *reg){
    // Regulator error equation
    reg->e[0] = reg->ref - reg->meas;
    // Proportional gain equation
    reg->up = reg->Kp * reg->e[0];
    // Integrated gain equation
    reg->ui[0] = (reg->e[0] + reg->e[1]) * reg->Ki * reg->w + reg->ui[1];
    // Active variable equation
    reg->u = reg->ui[0] + reg->up;
    // Shift results
    reg->e[1] = reg->e[0];
    reg->ui[1] = reg->ui[0];

    // Max/Min barriers condition
    reg->out = __fsat(reg->u, reg->Umax, reg->Umin);

    // Anti-windup setting
    if (reg->u == reg->out)
        reg->w = 1.0;
    else
        reg->w = 0.0;
}

void PIreg_Init(ts_PIreg *reg, float32 Kp, float32 Ki, float32 max, float32 min){
    reg->Kp = Kp;
    reg->Ki = Ki;
    reg->Umax = max;
    reg->Umin = min;

    reg->w = 1.0;
    reg->e[1] = 0.0;
    reg->ui[1] = 0.0;
}
