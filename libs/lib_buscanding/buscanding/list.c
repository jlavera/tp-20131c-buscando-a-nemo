#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

/*
 * @NAME: list_add_new
 * @DESC: Agrega una copia de lo que le pases (malloc)
*/
void list_add_new(t_list *self, void * data, int tamanio){
	void* aux = malloc(tamanio);
	memcpy(aux, data, tamanio);
	list_add(self, aux);
}

int list_add_new_con_return(t_list *self, void * data, int tamanio){
	void* aux = malloc(tamanio);
	memcpy(aux, data, tamanio);
	return list_add(self, aux);
}


//void list_add_as_set(t_list *list, void *data, int tamanio){
//	int cont;
//	void* levantadorData;
//
//	for (cont = 0; cont < list_size(list); cont++){
//		levantadorData = list_get(list, cont);
//
//		if (*data == *levantadorData)
//			list_add_new(t_list, data, tamanio);
//	}
//
//}
