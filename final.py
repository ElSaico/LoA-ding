#!/usr/bin/env python
import sys, pygame
from pygame.locals import *
from tabuleiro import *
from ia import *

border = 50
size = hsize, vsize = 640, 480

branco = 255, 255, 255
preto = 0, 0, 0
cinza = 127, 127, 127
vermelho = 255, 0, 0
verde = 0, 255, 0
azul = 0, 0, 255

mover = 0
mover_adv = 0
origem = 0
origem_adv = 0
venceu = False
vencedor = None

def posline(l, x):
	return (x-border)*8 / (l-2*border)

def linepos(x, y):
	return border + y*(x-2*border)/8

def sizepos(x):
	return (x-2*border) / 16

def coordXY(t, x, y):
	return coord(t, posline(vsize, y), posline(hsize, x))

def posXY(x, y):
	return pos(posline(vsize, y), posline(hsize, x))

def mostraPreto(l, c, cor):
	pygame.draw.ellipse(tela, cor, (linepos(hsize, c+0.25), linepos(vsize, l+0.25),
	                                sizepos(hsize),         sizepos(vsize)))

def mostraBranco(l, c, cor):
	pygame.draw.ellipse(tela, cor, (linepos(hsize, c+0.25), linepos(vsize, l+0.25),
	                                sizepos(hsize),         sizepos(vsize)), 1)

def draw(t):
	tela.fill(branco)
	
	if hsize >= 2*border and vsize >= 2*border:
		for i in range(8):
			tela.blit(font.render(str(8-i), True, preto, branco), (border/2, linepos(vsize, i+0.5)))
			tela.blit(font.render('A', True, preto, branco), (linepos(hsize, i+0.5), border/2))
			for j in range(8):
				if coord(mover, i, j):
					pygame.draw.rect(tela, azul, (linepos(hsize, j),  linepos(vsize, i),
					                            2*sizepos(hsize)+1, 2*sizepos(vsize)+1))
				elif coord(origem_adv, i, j):
					pygame.draw.rect(tela, vermelho, (linepos(hsize, j),  linepos(vsize, i),
					                                2*sizepos(hsize)+1, 2*sizepos(vsize)+1))
				
				if coord(origem, i, j):
					cor = azul
				else:
					cor = preto
				
				if coord(mover_adv, i, j):
					cor = vermelho
				else:
					cor = preto
				
				if coord(t.pecas["Branco"], i, j):
					mostraBranco(i, j, cor)
				elif (coord(t.pecas["Preto"], i, j)):
					mostraPreto(i, j, cor)
		
		for i in range(1, 8):
			pygame.draw.line(tela, preto, (border, linepos(vsize, i)), (hsize-border, linepos(vsize, i)))
			pygame.draw.line(tela, preto, (linepos(hsize, i), border), (linepos(hsize, i), vsize-border))
		
		if not venceu:
			if t.turno == t.jogador:
				quem = "Jogador"
			else:
				quem = "Adversario"
			mensagem = "Turno: {0} ({1})".format(t.turno, quem)
		else:
			if vencedor == t.jogador:
				quem = "Jogador"
			else:
				quem = "Adversario"
			mensagem = "{0} ({1}) venceu!".format(vencedor, quem)
		tela.blit(font.render(mensagem, True, preto, branco), (border, vsize-border/2))

	pygame.display.flip()

def jogarPC(t):
	t.mudaTurno()
	global mover, origem_adv, mover_adv
	mover = 0
	origem_adv = 0
	mover_adv = 0
	draw(t)
	origem_adv, mover_adv = negamax(t)
	t.move(origem_adv, mover_adv)
	t.mudaTurno()

def verificaVitoria(t):
	v, j = t.vitoria()
	if v:
		global vencedor, mover, origem_adv, return_adv
		vencedor = j
		mover = 0
		origem_adv = 0
		return_adv = 0
	return v 

def eventLoop(t):
	for e in pygame.event.get():
		if e.type == QUIT:
			return True
		elif e.type == VIDEORESIZE:
			global size, hsize, vsize
			size = e.size
			hsize = e.w
			vsize = e.h
			tela = pygame.display.set_mode(size, RESIZABLE)
			draw(t)
		elif e.type == MOUSEBUTTONDOWN:
			global venceu, mover, origem
			if not venceu:
				if mover:
					if e.button == 1:
						destino = posXY(*e.pos)
						if mover & destino:
							t.move(origem, destino)
							venceu = verificaVitoria(t)
							if venceu:
								draw(t)
								return False
							jogarPC(t)
							venceu = verificaVitoria(t)
							if venceu:
								draw(t)
								return False
					mover = 0
					origem = 0
				elif t.turno == t.jogador:
					origem = coordXY(t.pecas[t.jogador], *e.pos)
					if origem:
						mover = t.movePara(origem)
				draw(t)
	return False

def drawInicial():
	tela.fill(branco)
	tela.blit(font.render("LoA-ding - minimal lines, maximum action", True, preto, branco), (25, 25))
	tela.blit(font.render("Autor: Bruno Marques", True, preto, branco), (25, 50))
	tela.blit(font.render("Pressione P para jogar com as pretas", True, preto, branco), (25, 100))
	tela.blit(font.render("Pressione B para jogar com as brancas", True, preto, branco), (25, 125))
	pygame.display.flip()
	
def telaInicial():
	for e in pygame.event.get():
		if e.type == QUIT:
			sys.exit()
		elif e.type == KEYUP:
			if e.key == K_p:
				return True, "Preto"
			elif e.key == K_b:
				return True, "Branco"
	return False, None

tela = pygame.display.set_mode((400, 150))
pygame.display.set_caption("LoA-ding")
pygame.font.init()
font = pygame.font.Font(None, 18)

drawInicial()
sair = False
while not sair:
	sair, j = telaInicial()

t = Tabuleiro(j)
if (j == "Branco"):
	jogarPC(t)
tela = pygame.display.set_mode(size, RESIZABLE)
draw(t)

sair = False
while not sair:
	sair = eventLoop(t)
