. Recibe conexión de Personaje (caracter) y pedido de posición inicial.

. Envía posición inicial del personaje (spawn point).

. Recibe pedido posición de 1er recurso.

. Envía posición de recurso.

. Recibe pedido de movimiento.

. Envía confirmación ^ mueve personaje (modifica posición[sincronizado con el personaje]).
  - No tiene sentido enviar un mensaje de "No se pudo mover" porque si de alguna forma mágica llegó a los límites del mapa, no podría corregir su rumbo.

. Recibo pedido de recurso:

  - if disponible: confirma.

  - else le dice que no hay.

. Recibe salida del personaje y sus recursos para liberarlos.
