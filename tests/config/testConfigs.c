#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testConfigs.h"
#include <buscanding/config.h>
#include <buscanding/list.h>

typedef struct{
	char* nombre;
	t_list* objetivos;
} t_niveles;

int main (int argc, char *argv[])
{

	t_config *conf = config_try_create ("test.conf", "simbolo");

	char** niveles = config_try_get_array_value(conf, "planDeNiveles");
	t_list* listaNiveles;
	t_list* listaObjetivos;
	listaNiveles = list_create();

	t_niveles aux;

    int j,i = 0;
    char* stringABuscar = malloc(sizeof(char)*23);
    while (niveles[i] != NULL) {//Cicla los niveles
    	sprintf(stringABuscar, "obj[%s]", niveles[i]); //Arma el string a buscar

    	char** objetivos = config_try_get_array_value(conf, stringABuscar);//Lo busco
    	j=0;
    	//Por cada uno, genero una lista (el malloc esta en el list_create)
    	listaObjetivos = list_create();

    	while(objetivos[j] != NULL){//Vuelvo a ciclar por objetivos
			list_add_new(listaObjetivos, objetivos[j], sizeof(char*));//Armo la lista
			printf(">> %s \t %s \n", niveles[i], objetivos[j]);//Debug
			j++;
    	}
    	aux.nombre = niveles[i];
    	aux.objetivos = listaObjetivos;
    	list_add_new(listaNiveles, &aux, sizeof(t_niveles));
	    i++;
    }
    free(stringABuscar);


    t_niveles* levantador;
    printf("\n------------------\n");//Debug
    for(i=0; i<list_size(listaNiveles); i++){
    	levantador = list_get(listaNiveles, i);
    	for(j=0; j<list_size(levantador->objetivos); j++){//Imprimo all
    		char* obj = list_get(levantador->objetivos, j);
    		printf(">> %s \t %s \n", levantador->nombre , obj);//Debug
    	}
    	free(levantador->objetivos);
    }
    free(listaNiveles);
    config_destroy(conf);


	return EXIT_SUCCESS;
}
