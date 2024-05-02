CC = g++

CFLAGS = -g -Wall -Werror

TARGET = migrate-sim

$(TARGET): main.o
	$(CC) $(CFLAGS) main.o -o $(TARGET)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	$(RM) $(TARGET) main.o