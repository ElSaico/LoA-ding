#include <stdio.h>
#include <limits.h>
#include "ia.h"

int eval(Tabuleiro t, Jogador j) {
	return 0;
}

int minimax(Tabuleiro t, Jogador j, int n, int alfa, int beta, clock_t tm) {
	if (n == 0) {
		t.turno = j;
		return eval(t, j);
	}
	
	int a0 = alfa;
	uint64_t p = 0, d = 0, d0, p0;
	Tabuleiro tt = t;
	t.turno = j;
	
	p0 = pecas(t, j);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(t, p);
		while (d0) {
			if (desde(tm) >= 4.99)
				return alfa;
			d = (d0 & (d0-1)) ^ d0;
			tt.p_jogador = t.p_jogador;
			tt.p_adv = t.p_adv;
			move(&tt, p, d);
			a0 = -minimax(tt, adv(j), n-1, -beta, -alfa, tm);
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
	clock_t init = clock();
	int m = INT_MIN, m0;
	uint64_t p = 0, d = 0, d0, p0;
	Tabuleiro tt = t;
	
	p0 = pecas(t, t.turno);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(t, p);
		while (d0) {
			d = (d0 & (d0-1)) ^ d0;
			tt.p_jogador = t.p_jogador;
			tt.p_adv = t.p_adv;
			move(&tt, p, d);
			m0 = -minimax(tt, t.jogador, nmax, INT_MIN, INT_MAX, init);
			if (m0 > m) {
				m = m0;
				*or = p;
				*dst = d;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	
	double s = desde(init);
	printf("Nivel: %d~%d. Tempo: %.2lf segundos.\n", nmin, nmax, s);
	if (s < 5)
		++nmax;
	return m;
}
