#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_

#include <buscanding/protocolo.h>
#include <buscanding/sockets.h>
#include <buscanding/log.h>
#include <buscanding/list.h>
#include <buscanding/config.h>

#include <sys/inotify.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

typedef struct {
	char name;
	int sockID;
	int index;
	t_list* recursos;
} personaje_t;

typedef struct{
	char nomNivel[16];
	t_list* personajes;
} nivelEnWait;

typedef char recurso;

//--Estructura interna de lista de niveles en el orquestador//////////////
typedef struct {
	int sock;  //dudoso
	int puerto;
	int puertoPlan;
	char ip[16];
	char nombre[16];
	t_list* l_personajesRdy;
	t_list* l_personajesBlk;
} nivel_t;


int leerDesde( char* archivo);
void orquestador(void);


void* planificador(void*);
void iNotify(char*);

void orqTerminoTodo(void);
void cerrarTodo(int);

void imprimirLista(char* nivel, t_list* rdy, t_list* blk, int cont);

_Bool nivelYaEstaEnColaWait(t_list* l_pjsWaitPorNivel,char nomNivelSolicitado[16], int* indiceNivel);
void agregamosPersonaje(t_list* l_pjsWaitPorNivel,int indiceNivel,int sockIdPersonaje);
void agregamosNivelAColaWait(t_list* l_pjsWaitPorNivel,char nomNivelSolicitado[16],int *indiceNivel);
void imprimirColaWait(t_list* l_pjsPorNivel) ;

#endif
