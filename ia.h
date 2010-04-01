#ifndef _IA_H
#define _IA_H

#include "tabuleiro.h"

#define TSIZE 32768
#define HASH_NULL 777

typedef enum {H_ALFA, H_BETA, H_FOLHA} HashFlag;

typedef struct {
	uint64_t hash;
	int ply;
	HashFlag flag;
	int eval;
	uint64_t move;
} Trans;

uint64_t hash[2][64];
Trans trans[TSIZE];

void initHash();
int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t);

#endif
