#ifndef PERSONAJE_H_
#define PERSONAJE_H_

#include <buscanding/list.h>

// Estructura///////////////
typedef struct Nivel {
	char nomNivel[16];
	t_list *Objetivos;
} nivel;

void aumentarVidas();
void morir(char *mensajeMuerte);
void morirSenial();
void devolverRecursos();
int llegoAlRecurso(int posX, int posY, int posRX, int posRY);
int calculaMovimiento(int posX, int posY, int posRX, int posRY);
void actualizaPosicion(int movimiento, int *posX, int *posY);
void restarVidas();

#endif
