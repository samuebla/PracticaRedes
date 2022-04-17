# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
CFLAGS = -pedantic -pedantic-errors -pthread -Wall -Wextra -Werror -Wno-float-equal -Wno-padded

# the build target executable:
TARGET = ejercicio7
OUTPUT = echo_server_multi

all: $(TARGET)

$(TARGET): $(TARGET).cc
	$(CC) $(CFLAGS) -o $(OUTPUT) $(TARGET).cc

clean:
	$(RM) $(TARGET)