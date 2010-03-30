#include "tabuleiro.h"

#define max(x,y) ((x) > (y) ? (x) : (y))

int eval(uint64_t t);
uint64_t negamax(Tabuleiro t);
