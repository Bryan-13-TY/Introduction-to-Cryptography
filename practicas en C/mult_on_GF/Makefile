CC = gcc
CFLAGS = -Iinclude -Wall

SRC = src/main.c src/mult.c

TARGET = mult.exe

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	del $(TARGET)

run: $(TARGET)
	$(TARGET)
