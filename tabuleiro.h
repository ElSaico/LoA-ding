#ifndef _TABULEIRO_H
#define _TABULEIRO_H

#include <stdint.h>

#define bool uint8_t
#define false 0
#define true 1

#define INIT_BRANCO 0x7E0000000000007EULL
#define INIT_PRETO  0x0081818181818100ULL

#define coord(t,x,y) (t & linha(x) & coluna(y))
#define move(t,x0,x) ((x | x0) ^ t)
#define count(x) __builtin_popcountll(x)
#define grid(x,y) (x ^ y)
#define linha(x) (0xFF00000000000000ULL >> (x << 3))
#define coluna(x) (0x8080808080808080ULL >> x)
#define pecas(x) ((x).jogador == J_BRANCO ? (x).branco : (x).preto)

typedef enum TPlayer {J_BRANCO, J_PRETO} Jogador;

typedef struct tab {
	uint64_t branco;
	uint64_t preto;
	Jogador jogador;
	Jogador turno;
} Tabuleiro;

Tabuleiro novoTab(Jogador in);
bool moveH(uint64_t orig, uint64_t dest);

#endif
