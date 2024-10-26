/**
 * @file RMDControl.c
 * @author worranhin (worranhin@foxmail.com)
 * @brief Source file of the RMD motor control library.
 * @version 0.1
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "RMDControl.h"

HANDLE hSerial;
DWORD bytesRead, bytesWritten;

/**
 * Initializes the RMD serial port for communication.
 *
 * @param serialPort The name of the serial port.
 *
 * @return 0 if the serial port is successfully initialized, -1 otherwise
 *
 * @throws None
 */
int RMD_Init(const char* serialPort) {
  hSerial = CreateFile(serialPort, GENERIC_READ | GENERIC_WRITE, 0, 0,
                       OPEN_EXISTING, 0, 0);
  if (hSerial == INVALID_HANDLE_VALUE) {
    return -1;
  }

  WINBOOL bSuccess = SetupComm(hSerial, 100, 100);
  if (!bSuccess) {
    CloseHandle(hSerial);
    return -1;
  }

  COMMTIMEOUTS commTimeouts = {0};
  commTimeouts.ReadIntervalTimeout = 50;          // 读取时间间隔超时
  commTimeouts.ReadTotalTimeoutConstant = 100;    // 总读取超时
  commTimeouts.ReadTotalTimeoutMultiplier = 10;   // 读取超时乘数
  commTimeouts.WriteTotalTimeoutConstant = 100;   // 总写入超时
  commTimeouts.WriteTotalTimeoutMultiplier = 10;  // 写入超时乘数

  bSuccess = SetCommTimeouts(hSerial, &commTimeouts);
  if (!bSuccess) {
    CloseHandle(hSerial);
    return -1;
  }

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hSerial, &dcbSerialParams)) {
    CloseHandle(hSerial);
    return -1;
  }
  dcbSerialParams.BaudRate = CBR_115200;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  if (!SetCommState(hSerial, &dcbSerialParams)) {
    CloseHandle(hSerial);
    return -1;
  }

  return 0;
}

/**
 * Deinitializes the RMD serial port by closing the handle to the serial port.
 *
 * @return 0 if the serial port handle is successfully closed, -1 otherwise
 *
 * @throws None
 */
int RMD_DeInit() {
  CloseHandle(hSerial);
  return 0;
}

int RMD_GetMultiAngle_S(int64_t* angle) {
  static const uint8_t command[] = {0x3E, 0x92, 0x01, 0x00, 0xD1};
  static const DWORD bytesToRead = 14;
  static uint8_t readBuf[14];
  int64_t motorAngle = 0;

  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    return -1;
  }

  if (!ReadFile(hSerial, readBuf, bytesToRead, &bytesRead, NULL)) {
    return -1;
  }

  // check received length
  if (bytesRead != bytesToRead) {
    return -1;
  }

  // check received format
  if (readBuf[0] != 0x3E || readBuf[1] != 0x92 || readBuf[2] != 0x01 ||
      readBuf[3] != 0x08 || readBuf[4] != 0xD9) {
    return -1;
  }

  // check data sum
  uint8_t sum = 0;
  for (int i = 5; i < 13; i++) {
    sum += readBuf[i];
  }
  if (sum != readBuf[13]) {
    return -1;
  }

  // motorAngle = readBuf[5] | (readBuf[6] << 8) | (readBuf[7] << 16) |
  // (readBuf[8] << 24);
  *(uint8_t*)(&motorAngle) = readBuf[5];
  *((uint8_t*)(&motorAngle) + 1) = readBuf[6];
  *((uint8_t*)(&motorAngle) + 2) = readBuf[7];
  *((uint8_t*)(&motorAngle) + 3) = readBuf[8];
  *((uint8_t*)(&motorAngle) + 4) = readBuf[9];
  *((uint8_t*)(&motorAngle) + 5) = readBuf[10];
  *((uint8_t*)(&motorAngle) + 6) = readBuf[11];
  *((uint8_t*)(&motorAngle) + 7) = readBuf[12];

  *angle = motorAngle;
  return 0;
}

/**
 * Sends a command to a serial port to move a motor to a specified angle.
 *
 * @param angle the desired angle to move the motor to, in 0.01 degrees
 *
 * @return 0 if the command was successfully sent, -1 otherwise
 *
 * @throws None
 */
int RMD_GoToAngle(int64_t angle) {
  int64_t angleControl = angle;
  uint8_t checksum = 0;

  static uint8_t command[] = {0x3E, 0xA3, 0x01, 0x08, 0xEA, 0xA0, 0x0F,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAF};

  command[5] = *(uint8_t*)(&angleControl);
  command[6] = *((uint8_t*)(&angleControl) + 1);
  command[7] = *((uint8_t*)(&angleControl) + 2);
  command[8] = *((uint8_t*)(&angleControl) + 3);
  command[9] = *((uint8_t*)(&angleControl) + 4);
  command[10] = *((uint8_t*)(&angleControl) + 5);
  command[11] = *((uint8_t*)(&angleControl) + 6);
  command[12] = *((uint8_t*)(&angleControl) + 7);

  for (int i = 5; i < 13; i++) {
    checksum += command[i];
  }
  command[13] = checksum;

  // 调试：打印命令
  // for (int i = 0; i < 14; i++) {
  //   printf("%02X ", command[i]);
  // }

  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    return -1;
  }
  return 0;
}

/**
 * Sends a command to stop a motor connected to a serial port.
 *
 * @return 0 if the command was successfully sent, -1 otherwise
 *
 * @throws None
 */
int RMD_Stop() {
  static uint8_t command[] = {0x3E, 0x81, 0x01, 0x00, 0xc0};

  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    return -1;
  }

  return 0;
}

/**
 * Sends a command to get motor's PI parameters
 *
 * @param arrPI a array to obtain motor's PI, [angleKp, angleKi, speedKp, speedKi, torqueKp, torqueKi]
 * @return 0 if the command was successfully sent, -1 otherwise
 */
int RMD_GetPI(uint8_t* arrPI) {
  static uint8_t command[] = {0x3E, 0X30, 0x01, 0x00, 0x6F};
  static const DWORD bytesToRead = 12;
  static uint8_t readBuf[12];

  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    return -1;
  }

  if (!ReadFile(hSerial, readBuf, bytesToRead, &bytesRead, NULL)) {
    return -1;
  }

  if (bytesRead != bytesToRead) {
    return -1;
  }

  if (readBuf[0] != 0x3E || readBuf[1] != 0x30 || readBuf[2] != 0x01 ||
      readBuf[3] != 0x06 || readBuf[4] != 0x75) {
    return -1;
  }

  uint8_t sum = 0;
  for (int i = 5; i < 11; i++) {
    sum += readBuf[i];
  }
  if (sum != readBuf[11]) {
    return -1;
  }

  for (int i = 0; i < 6; i++) {
    arrPI[i] = (uint8_t)readBuf[5 + i];
  }

  return 0;
}

/**
 * Sends a command to debug PI parameters
 *
 * @param arrPI a array to config target PI, [angleKp, angleKi, speedKp, speedKi, torqueKp, torqueKi]
 * @return 0 if the command was successfully sent, -1 otherwise
 */
int RMD_WriteAnglePI_RAM(const uint8_t* arrPI) {
  static uint8_t command[12] = {0x3E, 0x31, 0x01, 0x06, 0x76};
  uint8_t checksum = 0;
  for (int i = 0; i < 6; i++) {
    command[5 + i] = (uint8_t)arrPI[i];
    checksum += command[5 + i];
  }
  command[11] = checksum;

  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    return -1;
  }

  return 0;
}

/**
 * Sends a command to change PI parameters
 *
 * @param arrPI a array to config target PI, [angleKp, angleKi, speedKp, speedKi, torqueKp, torqueKi]
 * @return 0 if the command was successfully sent, -1 otherwise
 */
int RMD_WriteAnglePI_ROM(const uint8_t* arrPI) {
  static uint8_t command[12] = {0x3E, 0x32, 0x01, 0x06, 0x77};
  uint8_t checksum = 0;
  for (int i = 0; i < 6; i++) {
    command[5 + i] = (uint8_t)arrPI[i];
    checksum += command[5 + i];
  }
  command[11] = checksum;

  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    return -1;
  }

  return 0;
}
