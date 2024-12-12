/*
 * SharedData.c
 *
 *  Created on: 14. 11. 2024
 *      Author: admin
 */

#include "SharedData.h"

#pragma DATA_SECTION(Vout, "Cla1ToCpuMsgRAM");
float32 Vout;

#pragma DATA_SECTION(Vin, "Cla1ToCpuMsgRAM");
float32 Vin;

#pragma DATA_SECTION(PIregulatorNapatia, "Cla1ToCpuMsgRAM");
tsPIreg PIregulatorNapatia;


