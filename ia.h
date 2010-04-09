#ifndef _IA_H
#define _IA_H

#include "tabuleiro.h"

#define TSIZE 131072
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

static int nmax = 3;

void initHash();
int eval(Tabuleiro t, Jogador j);
int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t);

#endif
