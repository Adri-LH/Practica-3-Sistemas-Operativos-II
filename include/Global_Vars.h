#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#define SEARCHERS_NUM 5     //Numero de buscadores.
#define USERS_NUM 6         //Numero de usuarios. Sus perfiles serán aleatorios.

#define FILES_PATH      "../resources/Libros/"          //Ruta donde estarán los ficheros txt donde los buscadores buscarán palabras.
#define RESULTS_PATH    "../resources/user_results/"    //Ruta donde se almacenarám los resultados de cada usuario.

#define MIN_FILES 1         //Numero minimo de archivos que puede buscar un usuario.
#define MAX_FILES 3         //Numero maximo de archivos que puede buscar un usuario.



std::vector<std::string> colors = {
    "\033[31m",    // Rojo
    "\033[35m",    // Magenta
    "\033[93m",    // Amarillo claro
    "\033[94m",    // Azul claro
    "\033[33m",    // Amarillo
    "\033[32m",    // Verde
    "\033[34m",    // Azul
    "\033[36m",    // Cyan
    "\033[90m",    // Gris
    "\033[92m",    // Verde claro
    "\033[95m",    // Magenta claro
    "\033[96m",    // Cyan claro
};

std::vector<std::string> dictionary = {
    "Capítulo",
    "Árbol",
    "Casa",
    "David",
    "Sol",
    "Nube",
    "Ley",
    "Grupo",
    "Mal",
    "forma",
    "talento",
    "Luna",
    "Americano",
    "poder",
};

#endif
