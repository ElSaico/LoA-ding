#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "ia.h"
#include "tabuleiro.h"

#define posline(l,x) (x-border)*8/(l-2*border)
#define linepos(x,y) border+(y)*(x-2*border)/8
#define sizepos(x) (x-2*border)/16
#define coordXY(t,x,y) coord(t, posline(vsize, y), posline(hsize, x))
#define posXY(x,y) pos(posline(vsize, y), posline(hsize, x))

#define C_BRANCO   0xFFFFFFFF
#define C_PRETO    0x000000FF
#define C_CINZA    0x777777FF
#define C_VERMELHO 0xFF0000FF
#define C_VERDE    0x00FF00FF
#define C_AZUL     0x0000FFFF

const Sint16 border = 50;
Sint16 hsize = 640;
Sint16 vsize = 480;
SDL_Surface *tela = NULL;
uint64_t mover_adv = 0;
uint64_t origem_adv = 0;
uint64_t mover = 0;
uint64_t origem = 0;
bool venceu = false;
Jogador vencedor;

void mostraPreto(int l, int c, Uint32 cor) {
	filledEllipseColor(tela, linepos(hsize, c+0.5), linepos(vsize, l+0.5),
	                    0.75*sizepos(hsize),   0.75*sizepos(vsize), cor);
}

void mostraBranco(int l, int c, Uint32 cor) {
	ellipseColor(tela, linepos(hsize, c+0.5), linepos(vsize, l+0.5),
	              0.75*sizepos(hsize),   0.75*sizepos(vsize), cor);
}

void mostra(Jogador j, int l, int c, Uint32 cor) {
	if (j == J_PRETO)
		mostraPreto(l, c, cor);
	else
		mostraBranco(l, c, cor);
}

void draw(Tabuleiro *t) {
	SDL_FillRect(tela, NULL, C_BRANCO);
	
	if (hsize >= 2*border && vsize >= 2*border) {
		Uint32 cor;
		for (int i = 0; i < 8; ++i) {
			characterColor(tela, border/2, linepos(vsize, i+0.5), i+'1', C_PRETO);
			characterColor(tela, linepos(hsize, i+0.5), border/2, i+'A', C_PRETO);
			for (int j = 0; j < 8; ++j) {
				if (coord(mover, i, j))
					boxColor(tela, linepos(hsize, j), linepos(vsize, i),
					       linepos(hsize, j+1), linepos(vsize, i+1), C_AZUL);
				else if (coord(origem_adv, i, j))
					boxColor(tela, linepos(hsize, j), linepos(vsize, i),
					       linepos(hsize, j+1), linepos(vsize, i+1), C_VERMELHO);
				cor = coord(origem, i, j) ? C_AZUL : C_PRETO;
				cor |= coord(mover_adv, i, j) ? C_VERMELHO : C_PRETO;
				if (coord(t->p_jogador, i, j))
					mostra(t->jogador, i, j, cor);
				else if (coord(t->p_adv, i, j))
					mostra(adv(t->jogador), i, j, cor);
				}
		}
		
		for (int i = 1; i < 8; ++i) {
			hlineColor(tela, border, hsize-border, linepos(vsize, i), C_PRETO);
			vlineColor(tela, linepos(hsize, i), border, vsize-border, C_PRETO);
		}
		
		const char *cores[] = {"Branco", "Preto"};
		const char *jogadores[] = {"Jogador", "Adversario"};
		char mensagem[30];
		if (!venceu) {
			uint8_t ncor = t->turno == J_BRANCO ? 0 : 1;
			uint8_t njogador = t->jogador == t->turno ? 0 : 1;
			sprintf(mensagem, "Turno: %s (%s)", cores[ncor], jogadores[njogador]);
		} else {
			uint8_t ncor = vencedor == J_BRANCO ? 0 : 1;
			uint8_t njogador = t->jogador == vencedor ? 0 : 1;
			sprintf(mensagem, "%s (%s) venceu!", cores[ncor], jogadores[njogador]);
		}
		stringColor(tela, border, vsize-border/2, mensagem, C_PRETO);
	}

	SDL_Flip(tela);
}

void jogarPC(Tabuleiro *t) {
	t->turno = adv(t->jogador);
	mover = 0;
	origem_adv = 0;
	mover_adv = 0;
	draw(t);
	int n = negamax(&origem_adv, &mover_adv, *t);
	move(t, origem_adv, mover_adv);
	printf("%+3d %016llx %016llx\n", n, origem_adv, mover_adv);
	t->turno = t->jogador;								
}

bool verificaVitoria(Tabuleiro *t) {
	if (vitoria(t->p_jogador)) {
		vencedor = t->jogador;
		mover = 0;
		origem_adv = 0;
		mover_adv = 0;
		return true;
	} else if (vitoria(t->p_adv)) {
		vencedor = adv(t->jogador);
		mover = 0;
		origem_adv = 0;
		mover_adv = 0;
		return true;
	}
	return false;
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
				if (!venceu) {
					if (mover) {
						if (e.button.button == SDL_BUTTON_LEFT) {
							destino = posXY(e.button.x, e.button.y);
							if (mover & destino) {
								move(t, origem, destino);
								venceu = verificaVitoria(t);
								if (venceu) {
									draw(t);
									return false;
								}
								jogarPC(t);
								venceu = verificaVitoria(t);
								if (venceu) {
									draw(t);
									return false;
								}
							}
						}
						mover = 0;
						origem = 0;
					} else if (t->turno == t->jogador) {
						origem = coordXY(t->p_jogador, e.button.x, e.button.y);
						if (origem)
							mover = movePara(*t, origem);
					}
					draw(t);
				}
				break;
		}
	return false;
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
	SDL_WM_SetCaption("LoA-ding", NULL);
	FPSmanager fps;
	SDL_initFramerate(&fps);
	
	Tabuleiro t = novoTab(J_BRANCO);
	jogarPC(&t);
	draw(&t);
	
	bool sair = false;
	while (!sair) {
		sair = eventLoop(&t);
		SDL_framerateDelay(&fps);
	}

	SDL_Quit();
	return 0;
}
