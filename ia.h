#ifndef _IA_H
#define _IA_H

#include <time.h>
#include "tabuleiro.h"

#define TSIZE 262144
#define HASH_NULL 777

#define desde(t) ((clock()-(t)) / (float)CLOCKS_PER_SEC)

typedef enum THFlag {H_ALFA, H_BETA, H_FOLHA} HashFlag;

typedef struct {
	uint64_t hash;
	int ply;
	HashFlag flag;
	int eval;
	Tabuleiro tab;
} Trans;

uint64_t hash[2][65]; // [i][0] = turno
Trans trans[TSIZE];

static const int nmax = 4;

static const uint8_t valorPos[] = 
	{-8, -4, -2, -2, -2, -2, -4, -8,
	 -4,  1,  1,  1,  1,  1,  1, -4,
	 -2,  1,  2,  2,  2,  2,  1, -2,
	 -2,  1,  2,  4,  4,  2,  1, -2,
	 -2,  1,  2,  4,  4,  2,  1, -2,
	 -2,  1,  2,  2,  2,  2,  1, -2,
	 -4,  1,  1,  1,  1,  1,  1, -4,
	 -8, -4, -2, -2, -2, -2, -4, -8};

void initHash();
int eval(Tabuleiro *t, Jogador j);
int negamax(uint64_t* or, uint64_t* dst, Tabuleiro *t);
int mtdf(uint64_t* or, uint64_t* dst, Tabuleiro *t, int f);

#endif
