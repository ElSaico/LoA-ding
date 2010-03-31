#ifndef _IA_H
#define _IA_H

#include "tabuleiro.h"

int eval(Tabuleiro t, Jogador j);
int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t);

#endif
