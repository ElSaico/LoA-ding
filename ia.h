#ifndef _IA_H
#define _IA_H

#include "tabuleiro.h"

#define desde(t) ((clock()-(t)) / (float)CLOCKS_PER_SEC)

static int nmax = 3;

int eval(Tabuleiro t, Jogador j);
int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t);

#endif
