#ifndef _TABULEIRO_H
#define _TABULEIRO_H

#include <stdint.h>

#define INIT_BRANCO 0x7E0000000000007EULL
#define INIT_PRETO  0x0081818181818100ULL

#define coord(t,x,y) (t & linhas[x] & colunas[y])
#define move(t,x0,x) (x | x0) ^ t

enum TTurno {J_NENHUM, J_BRANCO, J_PRETO};

static const uint64_t linhas[] = {0xFF00000000000000ULL, 0x00FF000000000000ULL,
           0x0000FF0000000000ULL, 0x000000FF00000000ULL, 0x00000000FF000000ULL,
           0x0000000000FF0000ULL, 0x000000000000FF00ULL, 0x00000000000000FFULL};

static const uint64_t colunas[] = {0x8080808080808080ULL, 0x4040404040404040ULL,
            0x2020202020202020ULL, 0x1010101010101010ULL, 0x0808080808080808ULL,
            0x0404040404040404ULL, 0x0202020202020202ULL, 0x0101010101010101ULL};

typedef enum TTurno Turno;

typedef struct tab {
	uint64_t branco;
	uint64_t preto;
	Turno turno;
} Tabuleiro;

Tabuleiro* novoTab(Turno in);
uint64_t bbranco(Tabuleiro* t);

#endif
