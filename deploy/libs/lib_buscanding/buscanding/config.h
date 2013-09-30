#ifndef BUSCANDOCONFIGS_H_
#define BUSCANDOCONFIGS_H_

#include <commons/config.h> //Lo tengo que agregar por el tipo "t_config"

t_config *config_try_create (char *path, char *options);
int file_exists (const char* path);
char **config_try_get_array_value(t_config*, char* key);

#endif
