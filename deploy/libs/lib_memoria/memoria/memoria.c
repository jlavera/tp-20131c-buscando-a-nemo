#include <stdlib.h>
#include <string.h> //para el memset
#include <stdio.h>
#include "memoria.h"

t_list* list;

int worst_fit( int tamanio, char id ){
	int i;
	int espacio = 0;
	int maxEsp = 0;
	int maxEspIndice = 0;
	t_particion *elem;


	for(i=0; i<list_size(list); ++i){//recorre toda la lista
		elem = list_get(list, i);
		if( elem->libre ){				//Si es un abujero
			espacio = elem->tamanio;	//levanta el tamanio
			if(espacio > maxEsp){		//Se queda con el maximo
				maxEsp = espacio;
				maxEspIndice = i;
			}
		}
	}
	/*if(tamanio > maxEsp){				//Si no entra en el segmento
	printf("%d) %d no entra en %d\n", maxEspIndice, tamanio, maxEsp);
		exit(EXIT_FAILURE);				//Si no lo exito, tiga SegFaul.
	}*/
	return maxEspIndice;				//Devuelve en que indice esta el abujero
}

static t_particion *arma_particion(char id, int inicio, int tamanio, t_memoria dato, bool libre){
	t_particion *tmp = malloc(sizeof(t_particion));
		tmp->id = id;
		tmp->inicio = inicio;
		tmp->tamanio = tamanio;
		tmp->dato = dato;
		tmp->libre = libre;
	return tmp;
}

t_memoria crear_memoria(int tamanio) {
	t_memoria memoria = malloc(tamanio); //Aloca memoria
	//memset(tmp, '0', tamanio); //seteo en 0 para evitar que Printf segFault'ee

	list = list_create();
	list_add(list, arma_particion('\0', 0, tamanio, NULL, true)); //Arma el primer abujero

	return memoria;
}


int almacenar_particion(t_memoria segmento, char id, int tamanio, char* contenido) {
	int indice = worst_fit( tamanio, id );
	int dato;

		t_particion* abujero = list_get(list, indice);		//Agara el abujero con mayor espacio
		dato = (int)segmento + abujero->inicio; //Aritmetica de punteros para todos :)



		if(abujero->tamanio == tamanio){	//Si la particion entra perfectamente
			abujero->id = id;				//Remplaza el abujero por la particion
			abujero->dato = (t_memoria)dato;
			abujero->libre = false;			//Deja de ser un abujero
		} else {
			list_add_in_index(list, indice, arma_particion(id, abujero->inicio, tamanio, (t_memoria)dato, false));
			abujero->dato = (t_memoria)dato;
			abujero->tamanio -= tamanio;	//Le saca un pedazo al abujero
			abujero->inicio += tamanio;		//Lo mueve hacia el costado
		}

	memcpy( (char*)dato, contenido, tamanio); //Copia desde inicio, tamanio veces, el contenido
	return 1;
}


int eliminar_particion(t_memoria segmento, char id) {
	t_particion *aux;
	int i;
	for(i=0; i<list_size(list); ++i){//Cicla en la lista
		aux = list_get(list, i);
		if(aux->id == id){//Cuando la encuentra, la borra
			aux->libre = true;
			aux->id = '\0';
			return 1;
		}
	}
	return -1;

}
void imprimir_lista(t_memoria segmento, int tamanio){
	printf("\n\n-----lista---------\n");
	printf("Libre\tIndice\tInicio\tFin\tID\tPuntero\tContenido\n");
	t_particion *aux;
	int i;
	for(i=0; i<list_size(list); ++i){
		aux = list_get(list, i);
		printf("%d)\t%d\t%d\t%d\t%c\t%p\t%s\n", aux->libre?1:0, i, aux->inicio, aux->inicio+aux->tamanio, aux->id, &(aux->dato), aux->dato);
		if( (aux->dato >= segmento) && ((aux->dato + aux->tamanio*sizeof(t_memoria)) <= (segmento + tamanio*sizeof(t_memoria)))){
			printf("!!!!\n");
		}
	}
	printf("-------------------\n");
}

void liberar_memoria(t_memoria segmento) {
	list_clean_and_destroy_elements(list, (void*)free);
	free(segmento);
}

t_list* particiones(t_memoria segmento) {
    t_list* list_imprimir = list_create();
    list_add_all(list_imprimir, list);
    int i;

    t_particion* levantador;
    for(i=0; i<list_size(list_imprimir); i++){
    	levantador = list_get(list_imprimir, i);				//Si es un abujero
        if( levantador->libre ){								//Hace que "dato" apunte al final
        	levantador->dato = segmento+levantador->inicio; 	//Iei Koopa 1.2
        }
    }


    return list_imprimir;
}
