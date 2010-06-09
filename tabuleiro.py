def pos(x, y):
	return linha(x) & coluna(y)

def coord(t, x, y):
	return t & pos(x, y)

def linha(x):
	return 0xFF00000000000000 >> (x << 3)

def coluna(x):
	return 0x8080808080808080 >> x

def count(x):
	return bin(x).count('1')

class Tabuleiro:
	DIAG_PRI = 0x8040201008040201
	DIAG_SEC = 0x8000000000000000
	LIMITE_O = 0x7F7F7F7F7F7F7F7F
	LIMITE_L = 0xFEFEFEFEFEFEFEFE

	pecas = {"Branco": 0x0081818181818100, "Preto": 0x7E0000000000007E}
	adv = {"Branco": "Preto", "Preto": "Branco"}
	turno = "Preto"
	
	def __init__(self, jogador):
		self.jogador = jogador
	
	def grid(self):
		return self.pecas["Branco"] ^ self.pecas["Preto"]
	
	def linhaXY(self, j):
		for i in range(8):
			if linha(i) & j:
				return linha(i)
		return 0
	
	def colunaXY(self, j):
		for i in range(8):
			if coluna(i) & j:
				return coluna(i)
		return 0
	
	def diagPriXY(self, jg):
		i, j = 0x80, 0x80
		while i <= self.DIAG_PRI:
			if i & jg:
				return i
			j >>= 1
			i = (i << 8) + j
		return 0
	
	def diagSecXY(self, jg):
		i, j = 1, 1
		while i <= self.DIAG_SEC:
			if i & jg:
				return i
			j <<= 1
			i = (i << 8) + j
		return 0
	
	def moveLinha(self, p0, ln, n):
		r, l, ret = 0, 0, 0
		c = count(ln & self.grid())
		rl = ln & (p0 << (n*c))
		rr = ln & (p0 >> (n*c))
	
		if rl:
			l = ((rl - p0) ^ p0) & ln
		if rr:
			r = ((p0 - rr) ^ rr) & ln

		p = self.pecas[self.adv[self.turno]]
		if not p & r:
			ret |= rr
		if not p & l:
			ret |= rl
		return ret & ~self.pecas[self.turno]
	
	def movePara(self, p0):
		return self.moveLinha(p0,   self.linhaXY(p0), 1) \
		     | self.moveLinha(p0,  self.colunaXY(p0), 8) \
		     | self.moveLinha(p0, self.diagPriXY(p0), 9) \
		     | self.moveLinha(p0, self.diagSecXY(p0), 7)
	
	def adj(self, t):
		to = t & self.LIMITE_O
		tl = t & self.LIMITE_L
		return ~t & ((to << 9) | (t  << 8) | (tl << 7) | (to << 1)
			       | (tl >> 1) | (to >> 7) | (t  >> 8) | (tl >> 9))
	
	def nGrupos(self, j):
		n = 0
		t = self.pecas[j]
		while t:
			p = (t & (t-1)) ^ t
			r = 0
			while p:
				r |= p
				p = t & self.adj(r)
			t &= ~r
			n += 1
		return n
	
	def vitoria(self):
		if self.nGrupos(self.turno) == 1:
			return True, self.turno
		elif self.nGrupos(self.adv[self.turno]) == 1:
			return True, self.adv[self.turno]
		return False, None
	
	def move(self, p0, d):
		self.pecas[self.turno] ^= p0 | d
		self.pecas[self.adv[self.turno]] &= ~d
	
	def mudaTurno(self):
		self.turno = self.adv[self.turno]
