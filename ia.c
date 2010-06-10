#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "ia.h"

uint64_t rand64() {
	return rand() ^ ((uint64_t)rand() << 15) ^ ((uint64_t)rand() << 30) ^
		((uint64_t)rand() << 45) ^ ((uint64_t)rand() << 60);
}

void initHash() {
	srand(time(NULL));
	for (int i = 0; i < 65; ++i) {
		hash[0][i] = rand64();
		hash[1][i] = rand64();
	}
}

uint64_t getHash(Tabuleiro t, Jogador j) {
	int i;
	uint64_t k, h = hash[j][0];
	for (i = 1, k = 1; i <= 64; ++i, k <<= 1) {
		if (k & t.p_jogador)
			h ^= hash[t.jogador][i];
		if (k & t.p_adv)
			h ^= hash[adv(t.jogador)][i];
	}
	//printf("%lld\n", h % TSIZE);
	return h;
}

int lerHash(Tabuleiro t, Jogador j, int n, int alfa, int beta) {
	Trans* v = &trans[getHash(t, j) % TSIZE];
	//printf("L %p %016llx %016llx %d\n", v, v->hash, getHash(t, j), v->ply);
	
	if (v->hash == getHash(t, j)) {
		if (v->ply >= n) {
			if (v->flag == H_FOLHA)
				return v->eval;
			if ((v->flag == H_ALFA) && (v->eval <= alfa))
				return alfa;
			if ((v->flag == H_BETA) && (v->eval >= beta))
				return beta;
		}
	}
    
	return HASH_NULL;
}

void gravarHash(Tabuleiro t, Jogador j, int n, int val, HashFlag flag) {
	Trans* v = &trans[getHash(t, j) % TSIZE];
	v->hash = getHash(t, j);
	v->tab = t;
	v->eval = val;
	v->flag = flag;
	v->ply = n;
	//printf("G %p %016llx %d\n", v, v->hash, v->ply);
}

int eval(Tabuleiro t, Jogador j) {
	return vitoria(pecas(t, j)) ? INT_MAX : rand();
}

int minimax(Tabuleiro t, Jogador j, int n, int alfa, int beta) {
	int val = lerHash(t, j, n, alfa, beta);
	if (val != HASH_NULL)
		return val;
	if (n == 0) {
		t.turno = j;
		val = eval(t, j);
		gravarHash(t, j, n, val, H_FOLHA);
		return val;
	}
	
	HashFlag flag = H_ALFA;
	int an = alfa;
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
			an = -minimax(tt, adv(j), n-1, -beta, -alfa);
			if (an >= beta) {
				gravarHash(t, j, n, an, H_BETA);
				return an;
			}
			if (an > alfa) {
				flag = H_FOLHA;
				alfa = an;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	
	gravarHash(t, j, n, alfa, flag);
	return alfa;
}

int minimax_root(uint64_t* or, uint64_t* dst, Tabuleiro t, int alfa, int beta) {
	clock_t init = clock();
	int m0;
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
			m0 = -minimax(tt, adv(t.turno), nmax, -beta, -alfa);
			if (m0 >= beta) {
				*or = p;
				*dst = d;
				return m0;
			}
			if (m0 > alfa) {
				alfa = m0;
				*or = p;
				*dst = d;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	
	double s = desde(init);
	printf("Tempo: %.2lf segundos.\n", s);
	return alfa;
}

int negamax(uint64_t* or, uint64_t* dst, Tabuleiro t) {
	return minimax_root(or, dst, t, INT_MIN+1, INT_MAX);
}

int mtdf(uint64_t* or, uint64_t* dst, Tabuleiro t, int f) {
	int g = f, beta;
	int upperBound = INT_MAX;
	int lowerBound = INT_MIN;
	uint64_t o, d;
	while (lowerBound < upperBound) {
		if (g == lowerBound)
			beta = g+1;
		else
			beta = g;
		g = minimax_root(&o, &d, t, beta-1, beta);
		if (g < beta)
			upperBound = g;
		else {
			*or = o;
			*dst = d;
			lowerBound = g;
		}
	}
	return g;
}
