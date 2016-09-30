
CC = gcc
CFLAGS = -Werror -Wall --pedantic

EXEC = testfile
OBJS = main.o srccompile.o list.o asmcommands.o mathconvert.o

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS) $(EXEC)

