#ifndef RMDCONTROL_H
#define RMDCONTROL_H

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define SERIAL_PORT "COM7"

int RMD_Init();
int RMD_DeInit();
int RMD_GoToAngle(int64_t angle);
int RMD_Stop();

#endif