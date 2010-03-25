#include <stdlib.h>
#include "tabuleiro.h"

Tabuleiro novoTab(Jogador in) {
	Tabuleiro t;
	t.branco = INIT_BRANCO;
	t.preto = INIT_PRETO;
	t.jogador = in;
	t.turno = J_BRANCO;
	return t;
}

bool moveH(uint64_t orig, uint64_t dest) {
	uint64_t linha;
	for (uint64_t i = linha(0); i; i >> 8) {
		linha = i & orig;
		if (linha)
			break;
	}
}
