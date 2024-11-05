#include "RMDControl.h"
#include "RMDMotor.h"
#include "SerialPort.h"
#include <iostream>

// void TestGoAngle();
// void TestGetAngle();
// void TestGetPI();

void TestSerialPort() {
  try {
    RMD::SerialPort sp("COM3");
    std::cout << "SerialPort: " << sp.GetHandle() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

int main() {
  RMD::SerialPort port("COM3");
  RMD::RMDMotor motor(port.GetHandle(), 0x01);
  RMD::RMDMotor motor2(port.GetHandle(), 0x02);

  return 0;
}

// void TestGetAngle() {
//   int64_t angle;
//   if (RMD_GetMultiAngle_S(&angle) == 0) {
//     printf("Curent multi angle: %lld\n", angle);
//   } else {
//     printf("RMD_GetMultiAngle_S error\n");
//   }
// }

// void TestGoAngle() {
//   char key = 0;
//   int num = 0;
//   bool isReadingNum = false;
//   int numSign = 1;

//   printf("Enter an angle to go to(unit: 0.01 degree)\n");
//   printf("Enter \'s\' to stop.\n");
//   printf("Enter \'q\' to quit.\n");

//   while (1) {
//     key = getchar();

//     if (isReadingNum && !isdigit(key)) {
//       RMD_GoAngleAbsolute(num);
//     }

//     if (key == 'q') {
//       break;
//     } else if (key == 's') {
//       RMD_Stop();
//       continue;
//     } else if (key == '+') {
//       isReadingNum = true;
//       numSign = 1;
//       num = 0;
//     } else if (key == '-') {
//       isReadingNum = true;
//       numSign = -1;
//       num = 0;
//     } else if (isdigit(key)) {
//       isReadingNum = true;
//       num = num * 10 + key - '0';

//       while ((key = getchar()) != '\n') {
//         num = num * 10 + key - '0';
//       }

//       num *= numSign;

//       if (num > 9000 || num < -9000) {
//         printf("输入角度超出范围\n");
//         continue;
//       } else {
//         RMD_GoAngleAbsolute(num);
//         num = 0;
//         numSign = 1;
//       }
//     }

//     // if (num > 9000 || num < -9000) {
//     //   printf("输入角度超出范围\n");
//     //   continue;
//     // }

//     // GoToAngle(num);
//   }
// }

// void TestGetPI() {
// uint8_t arrPI[6];
// if (RMD_GetPI(arrPI) == 0) {
//   printf("Curent PI: %d, %d, %d, %d, %d, %d\n", arrPI[0], arrPI[1], arrPI[2],
//          arrPI[3], arrPI[4], arrPI[5]);
// } else {
//   printf("RMD_GetPI error\n");
// }
// }