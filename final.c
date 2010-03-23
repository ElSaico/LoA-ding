#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "tabuleiro.h"

#define posline(l,x) (x-border)/(l-2*border)
#define linepos(x,y) border+(y)*(x-2*border)/8
#define sizepos(x)   (x-2*border)/16

#define C_BRANCO 0xFFFFFFFF
#define C_PRETO  0x000000FF

const Sint16 border = 50;
Sint16 hsize = 640;
Sint16 vsize = 480;
SDL_Surface *tela = NULL;

void draw(Tabuleiro *t) {
	SDL_FillRect(tela, NULL, C_BRANCO);
	
	if (hsize >= 2*border && vsize >= 2*border) {
		stringColor(tela, border/2, linepos(vsize, 0.5), "1", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 1), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 1.5), "2", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 2), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 2.5), "3", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 3), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 3.5), "4", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 4), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 4.5), "5", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 5), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 5.5), "6", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 6), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 6.5), "7", C_PRETO);
		hlineColor(tela, border, hsize-border, linepos(vsize, 7), C_PRETO);
		stringColor(tela, border/2, linepos(vsize, 7.5), "8", C_PRETO);
		
		stringColor(tela, linepos(hsize, 0.5), border/2, "A", C_PRETO);
		vlineColor(tela, linepos(hsize, 1), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 1.5), border/2, "B", C_PRETO);
		vlineColor(tela, linepos(hsize, 2), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 2.5), border/2, "C", C_PRETO);
		vlineColor(tela, linepos(hsize, 3), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 3.5), border/2, "D", C_PRETO);
		vlineColor(tela, linepos(hsize, 4), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 4.5), border/2, "E", C_PRETO);
		vlineColor(tela, linepos(hsize, 5), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 5.5), border/2, "F", C_PRETO);
		vlineColor(tela, linepos(hsize, 6), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 6.5), border/2, "G", C_PRETO);
		vlineColor(tela, linepos(hsize, 7), border, vsize-border, C_PRETO);
		stringColor(tela, linepos(hsize, 7.5), border/2, "H", C_PRETO);
				
		for (int l = 0; l < 8; ++l)
			for (int c = 0; c < 8; ++c)
				if (coord(t->preto, l, c))
					filledEllipseColor(tela, linepos(hsize, l+0.5), linepos(vsize, c+0.5),
					                    0.75*sizepos(hsize),   0.75*sizepos(vsize), C_PRETO);
				else if (coord(t->branco, l, c))
					ellipseColor(tela, linepos(hsize, l+0.5), linepos(vsize, c+0.5),
					              0.75*sizepos(hsize),   0.75*sizepos(vsize), C_PRETO);
		
		stringColor(tela, border, vsize-border/2, "Turno: ", C_PRETO);
	}

	SDL_Flip(tela);
}

int eventLoop(Tabuleiro *t) {
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type) {
			case SDL_QUIT:
				return 1;
			case SDL_VIDEORESIZE:
				hsize = e.resize.w;
				vsize = e.resize.h;
				tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
				draw(t);
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("X: %d Y: %d", e.motion.x, e.motion.y);
				break;
		}
	return 0;
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
	SDL_WM_SetCaption("Jogo Aleatório", NULL);
	FPSmanager fps;
	SDL_initFramerate(&fps);
	
	Tabuleiro* t = novoTab(J_BRANCO);
	draw(t);
	
	uint8_t sair = 0;
	while (!sair) {
		sair = eventLoop(t);
		SDL_framerateDelay(&fps);
		//draw(t); // usar apenas ao atualizar tabuleiro?
	}

	SDL_Quit();
	return 0;
}
