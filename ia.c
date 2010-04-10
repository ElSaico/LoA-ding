#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "ia.h"

int eval(Tabuleiro t, Jogador j) {
	return vitoria(pecas(t, j)) ? INT_MAX : rand();
}

int minimax(Tabuleiro t, Jogador j, int n, int alfa, int beta, clock_t tm) {
	if (n == 0) {
		t.turno = j;
		return eval(t, j);
	}
	
	int a0 = alfa, an = alfa;
	uint64_t p = 0, d = 0, d0, p0;
	Tabuleiro tt = t;
	t.turno = j;
	
	p0 = pecas(t, j);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(t, p);
		while (d0) {
			if (desde(tm) >= 4.99)
				return alfa == a0 ? eval(t, j) : alfa;
			d = (d0 & (d0-1)) ^ d0;
			tt.p_jogador = t.p_jogador;
			tt.p_adv = t.p_adv;
			move(&tt, p, d);
			an = -minimax(tt, adv(j), n-1, -beta, -alfa, tm);
			if (an > alfa)
				alfa = an;
			if (alfa >= beta)
				return alfa;
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	return alfa;
}

int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t) {
	srand(time(NULL));
	clock_t init = clock();
	int m = INT_MIN+1, m0;
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
			m0 = -minimax(tt, adv(t.turno), nmax, INT_MIN, -m, init);
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
	printf("Nivel: 1~%d. Tempo: %.2lf segundos.\n", nmax, s);
	if (s < 5)
		++nmax;
	return m;
}
