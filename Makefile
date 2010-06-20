OBJS = tabuleiro.o final.o ia.o

SRCS = $(OBJS,.o=.c)

all: final

final: $(OBJS)
	gcc -O3 -lSDL -lSDL_gfx -o final $(OBJS)

.c.o:
	gcc -c -O3 -Wall -std=c99 $<

clean:
	rm -f final *.o

win: $(OBJS)
	gcc -o final.exe $(OBJS) -lmingw32 -lSDLmain -lSDL_gfx -lSDL
	
win-clean:
	rm -f final.exe *.o
