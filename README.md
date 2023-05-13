# Practica-3-Sistemas-Operativos-II

## Solución del problema
Realizado por Adrián Carrasco Espinosa y Javier De la Concepción Dorado.

## Informacin general

- La autoevaluación se encuentra en el fichero pdf con el enunciado de la práctica.
- Para la solución no se han usado procesos, solo hilos. 
- El sistema de pago es un único hilo.
- Para modificar el número de buscadores y de usuarios, se debe modificar las constantes SEARCHERS_NUM y USERS_NUM del fichero /include/Global_Vars.h.
- Los buscadores crean un subhilo por cada archivo en el que deben buscar la palabra deseada.
- El programa crea las peticiones indicadas. A los 3 segundos, vuelve a enviar el mismo numero de peticiones.
- El programa no termina al procesar las peticiones. Deberá pararse con Cntrl + C.

## Archivo Make

- Usa la versión del compilador 20 de g++.
- Para crear ejecutables: sudo make all.
- Para ejecutar: sudo make solucion (necesario ejecutar desde el directorio raiz del archivo).
- Para limpiar archivos: sudo make clean.

## Funcionamiento del programa

El funcionamiento es el siguiente: Un usuario envía una peticion a la cola de peticiones, restringida mediante un semaforo.
Cuando lo hace, hace un notify a la variable de condicion que comparte con los buscadores. El usuario se bloquea con un semaforo privado. El buscador asignado procesa su peticion. Cuando termina, desbloquea el semaforo privado del cliente. Finalmente este crea un txt con los resultados en el directorio /resources/user_results.


Si se trata de un usuario premium limitado, cuando su saldo se acaba, ocurre algo similar, pero entre el buscador y el sistema de pago. El buscador envía la solicitud a la cola de peticiones del sistema de pago mediante un semáforo. Cuando lo hace, hace un notify a la variable de condición que comparte con el sistema, activandolo. El buscador se bloquea mediante un semaforo privado. El sistema de pago procesa la solicitud aumentando el saldo del cliente y desbloquea el semaforo privado del buscador, que continua con su trabajo.

## Recursos

El diccionario es un vector de strings que se encuentra en /include/Gloval_Vars.h

## Sistema de prioridad de usuarios 

Para hacer la relación 20% no premium y 80% premium, en la creación del usuario se crea un número entre 0 y 9.
Para un usuario no premium: 0-1 -> Se añade al principio de la cola | 2-9 -> Se añade al final de la cola.
Para un usuario premium: 0-1 -> Se añade al final de la cola | 2-9 -> Se añade al principio de la cola.

## Problemas

- En principio, las palabras se encuentran de forma correcta. Sin embargo, la palabra anterior y la posterior no, pero nos hemos querido centrar en los objetivos de esta práctica.
- Es posible que en los resultados de un usuarios haya fallos en algunos caracteres, como acentos o la letra 'ñ'.

