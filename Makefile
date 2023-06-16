CC = gcc

	# compiler flags:
	#  -g    adds debugging information to the executable file
	#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

	# the build target executable:
TARGET = tp2virtual
EXEC = main

all: $(TARGET)

$(TARGET): $(TARGET).c
		
	$(CC) $(CFLAGS) -o $(EXEC) $(TARGET).c

clean:
	$(RM) $(EXEC)
