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
	return linha(posLinha(indiceBit(t)));
}

uint64_t colunaXY(uint64_t t) {
	return coluna(posColuna(indiceBit(t)));
}

uint64_t diagPriXY(uint64_t t) {
	int ib = indiceBit(t);
	int lc = posLinha(ib) - posColuna(ib);
	if (lc >= 0)
		return DIAG_PRI >> (8*lc);
	else
		return DIAG_PRI << (8*-lc);
}

uint64_t diagSecXY(uint64_t t) {
	int ib = indiceBit(t);
	int lc = posLinha(ib) + posColuna(ib) - 7;
	if (lc >= 0)
		return DIAG_SEC >> (8*lc);
	else
		return DIAG_SEC << (8*-lc);
}

uint64_t moveLinha(Tabuleiro *t, uint64_t or, uint64_t ln, int n) {
	uint64_t r = 0, l = 0, rl, rr, p, ret = 0;
	int c = count(ln & grid(*t));
	rl = ln & (or << (n*c));
	rr = ln & (or >> (n*c));
	
	if (rl)
		l = ((rl - or) ^ or) & ln;
	if (rr)
		r = ((or - rr) ^ rr) & ln;

	p = t->pecas[adv(t->turno)];
	ret |= p & r ? 0 : rr;
	ret |= p & l ? 0 : rl;
	return ret & ~t->pecas[t->turno];
}

uint64_t movePara(Tabuleiro *t, uint64_t or) {
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
		p = t & -t;
		r = 0;
		while (p) {
			r |= p;
			p = t & adj(r);
		}
		t ^= r;
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
