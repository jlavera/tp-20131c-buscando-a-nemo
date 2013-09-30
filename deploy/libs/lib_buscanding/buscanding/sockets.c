#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <commons/string.h>

#include "sockets.h"
#include "protocolo.h"

void iniSocks(fd_set *master, fd_set *temp, struct sockaddr_in *myAddress, struct sockaddr_in remoteAddress, int *maxSock, int *sockListener, int puerto, t_log* logger) {

	int yes = 1;

	FD_ZERO(master);
	FD_ZERO(temp);

	//--Crea el socket
	if ((*sockListener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(logger, "socket: %s", strerror(errno));
		exit(1);
	}

	//--Setea las opciones para que pueda escuchar varios al mismo tiempo
	if (setsockopt(*sockListener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		log_error(logger, "setsockopt: %s", strerror(errno));
		exit(1);
	}

	//--Arma la información que necesita para mandar cosas
	myAddress->sin_family = AF_INET;
	myAddress->sin_addr.s_addr = INADDR_ANY;
	myAddress->sin_port = htons(puerto);

	//--Bindear socket al proceso server
	if (bind(*sockListener, (struct sockaddr *) myAddress, sizeof(*myAddress)) == -1) {
		log_error(logger, "bind: %s", strerror(errno));
		exit(1);
	}

	//--Escuchar
	if (listen(*sockListener, 100) == -1) {
		log_error(logger, "listen: %s", strerror(errno));
		exit(1);
	}

	//--Prepara la lista
	FD_SET(*sockListener, master);
	*maxSock = *sockListener;
}

int selectSocks(fd_set *master, fd_set *temp, int *maxSock, int sockListener, struct sockaddr_in remoteAddress, void *buf) {

	return -1;
}
/*
 * @NAME: getSockChanged
 * @DESC: Multiplexa con Select
 *
 * 	Valores de salida:
 * 	=0 = se agrego un nuevo soquet
 * 	<0 = Se cerro el soquet que devuelce
 * 	>0 = Cambio el soquet que devuelce
 */
signed int getSockChanged(fd_set *master, fd_set *temp, int *maxSock, int sockListener, struct sockaddr_in *remoteAddress, void *buf, int bufSize, t_log* logger) {

	int addressLength;
	int i;
	int newSock;
	int nBytes;

	*temp = *master;

	//--Multiplexa conexiones
	if (select(*maxSock + 1, temp, NULL, NULL, NULL ) == -1) {
		log_error(logger, "select: %s", strerror(errno));
		exit(1);
	}

	//--Cicla las conexiones para ver cual cambió
	for (i = 0; i <= *maxSock; i++) {
		//--Si el i° socket cambió
		if (FD_ISSET(i, temp)) {
			//--Si el que cambió, es el listener
			if (i == sockListener) {
				addressLength = sizeof(*remoteAddress);
				//--Gestiona nueva conexión
				newSock = accept(sockListener, (struct sockaddr *) remoteAddress, (socklen_t *) &addressLength);
				log_trace(logger, "Nueva coneccion en %d", newSock);
				if (newSock == -1)
					log_error(logger, "accept: %s", strerror(errno));
				else {
					//--Agrega el nuevo listener
					FD_SET(newSock, master);
					if (newSock > *maxSock)
						*maxSock = newSock;
				}
			} else {
				//--Gestiona un cliente ya conectado
				if ((nBytes = recv(i, buf, bufSize, 0)) <= 0) {

					//--Si cerró la conexión o hubo error
					if (nBytes == 0)
						log_trace(logger, "Fin de conexcion de %d.", i);
					else
						log_error(logger, "recv: %s", strerror(errno));

					//--Cierra la conexión y lo saca de la lista
					close(i);
					FD_CLR(i, master);
				} else {
					return i;
				}
			}
		}
	}
	return -1;
}

void enviaMensaje(int numSock, void* message, int size, t_log* logger){
	if (send(numSock, message, size, 0) == -1) {
		log_error(logger, strerror(errno));
		exit(EXIT_FAILURE);
	}
}
void recibeMensaje(int numSock, void* message, int size, t_log* logger){
	if ((recv(numSock, message, size, 0)) == -1) {
		log_error(logger, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

