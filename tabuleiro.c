#include <stdlib.h>
#include "tabuleiro.h"

Tabuleiro* novoTab(Turno in) {
	Tabuleiro *t = (Tabuleiro*) malloc(sizeof(Tabuleiro));
	t->branco = INIT_BRANCO;
	t->preto = INIT_PRETO;
	t->turno = in;
	return t;
}

uint64_t branco(Tabuleiro* t) {
	return t->branco;
}
