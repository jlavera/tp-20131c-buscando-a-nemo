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

#define DIRECCION "127.0.0.1"
#define PUERTO 5000

typedef struct {
	int8_t type;
	int8_t detail;
	int8_t detail2;
	char name;
} message_t;

typedef struct { // Desde el punto de vista del orquestador
	int8_t type; 	// Nivel					-	Personaje
	int8_t detail;	// Saludo/Salir
	int8_t port; // Recibe puerto del nivel	-	Manda puerto del nivel
	char ip[16]; 	// Recibe Ip				-	Manda Ip
	char name[16]; // Nombre de nivel			-	Nombre de nivel pedido
} orq_t;

int main(void) {


	int unSocket;
	int nBytes;

	orq_t orq;

	struct sockaddr_in socketInfo;

	printf("Conectando...\n");

	// Crear un socket:
	if ((unSocket = socket(AF_INET, SOCK_STREAM, 0)) != 0) {

		socketInfo.sin_family = AF_INET;
		socketInfo.sin_addr.s_addr = inet_addr(DIRECCION);
		socketInfo.sin_port = htons(PUERTO);

		// Conectar el socket con la direccion 'socketInfo'.
		if (connect(unSocket, (struct sockaddr*) &socketInfo,
				sizeof(socketInfo)) == 0) {

			printf("Conectado!\n");

			char aux;
			char buffer[10];

			while (1) {
				printf("\nSender:");
				scanf("%c", &aux);

				//--Si el que envía es un nivel
				switch (aux) {
				case 'N':

					orq.type = 1;
					orq.detail = 1;
					strcpy(orq.ip, "255.255.255.255");
					orq.port = 34;

					getchar(); //--Sin esto, hace boludeces
					printf("\nMensaje: ");
					scanf("%s",  (char*)&(orq.name));

					getchar(); //--Sin esto, hace boludeces
					printf("Codigo: ");
					scanf("%c", &aux);

					switch (aux) {
					case 'A':
						orq.detail = 1;
						break;
					case 'B':
						orq.detail = 9;
						break;
					case 'D':
						orq.detail = 3;
						break;
					}

					if (orq.detail == 3)
						printf("Modo Debug\n\n");

					if (orq.detail == 9)
						printf("Eliminar\n\n");

					//--Envía el "Saludo"
					if (send(unSocket, &orq, sizeof(orq_t), 0) == -1) {
						perror("Saludo");
					}

					break;
				case 'P':
					//--Si el que envía es un personaje

					orq.type = 2;
					orq.detail = 1;

					getchar(); //--Sin esto, hace boludeces
					printf("\nMensaje: ");
					scanf("%s",  (char*)&(orq.name));

					//--Envía el "Saludo"
					if (send(unSocket, &orq, sizeof(orq_t), 0) == -1) {
						perror("Saludo");
					}

					if ((nBytes = recv(unSocket, &orq, sizeof(orq_t), 0))
							<= 0) {
						perror("Sock");
						exit(1);
					}

					close(unSocket);
					printf("\nRecibido:\n\tIp: %s\n\tPuerto: %d", orq.ip,
							orq.port);
					break;
				case 'C':

					getchar(); //--Sin esto, hace boludeces
					printf("\nMensaje: ");
					scanf("%s", (char*)&buffer);


					//--Envía el "Saludo"
					if (send(unSocket, &buffer, 10, 0) == -1) {
						perror("Saludo");
					}
					break;
				}
			}
		}
	}
	return 0;
}
