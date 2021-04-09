# define compilers for program
CC=gcc

# compiler flags
# -g	adds debugging info
# -Wall	enable a lot of compiler warnings
CFLAGS= -g -Wall

INCLUDES = -I../../include

SRCS = main.c

OBJS = $(SRCS:.c=.o)

# target binary file name
TARGET=basic

.PHONY: depend clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE