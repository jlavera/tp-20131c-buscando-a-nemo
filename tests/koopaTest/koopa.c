#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <memoria.h>

int main(){
	t_memoria segm = crear_memoria(150);
	t_list* replica;

	almacenar_particion(segm, 'A', 2, "A\0");
	almacenar_particion(segm, 'B', 20, "bbbbbbbbbbbbbbbbbbb\0");
	almacenar_particion(segm, 'C', 30, "ccccccccccccccccccccccccccccc\0");
	almacenar_particion(segm, 'D', 40, "ddddddddddddddddddddddddddddddddddddddd\0");
	almacenar_particion(segm, 'E', 56, "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEeeeee\0");

	eliminar_particion(segm, 'C');
	almacenar_particion(segm, 't', 3, "tt\0");
	almacenar_particion(segm, 'C', 26, "kkkkkkkkkkkkkkkkkkkkkkkkk\0");
	almacenar_particion(segm, '1', 1, "1");


	//imprimir_lista();

	replica = particiones(segm);
	liberar_memoria(segm);

	printf("\n\n-----lista---------\n");
	printf("Libre\tIndice\tInicio\tFin\tID\tContenido\n");
	t_particion *aux;
	int i;
	for(i=0; i<list_size(replica); ++i){
		aux = list_get(replica, i);
		printf("%d)\t%d\t%d\t%d\t%c\t%s\n", aux->libre?1:0, i, aux->inicio, aux->inicio+aux->tamanio, aux->id, aux->dato);
	}
	printf("-------------------\n");

	return EXIT_SUCCESS;
}
