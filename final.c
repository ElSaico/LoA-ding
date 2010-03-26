#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "tabuleiro.h"

#define posline(l,x) (x-border)*8/(l-2*border)
#define linepos(x,y) border+(y)*(x-2*border)/8
#define sizepos(x) (x-2*border)/16
#define coordXY(t,x,y) coord(t, posline(vsize, y), posline(hsize, x))
#define posXY(x,y) pos(posline(vsize, y), posline(hsize, x))

#define C_BRANCO 0xFFFFFFFF
#define C_PRETO  0x000000FF
#define C_CINZA  0x777777FF

const Sint16 border = 50;
Sint16 hsize = 640;
Sint16 vsize = 480;
SDL_Surface *tela = NULL;
uint64_t move = 0;
uint64_t origem = 0;

void mostraPreto(int l, int c, Sint16 cor) {
	filledEllipseColor(tela, linepos(hsize, c+0.5), linepos(vsize, l+0.5),
	                    0.75*sizepos(hsize),   0.75*sizepos(vsize), cor);
}

void mostraBranco(int l, int c, Sint16 cor) {
	ellipseColor(tela, linepos(hsize, c+0.5), linepos(vsize, l+0.5),
	              0.75*sizepos(hsize),   0.75*sizepos(vsize), cor);
}

void mostra(Jogador j, int l, int c, Sint16 cor) {
	if (j == J_PRETO)
		mostraPreto(l, c, cor);
	else
		mostraBranco(l, c, cor);
}

void draw(Tabuleiro *t) {
	SDL_FillRect(tela, NULL, C_BRANCO);
	
	if (hsize >= 2*border && vsize >= 2*border) {
		char n[2] = {' ', '\0'};
		for (int i = 0; i < 8; ++i) {
			n[0] = i + '1';
			stringColor(tela, border/2, linepos(vsize, i+0.5), n, C_PRETO);
			n[0] = i + 'A';
			stringColor(tela, linepos(hsize, i+0.5), border/2, n, C_PRETO);
			for (int j = 0; j < 8; ++j) {
				if (coord(move, i, j))
					boxColor(tela, linepos(hsize, j), linepos(vsize, i),
					       linepos(hsize, j+1), linepos(vsize, i+1), C_CINZA);

				if (coord(t->p_jogador, i, j))
					mostra(t->jogador, i, j, C_PRETO);
				else if (coord(t->p_adv, i, j))
					mostra(adv(*t), i, j, C_PRETO);				
				}
		}
		
		for (int i = 1; i < 8; ++i) {
			hlineColor(tela, border, hsize-border, linepos(vsize, i), C_PRETO);
			vlineColor(tela, linepos(hsize, i), border, vsize-border, C_PRETO);
		}
		
		const char *cores[] = {"Branco", "Preto"};
		uint8_t cor = t->turno == J_BRANCO ? 0 : 1;
		const char *jogadores[] = {"Jogador", "Adversario"};
		uint8_t jogador = t->jogador == t->turno ? 0 : 1;
		char mensagem[30];
		sprintf(mensagem, "Turno: %s (%s)", cores[cor], jogadores[jogador]);
		stringColor(tela, border, vsize-border/2, mensagem, C_PRETO);
	}

	SDL_Flip(tela);
}

bool eventLoop(Tabuleiro *t) {
	uint64_t destino;
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type) {
			case SDL_QUIT:
				return true;
			case SDL_VIDEORESIZE:
				hsize = e.resize.w;
				vsize = e.resize.h;
				tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
				draw(t);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (move) {
					if (e.button.button == SDL_BUTTON_LEFT) {
						destino = posXY(e.button.x, e.button.y);
						if (move & destino) {
							t->p_jogador = move(t->p_jogador, origem, destino);
							t->p_adv &= ~destino;
							t->turno = adv(*t);
						}
					}
					move = 0;
				} else if (t->turno == t->jogador) {
					origem = coordXY(t->p_jogador, e.button.x, e.button.y);
					if (origem)
						move = moveH(*t, origem) | moveV(*t, origem)
							| moveDp(*t, origem) | moveDs(*t, origem);
				}
				draw(t);
				break;
		}
	return false;
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
	SDL_WM_SetCaption("Jogo Aleatório", NULL);
	FPSmanager fps;
	SDL_initFramerate(&fps);
	
	Tabuleiro t = novoTab(J_PRETO);
	t.turno = J_PRETO;
	draw(&t);
	
	bool sair = false;
	while (!sair) {
		sair = eventLoop(&t);
		SDL_framerateDelay(&fps);
		//draw(t); // usar apenas ao atualizar tabuleiro?
	}

	SDL_Quit();
	return 0;
}
