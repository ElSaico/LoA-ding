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

uint64_t getHash(Tabuleiro *t, Jogador j) {
	int i;
	uint64_t k, h = hash[j][0];
	for (i = 1, k = 1; i <= 64; ++i, k <<= 1) {
		if (k & t->pecas[J_BRANCO])
			h ^= hash[J_BRANCO][i];
		if (k & t->pecas[J_PRETO])
			h ^= hash[J_PRETO][i];
	}
	return h;
}

int lerHash(Tabuleiro *t, Jogador j, int n, int alfa, int beta) {
	Trans* v = &trans[getHash(t, j) % TSIZE];
	
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

void gravarHash(Tabuleiro *t, Jogador j, int n, int val, HashFlag flag) {
	Trans* v = &trans[getHash(t, j) % TSIZE];
	v->hash = getHash(t, j);
	v->tab = *t;
	v->eval = val;
	v->flag = flag;
	v->ply = n;
}

int eval(Tabuleiro *t, Jogador j) {
	return abs(rand());
}

int negascout(Tabuleiro *t, Jogador j, int n, int alfa, int beta) {
	int val = lerHash(t, j, n, alfa, beta);
	if (val != HASH_NULL)
		return val;
	if (n == 0) {
		val = eval(t, j);
		gravarHash(t, j, n, val, H_FOLHA);
		return val;
	}
	
	HashFlag flag = H_ALFA;
	int an = alfa, bn = beta;
	uint64_t p = 0, d = 0, d0, p0;
	uint64_t backupBr = t->pecas[J_BRANCO];
	uint64_t backupPr = t->pecas[J_PRETO];
	
	p0 = t->pecas[j];
	t->turno = j;
	while (p0) {
		p = p0 & -p0;
		d0 = movePara(t, p);
		while (d0) {
			d = d0 & -d0;
			move(t, p, d);
			if (vitoria(t->pecas[adv(j)])) {
				gravarHash(t, j, n-1, INT_MIN+1, H_FOLHA);
				gravarHash(t, adv(j), n-1, INT_MAX, H_FOLHA);
				return INT_MIN+1;
			} else if (vitoria(t->pecas[j])) {
				gravarHash(t, j, n-1, INT_MAX, H_FOLHA);
				gravarHash(t, adv(j), n-1, INT_MIN+1, H_FOLHA);
				return INT_MAX;
			}
			an = -negascout(t, adv(j), n-1, -bn, -alfa);
			t->pecas[J_BRANCO] = backupBr;
			t->pecas[J_PRETO]  = backupPr;
			t->turno = j;
			if ((alfa < an) && (an < beta)) {
				an = -negascout(t, adv(j), n-1, -beta, -alfa);
				t->pecas[J_BRANCO] = backupBr;
				t->pecas[J_PRETO]  = backupPr;
				t->turno = j;
			}
			if (an > alfa) {
				flag = H_FOLHA;
				alfa = an;
			}
			if (alfa >= beta) {
				gravarHash(t, j, n, alfa, H_BETA);
				return alfa;
			}
			bn = alfa + 1;
			d0 ^= d;
		}
		p0 ^= p;
	}
	
	gravarHash(t, j, n, alfa, flag);
	return alfa;
}

int minimax_root(uint64_t* or, uint64_t* dst, Tabuleiro *t, int alfa, int beta) {
	int val = lerHash(t, t->turno, nmax, alfa, beta);
	if (val != HASH_NULL)
		return val;

	clock_t init = clock();
	HashFlag flag = H_ALFA;
	int m0;
	uint64_t p = 0, d = 0, d0, p0;
	uint64_t backupBr = t->pecas[J_BRANCO];
	uint64_t backupPr = t->pecas[J_PRETO];
	Jogador j = t->turno;
	
	p0 = t->pecas[t->turno];
	while (p0) {
		p = p0 & -p0;
		d0 = movePara(t, p);
		while (d0) {
			d = d0 & -d0;
			move(t, p, d);
			if (!(*or | *dst)) {
				*or = p;
				*dst = d;
			}
			if (vitoria(t->pecas[adv(j)])) {
				t->pecas[J_BRANCO] = backupBr;
				t->pecas[J_PRETO]  = backupPr;
				d0 ^= d;
				continue;
			} else if (vitoria(t->pecas[j])) {
				t->pecas[J_BRANCO] = backupBr;
				t->pecas[J_PRETO]  = backupPr;
				gravarHash(t, j, nmax-1, INT_MAX, H_FOLHA);
				gravarHash(t, adv(j), nmax-1, INT_MIN+1, H_FOLHA);
				*or = p;
				*dst = d;
				return INT_MAX;
			}
			m0 = -negascout(t, adv(t->turno), nmax, -beta, -alfa);
			t->pecas[J_BRANCO] = backupBr;
			t->pecas[J_PRETO]  = backupPr;
			t->turno = j;
			if (m0 >= beta) {
				*or = p;
				*dst = d;
				return m0;
			}
			if (m0 > alfa) {
				flag = H_FOLHA;
				alfa = m0;
				*or = p;
				*dst = d;
			}
			d0 ^= d;
		}
		p0 ^= p;
	}
	
	double s = desde(init);
	printf("Tempo: %.2lf segundos.\n", s);
	gravarHash(t, j, nmax, alfa, flag);
	return alfa;
}

int negamax(uint64_t* or, uint64_t* dst, Tabuleiro *t) {
	return minimax_root(or, dst, t, INT_MIN+1, INT_MAX);
}

int mtdf(uint64_t* or, uint64_t* dst, Tabuleiro *t, int f) {
	int g = f, beta;
	int upperBound = INT_MAX;
	int lowerBound = INT_MIN+1;
	while (lowerBound < upperBound) {
		if (g == lowerBound)
			beta = g+1;
		else
			beta = g;
		g = minimax_root(or, dst, t, beta-1, beta);
		if (g < beta)
			upperBound = g;
		else
			lowerBound = g;
	}
	return g;
}
