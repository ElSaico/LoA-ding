#include <stdio.h>
#include <limits.h>
#include "ia.h"

int eval(Tabuleiro t, Jogador j) {
	uint64_t pc = pecas(t, j);
	uint64_t pca = pecas(t, adv(j));
	
	/*uint64_t p, p0 = pc, pa0 = pca;
	int gl = 0, gla = 0;
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		gl += count(movePara(t, p));
		p0 &= ~p;
	}*/
	
	//printf("%d\n", gl);
	if (vitoria(pc))
		return INT_MAX;
	else if (vitoria(pca))
		return INT_MIN;
	else
		return 1000*nGrupos(pca) + 500*(8-nGrupos(pc)) /*+ 100*gl*/;
}

int minimax(Tabuleiro t, Jogador j, int n, int alfa, int beta) {
	if (n == 0)
		return eval(t, j);
	
	int a0 = alfa;
	uint64_t p = 0, d = 0, d0, p0;
	t.turno = j;
	p0 = pecas(t, j);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(t, p);
		while (d0) {
			d = (d0 & (d0-1)) ^ d0;
			move(&t, p, d);
			a0 = -minimax(t, adv(j), n-1, -beta, -alfa);
			if (a0 > alfa)
				alfa = a0;
			if (alfa >= beta)
				return alfa;
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	return alfa;
}

int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t) {
	int m = INT_MIN, m0;
	uint64_t p = 0, d = 0, d0, p0;
	t.turno = adv(t.jogador);
	p0 = pecas(t, t.turno);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(t, p);
		while (d0) {
			d = (d0 & (d0-1)) ^ d0;
			move(&t, p, d);
			m0 = -minimax(t, t.jogador, 4, INT_MIN, INT_MAX);
			if (m0 > m) {
				m = m0;
				*or = p;
				*dst = d;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	return m;
}
