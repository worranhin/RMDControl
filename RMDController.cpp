#include "RMDController.h"

// 日志输出-----------------------------------------
#define HIGHLIGHT_(...)                                                        \
  do {                                                                         \
    printf("\033[35minfo - \033[0m" __VA_ARGS__);                              \
    printf("\n");                                                              \
  } while (false)

#define WARNING_(...)                                                          \
  do {                                                                         \
    printf("\033[33mwarn - \033[0m" __VA_ARGS__);                              \
    printf("\n");                                                              \
  } while (false)

#define PASS_(...)                                                             \
  do {                                                                         \
    printf("\033[32minfo - \033[0m" __VA_ARGS__);                              \
    printf("\n");                                                              \
  } while (false)

#define ERROR_(...)                                                            \
  do {                                                                         \
    printf("\033[31m err - \033[0m" __VA_ARGS__);                              \
    printf("\n");                                                              \
  } while (false)

#define INFO_(...)                                                             \
  do {                                                                         \
    printf("info - " __VA_ARGS__);                                             \
    printf("\n");                                                              \
  } while (false)

// 构造析构------------------------------------------
RMD::RMD(const char *serialPort) : _serialPort(serialPort) { _isInit = Init(); }
RMD::~RMD() { CloseHandle(_handle); }

// 句柄初始化-----------------------------------------
bool RMD::Init() {
  _handle = CreateFile(_serialPort, GENERIC_READ | GENERIC_WRITE, 0, 0,
                       OPEN_EXISTING, 0, 0);
  if (_handle == INVALID_HANDLE_VALUE) {
    ERROR_("Invalid serialport");
    return false;
  }

  BOOL bSuccess = SetupComm(_handle, 100, 100);
  if (!bSuccess) {
    ERROR_("Failed to init serial device buffer");
    return false;
  }

  COMMTIMEOUTS commTimeouts = {0};
  commTimeouts.ReadIntervalTimeout = 50;         // 读取时间间隔超时
  commTimeouts.ReadTotalTimeoutConstant = 100;   // 总读取超时
  commTimeouts.ReadTotalTimeoutMultiplier = 10;  // 读取超时乘数
  commTimeouts.WriteTotalTimeoutConstant = 100;  // 总写入超时
  commTimeouts.WriteTotalTimeoutMultiplier = 10; // 写入超时乘数

  bSuccess = SetCommTimeouts(_handle, &commTimeouts);
  if (!bSuccess) {
    ERROR_("Failed to config Timeouts value");
    return false;
  }

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(_handle, &dcbSerialParams)) {
    ERROR_("Failed to obtain device comm status");
    return false;
  }
  dcbSerialParams.BaudRate = CBR_115200;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  if (!SetCommState(_handle, &dcbSerialParams)) {
    ERROR_("Failed to config DCB");
    return false;
  }

  return true;
}

// 是否初始化-----------------------------------------
bool RMD::isInit() { return _isInit; }

// 设备重连------------------------------------------
bool RMD::Reconnect() {
  if (_handle != nullptr) {
    CloseHandle(_handle);
  }
  return Init();
}

// 获取当前角度---------------------------------------
bool RMD::GetMultiAngle_s(int64_t *angle, const uint8_t id) {
  uint8_t command[] = {0x3E, 0x92, 0x00, 0x00, 0x00};
  command[2] = id;
  command[4] = GetHeaderCheckSum(command);
  const DWORD bytesToRead = 14;
  uint8_t readBuf[bytesToRead];
  int64_t motorAngle = 0;

  if (!WriteFile(_handle, command, sizeof(command), &_bytesWritten, NULL)) {
    ERROR_("GetMultiAngle_s: Failed to send command to device");
    return false;
  }

  if (!ReadFile(_handle, readBuf, bytesToRead, &_bytesRead, NULL)) {
    ERROR_("GetMultiAngle_s: Failed to revice data from device");
    return false;
  }

  // check received length
  if (_bytesRead != bytesToRead) {
    ERROR_("GetMultiAngle_s: Abnormal received data - byte count");
    return false;
  }

  // check received format
  if (readBuf[0] != 0x3E || readBuf[1] != 0x92 || readBuf[2] != id ||
      readBuf[3] != 0x08 || readBuf[4] != (0x3E + 0x92 + id + 0x08)) {
    ERROR_("GetMultiAngle_s: Abnormal received data - frame header");
    return false;
  }

  // check data sum
  uint8_t sum = 0;
  for (int i = 5; i < 13; i++) {
    sum += readBuf[i];
  }
  if (sum != readBuf[13]) {
    ERROR_("GetMultiAngle_s: Abnormal received data - data");
    return false;
  }

  // motorAngle = readBuf[5] | (readBuf[6] << 8) | (readBuf[7] << 16) |
  // (readBuf[8] << 24);
  *(uint8_t *)(&motorAngle) = readBuf[5];
  *((uint8_t *)(&motorAngle) + 1) = readBuf[6];
  *((uint8_t *)(&motorAngle) + 2) = readBuf[7];
  *((uint8_t *)(&motorAngle) + 3) = readBuf[8];
  *((uint8_t *)(&motorAngle) + 4) = readBuf[9];
  *((uint8_t *)(&motorAngle) + 5) = readBuf[10];
  *((uint8_t *)(&motorAngle) + 6) = readBuf[11];
  *((uint8_t *)(&motorAngle) + 7) = readBuf[12];

  *angle = motorAngle;
  return true;
}

// 帧头计算------------------------------------------
uint8_t RMD::GetHeaderCheckSum(uint8_t *command) {
  uint8_t sum = 0x00;
  for (int i = 0; i < 4; ++i) {
    sum += command[i];
  }
  return sum;
}

// 旋转角度------------------------------------------
bool RMD::GoToAngle(int64_t angle, const uint8_t id) {
  int64_t angleControl = angle;
  uint8_t checksum = 0;

  uint8_t command[] = {0x3E, 0xA3, 0x00, 0x08, 0x00, 0xA0, 0x0F,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAF};

  command[2] = id;
  command[4] = GetHeaderCheckSum(command);

  command[5] = *(uint8_t *)(&angleControl);
  command[6] = *((uint8_t *)(&angleControl) + 1);
  command[7] = *((uint8_t *)(&angleControl) + 2);
  command[8] = *((uint8_t *)(&angleControl) + 3);
  command[9] = *((uint8_t *)(&angleControl) + 4);
  command[10] = *((uint8_t *)(&angleControl) + 5);
  command[11] = *((uint8_t *)(&angleControl) + 6);
  command[12] = *((uint8_t *)(&angleControl) + 7);

  for (int i = 5; i < 13; i++) {
    checksum += command[i];
  }
  command[13] = checksum;

  if (!WriteFile(_handle, command, sizeof(command), &_bytesWritten, NULL)) {
    ERROR_("GoToAngle: Failed to send command to device");
    return false;
  }
  return true;
}

// 急停----------------------------------------------
bool RMD::Stop(const uint8_t id) {
  uint8_t command[] = {0x3E, 0x81, 0x00, 0x00, 0x00};
  command[2] = id;
  command[4] = GetHeaderCheckSum(command);
  if (!WriteFile(_handle, command, sizeof(command), &_bytesWritten, NULL)) {
    ERROR_("Stop: Failed to send command to device");
    return false;
  }
  return true;
}

// 获取PI参数-----------------------------------------
bool RMD::GetPI(uint8_t *arrPI, const uint8_t id) {
  uint8_t command[] = {0x3E, 0X30, 0x00, 0x00, 0x00};
  command[2] = id;
  command[4] = GetHeaderCheckSum(command);
  const DWORD bytesToRead = 12;
  uint8_t readBuf[bytesToRead];

  if (!WriteFile(_handle, command, sizeof(command), &_bytesWritten, NULL)) {
    ERROR_("GetPI: Failed to send command to device");
    return false;
  }

  if (!ReadFile(_handle, readBuf, bytesToRead, &_bytesRead, NULL)) {
    ERROR_("GetPI: Failed to revice data from device");
    return false;
  }

  if (_bytesRead != bytesToRead) {
    ERROR_("GetPI: Abnormal received data - byte count");
    return false;
  }

  if (readBuf[0] != 0x3E || readBuf[1] != 0x30 || readBuf[2] != id ||
      readBuf[3] != 0x06 || readBuf[4] != (0x3E + 0x30 + id + 0x06)) {
    ERROR_("GetPI: Abnormal received data - frame header");
    return false;
  }

  uint8_t sum = 0;
  for (int i = 5; i < 11; i++) {
    sum += readBuf[i];
  }
  if (sum != readBuf[11]) {
    ERROR_("GetPI: Abnormal received data - data");
    return false;
  }

  for (int i = 0; i < 6; i++) {
    arrPI[i] = (uint8_t)readBuf[5 + i];
  }

  return true;
}

// 改写PI参数----------------------------------------
bool RMD::WriteAnglePI(const uint8_t *arrPI, const uint8_t id) {
  uint8_t command[12] = {0x3E, 0x32, 0x00, 0x06, 0x00};
  command[2] = id;
  command[4] = GetHeaderCheckSum(command);

  uint8_t checksum = 0;
  for (int i = 0; i < 6; i++) {
    command[5 + i] = (uint8_t)arrPI[i];
    checksum += command[5 + i];
  }
  command[11] = checksum;

  if (!WriteFile(_handle, command, sizeof(command), &_bytesWritten, NULL)) {
    ERROR_("WriteAnglePI: Failed to send command to device");
    return false;
  }

  return true;
}

// 调试PI参数-------------------------------------------
bool RMD::DebugAnglePI(const uint8_t *arrPI, const uint8_t id) {
  uint8_t command[12] = {0x3E, 0x31, 0x00, 0x06, 0x00};
  command[2] = id;
  command[4] = GetHeaderCheckSum(command);
  uint8_t checksum = 0;
  for (int i = 0; i < 6; i++) {
    command[5 + i] = (uint8_t)arrPI[i];
    checksum += command[5 + i];
  }
  command[11] = checksum;

  if (!WriteFile(_handle, command, sizeof(command), &_bytesWritten, NULL)) {
    ERROR_("DebugAnglePI: Failed to send command to device");
    return false;
  }
  return true;
}