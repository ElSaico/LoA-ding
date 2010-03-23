OBJS = tabuleiro.o final.o

SRCS = $(OBJS,.o=.c)

all: final

final: $(OBJS)
	gcc -lSDL -lSDL_gfx -o final $(OBJS)

.c.o:
	gcc -c -O2 -Wall -std=c99 $<

clean:
	rm -f final *.o
