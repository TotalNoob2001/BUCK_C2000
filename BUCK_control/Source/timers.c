#include <timers.h>

void Timer0_Init(void)
{
    CpuTimer0Regs.TCR.bit.TSS = 1;      // zastavi casovac CpuTimer0

    CpuTimer0Regs.PRD.all = 20000000; // nastavi periodu casovaca na 200ms pri fsysclk = 100MHz
                                      // PRD = Ttimer / Tsysclk = 200ms / 10ns = 20 000 000
    CpuTimer0Regs.TCR.bit.TRB = 1; // Reload; naplni TIM register casovaca hodnotou v PRD registri
    CpuTimer0Regs.TCR.bit.TIF = 1; // zmaze priznak TIF (ten sa nastavuje ked TIM napocita do nuly)

    CpuTimer0Regs.TCR.bit.TIE = 1; // povoli generovanie ziadosti o prerusenie od Timer0
                                   // pri nastaveni TIF flagu
    CpuTimer0Regs.TCR.bit.TSS = 0;      // spusti casovac CpuTimer0
}
