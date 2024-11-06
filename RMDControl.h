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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

// #define SERIAL_PORT "COM7"

#ifdef __cplusplus
extern "C" {
#endif

int RMD_Init(const char *serialPort);
int RMD_DeInit();
int RMD_GetMultiAngle_S(int64_t *angle, const uint8_t id);
int RMD_GoAngleAbsolute(int64_t angle, const uint8_t id);
int RMD_GoAngleRelative(int32_t angle, const uint8_t id);
int RMD_Stop(const uint8_t id);
int RMD_GetPI(uint8_t *arrPI, const uint8_t id);
int RMD_WriteAnglePI_ROM(const uint8_t *arrPI, const uint8_t id);
int RMD_WriteAnglePI_RAM(const uint8_t *arrPI, const uint8_t id);
uint8_t RMD_GetHeaderCheckSum(uint8_t *command);

#ifdef __cplusplus
}
#endif

#endif // RMDCONTROL_H