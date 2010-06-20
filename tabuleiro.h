#ifndef _TABULEIRO_H
#define _TABULEIRO_H

#include <stdint.h>
#include <stdbool.h>

#define INIT_BRANCO 0x0081818181818100ULL
#define INIT_PRETO  0x7E0000000000007EULL
#define DIAG_PRI    0x8040201008040201ULL
#define DIAG_SEC    0x0102040810204080ULL
#define LIMITE_O    0x7F7F7F7F7F7F7F7FULL
#define LIMITE_L    0xFEFEFEFEFEFEFEFEULL

#define pos(x,y) (0x8000000000000000ULL >> (((x) << 3)+(y)))
#define count(x) __builtin_popcountll(x)
#define grid(t) ((t).pecas[J_BRANCO] ^ (t).pecas[J_PRETO])
#define linha(x) (0xFF00000000000000ULL >> ((x) << 3))
#define coluna(x) (0x8080808080808080ULL >> (x))
#define adv(t) (t == J_BRANCO ? J_PRETO : J_BRANCO)
#define posLinha(t) (7-((t) >> 3))
#define posColuna(t) (7-((t) & 0x07))

typedef enum {
	J_BRANCO = 0,
	J_PRETO = 1,
	J_NENHUM
} Jogador;

typedef struct tab {
	uint64_t pecas[2];
	Jogador jogador;
	Jogador turno;
} Tabuleiro;

Tabuleiro novoTab(Jogador in);
void move(Tabuleiro* t, uint64_t or, uint64_t d);
uint64_t movePara(Tabuleiro* t, uint64_t or);
int nGrupos(uint64_t t);
bool vitoria(uint64_t t);
uint8_t indiceBit(uint64_t t);

#endif
