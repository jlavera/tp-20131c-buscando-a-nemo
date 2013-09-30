/*
 * testConfigs.h
 *
 *  Created on: Apr 16, 2013
 *      Author: utnso
 */

#ifndef TESTCONFIGS_H_
#define TESTCONFIGS_H_

#include <commons/config.h> //Lo tengo que agregar por el tipo "t_config"

t_config *config_try_create (char *path, char *options);
/*Trata de levantar las opciones.
 * 	Valida si existe el archivo y lo puede leer
 * 	Valida si ALMENOS tiene los parametro que le pases (como string delimitada con comas)
 *	TODO: validar los `obj` de conf de personajes
 *
 * 	Si Falla, imprime el error y aborta
 * 	Sino devuelve el t_config
 */
int file_exists (const char* path);
/* Revisa si existe el archivo
 * Usa stat()
 */

#endif /* TESTCONFIGS_H_ */
