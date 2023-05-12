#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

/************************************************************************************
* 
* Nombre: Global_Vars.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO2 2023
*
* Descripción: Header que contiene variables y estructuras globales que usan la mayoria
* de programas de la practica.
*
************************************************************************************/

#define SEARCHERS_NUM   4       //Numero de buscadores a crear.
#define USERS_NUM       50      //Numero de usuarios a crear. Sus tipos (free, premium) seran aleatorios.

#define FILES_PATH      "./resources/Libros/"          //Ruta donde estarán los ficheros txt donde los buscadores buscarán palabras.
#define RESULTS_PATH    "./resources/user_results/"    //Ruta donde se almacenaran los resultados de cada usuario.

#define MIN_FILES 1         //Numero minimo de archivos que puede buscar un usuario.
#define MAX_FILES 10         //Numero maximo de archivos que puede buscar un usuario.

#define USER_BALANCE    10   //Saldo por defecto que tendra un usuario
#define RELOAD_BALANCE  5   //Cantidad de recargo de saldo que aplicara el siste,a de pago

enum USER_TYPE { FREE, PREMIUMLIMITED, PREMIUM };   //Tipos de usuario posibles

/*Descripcion: Dado un tipo de usuario (ya que un enum es un entero) devuelve su respectivo string*/
std::string UserTypeToString(USER_TYPE type){
    switch (type)
    {
    case USER_TYPE::FREE:
        return "Gratuito";
        break;
    
    case USER_TYPE::PREMIUMLIMITED:
        return "Premium limitado";
        break;

    case USER_TYPE::PREMIUM:
        return "Premium";
        break;

    default:
        std::cout << "Usuario no permitido\n";
        return "NULL";
        break;
    }
}

/*Descripcion: Vector que contiene colores. Lo usan los buscadores para imprimir informacion por pantalla*/
std::vector<std::string> COLORS = {
    "\033[31m",    // Rojo
    "\033[35m",    // Magenta
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

/*Descripcion: Vector que contiene el diccionario de palabras. Lo usan los usuarios para elegir una
palabra aleatoria a buscar*/
std::vector<std::string> DICTIONARY = {
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
