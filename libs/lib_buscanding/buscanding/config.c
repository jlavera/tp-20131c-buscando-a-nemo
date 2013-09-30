#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"


/*
 * @NAME: config_try_create
 * @DESC: Revisa si existe el archivo, y carga la configuracion
 * @PARAMS:
 * 		path - path del archivo de configuracion
 * 		optiones - (string delimitada con comas) Valida si ALMENOS tiene los parametro que le pases
 */
t_config *config_try_create (char *path, char* options){


	if (!file_exists (path))	//Si no existe, sale y muestra el error
	{
		printf("[[ERROR]] `%s` no encontrado o no disponible para la lectura.\n", path);
		exit(0);
	}
	t_config *conf = config_create (path);

    char* str = strdup(options);
    char* token=strtok (str,",");

    while(token)//va levantando cada string
    {
    	if (!config_get_string_value (conf, token)) //Si no existe, imprime el error
    	{
			printf("[[ERROR]] No se encontro `%s` en `%s`\n", token, path);
			exit(0); //Linux limpia la memoria cuando se cierra, asi que no me gasto en hacer free
		}
        token=strtok (NULL,","); //Pasa al siguiente token
    }

	free(str); //Libero las opciones, si llego hasta aca, es porque levanto bien
	return conf;
}

int file_exists( const char* path ){
    struct stat buffer;
    int exist = stat(path, &buffer);
    return (exist+1); //Devolveria 0 si lo cuentra, -1 si no
}

/*
 * @NAME: config_try_get_array_value
 * @DESC: Retorna un array con los valores asociados a la key especificada.
 * En el archivo de configuracion un valor de este tipo debería ser representado
 * de la siguiente forma [lista_valores_separados_por_coma]
 * Si no existe la key, revuelve -1
*/
char **config_try_get_array_value(t_config* conf, char* key){
	if( config_has_property(conf, key) )
		return config_get_array_value(conf, key);
	return NULL;
}
