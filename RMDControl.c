#include "RMDControl.h"

HANDLE hSerial;
char buffer[100];
DWORD bytesRead, bytesWritten;

/**
 * Initializes the RMD serial port for communication.
 *
 * @return 0 if the serial port is successfully initialized, -1 otherwise
 *
 * @throws None
 */
int RMD_Init() {
  hSerial = CreateFile(SERIAL_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0,
                       OPEN_EXISTING, 0, 0);
  if (hSerial == INVALID_HANDLE_VALUE) {
    printf("Error opening serial port!\n");
    return -1;
  }

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hSerial, &dcbSerialParams)) {
    printf("Error getting serial port state!\n");
    CloseHandle(hSerial);
    return -1;
  }
  dcbSerialParams.BaudRate = CBR_115200;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  if (!SetCommState(hSerial, &dcbSerialParams)) {
    printf("Error setting serial port state!\n");
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
    printf("Error writing to serial port!\n");
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
  static uint8_t command[] = {0x3E, 0x81, 0x01, 0x00, 0x00};
  uint8_t checksum = 0;
  for (int i = 0; i < 4; i++) {
    checksum += command[i];
  }

  command[4] = checksum;
  if (!WriteFile(hSerial, command, sizeof(command), &bytesWritten, NULL)) {
    printf("Error writing to serial port!\n");
    return -1;
  }

  return 0;
}