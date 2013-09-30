Proceso Koopa
=============

Este proceso tiene las siguientes dependencias:
- libmemoria.so (Librería de manejo de memoria)
- libso-commons-library.so

Para ejecutarlo, setear antes la variable de entorno LD_LIBRARY_PATH desde bash. El valor de la variable debe ser una cadena con las rutas donde estén ubicadas las librerías separadas por :
-> Ejemplo:
export LD_LIBRARY_PATH=/home/utnso/workspace/memoria/Debug/:/home/utnso/workspace/so-commons-library/Debug

Parámetros
----------
./koopa [archivo.txt]
Modo normal. Ejecutará la lista de pedidos que contenga el archivo y notificará si el algoritmo funciona correctamente.

./koopa -debug
Modo de pruebas, para verificar rápidamente el funcionamiento de la librería. Se escriben por la entrada operaciones como GRABAR:#:4:abcd y se van mostrando por pantalla los resultados a medida que se ejecutan.

Librería
----------
En /memoria hay un ejemplo de implementación de la librería de memoria. Al importar ese proyecto al eclipse debería poderse compilar y empezar a trabajar sobre el código.
La única precondición es tener importado también el proyecto so-commons-library.
