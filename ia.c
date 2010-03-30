#include <stdio.h>
#include <limits.h>
#include "ia.h"

int eval(uint64_t t) {
	return 0;
}

int minimax(uint64_t* or, uint64_t* d, Tabuleiro ti, Jogador j, int n, int alfa, int beta) {
	if (n == 0) {
		if (vitoria(pecas(ti, j)))
			return INT_MAX;
		else if (vitoria(pecas(ti, adv(j))))
			return INT_MIN;
		else
			return eval(pecas(ti, j));
	}
	
	Tabuleiro t = ti;
	uint64_t p = 0, d = 0, d0, p0;
	p0 = pecas(ti, j);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(ti, p);
		while (d0) {
			d = (d0 & (d0-1)) ^ d0;
			t.p_jogador = ti.p_jogador;
			t.p_adv = ti.p_adv;
			move(&t, p, d);
			alfa = max(alfa, -minimax(or, d, t, adv(j), n-1, -beta, -alfa));
			if (alfa >= beta) {
				*or = p;
				*d = d;
				return alfa;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	*or = p;
	*d = d;
	return alfa;
}

int negamax(uint64_t* or, uint64_t* d, Tabuleiro t) {
	return minimax(or, d, t, adv(t.jogador), 3, INT_MIN, INT_MAX);
}
