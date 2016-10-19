
CC = gcc
CFLAGS = -Wall -Werror --pedantic -g -Iinclude/

EXEC = ascript 
OBJS = src/main.o src/srccompile.o src/list.o src/asmcommands.o src/mathconvert.o src/pemdas.o

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS) $(EXEC)

re:
	make fclean all

