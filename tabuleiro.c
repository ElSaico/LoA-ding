#include <stdio.h>
#include <stdlib.h>
#include "tabuleiro.h"

Tabuleiro novoTab(Jogador in) {
	Tabuleiro t;
	t.p_jogador = in == J_BRANCO ? INIT_BRANCO : INIT_PRETO;
	t.p_adv = in == J_BRANCO ? INIT_PRETO : INIT_BRANCO;
	t.jogador = in;
	t.turno = J_BRANCO;
	return t;
}

uint64_t moveH(Tabuleiro t, uint64_t or) {
	if (or & ~t.p_jogador)
		return 0;
	
	uint64_t l, r, rl, rr;
	for (int i = 0; i < 8; ++i) {
		l = linha(i);
		if (l & or)
			break;
	}
	
	int c = count(l & grid(t));
	rl = l & (or << c);
	rr = l & (or >> c);
	
	r = 0;
	if (rl)
		r |= (rl - or) ^ or;
	if (rr)
		r |= (or - rr) ^ rr;

	return (t.p_adv & r) ? 0 : (rl | rr) & ~t.p_jogador;
}

uint64_t moveV(Tabuleiro t, uint64_t or) {
	if (or & ~t.p_jogador)
		return 0;
	
	uint64_t l, r, rl, rr;
	for (int i = 0; i < 8; ++i) {
		l = coluna(i);
		if (l & or)
			break;
	}
	
	int c = count(l & grid(t));
	rl = or << (c << 3);
	rr = or >> (c << 3);
	
	r = 0;
	if (rl)
		r |= ((rl - or) ^ or) & l;
	if (rr)
		r |= ((or - rr) ^ rr) & l;

	return (t.p_adv & r) ? 0 : (rl | rr) & ~t.p_jogador;
}
