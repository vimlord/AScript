
CC = gcc
CFLAGS = -Wall -Werror --pedantic -g -Iinclude/

EXEC = ascript 

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS) $(EXEC)

re:
	make fclean all

