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
	for (int i = 0; i < 64; ++i) {
		hash[0][i] = rand64();
		hash[1][i] = rand64();
	}
}

int getHash(Tabuleiro t) {
	return 0;
}

int lerHash(Tabuleiro t, int n, int alfa, int beta) {
	/*Trans* v = &trans[getHash(t) % TSIZE];
	
	if (v->hash == getHash(t)) {
		if (v->ply >= n) {
			if (v->flag == H_FOLHA)
				return v->eval;
			if ((v->flag == H_ALFA) && (v->eval <= alfa))
				return alfa;
			if ((v->flag == H_BETA) && (v->eval >= beta))
				return beta;
		}
	}*/
    
	return HASH_NULL;
}

void gravarHash(Tabuleiro t, Jogador j, int n, int val, HashFlag flag) {
	Trans* v = &trans[getHash(t) % TSIZE];
	v->hash = getHash(t);
	v->move = pecas(t, j);
	v->eval = val;
	v->flag = flag;
	v->ply = n;
}

int eval(Tabuleiro t, Jogador j) {
	return 0;
}

int minimax(Tabuleiro t, Jogador j, int n, int alfa, int beta) {
	int val = lerHash(t, n, alfa, beta);
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
	return m;
}
