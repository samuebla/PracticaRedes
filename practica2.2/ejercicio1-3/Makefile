# the compiler: gcc for C program, define as g++ for C++
CC = g++

# the build target executable:
TARGET = ejercicio1
OUTPUT = serialization

all: $(TARGET)

$(TARGET): $(TARGET).cc
	$(CC) -o $(OUTPUT) $(TARGET).cc

clean:
	$(RM) $(TARGET)