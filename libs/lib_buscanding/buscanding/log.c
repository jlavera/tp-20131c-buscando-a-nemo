#include "log.h"

/*
 * @NAME: logInit
 * @DESC: Arma el archivo de log
 *
 * 	Default:
 * 	No verbose
 * 	Nivel: Error
 * 	Path: /log.log
*/
t_log* logInit(char** argv, char* who){
	int i = 0;
	//Valores default
	_Bool verbose = false;
	int logLevel = LOG_LEVEL_DEBUG;
	char path[30] = { 0 };
	strcpy(path, "log.log");

	while(argv[i] != NULL){
		if(string_equals_ignore_case(argv[i], "-v"))
			verbose = true;
		else if(string_equals_ignore_case(argv[i], "-ll")){ //Log level
			if(string_equals_ignore_case(argv[i+1], "trace"))
				logLevel = LOG_LEVEL_TRACE;
			if(string_equals_ignore_case(argv[i+1], "debug"))
				logLevel = LOG_LEVEL_DEBUG;
			if(string_equals_ignore_case(argv[i+1], "info"))
				logLevel = LOG_LEVEL_INFO;
			if(string_equals_ignore_case(argv[i+1], "warning"))
				logLevel = LOG_LEVEL_WARNING;
			if(string_equals_ignore_case(argv[i+1], "error"))
				logLevel = LOG_LEVEL_ERROR;
		} else if(string_equals_ignore_case(argv[i], "-log")){//path
			strcpy(path, argv[i+1]);
		}
		i++;
	}

	char aux[40] = { 0 };
		strcat(aux, "touch ");
		strcat(aux, path);
	system(aux);

	t_log* auxLog = log_create(path, who, verbose, logLevel);

	log_error(auxLog, "-----------------------------------------");
	return auxLog;
}
