/*
 * sockets.h
 *
 *  Created on: 08/05/2013
 *      Author: utnso
 */

#ifndef BUSCANDINGSOCKETS_H_
#define BUSCANDINGSOCKETS_H_

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/log.h>
#include <errno.h>

#include "protocolo.h"

void iniSocks(fd_set *master, fd_set *temp, struct sockaddr_in *myAddress,
		struct sockaddr_in remoteAddress, int *maxSock, int *sockListener,
		int puerto, t_log* logger);

int selectSocks(fd_set *master, fd_set *temp, int *maxSock, int sockListener, struct sockaddr_in remoteAddress, void *buf);

int getSockChanged(fd_set *master, fd_set *temp, int *maxSock, int sockListener, struct sockaddr_in *remoteAddress, void *buf, int bufSize, t_log* logger);

void enviaMensaje(int numSock, void* message, int size, t_log* logger);

void recibeMensaje(int numSock, void* message, int size, t_log* logger);

#endif /* SOCKETS_H_ */
