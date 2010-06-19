#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "ia.h"
#include "tabuleiro.h"

typedef struct {
	uint64_t mov_validos;
	uint64_t origem;
	uint64_t origem_adv;
	uint64_t destino;
	uint64_t destino_adv;
} Movimentos;

#define posH(h) (h-border)*8/(hsize-2*border)
#define posV(v) (v-border)*8/(vsize-2*border)

#define lineX(x) border+(x)*(hsize-2*border)/8
#define lineY(y) border+(y)*(vsize-2*border)/8

#define sizeX (hsize-2*border)/16
#define sizeY (vsize-2*border)/16

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

void mostraPreto(int l, int c, Uint32 cor) {
	filledEllipseColor(tela, lineX(c+0.5), lineY(l+0.5), 0.75*sizeX, 0.75*sizeY, cor);
}

void mostraBranco(int l, int c, Uint32 cor) {
	ellipseColor(tela, lineX(c+0.5), lineY(l+0.5), 0.75*sizeX, 0.75*sizeY, cor);
}

void draw(Tabuleiro *t, Movimentos m, bool venceu, Jogador vencedor) {
	SDL_FillRect(tela, NULL, C_BRANCO);
	
	if (hsize >= 2*border && vsize >= 2*border) {
		Uint32 cor;
		for (int i = 0; i < 8; ++i) {
			characterColor(tela, border/2, lineY(i+0.5), '8'-i, C_PRETO);
			characterColor(tela, lineX(i+0.5), border/2, i+'A', C_PRETO);
			for (int j = 0; j < 8; ++j) {
				if (coord(m.mov_validos, i, j))
					boxColor(tela, lineX(j), lineY(i), lineX(j+1), lineY(i+1), C_AZUL);
				else if (coord(m.origem_adv, i, j))
					boxColor(tela, lineX(j), lineY(i), lineX(j+1), lineY(i+1), C_VERMELHO);
				
				if (coord(m.origem, i, j))
					cor = C_AZUL;
				else if (coord(m.destino_adv, i, j))
					cor = C_VERMELHO;
				else
					cor = C_PRETO;
				
				if (coord(t->pecas[J_BRANCO], i, j))
					mostraBranco(i, j, cor);
				else if (coord(t->pecas[J_PRETO], i, j))
					mostraPreto(i, j, cor);
				}
		}
		
		for (int i = 1; i < 8; ++i) {
			hlineColor(tela, border, hsize-border, lineY(i), C_PRETO);
			vlineColor(tela, lineX(i), border, vsize-border, C_PRETO);
		}
		
		static const char *cores[] = {"Branco", "Preto"};
		static const char *jogadores[] = {"Jogador", "Adversario"};
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

void jogarPC(Tabuleiro *t, Movimentos *m) {
	t->turno = adv(t->jogador);
	m->mov_validos = 0;
	m->origem_adv = 0;
	m->destino_adv = 0;
	draw(t, *m, false, J_NENHUM);
	int n = negamax(&m->origem_adv, &m->destino_adv, t);
	move(t, m->origem_adv, m->destino_adv);
	printf("%+3d %016llx %016llx\n", n, m->origem_adv, m->destino_adv);
	t->turno = t->jogador;								
}

bool verificaVitoria(Tabuleiro *t, Movimentos *m, Jogador j) {
	if (vitoria(t->pecas[j])) {
		m->mov_validos = 0;
		m->origem_adv = 0;
		m->destino_adv = 0;
		return true;
	}
	return false;
}

bool eventLoop(Tabuleiro *t, Movimentos *m) {
	static bool venceu = false;
	static Jogador vencedor;
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type) {
			case SDL_QUIT:
				return true;
			case SDL_VIDEORESIZE:
				hsize = e.resize.w;
				vsize = e.resize.h;
				tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
				draw(t, *m, venceu, vencedor);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (!venceu) {
					int posx = posH(e.button.x);
					int posy = posV(e.button.y);
					if ((e.button.button == SDL_BUTTON_LEFT) && m->mov_validos) {
						m->destino = pos(posy, posx);
						if (m->mov_validos & m->destino) {
							move(t, m->origem, m->destino);
							venceu = verificaVitoria(t, m, t->jogador);
							if (venceu) {
								draw(t, *m, true, t->jogador);
								return false;
							}
							venceu = verificaVitoria(t, m, adv(t->jogador));
							if (venceu) {
								draw(t, *m, true, adv(t->jogador));
								return false;
							}
							jogarPC(t, m);
							venceu = verificaVitoria(t, m, adv(t->jogador));
							if (venceu) {
								draw(t, *m, true, adv(t->jogador));
								return false;
							}
							venceu = verificaVitoria(t, m, t->jogador);
							if (venceu) {
								draw(t, *m, true, t->jogador);
								return false;
							}
						}
						m->mov_validos = 0;
						m->origem = 0;
					} else if (t->turno == t->jogador) {
						m->origem = coord(t->pecas[t->jogador], posy, posx);
						if (m->origem)
							m->mov_validos = movePara(t, m->origem);
					}
					draw(t, *m, venceu, vencedor);
				}
				break;
		}
	return false;
}

void drawInicial() {
	SDL_FillRect(tela, NULL, C_BRANCO);
	stringColor(tela, 25,  25, "LoA-ding - minimal lines, maximum action", C_PRETO);
	stringColor(tela, 25,  50, "Autor: Bruno Marques", C_PRETO);
	stringColor(tela, 25, 100, "Pressione P para jogar com as pretas", C_PRETO);
	stringColor(tela, 25, 125, "Pressione B para jogar com as brancas", C_PRETO);
	SDL_Flip(tela);
}

bool telaInicial(Jogador *j) {
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type) {
			case SDL_QUIT:
				SDL_Quit();
				exit(EXIT_SUCCESS);
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
					case SDLK_p:
						*j = J_PRETO;
						return true;
					case SDLK_b:
						*j = J_BRANCO;
						return true;
					default: break;
				}
				break;
		}
	return false;
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	tela = SDL_SetVideoMode(400, 150, 16, SDL_SWSURFACE);
	SDL_WM_SetCaption("LoA-ding", NULL);
	FPSmanager fps;
	SDL_initFramerate(&fps);
	
	initHash();
	drawInicial();
	
	Jogador j;
	bool sair = false;
	while (!sair) {
		sair = telaInicial(&j);
		SDL_framerateDelay(&fps);		
	}
	
	Tabuleiro t = novoTab(j);
	Movimentos m = {0, 0, 0, 0, 0};
	if (j == J_BRANCO)
		jogarPC(&t, &m);
	tela = SDL_SetVideoMode(hsize, vsize, 16, SDL_RESIZABLE);
	draw(&t, m, false, J_NENHUM);
	
	sair = false;
	while (!sair) {
		sair = eventLoop(&t, &m);
		SDL_framerateDelay(&fps);
	}

	SDL_Quit();
	return 0;
}
