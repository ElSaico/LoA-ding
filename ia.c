#include <stdlib.h>
#include <time.h>
#include <limits.h>
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
	int val = lerHash(t, j, n, alfa, beta);
	if (val != HASH_NULL)
		return val;
	if (n == 0) {
		val = eval(t, j);
		gravarHash(t, j, n, val, H_FOLHA);
		return val;
	}
	
	HashFlag flag = H_ALFA;
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
			move(&tt, p, d);
			a0 = -minimax(tt, adv(j), n-1, -beta, -alfa);
			tt.p_jogador = t.p_jogador;
			tt.p_adv = t.p_adv;
			if (a0 > alfa) {
				flag = H_FOLHA;
				alfa = a0;
			}
			if (alfa >= beta) {
				gravarHash(t, j, n, beta, H_BETA);
				return alfa;
			}
			d0 &= ~d;
		}
		p0 &= ~p;
	}
	
	gravarHash(t, j, n, alfa, flag);
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
