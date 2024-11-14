/*
 * sharedData.c
 *
 *  Created on: 14 Nov 2024
 *      Author: Erik
 */

#include <sharedData.h>

#pragma DATA_SECTION(Vout, "Cla1ToCpuMsgRAM");
float32 Vout;

#pragma DATA_SECTION(Vin, "Cla1ToCpuMsgRAM");
float32 Vin;

#pragma DATA_SECTION(voltage_PIreg, "Cla1ToCpuMsgRAM");
ts_PIreg voltage_PIreg;
