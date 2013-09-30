#include <pthread.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include "buscanding/sockets.h"
#include <buscanding/log.h>
#include <errno.h>

#define DIRECCION "127.0.0.1"
#define PUERTO 5000

t_log* loger;
void* hilaje(void* arg);

int main(int argc, char* argv[]) {

	loger = logInit(argv, "TESTSERVER");


	//--Boludeces de los sockets
	fd_set master, temp;
	struct sockaddr_in myAddress;
	struct sockaddr_in remoteAddress;
	int maxSock;
	int sockListener;
	int bufSize = 10;

	char buf[bufSize];

	iniSocks(&master, &temp, &myAddress, remoteAddress, &maxSock, &sockListener,
			PUERTO, loger);

	log_trace(loger, "Inicializado");

	pthread_t* threads;
	int cantidadHilos = -1; //Inicia en -1 para hacer el ++

	while (1) {
		getSockChanged(&master, &temp, &maxSock, sockListener, &remoteAddress, &buf, bufSize, loger);
		threads = (pthread_t*)realloc(NULL, sizeof(pthread_t)); //Realoca memoria para manejar mas hilos

		if (pthread_create(&threads[cantidadHilos], NULL, hilaje, (void *)buf)){
			log_error(loger, strerror(errno));
			exit(-1);
		}
		log_debug(loger, "Creado nuevo hilo");
	}
	for(; cantidadHilos > 0; cantidadHilos--)
		pthread_join(threads[cantidadHilos], NULL);
	free(threads);
	return 0;
}

void* hilaje(void* arg){
	char letras[15];
	strcpy(letras,arg);
	while(1){
		puts(letras);
		log_info(loger, "Impreso: %s", letras);
		sleep(3);
	}
  pthread_exit(0);
}
