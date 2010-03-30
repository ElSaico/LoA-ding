#include "tabuleiro.h"

#define max(x,y) ((x) > (y) ? (x) : (y))

int eval(uint64_t t);
int negamax(uint64_t* or, uint64_t* d, Tabuleiro t);
