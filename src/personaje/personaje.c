#include "personaje.h"
#include <buscanding/protocolo.h>
#include <buscanding/config.h>
#include <buscanding/log.h>
#include <buscanding/sockets.h>

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Constantes///////////////////
#define MUERTESIGTERM " señal SIGTERM"
#define MUERTEINTERBLOQUEO " requerimiento del orquestador"
#define CARACTERFINENVIORECURSOS ' '

// Variables Globales
t_log* logger;
char * nombre;
char simbolo;
int vidas;
t_list *listaNiveles;
char * orquestador;
int sockPlanificador;
int sockOrquestador;
int sockNivel;
int nBytes;

//Globales para la se~al de morir
_Bool murioPersonaje = false;
int currNivel, currObj;

_Bool r; //para alternar en el movimiento

void cerrarSenial(int signum); //Prototipo del handler de SIGINT

int main(int argc, char *argv[]) {

	logger = logInit(argv, argv[1]);

	signal(SIGTERM, morirSenial);
	signal(SIGUSR1, aumentarVidas);
	signal(SIGINT, cerrarSenial);

	struct sockaddr_in socketInfo;
	t_config *configPersonaje;

	// Creamos el config
	configPersonaje = config_try_create( //"personaje1.config", // Modo Debug
			argv[1], "nombre,simbolo,planDeNiveles,vidas,orquestador");

	// Obtenemos el nombre
	nombre = config_get_string_value(configPersonaje, "nombre");

	// Obtenemos el simbolo
	simbolo = config_get_string_value(configPersonaje, "simbolo")[0];

	// Levantamos la lista de niveles y la lista de objetivos de cada nivel.
	// Creamos la lista de Niveles.
	char** niveles = config_try_get_array_value(configPersonaje, "planDeNiveles");
	t_list* listaObjetivos;
	listaNiveles = list_create();

	nivel aux;

	int j, i = 0;
	char* stringABuscar = malloc(sizeof(char) * 23);
	//Armamos lista de niveles con sus listas de objetivos del config
	while (niveles[i] != NULL ) {	//Cicla los niveles
		sprintf(stringABuscar, "obj[%s]", niveles[i]); //Arma el string a buscar

		char** objetivos = config_try_get_array_value(configPersonaje, stringABuscar); //Lo busco
		j = 0;
		//Por cada uno, genero una lista (el malloc esta en el list_create)
		listaObjetivos = list_create();
		while (objetivos[j] != NULL ) { //Vuelvo a ciclar por objetivos
			list_add_new(listaObjetivos, objetivos[j], sizeof(char)); //Armo la lista
			j++;
		}
		strcpy(aux.nomNivel, niveles[i]);
		aux.Objetivos = listaObjetivos;
		list_add_new(listaNiveles, &aux, sizeof(nivel));
		i++;
	}
	free(stringABuscar);

	// Obetenemos los datos del orquestador
	char * dir_orq = config_get_string_value(configPersonaje, "orquestador");
	char * ip_orq = strtok(dir_orq, ":");  //--Separar ip
	char * port_orq = strtok(NULL, ":"); //--Separar puerto

	int port_nivel;
	char ip_nivel[16];
	int port_planif;
	char ip_planif[16];

	// Ciclo para todas las veces que reinicia el proceso personaje.
	while (1) {
		// Obtenemos las vidas
		vidas = config_get_int_value(configPersonaje, "vidas");
		log_info(logger, "Vidas de %c: %d", simbolo, vidas);

		log_trace(logger, "Conectado Con el Orquestador");

		currNivel = 0;

		murioPersonaje = false;

		while (vidas > 0) {
			murioPersonaje = false;

			// Ciclo por todos los niveles
			for (; currNivel < list_size(listaNiveles) && vidas > 0; currNivel++) {

				// Creamos el socket que vamos a usar con el orquestador
				if ((sockOrquestador = socket(AF_INET, SOCK_STREAM, 0)) != 0) {
					socketInfo.sin_family = AF_INET;
					socketInfo.sin_addr.s_addr = inet_addr(ip_orq);
					socketInfo.sin_port = htons(atoi(port_orq));
				}

				// Conectar el socket con la direccion 'socketInfo'.
				if (connect(sockOrquestador, (struct sockaddr*) &socketInfo, sizeof(socketInfo)) == -1) {
					log_error(logger, "send: %s", strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Armamos mensaje de saludo con nuestro nombre
				orq_t messageOrq;
				messageOrq.type = PERSONAJE;
				messageOrq.detail = SALUDO;
				messageOrq.ip[0] = simbolo;
				nivel *nivel = list_get(listaNiveles, currNivel);
				strcpy(messageOrq.name, nivel->nomNivel);

				log_trace(logger, "Pide nivel: %s", messageOrq.name);

				// Envía el "Saludo"
				enviaMensaje(sockOrquestador, &messageOrq, sizeof(messageOrq), logger, "Saludo al orquestador");

				// Recibe el "Saludo" con la info del nivel
				recibeMensaje(sockOrquestador, &messageOrq, sizeof(messageOrq), logger, "Info del nivel");

				log_trace(logger, "Recibe info: %s", messageOrq.name);

				if (messageOrq.detail == SALUDO) {
					// Tenemos el saludo, sacamos los datos (ip,puerto)
					strcpy(ip_nivel, messageOrq.ip);
					port_nivel = messageOrq.port;
				} else if (messageOrq.detail == NADA) {
					log_error(logger, "No existe el nivel.");
					exit(EXIT_FAILURE);
				} else {
					log_error(logger, "Tipo de mensaje incorrecto, se esperaba SALUDO");
					exit(EXIT_FAILURE);
				}

				// Recibe el "Saludo" con la info del planificador
				recibeMensaje(sockOrquestador, &messageOrq, sizeof(messageOrq), logger, "Info planificador");
				
				if (messageOrq.detail == SALUDO) {
					// Tenemos el saludo, sacamos los datos (ip,puerto)
					strcpy(ip_planif, messageOrq.ip);
					port_planif = messageOrq.port;
				} else {
					log_error(logger, "Tipo de mensaje incorrecto, se esperaba SALUDO");
					exit(EXIT_FAILURE);
				}

				//--Cierra socket con el orquestador
				close(sockOrquestador);

				int posX, posY; //para la pos del personaje
				int posRecursoY, posRecursoX; //para la pos del recurso

				// Creamos el socket que vamos a usar con el nivel
				if ((sockNivel = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
					log_error(logger, "send: %s", strerror(errno));
					exit(EXIT_FAILURE);
				}

				socketInfo.sin_family = AF_INET;
				socketInfo.sin_addr.s_addr = inet_addr(ip_nivel);
				socketInfo.sin_port = htons(port_nivel);

				log_info(logger, "IP del Nivel: %s \t Puerto del nivel: %i", ip_nivel, port_nivel);
				// Conectar el socket del nivel con la direccion 'socketInfo'.
				if (connect(sockNivel, (struct sockaddr*) &socketInfo, sizeof(socketInfo)) == -1) {
					log_error(logger, "send: %s", strerror(errno));
					log_info(logger, "ERROR DE CONEXION CON EL NIVEL");
					// TODO aca esta el errooor! No se porque entra en este if (no se puede conectar)
					exit(EXIT_FAILURE);
				}

				log_trace(logger, "Conecta a nivel: %s, Ip: %s, Puerto: %d\n", messageOrq.name, messageOrq.ip, messageOrq.port);
				// Creamos el socket que vamos a usar con el planificador
				if ((sockPlanificador = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
					log_error(logger, "send: %s", strerror(errno));
					exit(EXIT_FAILURE);
				}

				socketInfo.sin_family = AF_INET;
				socketInfo.sin_addr.s_addr = inet_addr(ip_planif);
				socketInfo.sin_port = htons(port_planif);

				// Conectar el socket del planificador con la direccion 'socketInfo'.
				if (connect(sockPlanificador, (struct sockaddr*) &socketInfo, sizeof(socketInfo)) == -1) {
					log_error(logger, "send: %s", strerror(errno));
					exit(EXIT_FAILURE);
				}
				log_trace(logger, "Conecta a planificador: %s, Ip: %s, Puerto: %d\n", messageOrq.name, messageOrq.ip, messageOrq.port);

				// Armamos mensaje de saludo con nuestro nombre
				message_t message;
				message.type = SALUDO;
				message.name = simbolo;

				// Envía el "Saludo" al nivel
				enviaMensaje(sockNivel, &message, sizeof(message), logger, "Saludo al nivel");

				message.type = SALUDO;
				// Envía el "Saludo" a Planificador para que lo ponga en la lista de listos
				enviaMensaje(sockPlanificador, &message, sizeof(message), logger, "Saludo al planificador");

				log_trace(logger, "Saludé a planificador");

				// Recibe el "Saludo" del nivel
				recibeMensaje(sockNivel, &message, sizeof(message), logger, "Saludo del nivel");
				if (message.type == SALUDO) {
					// Tenemos el saludo, sacamos los datos (posicion inicial)
					posX = message.detail;
					posY = message.detail2;
				} else {
					log_error(logger, "Tipo de mensaje incorrecto, se esperaba SALUDO");
					exit(EXIT_FAILURE);
				}

				int8_t *auxObj;
				
				// Mientras tenga objetivos.
				for (currObj = 0; currObj < list_size(nivel->Objetivos); currObj++) {

					// Armamos mensaje de pedido de posicion con el objeto que necesitamos
					message.type = POSICION;
					auxObj = list_get(nivel->Objetivos, currObj);
					message.detail = *auxObj;

					// Envía el requerimiento de posicion
					enviaMensaje(sockNivel, &message, sizeof(message), logger, "Requerimiento de posicion");

					// Recibe la posicion del objetivo.
					recibeMensaje(sockNivel, &message, sizeof(message), logger, "Posicion del objeto");
					//--Guardar posición del objetivo
					if (message.type == POSICION) {
						posRecursoX = message.detail;
						posRecursoY = message.detail2;
					} else {
						log_error(logger, "Tipo de mensaje Incorrecto, se esperaba Posicion");
						exit(EXIT_FAILURE);
					}

					while (1) {
						if (murioPersonaje) { //Cuando haces SIGTERM, lo mas probable sea que estes clavado en el RECV, entonces catchea esee "error"
							morir("Muerte por señal");
							break;
						}

						// Esperamos el turno (recv del planificador)
						recibeMensaje(sockPlanificador, &message, sizeof(message), logger, "Espera turno");

						if (message.detail == DIEPOTATO) {
							morir("Muerte por Deadlock");
							break;
						}

						if (message.detail != TURNO) {
							log_error(logger, "Llegaron (%d, %d, %c, %d) cuando debía llegar TURNO", message.detail, message.detail2, message.name, message.type);
							exit(EXIT_FAILURE);
						}

						// Armamos mensaje de movimiento a realizar con el objeto que necesitamos.
						message.type = MOVIMIENTO;
						message.detail = calculaMovimiento(posX, posY, posRecursoX, posRecursoY); // Calculamos la siguiente posicion y devuelve el tipo de movimiento
						message.detail2 = *auxObj;
						message.name = simbolo;

						// Enviamos el requerimiento de movimiento
						enviaMensaje(sockNivel, &message, sizeof(message), logger, "Requerimiento de movimiento");

						// Recibe la confirmacion
						recibeMensaje(sockNivel, &message, sizeof(message), logger, "Confirmacion de movimiento");
						if (message.type == MOVIMIENTO) {
							actualizaPosicion(message.detail2, &posX, &posY);
							// Si nos autorizo el movimiento el nivel, actualizamos nuestra posicion a partir del mov.
							switch (message.detail) {
							case NADA:  //--Este NADA significa que se movió y no obtuvo nada
								message.detail = NADA;
								break;
							case OTORGADO:	//--Esto significa que le dieron el recurso
								message.detail = OTORGADO;
								log_trace(logger, "Recurso %c otorgado.", *auxObj);
								break;
							case BLOCK:	//--Esto significa que se bloqueó
								message.detail = BLOCK;
								log_trace(logger, "Bloquedo por recurso %c.", *auxObj);
								break;
							}
							//--Avisar al planificador que se temrinó el turno
							message.type = TURNO;
							message.detail2 = *auxObj;
							message.name = simbolo;
							enviaMensaje(sockPlanificador, &message, sizeof(message), logger, "Se termino el turno");

							if (message.detail != NADA)	//--Para pasar al próximo recursos
								break;
						} else {
							log_error(logger, "Tipo de mensaje Incorrecto, se esperaba Movimiento");
							exit(EXIT_FAILURE);
						}

						// fin llego al recurso
					} // Aca salimos del ciclo while.

				}
				if (message.detail == BLOCK) {
					// Si no tiene mas recursos, pero estaba bloqueado, espera 1 turno mas
					//Y si no está bloqueado, espera uno más porque pedir consume quantum <--MAL
					recibeMensaje(sockPlanificador, &message, sizeof(message), logger, "Turno de mas");

					//Puede llegar o DIE o TURNO
					//si te dio el turno, estas desbloquado, segui con tu vida
					if (message.detail == DIEPOTATO)
						morir("Muerte por DeadLock");

				}
				
				// Si llegamos aca es por 2 motivos: O murio el personaje o termino todo su plan de niveles
				// Entonces, si no murio significa que termino exitosamente -> cerramos todo.
				if (!murioPersonaje) {	//Sino intenta liberar 2 veces
					//--Cierra conexión a nivel
					log_info(logger, "Libera recursos");
					devolverRecursos(currNivel);
					if (currNivel >= list_size(listaNiveles) - 1) {

						log_destroy(logger);
						config_destroy(configPersonaje);
						list_destroy_and_destroy_elements(listaNiveles, free);
						list_clean_and_destroy_elements(listaObjetivos, free);
						free(listaObjetivos);

						exit(EXIT_SUCCESS);
					}
				}
				murioPersonaje = false;

				close(sockNivel); //Terminamos el nivel
				close(sockPlanificador);
			}	// Aca salimos del For (ya no me faltan recursos de este nivel)

			// ESTE ES EL UNICO PUNTO DE SALIDA DEL PROCESO, ADEMAS DE LAS SEÑALES.
			// Si salí del for de niveles y no fue por muerte del personaje, terminamos todos los niveles.

		}			// Aca salimos del while de vidas mayores a 0. Y vuelve arriba y ejecuta el proceso de nuevo.

	}			// Aca salimos del ciclo infinito del personaje.

	return 1;

}

// Funcion que le envia mensaje de salir
void devolverRecursos( currentNivel) {
	message_t message;
	message.type = SALIR;
	message.name = simbolo;

	if (currentNivel >= list_size(listaNiveles) - 1) {
		//--Si es el ultimo nivel en la lista, avisa
		message.detail2 = FOSHIZZLE;
	} else {
		message.detail2 = NADA;
	}

	//--Enviar mensaje de salida al nivel
	enviaMensaje(sockNivel, &message, sizeof(message), logger, "Salida del nivel");
	

	//--Enviar mensaje de salida al planificador y esperar confirmación
	enviaMensaje(sockPlanificador, &message, sizeof(message), logger, "Salida al planificador");
	recibeMensaje(sockPlanificador, &message, sizeof(message), logger, "Salida al planificador");

	//close(sockPlanificador);
	log_trace(logger, "Recursos liberados");
}

// Señal de SIGTERM cierra conexión con el nivel, devolviendo recursos (falta algo más??)
void cerrarSenial(int signum) {
	log_trace(logger, "Cerrado Forzoso.");
	devolverRecursos(-1);
	exit(EXIT_FAILURE);
}
// Le suma una vida al personaje. (PRIMERA FUNCION DE PERSONAJE =D)
void aumentarVidas() {
	vidas++;
	log_info(logger, "Vidas de %c: %d", simbolo, vidas);
}

void restarVidas() {
	vidas--;
	log_info(logger, "Vidas de %c: %d", simbolo, vidas);
}

// Funcion auxiliar que llama a morir con el parametro correspondiente
void morirSenial() {
	murioPersonaje = true;
}

// Imprime por pantalla, y reinicia el nivel o resetea el proceso segun corresponda
void morir(char* causaMuerte) {
	murioPersonaje = true;
	log_info(logger, "%s", causaMuerte);
	devolverRecursos(-1);
	restarVidas();
	currNivel--;
	currObj = 700; //Trampa para salir del For de objetivos
	sleep(1); //Sino re pi de antes que Plataforma lo saque
}

// Devolvemos 1 si el personaje no llego al recurso todavia y 0 si el
int llegoAlRecurso(int posX, int posY, int posRX, int posRY) {
	return ((posX == posRX) && (posY == posRY));
}

// Calcula el movimiento del personaje y devuelve el tipo de movimiento a ejecutar.
int calculaMovimiento(int posX, int posY, int posRX, int posRY) {

	if (posX == posRX && posY == posRY)
		return -1; // Es igual el X y el Y, entonces llego.

	while (1) {
		r = rand() % 2;
		// Analizamos si hay que moverse sobre el eje x

		if (r) {
			if (r && posX < posRX)
				return DERECHA;
			else if (posX > posRX)
				return IZQUIERDA;
		} else {
			// Sobre el eje y
			if (posY < posRY)
				return ABAJO;
			else if (posY > posRY)
				return ARRIBA;
		}
	}

	return -700; //Oh silly Eclipse
}

// Actualiza las variables posicion del personaje a partir del movimiento que recibe por parametro.
void actualizaPosicion(int movimiento, int *posX, int *posY) {
	switch (movimiento) {
// El eje Y es alreves, por eso para ir para arriba hay que restar en el eje y.
	case ARRIBA:
		(*posY)--;
		break;
	case ABAJO:
		(*posY)++;
		break;
	case DERECHA:
		(*posX)++;
		break;
	case IZQUIERDA:
		(*posX)--;
		break;

	}
}
