#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>

class RMD {
 public:
  RMD(const char* serialPort);
  ~RMD();
  bool Init();
  bool isInit();
  bool Reconnect();
  bool GetMultiAngle_s(int64_t* angle, const uint8_t id);
  uint8_t GetHeaderCheckSum(uint8_t* command);
  bool GoToAngle(int64_t angle, const uint8_t id);
  bool Stop(const uint8_t id);
  bool GetPI(uint8_t* arrPI, const uint8_t id);
  bool WriteAnglePI(const uint8_t* arrPI, const uint8_t id);
  bool DebugAnglePI(const uint8_t* arrPI, const uint8_t id);

 private:
  const char* _serialPort;
  HANDLE _handle;
  DWORD _bytesRead;
  DWORD _bytesWritten;
  bool _isInit;
};

typedef enum ID_ENTRY {
    ID_01 = 0x01,
    ID_02 = 0x02,
};

