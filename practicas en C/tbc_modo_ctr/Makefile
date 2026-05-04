CC = gcc
CFLAGS = -Iinclude -Wall

SRC = src/main.c src/ctr_crypto.c src/utils_crypto.c src/ctr_status.c

TARGET = modo_ctr.exe

all: data $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

data:
	mkdir data 2>nul || echo data ya existe

clean:
	del $(TARGET)

run: $(TARGET)
	$(TARGET)
