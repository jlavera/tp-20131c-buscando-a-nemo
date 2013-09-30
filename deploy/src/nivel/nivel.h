/*
 * nivel.h
 *
 *  Created on: 27/04/2013
 *      Author: utnso
 */

#ifndef NIVEL_H_
#define NIVEL_H_

#include "gui/tad_items.h" //Este importa GUI
#include <buscanding/protocolo.h>
#include <buscanding/config.h>
#include <buscanding/sockets.h>
#include <buscanding/list.h>
#include <buscanding/log.h>

#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#define INIX 0
#define INIY 0

typedef struct {
	int numSock;
	_Bool blocked;
	_Bool marcado;
	char chirimbolo;
	t_list* recursos;
} personaje_t;

typedef struct {
	int numSock;
	int tiempoChequeo;
	int recovery;
} paramInterbloqueo_t;

void cerrarForzado(int);
void detectInterbloqueos(void* parametros);
void salidaPersonajes();
void notificarPersonajes();
void cerrarNivel(char * messageLog);

#endif /* NIVEL_H_ */
