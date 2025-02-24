  UW PICO 5.09                                                               File: Makefile                                                                  

CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = merge_sort
SRCS = merge_sort.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)  
        $(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
        rm -f $(TARGET) $(OBJS)
