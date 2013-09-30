#include "tad_items.h"
#include "stdlib.h"

void CrearItem(ITEM_NIVEL** ListaItems, int numSock, char id, int x, int y, char tipo, int cant_rec) {
	ITEM_NIVEL * temp;
	temp = malloc(sizeof(ITEM_NIVEL));

	temp->numSock = numSock;	// -1 si es box
	temp->id = id;
	temp->posx = x;
	temp->posy = y;
	temp->item_type = tipo;
	temp->quantity = cant_rec;
	temp->next = *ListaItems; // Agrega el temp al comienzo de la lista y mueve t0d0 a la derecha
	*ListaItems = temp;
}

//--Ahora también se reciben la cantidad de vidas por parámetro.
void CrearPersonaje(ITEM_NIVEL** ListaItems, int numSock, char id, int x, int y) {
	CrearItem(ListaItems, numSock, id, x, y, PERSONAJE_ITEM_TYPE, 0);
}

void CrearCaja(ITEM_NIVEL** ListaItems, char id, int x, int y, int cant) {
	CrearItem(ListaItems, -1, id, x, y, RECURSO_ITEM_TYPE, cant);
}

void BorrarRec(ITEM_NIVEL** ListaItems, char id) {
	ITEM_NIVEL * temp = *ListaItems;
	ITEM_NIVEL * oldtemp;

	if ((temp != NULL )&& (temp->id == id)){
	*ListaItems = (*ListaItems)->next;
	free(temp);
} else {
	while((temp != NULL) && (temp->id != id)) {
		oldtemp = temp;
		temp = temp->next;
	}
	if ((temp != NULL) && (temp->id == id)) {
		oldtemp->next = temp->next;
		free(temp);
	}
}
}

void BorrarPer(ITEM_NIVEL** ListaItems, int numSock) {
	ITEM_NIVEL * temp = *ListaItems;
	ITEM_NIVEL * oldtemp;

	if ((temp != NULL )&& (temp->numSock == numSock)){
	*ListaItems = (*ListaItems)->next;
	free(temp);
} else {
	while((temp != NULL) && (temp->numSock != numSock)) {
		oldtemp = temp;
		temp = temp->next;
	}
	if ((temp != NULL) && (temp->numSock == numSock)) {
		oldtemp->next = temp->next;
		free(temp);
	}
}
}

void MoverPersonaje(ITEM_NIVEL* ListaItems, int numSock, int x, int y) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->numSock != numSock)){
	temp = temp->next;
}
	if ((temp != NULL )&& (temp->numSock == numSock)){
	temp->posx = x;
	temp->posy = y;
}

}

int restarQuantityRec(ITEM_NIVEL* ListaItems, char id) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->id != id)){
	temp = temp->next;
}
	if ((temp != NULL )&& (temp->id == id)){
		//Que lo reste siempre, asi cunado sale y se lo da, queda bien
		return --(temp->quantity);

}
	return -1;
}

int restarQuantityPer(ITEM_NIVEL* ListaItems, int numSock) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->numSock != numSock)){
	temp = temp->next;
}
	if ((temp != NULL )&& (temp->numSock == numSock)){
	if (temp->quantity > 0) {
		return --(temp->quantity);
	} else
	return -1;
}
	return -1;
}

void sumarQuantityRec(ITEM_NIVEL* ListaItems, char id, int cant) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->id != id)){
	temp = temp->next;
}
	if ((temp != NULL )&& (temp->id == id))
		temp->quantity = temp->quantity + cant;
}

void sumarQuantityPer(ITEM_NIVEL* ListaItems, int numSock, int cant) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->numSock != numSock)){
	temp = temp->next;
}
	if ((temp != NULL )&& (temp->numSock == numSock))temp->quantity = temp->quantity + cant;
}

int getQuantityItem(ITEM_NIVEL* ListaItems, char id) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->id != id)){
	temp = temp->next;
}

	return temp->quantity;
}

int getPosPer(ITEM_NIVEL* ListaItems, int sock, int *x, int *y) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->numSock != sock)){
	temp = temp->next;
}
	if (temp->numSock == sock) {
		*x = temp->posx;
		*y = temp->posy;
		return 0;
	} else
		return -1;
}

int getPosRec(ITEM_NIVEL* ListaItems, char id, int *x, int *y) {

	ITEM_NIVEL * temp;
	temp = ListaItems;

	while ((temp != NULL )&& (temp->id != id)){
	temp = temp->next;
}
	if (temp->id == id) {
		*x = temp->posx;
		*y = temp->posy;
		return 0;
	} else
		return -1;
}
