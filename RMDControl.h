/**
 * @file RMDControl.h
 * @author worranhin (worranhin@foxmail.com)
 * @brief Head file of the RMD motor control libray.
 * @version 0.1
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RMDCONTROL_H
#define RMDCONTROL_H

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// #define SERIAL_PORT "COM7"

#ifdef __cplusplus
extern "C" {
#endif

int RMD_Init(const char* serialPort);
int RMD_DeInit();
int RMD_GetMultiAngle_S(int64_t* angle);
int RMD_GoToAngle(int64_t angle);
int RMD_Stop();
int RMD_GetPI(uint8_t *arrPI);
int RMD_WriteAnglePI_ROM(const uint8_t *arrPI);
int RMD_WriteAnglePI_RAM(const uint8_t *arrPI);

#ifdef __cplusplus
}
#endif

#endif  // RMDCONTROL_H