#include <stdio.h>
#include <limits.h>
#include <time.h>
#include "ia.h"

int eval(Tabuleiro t, Jogador j) {
	t.turno = j;
	uint64_t pc = pecas(t, j);
	uint64_t pca = pecas(t, adv(j));
	
	uint64_t p, p0;
	int gl = 0, gla = 0;
	
	p0 = pc;
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		gl += count(movePara(t, p));
		p0 &= ~p;
	}
	
	p0 = pca;
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		gla += count(movePara(t, p));
		p0 &= ~p;
	}
	
	uint64_t gc = nGrupos(pc), gca = nGrupos(pca);
	if (gc == 1)
		return INT_MAX;
	else if (gca == 1)
		return INT_MIN;
	else
		return 1000*(13-gc) - 1500*(13-gca) + 250*gl - 500*gla;
}

int minimax(Tabuleiro t, Jogador j, int n, int alfa, int beta) {
	if (n == 0)
		return eval(t, j);
	
	int a0 = alfa;
	uint64_t p = 0, d = 0, d0, p0;
	Tabuleiro tt = t;
	t.turno = j;
	
	p0 = pecas(t, j);
	while (p0) {
		p = (p0 & (p0-1)) ^ p0;
		d0 = movePara(t, p);
		while (d0) {
			d = (d0 & (d0-1)) ^ d0;
			tt.p_jogador = t.p_jogador;
			tt.p_adv = t.p_adv;
			move(&tt, p, d);
			a0 = -minimax(tt, adv(j), n-1, -beta, -alfa);
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
			m0 = -minimax(tt, t.jogador, 3, INT_MIN, INT_MAX);
			if (m0 > m) {
				m = m0;
				*or = p;
				*dst = d;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	
	printf("Tempo: %.2lf segundos.\n", (clock()-init) / (float)CLOCKS_PER_SEC);
	return m;
}
