
CC = gcc
CFLAGS = -Wall -Werror --pedantic -g

EXEC = ascript 
OBJS = main.o srccompile.o list.o asmcommands.o mathconvert.o pemdas.o

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS) $(EXEC)

re:
	make fclean all

