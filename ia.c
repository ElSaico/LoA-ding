#include <limits.h>
#include "ia.h"

int eval(uint64_t t) {
	return 0;
}

// TODO: fazer algo que FUNCIONE.
int minimax(Tabuleiro* ti, Jogador j, int n, int alfa, int beta) {
	if (n == 0) {
		if (vitoria(pecas(*ti, j)))
			return INT_MAX;
		else if (vitoria(pecas(*ti, adv(j))))
			return INT_MIN;
		else
			return eval(pecas(*ti, j));
	}
	
	Tabuleiro t = *ti;
	t.turno = j;
	uint64_t p, m, m0, p0;
	p0 = pecas(*ti, j);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		m0 = movePara(t, p);
		while (m0) {
			m = (m0 & (m0-1)) ^ m0;
			move(&t, p, m);
			alfa = max(alfa, -minimax(&t, adv(j), n-1, -beta, -alfa));
			if (alfa >= beta) {
				*ti = t;
				return alfa;
			}
			m0 &= ~m;
		}
		p0 &= ~p;
	}
	*ti = t;
	return alfa;
}

uint64_t negamax(Tabuleiro t) {
	Tabuleiro nt = t;
	minimax(&nt, t.jogador, 5, INT_MIN, INT_MAX);
	return pecas(nt, t.turno);
}
