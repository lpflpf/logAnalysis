CC = gcc
CFLAGS = -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -g -Wall
TARGET = logSearch
SOURCE = logSearch.c fileOperation.c

INCLUDE = -I .

$(TARGET) : $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -o $(TARGET) $(INCLUDE) 
clean:
	rm -rf $(TARGET)
