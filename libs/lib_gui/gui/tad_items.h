
#ifndef BUSCANDINGTADITEMS_H_
#define BUSCANDINGTADITEMS_H_

#include "gui.h"

void CrearItem(ITEM_NIVEL** ListaItems, int numSock, char id, int x, int y, char tipo, int cant_rec);

void CrearPersonaje(ITEM_NIVEL** ListaItems, int numSock, char id, int x, int y);

void CrearCaja(ITEM_NIVEL** ListaItems, char id, int x, int y, int cant);

void BorrarRec(ITEM_NIVEL** ListaItems, char id);

void BorrarPer(ITEM_NIVEL** ListaItems, int numSock);

void MoverPersonaje(ITEM_NIVEL* ListaItems, int numSock, int x, int y);

int restarQuantityRec(ITEM_NIVEL* ListaItems, char id);

int restarQuantityPer(ITEM_NIVEL* ListaItems, int numSock);

void sumarQuantityRec(ITEM_NIVEL* ListaItems, char id, int cant);

void sumarQuantityPer(ITEM_NIVEL* ListaItems, int numSock, int cant);

int getQuantityItem(ITEM_NIVEL* ListaItems, char id);

int getPosPer(ITEM_NIVEL* ListaItems, int sock, int *x, int *y);

int getPosRec(ITEM_NIVEL* ListaItems, char id, int *x, int *y);

#endif /* TADITEMS_H_*/
