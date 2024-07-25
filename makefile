# 定义编译器
CC = gcc

# 定义编译选项
CFLAGS = -Wall -g

# 定义源文件
SRC = main.c RMDControl.c

# 定义目标文件
OBJ = $(SRC:.c=.o)

# 定义最终可执行文件
TARGET = program

# 默认目标
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean