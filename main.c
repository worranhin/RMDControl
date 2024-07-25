#include "RMDControl.h"

int main() {
  char key = 0;
  int num = 0;
  bool isReadingNum = false;
  int numSign = 1;

  RMD_Init();

  printf("输入一个角度以前往(单位: 0.01度)\n");
  printf("输入 \'s\' 停止电机\n");
  printf("按 \'q\' 退出\n");

  while (1) {
    key = getchar();

    if (isReadingNum && !isdigit(key)) {
      RMD_GoToAngle(num);
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
        RMD_GoToAngle(num);
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