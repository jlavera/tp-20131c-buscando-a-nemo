// export LD_LIBRARY_PATH=/home/utnso/git/tp-20131c-buscando-a-nemo/libs/lib_gui/Debug:/home/utnso/git/tp-20131c-buscando-a-nemo/libs/lib_buscanding/Debug:/home/utnso/git/tp-20131c-buscando-a-nemo/libs/lib_commons/Debug
// export LD_LIBRARY_PATH=/home/utnso/git/buscando-a-nemo/libs/lib_gui/Debug:/home/utnso/git/buscando-a-nemo/libs/lib_buscanding/Debug:/home/utnso/git/buscando-a-nemo/libs/lib_commons/Debug

#include "nivel.h"

t_list* personajes;
t_log* logger;

pthread_mutex_t semNiv;

int main(int argc, char* argv[]) {
	// Lo puse t0d0 en el llamado de abajo, no se porque y si se puede capturar 2 veces la señal como estaba hecho.
	// signal(SIGINT, notificarPersonajes);

	pthread_mutex_init(&semNiv, NULL);

	logger = logInit(argv, argv[1]);

	// Capturamos sigint y avisamos a los personajes para que cierren y cerramos el nivel.
	signal(SIGINT, cerrarForzado);

	t_config *configNivel;
	char** arrCaja;
	char* nom_nivel;
	char* ip_orq;
	char* port_orq;
	char* dir_orq;
	char ip[16];
	int puerto;

	//--Creamos el config
	configNivel = config_try_create(
	//"nivel1.config" //Modo Debug
			argv[1], "Nombre,orquestador,TiempoChequeoDeadlock,Recovery,ip,puerto,Caja1");

	ITEM_NIVEL *ListaItems = NULL;

	//--Creamos cada caja de recursos
	char* cajaAux;
	cajaAux = malloc(sizeof(char) * 6);
	sprintf(cajaAux, "Caja1");

	//--Inicializa la lista con los personajes y sus recursos
	personajes = list_create();
	personaje_t pjAux;

	int t = 1;
	int cols = 0;
	int rows = 0;
	int posXCaja = 0;
	int posYCaja = 0;
	nivel_gui_inicializar();
	// Conseguimos el area del nivel
	char state = nivel_gui_get_area_nivel(&cols, &rows);
	// Validamos que no haya habido error
	if (state != EXIT_SUCCESS) {
		cerrarNivel("Error al intentar conseguir el area del nivel (GUI).");
		exit(EXIT_FAILURE);
	}
	char* messageLimitErr;
	messageLimitErr = malloc(sizeof(char) * 100);

	//--Mientras pueda levantar el array
	while ((arrCaja = config_try_get_array_value(configNivel, cajaAux)) != NULL ) {
		posXCaja = atoi(arrCaja[3]);
		posYCaja = atoi(arrCaja[4]);
		// Validamos que la caja a crear esté dentro de los valores posibles del mapa
		if (posXCaja > rows || posYCaja > cols || posXCaja < 1 || posYCaja < 1) {
			sprintf(messageLimitErr, "La caja %c excede los limites de la pantalla. (%d,%d) - (%d,%d)", arrCaja[1][0], posXCaja, posYCaja, rows, cols);
			cerrarNivel(messageLimitErr);
			exit(EXIT_FAILURE);
		}
		// Si pasó la validacion, la creamos.
		CrearCaja(&ListaItems, arrCaja[1][0], atoi(arrCaja[3]), atoi(arrCaja[4]), atoi(arrCaja[2]));
		//--Rearma el cajaAux para la iteracion
		sprintf(cajaAux, "Caja%d", ++t);
	}

	free(cajaAux);

	//--Boludeces de los sockets
	message_t message;
	fd_set master, temp;
	struct sockaddr_in myAddress;
	struct sockaddr_in remoteAddress;
	int maxSock;
	int sockListener;
	int sockOrq;

	//--Saludo al orquestador

	//--Obetenemos el string del nombre y dirección del orquestador
	nom_nivel = config_get_string_value(configNivel, "Nombre");
	dir_orq = config_get_string_value(configNivel, "orquestador");
	puerto = config_get_int_value(configNivel, "puerto");
	memcpy(ip, config_get_string_value(configNivel, "ip"), 16);
	// Conseguimos el tiempo de espera de chequeo
	int tiempoChequeo = config_get_int_value(configNivel, "TiempoChequeoDeadlock");

	ip_orq = strtok(dir_orq, ":"); 	//--Separar ip
	port_orq = strtok(NULL, ":");	//--Separar puerto

	// Crear un socket:
	struct sockaddr_in socketInfo;
	if ((sockOrq = socket(AF_INET, SOCK_STREAM, 0)) != 0) {

		socketInfo.sin_family = AF_INET;
		socketInfo.sin_addr.s_addr = inet_addr(ip_orq);
		socketInfo.sin_port = htons(atoi(port_orq));

		// Conectar el socket con la direccion 'socketInfo'.
		if (connect(sockOrq, (struct sockaddr*) &socketInfo, sizeof(socketInfo)) == -1) {
			perror("Connect");
			exit(EXIT_FAILURE);
		}

		log_info(logger, "Conexión con orquestador.");

		orq_t orqMsj;
		orqMsj.type = NIVEL;
		orqMsj.detail = SALUDO;
		strcpy(orqMsj.name, nom_nivel);

		//memcpy(orqMsj.ip, &localhost->sin_addr.s_addr, 16);
		strcpy(orqMsj.ip, ip);
		orqMsj.port = puerto;

		//--Envía el "Saludo" para ser agregado
		if (send(sockOrq, &orqMsj, sizeof(orq_t), 0) == -1) {
			perror("Saludo");
			exit(EXIT_FAILURE);
		}
	}

	// Definimos los threads
	pthread_t thr_interbloqueos;
	paramInterbloqueo_t parametros;
	parametros.numSock = sockOrq;
	parametros.tiempoChequeo = tiempoChequeo;
	parametros.recovery = config_get_int_value(configNivel, "Recovery");

	// y los lanzamos
	if (pthread_create(&thr_interbloqueos, NULL, (void*) detectInterbloqueos, (void *) &parametros)) {
		log_error(logger, strerror(errno));
		exit(EXIT_FAILURE);
	}

	int maxRows, maxCols;
	int posX, posY;
	int posItemY, posItemX;
	int i;

	nivel_gui_get_area_nivel(&maxRows, &maxCols);

	nivel_gui_dibujar(ListaItems, nom_nivel);

	iniSocks(&master, &temp, &myAddress, remoteAddress, &maxSock, &sockListener, puerto, logger);
	while (1) {
		//--Gestiona un cliente ya conectado
		i = getSockChanged(&master, &temp, &maxSock, sockListener, &remoteAddress, &message, sizeof(message_t), logger);
		if (i != -1) {

			pthread_mutex_lock(&semNiv);

			int contPj;
			int contRec;
			char *auxRec;
			personaje_t *tempAux;
			//--Recibe mensaje y define comportamiento según el tipo
			switch (message.type) {
			case SALUDO:
				//--Agregamos personaje a la lista de items y a la de personajes/recursos
				CrearPersonaje(&ListaItems, i, message.name, INIX, INIY);
				pjAux.numSock = i;
				pjAux.chirimbolo = message.name;
				pjAux.blocked = false;
				pjAux.recursos = list_create();
				list_add_new(personajes, &pjAux, sizeof(personaje_t));

				//--Armamos la estrucura para enviar la posición inicial
				message.type = SALUDO;
				message.detail = INIX;
				message.detail2 = INIY;

				log_info(logger, "Se agregó el personaje %c con el socket %d", message.name, i);

				if (send(i, &message, sizeof(message), 0) == -1)
					perror("Respuesta posición inicial");
				break;
			case POSICION:
				//--Obtiene posición del item pedido
				getPosRec(ListaItems, message.detail, &posX, &posY);

				//--Armamos la estrucura para enviar la posición del recurso pedido
				message.type = POSICION;
				message.detail = posX;
				message.detail2 = posY;

				if (send(i, &message, sizeof(message), 0) == -1)
					perror("Respuesta posición recurso");
				break;
			case MOVIMIENTO:
				//--Devuelve las posiciones X e Y del item
				if (getPosPer(ListaItems, i, &posX, &posY) == -1)
					perror("GetPosItem");

				switch (message.detail) {

				case ARRIBA:
					if (posY > 1)
						posY--;
					break;

				case ABAJO:
					if (posY < maxRows)
						posY++;
					break;

				case IZQUIERDA:
					if (posX > 1)
						posX--;
					break;
				case DERECHA:
					if (posX < maxCols)
						posX++;
					break;
				}

				//--Define confirmación y la devuelve
				message.type = MOVIMIENTO;
				message.detail2 = message.detail;
				message.detail = 1;
				if (send(i, &message, sizeof(message), 0) == -1)
					perror("Confirmacion");

				//--Desbloquear en caso de que esté bloqueado
				for (contPj = 0; contPj < list_size(personajes); contPj++){
					tempAux = list_get(personajes, contPj);

					if (tempAux->numSock == i && tempAux->blocked)
						tempAux->blocked = false;
				}

				MoverPersonaje(ListaItems, i, posX, posY);
				break;
			case PEDIDO:
				//--Obtiene posición del item pedido y del personaje
				getPosRec(ListaItems, message.detail, &posItemX, &posItemY);
				getPosPer(ListaItems, i, &posX, &posY);

				//--Valida si llegó al recurso
				if ((posItemX == posX) && (posItemY == posY)) {

					//-- Siempre le tiene que agregar a la lista de recursos, este o no bloqueado
					//--Agrega el recurso a la lista de recursos del personaje
					int contPj;
					personaje_t* tempAux;
					for (contPj = 0; contPj < list_size(personajes); contPj++) {
						tempAux = list_get(personajes, contPj);

						if (tempAux->numSock == i) {
							list_add_new(tempAux->recursos, &(message.detail), sizeof(message.detail));
							break;
						}
					}

					//--Resta uno a la cantidad del item
					int result = restarQuantityRec(ListaItems, message.detail);

					if (result >= 0) {

						log_info(logger, "A %d se le da el recurso %c", i, message.detail);

						//--Si pudo restar la cantidad
						tempAux->blocked = false;
						//--Define confirmación y la devuelve
						message.detail = 1;
						if (send(i, &message, sizeof(message), 0) == -1)
							perror("Confirmacion");

					} else {
						//--Si el recurso ya no tiene instacias
						tempAux->blocked = true;

						log_info(logger, "A %d se le niega el recurso %c", i, message.detail);
						//--Define rechazo y lo devuelve
						message.detail = 0;
						if (send(i, &message, sizeof(message), 0) == -1)
							perror("Confirmacion");
					}
				}else{
					log_error(logger, "Posicion erronea al pedir recurso");
					exit(EXIT_FAILURE);
				}

				break;
			case SALIR:
				for (contPj = 0; contPj < list_size(personajes); contPj++) {
					tempAux = list_get(personajes, contPj);

					if (tempAux->numSock == i) {
						//--Elimina y libera la lista de recursos de personaje

						for (contRec = 0; contRec < list_size(tempAux->recursos); contRec++) {
							auxRec = list_get(tempAux->recursos, contRec);
							//--Suma la cantidad de recursos que el personaje libera
							sumarQuantityRec(ListaItems, *auxRec, 1);
						}

						list_destroy_and_destroy_elements(tempAux->recursos, free);

						//--Elimina y libera al personaje de la lista de personajes
						list_remove_and_destroy_element(personajes, contPj, free);
						BorrarPer(&ListaItems, i);
						break;
					}
				}

				log_debug(logger, "%d se desconectó", i);
				break;
			}
			nivel_gui_dibujar(ListaItems, nom_nivel);

			pthread_mutex_unlock(&semNiv);
		}

	}

	nivel_gui_terminar();
	return 0;
}

_Bool tieneLoQueNecesito(personaje_t* a, personaje_t* b) {//--Electrolitos
	char* blkB = list_get(b->recursos, list_size(b->recursos) - 1); //--Guardar recurso por el que se bloquée B
	log_trace(logger, "Se esta buscando %c", *blkB);

	int contRec;
	char* levantadorA;
	for (contRec = 0; contRec < list_size(a->recursos) - (a->blocked ? 1 : 0); contRec++) {
		levantadorA = list_get(a->recursos, contRec);
		log_trace(logger, "\t\t%c tiene: %c", a->chirimbolo, *levantadorA);
		if (*blkB == *levantadorA)
			return true;
	}
	return false;
}

void detectInterbloqueos(void* argumentos) {

	t_list* bloqueados;
	paramInterbloqueo_t parametros;
	memcpy(&parametros, argumentos, sizeof(paramInterbloqueo_t));

	int cantBloq, marcados;

	// Iteramos infinitamente
	while (1) {
		pthread_mutex_lock(&semNiv);
		cantBloq = 0;
		marcados = 0;
		bloqueados = list_create();
		//log_trace(logger, "Initializing locking sysem activatinasdqu..,");
		int contPer1, contPer2;
		personaje_t* levantador1, *levantador2;

		//--Recorrer los personajes
		for (contPer1 = 0; contPer1 < list_size(personajes); contPer1++) {
			levantador1 = list_get(personajes, contPer1);

			//--marca a los que no estan bloqueados
			if (levantador1->blocked) {
				levantador1->marcado = false;
				cantBloq++;
			} else {
				levantador1->marcado = true;
				marcados++;
			}
		}

		for (; cantBloq >= 0; cantBloq--) { //(En el peor de los casos, tiene que asignar 2n-1 veces)
			//Por cada pj no marcado
			for (contPer1 = 0; contPer1 < list_size(personajes); contPer1++) {
				levantador1 = list_get(personajes, contPer1);
				if (!levantador1->marcado) {
					//Si necesita un recurso de uno marcado
					for (contPer2 = 0; contPer2 < list_size(personajes); contPer2++) {
						levantador2 = list_get(personajes, contPer2);

						if (levantador2->marcado && tieneLoQueNecesito(levantador2, levantador1)) {
							log_trace(logger, "Marque a %c", levantador1->chirimbolo);
							levantador1->marcado = true;
							marcados++;
							break;
						}
					}
				}
			}

		};

		//Estan en DeadLock los que no esten marcados
		for (contPer1 = 0; contPer1 < list_size(personajes); contPer1++) {
			levantador1 = list_get(personajes, contPer1);
			if (!levantador1->marcado) {
				list_add_new(bloqueados, levantador1, sizeof(personaje_t));
				log_trace(logger, "%c esta en Deadlock", levantador1->chirimbolo);
			}
		}

		//--Si la lista tiene más de 1 deadlockeados, se la mandamos al orquestador
		if ((list_size(bloqueados) > 1) && (parametros.recovery)) {
			//--Envía un header con la cantidad de personajes
			orq_t header;
			header.type = NIVEL;
			header.detail = HAYINTERBLOQUEO;
			header.port = list_size(bloqueados);

			if (send(parametros.numSock, &header, sizeof(header), 0) == -1) {
				log_error(logger, "Send: %s", strerror(errno));
				exit(EXIT_FAILURE);
			}
			log_trace(logger, "Header: cantidad: %d", list_size(bloqueados));

			//--Envía los personajes
			message_t message;
			personaje_t* levantadorBlk;
			while (list_size(bloqueados) != 0) {
				levantadorBlk = list_remove(bloqueados, 0);
				message.name = levantadorBlk->chirimbolo;

				if (send(parametros.numSock, &message, sizeof(message), 0) == -1) {
					log_error(logger, "Send: %s", strerror(errno));
					exit(EXIT_FAILURE);
				}

				log_trace(logger, "\tPersonaje: %c", message.name);
			}
			list_destroy_and_destroy_elements(bloqueados, free);
		}

		// Mandamos el proceso a dormir para que espere el tiempo definido por archivo de config.
		pthread_mutex_unlock(&semNiv);
		sleep(parametros.tiempoChequeo);
		log_trace(logger, ">>>Revisando DL<<<");
	}
}

// Envia mensaje de muerte a todos los personajes de su lista.
void notificarPersonajes() {
	int i = 0;
	message_t message;
	message.type = SIGNALKILL;
	personaje_t *personaje;
	while (i < list_size(personajes)) {	//Cicla los personajes
		// Conseguimos el personaje
		personaje = (personaje_t *) list_get(personajes, i);
		//Enviamos mensaje de muerte al personaje
		if (send(personaje->numSock, &message, sizeof(message), 0) == -1) {
			log_error(logger, "Send: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		i++;
	}
}

void imprimirLista(t_log* logger, t_list* personajes) {

	int contPj, contRec;

	for (contPj = 0; contPj < list_size(personajes); contPj++) {
		personaje_t* auxPj;
		auxPj = list_get(personajes, contPj);

		log_warning(logger, "PJ: %d", auxPj->numSock);

		for (contRec = 0; contRec < list_size(auxPj->recursos); contRec++) {
			char* auxRec;
			auxRec = list_get(auxPj->recursos, contRec);

			log_warning(logger, "\tRec: %c", *auxRec);
		}
	}
}

void cerrarForzado(int sig) {
	cerrarNivel("Cerrado Forzoso Nivel.");
}

void cerrarNivel(char * messageLog) {
	log_trace(logger, messageLog);
	notificarPersonajes();
	nivel_gui_terminar();
	printf("%s", messageLog);
	exit(EXIT_FAILURE);
}
