#include <stdio.h>
#include <stdlib.h>
#include "tabuleiro.h"

Tabuleiro novoTab(Jogador in) {
	Tabuleiro t;
	t.pecas[J_BRANCO] = INIT_BRANCO;
	t.pecas[J_PRETO]  = INIT_PRETO;
	t.jogador = in;
	t.turno = J_PRETO;
	return t;
}

uint64_t linhaXY(uint64_t t) {
	for (int i = 0; i < 8; ++i)
		if (linha(i) & t)
			return linha(i);
	return 0;
}

uint64_t colunaXY(uint64_t t) {
	for (int i = 0; i < 8; ++i)
		if (coluna(i) & t)
			return coluna(i);
	return 0;
}

uint64_t diagPriXY(uint64_t t) {
	uint64_t i;
	uint8_t j;
	for (i = 0x80, j = 0x80; i <= DIAG_PRI; j >>= 1, i = (i<<8)+j)
		if (i & t)
			return i;
	return 0;
}

uint64_t diagSecXY(uint64_t t) {
	uint64_t i;
	uint8_t j;
	for (i = 1, j = 1; i <= DIAG_SEC; j <<= 1, i = (i<<8)+j)
		if (i & t)
			return i;
	return 0;
}

uint64_t moveLinha(Tabuleiro t, uint64_t or, uint64_t ln, int n) {
	uint64_t r = 0, l = 0, rl, rr, p, ret = 0;
	int c = count(ln & grid(t));
	rl = ln & (or << (n*c));
	rr = ln & (or >> (n*c));
	
	if (rl)
		l = ((rl - or) ^ or) & ln;
	if (rr)
		r = ((or - rr) ^ rr) & ln;

	p = t.pecas[adv(t.turno)];
	ret |= p & r ? 0 : rr;
	ret |= p & l ? 0 : rl;
	return ret & ~t.pecas[t.turno];
}

uint64_t movePara(Tabuleiro t, uint64_t or) {
	return moveLinha(t, or,   linhaXY(or), 1) | moveLinha(t, or,  colunaXY(or), 8)
	     | moveLinha(t, or, diagPriXY(or), 9) | moveLinha(t, or, diagSecXY(or), 7);
}

uint64_t adj(uint64_t t) {
	uint64_t to = t & LIMITE_O;
	uint64_t tl = t & LIMITE_L;
	return ~t & ((to << 9) | (t  << 8) | (tl << 7) | (to << 1)
	           | (tl >> 1) | (to >> 7) | (t  >> 8) | (tl >> 9));
}

int nGrupos(uint64_t t) {
	int n = 0;
	uint64_t p, r;
	while (t) {
		p = (t & (t-1)) ^ t;
		r = 0;
		while (p) {
			r |= p;
			p = t & adj(r);
		}
		t &= ~r;
		n++;
	}
	return n;
}

bool vitoria(uint64_t t) {
	return nGrupos(t) == 1;
}

void move(Tabuleiro* t, uint64_t or, uint64_t d) {
	t->pecas[t->turno] = (or | d) ^ t->pecas[t->turno];
	t->pecas[adv(t->turno)] &= ~d;
}

// descobre posição de bit, via sequência de De Bruijn
// a entrada precisa ter APENAS UM bit ligado, senão avacalha tudo
uint8_t indiceBit(uint64_t t) {
	static const uint64_t deBruijn = 0x022fdd63cc95386d;
	static const uint8_t offsets[] = {0,  1,  2, 53,  3,  7, 54, 27,
	                                  4, 38, 41,  8, 34, 55, 48, 28,
	                                 62,  5, 39, 46, 44, 42, 22,  9,
	                                 24, 35, 59, 56, 49, 18, 29, 11,
	                                 63, 52,  6, 26, 37, 40, 33, 47,
	                                 61, 45, 43, 21, 23, 58, 17, 10,
	                                 51, 25, 36, 32, 60, 20, 57, 16,
	                                 50, 31, 19, 15, 30, 14, 13, 12};
	return offsets[(t * deBruijn) >> 58];
}
