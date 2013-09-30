#include "plataforma.h"

#define PUERTO_ORQ 5050
#define PUERTO_PLAN 5080

t_log* logger;
t_list* l_niveles;

char* feedKoopa;
int Quantum = 3;
int delay = 50000;

pthread_mutex_t semNiv;

t_config *config;

int main(int argc, char *argv[]) {

	pthread_mutex_init(&semNiv, NULL );

	if (argc < 4 || string_equals_ignore_case(argv[1], "-h")) {
		// Help, I need somebody
		// Help, not just anybody
		// Help, you know, I need someone
		// Help
		printf("Necesita minimo 3 argumentos:\n"
				"\t1) Archivo de configuracion del \"delay\"\n"
				"\t2) Archivo para actualizar Quantums\n"
				"\t3) Archivo que se pasara como parametro a Koopa (en el directorio de Koopa)\n"
				"[opcionales]\n"
				"\t\t> -v: Verboso\n"
				"\t\t> -ll [trace/debug/info/warning/error]: Nivel de logeo.\n"
				"\t\t> -log [PATH]: Nombre del archivo para logear (Crea y apenda)");
		exit(EXIT_SUCCESS);
	}

	signal(SIGINT, cerrarTodo);

	feedKoopa = malloc(sizeof(char) * 30);
	string_append_with_format(&feedKoopa, "./koopa %s", argv[3]);

	pthread_t tQuantum; //Thread para el quantum

	config = config_try_create(argv[1], "delay");
	// Obtenemos el delay
	delay = config_get_int_value(config, "delay");

	// y la sacamos
	config_destroy(config);

	logger = logInit(argv, "PLATAFORMA");

	Quantum = leerDesde(argv[2]); //Hace la primera lectura

	//Levnatamos el hilo para iNotify
	if (pthread_create(&tQuantum, NULL, (void*)iNotify, (void *) argv[2])) {
	 log_error(logger, "pthread_create: %s", strerror(errno));
	 exit(EXIT_FAILURE);
	 }

	orquestador();

	return EXIT_FAILURE;

}

void iNotify(char* archivo) {
	int fdnotify = -1;

	fdnotify = inotify_init();
	if (fdnotify < 0) {
		log_error(logger, "iNotify init: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	inotify_add_watch(fdnotify, archivo, IN_MODIFY | IN_CREATE);

	while (1) {
		char buffer[16]; //Magic number, mantiene 2 notificaciones :D

		if (read(fdnotify, buffer, sizeof(buffer)) < 0) {
			log_error(logger, "iNotify read: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		//-- Cunado llega aca, es porque algo cambio
		//-- Como solo watchea un archivo, no hace falta preguntar nada :D
		Quantum = leerDesde(archivo);
		sleep(1); //Hack porque el Vim lo rompe y lo vuelve a crear
	}

}

int leerDesde(char* archivo) {
	FILE* file = fopen(archivo, "rt");
	if (file == NULL ) {
		log_error(logger, "fopen(%s): %s", archivo, strerror(errno));
		exit(EXIT_FAILURE);
	}
	char quantum[10];
	fgets(quantum, 9, file);
	log_trace(logger, "Cambio de Quantum a: %d", atoi(quantum));
	fclose(file);

	return atoi(quantum);
}

void orquestador(void) {
	//Tirar hilos
	pthread_t threads[20]; //Solo 20 planificadores concurrentemente
	int cantidadHilos = 0; //Para que cuando sume empieze en 0;

	l_niveles = list_create();
	orq_t orqMsj;

	int lastPlan = PUERTO_PLAN;

	//--Boludeces de los sockets
	fd_set master, temp;
	struct sockaddr_in myAddress;
	struct sockaddr_in remoteAddress;
	int maxSock;
	int sockListener;
	int i;
	_Bool nivelEncontrado;

	iniSocks(&master, &temp, &myAddress, remoteAddress, &maxSock, &sockListener, PUERTO_ORQ, logger);
	while (1) {
		i = getSockChanged(&master, &temp, &maxSock, sockListener, &remoteAddress, &orqMsj, sizeof(orqMsj), logger);
		if (i != -1) { //Solo lo hace si el select esta levantando un socket que ya tenia. La primera vuelta sale con -700
			nivel_t nivel; // = (nivel_t*) malloc(sizeof(nivel_t));
			char* nombrePJ = malloc(sizeof(char));
			nivel_t* aux;
			int indList;
			int indListPers;

			pthread_mutex_lock(&semNiv);
			//--Switch según quien envió el mensaje
			switch (orqMsj.type) {
			case NIVEL:
				//--Switch según el tipo de mensaje
				switch (orqMsj.detail) {
				case SALUDO:

					//--Armar estructura nueva
					strcpy(nivel.ip, orqMsj.ip);
					strcpy(nivel.nombre, orqMsj.name);
					nivel.puerto = orqMsj.port;

					lastPlan = lastPlan + 15;
					nivel.puertoPlan = lastPlan;
					nivel.l_personajesRdy = list_create();
					nivel.l_personajesBlk = list_create();
					nivel.sock = i;

					list_add_new(l_niveles, (void*) &nivel, sizeof(nivel_t));

					log_trace(logger, "Se conectó el nivel: %s, IP: %s, Puerto: %d", orqMsj.name, orqMsj.ip, orqMsj.port);
					//Tira el nuevo hilo

					if (pthread_create(&threads[cantidadHilos++], NULL, planificador, (void *) &nivel)) { //-- Mandamos al hilo t0do el nivel
						log_error(logger, "pthread_create: %s", strerror(errno));
						exit(EXIT_FAILURE);
					}
					log_debug(logger, "Nuevo hilo de '%s' que atiende puerto: %d", orqMsj.name, lastPlan);

					break;
				case SALIR:
					for (indList = 0; indList < list_size(l_niveles); ++indList) { //Cicla en la lista niveles
						aux = (nivel_t*) list_get(l_niveles, indList);
						if (aux->sock == i) { //Cuando lo encuentra, la borra
							list_remove(l_niveles, indList);
							break;
						}
					}

					break;
					//----------------------------REGION CRITICA--------------------
				case HAYINTERBLOQUEO:
					log_trace(logger, "Llego mensaje de deadlock en nivel de socket %d", i);
					for (indList = 0; indList < list_size(l_niveles); ++indList) { //Cicla en la lista niveles
						aux = (nivel_t*) list_get(l_niveles, indList);
						if ((aux->sock == i) && (list_size(aux->l_personajesBlk) != 0)) { // Busca el nivel y si tiene algun personaje bloqueado

							message_t message;
							personaje_t *levantadorPJ;
							personaje_t victima;
							victima.index = 0;

							int contPjDLk, tope;
							tope = orqMsj.port;

							//--Ciclo que recibe todos los personajes del deadlock
							for (contPjDLk = 1; contPjDLk <= tope; contPjDLk++) {
								if (recv(i, &message, sizeof(message), 0) == -1) {
									log_error(logger, "Recv: %s", strerror(errno));
									exit(EXIT_FAILURE);
								}

								//--Buscar el personaje que nos mandaron
								int contPjBlk;
								for (contPjBlk = 0; contPjBlk < list_size(aux->l_personajesBlk); contPjBlk++) {
									levantadorPJ = list_get(aux->l_personajesBlk, contPjBlk);

									//--Si lo encuentra, sale del ciclo
									if (levantadorPJ->name == message.name)
										break;
								}
								//--Si el índice es mayor, setear nueva víctima
								if (victima.index < levantadorPJ->index)
									victima = *levantadorPJ;
							}

							log_info(logger, "\n>>>>>VICTIMA: %c\n", victima.name);
							message.detail = DIEPOTATO;
							//--Matar víctima
							if (send(victima.sockID, &message, sizeof(message), 0) == -1) {
								log_error(logger, "Send: %s", strerror(errno));
								exit(EXIT_FAILURE);
							}

							break;
						}
					}
					break;
					//****************REGION CRITICA*******************************
				}
				break;
			case PERSONAJE:
				//--Switch según el tipo de mensaje
				switch (orqMsj.detail) {
				case SALUDO:
					nivelEncontrado = false;
					//--Busca en la lista de niveles, el nivel pedido
					for (indList = 0; indList < list_size(l_niveles); ++indList) { //Cicla en la lista de niveles
						aux = (nivel_t*) list_get(l_niveles, indList);
						if (string_equals_ignore_case(aux->nombre, orqMsj.name)) {
							nivelEncontrado = true;
							log_trace(logger, "Se conectó el personaje: %c, Pide: %s", orqMsj.ip[0], orqMsj.name);

							//Cuando la encuentra, arma la estructura con la información del NIVEL y la envía.
							orqMsj.port = aux->puerto;
							strcpy(orqMsj.ip, aux->ip);

							if (send(i, &orqMsj, sizeof(orq_t), 0) == -1) {
								log_error(logger, "send: %s", strerror(errno));
								exit(EXIT_FAILURE);
							}

							//--Arma la estructura con la información del PLANIFICADOR de ese nivel y la envía

							//strcpy(orqMsj.ip, ipOrq);
							orqMsj.port = aux->puertoPlan;

							if (send(i, &orqMsj, sizeof(orq_t), 0) == -1) {
								log_error(logger, "send: %s", strerror(errno));
								exit(EXIT_FAILURE);
							}
							break;
						}
					}
					if (!nivelEncontrado) { //Si no encontro el nivel, decirselo
						orqMsj.detail = NADA;
						if (send(i, &orqMsj, sizeof(orq_t), 0) == -1) {
							log_error(logger, "send: %s", strerror(errno));
							exit(EXIT_FAILURE);
						}
					}
					break;
				case SALIR:
					//--Busca en la lista de niveles, el nivel pedido
					for (indList = 0; indList < list_size(l_niveles); ++indList) { //Cicla en la lista de niveles
						aux = (nivel_t*) list_get(l_niveles, indList);
						if (!strcmp(aux->nombre, orqMsj.name)) {
							for (indListPers = 0; indListPers < list_size(aux->l_personajesRdy); ++indListPers) { //--Busca al personaje en la lista del nivel.

								nombrePJ = list_get(aux->l_personajesRdy, indListPers);
								if (orqMsj.ip[0] == *nombrePJ)
									list_remove(aux->l_personajesRdy, indListPers);
								break;
							}
						}
					}
					break;
				}
				break;
			}
			pthread_mutex_unlock(&semNiv);
		}
	}
}

void* planificador(void* argumentos) {
	int q = 1; //En que parte del quantum esta
	int contIndex = 0;
	int proxPj = 0;

	nivel_t argu;
	memcpy(&argu, argumentos, sizeof(nivel_t));

	message_t mensaje;
	personaje_t pjAuxiliar; //--Este sirve para crear los personajes nuevos
	personaje_t* pjLevantador = malloc(sizeof(personaje_t)); //--Este sirve para levantar de la lista y usar

//--Boludeces de los sockets
	fd_set master, temp;
	struct sockaddr_in myAddress;
	struct sockaddr_in remoteAddress;
	int maxSock;
	int sockListener;
	int i, contPj;
	_Bool encontrado = false, pedirNoRequiereQuantum;

//para forzar el turno si se desbloquea

	iniSocks(&master, &temp, &myAddress, remoteAddress, &maxSock, &sockListener, argu.puertoPlan, logger);
	while (1) {
		i = getSockChanged(&master, &temp, &maxSock, sockListener, &remoteAddress, &mensaje, sizeof(mensaje), logger);
		if (i != -1) { //Conexcion que cambia
			pthread_mutex_lock(&semNiv);
			switch (mensaje.type) {
			case SALUDO:
				pjAuxiliar.name = mensaje.name;
				pjAuxiliar.sockID = i;
				pjAuxiliar.index = ++contIndex;
				pjAuxiliar.recursos = list_create(); //Arma la lista delos recursos

				list_add_new(argu.l_personajesRdy, &pjAuxiliar, sizeof(personaje_t));

				log_trace(logger, "Se agrego %c a la lista (%d)", mensaje.name, list_size(argu.l_personajesRdy));
				imprimirLista(argu.nombre, argu.l_personajesRdy, argu.l_personajesBlk, proxPj);

				if (list_size(argu.l_personajesRdy) == 1) {
					//--Mandar primer turno al primero

					mensaje.type = PERSONAJE;
					mensaje.detail = TURNO;

					if (send(pjAuxiliar.sockID, &mensaje, sizeof(message_t), 0) == -1) {
						log_error(logger, "send: %s", strerror(errno));
						exit(EXIT_FAILURE);
					}
				}

				break;
			case TURNO:
				pedirNoRequiereQuantum = true;
				if (mensaje.detail2 != NADA) {
					//--Agregar recurso pedido a su lista de recursos
					// (se lo agregamos aca, para saber poque recurso esta bloqueado, antes de mandarlo a la lista de bloqueados)
					list_add_new(pjLevantador->recursos, &(mensaje.detail2), sizeof(mensaje.detail2));
					pedirNoRequiereQuantum = false;

					if (mensaje.detail == BLOCK) { //Si volvió bloqueado, marcar personaje como bloqueado

						log_info(logger, "Personaje: %c esta bloquado por: %c", pjLevantador->name, mensaje.detail2);
						//-- Lo saca de listos, y lo pone en bloquados
						list_add(argu.l_personajesBlk, list_remove(argu.l_personajesRdy, proxPj));

						//Para que pase al otro PJ
						proxPj--;
						imprimirLista(argu.nombre, argu.l_personajesRdy, argu.l_personajesBlk, proxPj);

						q = Quantum + 1;
					}
					q--;
				}

				if (list_size(argu.l_personajesRdy) != 0) {

					//--Si ya terminó su quantum
					if (q >= Quantum) {
						proxPj++; //Solo avanza en la lista si se le acabo el Qauntum
						proxPj = abs(proxPj) % list_size(argu.l_personajesRdy);
						q = 1;
					} else {
						q++;
					}
					//--Cachear pj
					pjLevantador = list_get(argu.l_personajesRdy, proxPj);

					//--Si hay a quien enviarle el próximo turno

					mensaje.type = PERSONAJE;
					mensaje.detail = TURNO;

					if (pedirNoRequiereQuantum)
						usleep(delay);
					log_trace(logger, "Turno para %c", pjLevantador->name);
					if (send(pjLevantador->sockID, &mensaje, sizeof(message_t), 0) == -1) {
						log_error(logger, "send: %s", strerror(errno));
					}
				}
				break;
			case REINICIAR:
				mensaje.detail = DIEPOTATO;
				// Enviar mensaje de muerte al personaje, porque solicitó reiniciar.
				if (send(i, &mensaje, sizeof(mensaje), 0) == -1) {
					log_error(logger, "send: %s", strerror(errno));
					exit(EXIT_FAILURE);
				}
				break;
			case SALIR:
				encontrado = false;

				for (contPj = 0; contPj < list_size(argu.l_personajesBlk); contPj++) { //Cicla los personajes
					pjLevantador = list_get(argu.l_personajesBlk, contPj);
					if (pjLevantador->sockID == i) {
						encontrado = true; //Si lo encontras en Blk
						list_remove(argu.l_personajesBlk, contPj); //y sacarlo de la lista
						break;
					}
				}

				//Si no lo encuentra
				if (!encontrado) { //Si no lo encontras, buscarlo en rdy
					for (contPj = 0; contPj < list_size(argu.l_personajesRdy); contPj++) { //Cicla los personajes
						pjLevantador = list_get(argu.l_personajesRdy, contPj);
						if (pjLevantador->sockID == i) {
							list_remove(argu.l_personajesRdy, contPj); //y sacarlo de la lista
							break;
						}
					}
				}
				//Jamas deveria pasar que no lo encuentre

				int contRec;
				char* recurso;
				int j;
				personaje_t* pjLevantadorBlk = malloc(sizeof(personaje_t));
				//Si esta seteado "encontrado" es porque estaba bloqueado, entonces el ultimo recurso de la lista no es un recurso real.
				for (contRec = 0; contRec < list_size(pjLevantador->recursos) - (encontrado ? 1 : 0); contRec++) {
					recurso = list_get(pjLevantador->recursos, contRec);

					for (j = 0; j < list_size(argu.l_personajesBlk); j++) {
						//--Cicla los personajes bloqueados
						pjLevantadorBlk = list_get(argu.l_personajesBlk, j);
						//Si el ultimo recurso, es el que libera

						char* recursoLevantado = list_get(pjLevantadorBlk->recursos, list_size(pjLevantadorBlk->recursos) - 1);
						if (*recursoLevantado == *recurso) {
							log_trace(logger, "Se desbloqueo %c por %c recurso", pjLevantadorBlk->name, *recurso);
							//Desbloquear
							list_add(argu.l_personajesRdy, pjLevantadorBlk);
							list_remove(argu.l_personajesBlk, j);

							imprimirLista(argu.nombre, argu.l_personajesRdy, argu.l_personajesBlk, proxPj - 1);

							break;
						}
					}
				}

				//Limpia las cosas, porque se fue
				list_destroy_and_destroy_elements(pjLevantador->recursos, free);

				// forzar un mensaje de turno para volver a multiplexar
				if (list_size(argu.l_personajesRdy) > 0) {

					proxPj--;
					proxPj = abs(proxPj) % list_size(argu.l_personajesRdy);
					q = 1;

					imprimirLista(argu.nombre, argu.l_personajesRdy, argu.l_personajesBlk, proxPj);
					//--Cachear pj
					usleep(delay);
					mensaje.detail = TURNO;
					pjLevantador = list_get(argu.l_personajesRdy, proxPj);

					log_trace(logger, "Turno para %c", pjLevantador->name);
					if (send(pjLevantador->sockID, &mensaje, sizeof(message_t), 0) == -1) {
						log_error(logger, "send: %s", strerror(errno));
					}

				}

				if (mensaje.detail2 == FOSHIZZLE) {
					mensaje.detail2 = NADA;
					//Cuando un PJ termina tod0 su plan, se fija si hay otros PJs dando vueltas, o termina
					orqTerminoTodo(); //<-- Aca esta el execve
				}
				//End case SALIR
				break;
			}
			pthread_mutex_unlock(&semNiv);
		}
	}
	return NULL ;
}

void imprimirLista(char* nivel, t_list* rdy, t_list* blk, int cont) {
	char* tmp = malloc(20);
	char* retorno = malloc(500);
	int i;

	personaje_t* levantador;
	if (list_size(rdy) == 0 || cont < 0) { //Si no hay nadie listo, no se quien esta ejecutando
		sprintf(retorno, "Lista de: %s\n\tEjecutando:\n\tListos: \t", nivel);
	} else {
		levantador = list_get(rdy, cont);
		sprintf(retorno, "Lista de: %s\n\tEjecutando: %c\n\tListos: \t", nivel, levantador->name);
	}
	for (i = 0; i < list_size(rdy); i++) {
		levantador = list_get(rdy, i);
		sprintf(tmp, "%c -> ", levantador->name);
		string_append(&retorno, tmp);
	}
	sprintf(tmp, "\n\tBloqueados: \t");
	string_append(&retorno, tmp);
	for (i = 0; i < list_size(blk); i++) {
		levantador = list_get(blk, i);
		sprintf(tmp, "%c -> ", levantador->name);
		string_append(&retorno, tmp);
	}

	log_info(logger, retorno);
	free(tmp);
	free(retorno);
}

void orqTerminoTodo(void) {
	_Bool noHayNadieMas = true;
	int i;
	nivel_t* nivelLevantador;

	for (i = 0; i < list_size(l_niveles); i++) {
		nivelLevantador = list_get(l_niveles, i);

		//Tiene que no haber nadie mas en todos los niveles
		noHayNadieMas = noHayNadieMas && ((list_size(nivelLevantador->l_personajesRdy) == 0) && (list_size(nivelLevantador->l_personajesBlk) == 0));
	}

	if (noHayNadieMas) {
		log_info(logger, "Termino todo bien.");
		//Forzar cerrar niveles (personajes tendrian que estar cerrados)

		chdir("../../koopa/"); //TODO caundo haga los makefiles, tenemos que editar este
		//feedKoopa es global :D
		if (system(feedKoopa)) {
			log_trace(logger, "Orquestador finalizó exitosamente");
			cerrarTodo(EXIT_SUCCESS);
		} else {
			log_trace(logger, "Error en feedKoopa");
			cerrarTodo(EXIT_FAILURE);
		}
	}
}

void cerrarTodo(int razon) {
	system("killall -SIGINT nivel > /dev/null"); //No me digas si no hay nada!
	system("killall -SIGINT personaje > /dev/null");
	log_trace(logger, "Cerrado (%d).", razon);
	exit(EXIT_FAILURE);
}
