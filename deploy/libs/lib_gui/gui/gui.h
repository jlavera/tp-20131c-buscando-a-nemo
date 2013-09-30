#define PERSONAJE_ITEM_TYPE 0
#define RECURSO_ITEM_TYPE 1

struct item {
	int numSock;
	char id;
	int posx;
	int posy;
	char item_type; // PERSONAJE o CAJA_DE_RECURS
	int quantity;
	struct item *next;
};

typedef struct item ITEM_NIVEL;

int nivel_gui_dibujar(ITEM_NIVEL* items, char* nom_nivel);
int nivel_gui_terminar(void);
int nivel_gui_inicializar(void);
int nivel_gui_get_area_nivel(int * rows, int * cols);
