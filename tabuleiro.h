#ifndef _TABULEIRO_H
#define _TABULEIRO_H

#include <stdint.h>

#define bool uint8_t
#define false 0
#define true 1

#define INIT_BRANCO 0x7E0000000000007EULL
#define INIT_PRETO  0x0081818181818100ULL
#define DIAG_PRI    0x8040201008040201ULL
#define DIAG_SEC    0x8000000000000000ULL

#define pos(x,y) (linha(x) & coluna(y))
#define coord(t,x,y) (t & pos(x, y))
#define move(t,x0,x) ((x | x0) ^ t)
#define count(x) __builtin_popcountll(x)
#define grid(t) (t.p_jogador ^ t.p_adv)
#define linha(x) (0xFF00000000000000ULL >> ((x) << 3))
#define coluna(x) (0x8080808080808080ULL >> (x))
#define adv(t) ((t).jogador == J_BRANCO ? J_PRETO : J_BRANCO)
#define p_turno(t) ((t).jogador == (t).turno ? (t).p_jogador : (t).p_adv)
#define p_turno_adv(t) ((t).jogador == (t).turno ? (t).p_adv : (t).p_jogador)

typedef enum TPlayer {J_BRANCO, J_PRETO} Jogador;

typedef struct tab {
	uint64_t p_jogador;
	uint64_t p_adv;
	Jogador jogador;
	Jogador turno;
} Tabuleiro;

Tabuleiro novoTab(Jogador in);
uint64_t movePara(Tabuleiro t, uint64_t or);
bool vitoria(uint64_t t);

#endif
