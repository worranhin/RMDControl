#include "RMDControl.h"

int main() {
  char key = 0;
  int num = 0;
  bool isReadingNum = false;
  int numSign = 1;

  RMD_Init("COM7");

  int64_t angle;
  if (RMD_GetMultiAngle_S(&angle) == 0) {
    printf("Curent multi angle: %lld\n", angle);
  } else {
    printf("RMD_GetMultiAngle_S error\n");
  }

  printf("Enter an angle to go to(unit: 0.01 degree)\n");
  printf("Enter \'s\' to stop.\n");
  printf("Enter \'q\' to quit.\n");

  while (1) {
    key = getchar();

    if (isReadingNum && !isdigit(key)) {
      RMD_GoAngleAbsolute(num);
    }

    if (key == 'q') {
      break;
    } else if (key == 's') {
      RMD_Stop();
      continue;
    } else if (key == '+') {
      isReadingNum = true;
      numSign = 1;
      num = 0;
    } else if (key == '-') {
      isReadingNum = true;
      numSign = -1;
      num = 0;
    } else if (isdigit(key)) {
      isReadingNum = true;
      num = num * 10 + key - '0';

      while ((key = getchar()) != '\n') {
        num = num * 10 + key - '0';
      }

      num *= numSign;

      if (num > 9000 || num < -9000) {
        printf("输入角度超出范围\n");
        continue;
      } else {
        RMD_GoAngleAbsolute(num);
        num = 0;
        numSign = 1;
      }
    }

    // if (num > 9000 || num < -9000) {
    //   printf("输入角度超出范围\n");
    //   continue;
    // }

    // GoToAngle(num);
  }

  RMD_DeInit();
  return 0;
}