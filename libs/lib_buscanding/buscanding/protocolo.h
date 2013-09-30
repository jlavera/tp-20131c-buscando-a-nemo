#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdint.h> //para los "int8_t"//////
#include <unistd.h> //para que no tire warning el close(i);

#define SALUDO 1
#define MOVIMIENTO 2
#define POSICION 32
#define PEDIDO 4
#define CANTIDAD 5
#define TURNO 22
#define SALIR 9
#define SIGNALKILL 30
#define HAYINTERBLOQUEO 33
#define DIEPOTATO 66
#define REINICIAR 74

#define BLOCK 13
#define OTORGADO 54
#define FINTURNO 14
#define NADA 0
#define FOSHIZZLE 47 //El Pj termino tod0 su plan de todos sus niveles

#define ARRIBA 24
#define DERECHA 26
#define ABAJO 25
#define IZQUIERDA 27

#define NIVEL 1
#define PERSONAJE 2

typedef struct{
	int8_t type;
	int8_t detail;
	int8_t detail2;
	char name;
} message_t;

typedef struct{// Desde el punto de vista del orquestador
	int8_t type; 	// Nivel					-	Personaje
	int8_t detail;	// Saludo/Salir
	int	port; // Recibe puerto del nivel	-	Manda puerto del nivel
	char ip[16]; 	// Recibe Ip				-	Manda Ip
	char name[16]; // Nombre de nivel			-	Nombre de nivel pedido
} orq_t;

#endif
