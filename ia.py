def evalTab(t, j):
	return 0

def minimax(t, j, n, alfa, beta):
	if n == 0:
		t.turno = j
		return evalTab(t, j)
	
	a0 = alfa
	p = 0
	d = 0
	tt = t
	t.turno = j
	
	p0 = t.pecas[j]
	while p0:
		p = (p0 & (p0-1)) ^ p0
		d0 = t.movePara(p)
		while d0:
			d = (d0 & (d0-1)) ^ d0
			tt.pecas["Branco"] = t.pecas["Branco"]
			tt.pecas["Preto"] = t.pecas["Preto"]
			tt.move(p, d)
			a0 = -minimax(tt, t.adv[j], n-1, -beta, -alfa)
			if a0 > alfa:
				alfa = a0
			if alfa >= beta:
				return alfa
			d0 &= ~d
		p0 &= ~p
	
	return alfa

def negamax(t):
	nmin = 1000000
	m = nmin
	p = 0
	d = 0
	tt = t
	
	p0 = t.pecas[t.turno]
	org = 0
	dst = 0
	while p0:
		p = (p0 & (p0-1)) ^ p0
		d0 = t.movePara(p)
		while d0:
			d = (d0 & (d0-1)) ^ d0
			tt.pecas["Branco"] = t.pecas["Branco"]
			tt.pecas["Preto"] = t.pecas["Preto"]
			tt.move(p, d)
			m0 = -minimax(tt, t.adv[t.turno], 3, nmin, -m)
			if m0 > m:
				m = m0
				org = p
				dst = d
			d0 &= ~d
		p0 &= ~p
	
	return org, dst
