#pragma once
#include <windows.h>

namespace RMD {
class SerialPort {
public:
  SerialPort(const char *serialPort);
  ~SerialPort();
  HANDLE GetHandle();

private:
  HANDLE _handle;
};

} // namespace RMD