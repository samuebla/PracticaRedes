CC = g++

# flags, used by the compiler:
CFLAGS = -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-float-equal -Wno-padded

# path definitions:
TARGET = ejercicio1
OUTPUT = gai

all: $(TARGET)

$(TARGET): $(TARGET).cc
	$(CC) $(CFLAGS) -o $(OUTPUT) $(TARGET).cc

clean:
	$(RM) $(TARGET)