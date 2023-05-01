/************************************************************************************
*
* Nombre: myfiles.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: 13/04/2023

* Descripción: Tratamiento de ficheros

************************************************************************************/

#ifndef MYFILES_H
#define MYFILES_H

#include <thread>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*********************************************************************************
 * 
 * Nombre de la función: openFileRead
 *
 *
 * Descripción de la función: Este metodo sera utilizado para abrir el archivo y en caso
 * de error mostrarnos un mensaje
 * 
 * Argumentos utilizados: 'file_path' que sera la ruta del archivo
 * 
 * Valor de regreso: fstream retorna el descriptor del archivo
 *
*********************************************************************************/
std::fstream openFileRead(std::string file_path) {
    std::fstream file;
    file.open(file_path, std::ios::in);
    
    if (!file) {
        std::cout << "Error opening file" << std::endl;
        file.close();
    }

    return file;
}

/*********************************************************************************
 * 
 * Nombre de la función: countLines
 *
 *
 * Descripción de la función: Este metodo sera utilizado para contar las lineas de
 * un archivo
 * 
 * Argumentos utilizados: 'path' que sera la ruta del archivo
 * 
 * Valor de regreso: int retorna el numero de lineas
 *
*********************************************************************************/
int countLines(std::string path) {
    std::fstream file;
    file.open(path, std::ios::in);
    
    if (!file) {
        std::cout << "Error opening file" << std::endl;
        file.close();
    }

    int count = 0;
    std::string line;
    while (std::getline(file, line)) {
        count++;
    }
    return count;
}

/*********************************************************************************
 * 
 * Nombre de la función: getRelativeWord
 *
 *
 * Descripción de la función: Devuelve la palabra adyacente la indicada en la posicion.
 * La posicion de la palabra debe ser la primera letra que la compone
 * 
 * Argumentos utilizados: 'line' linea en la que se encuentra la palabra, 'pos' posicion 
 * de la primera letra de la palabra, 'previous' booleano para saber si devolver la 
 * antecesora o la predecesora
 * 
 * Valor de regreso: string retorna la palabra encontrada
 *
*********************************************************************************/
std::string getRelativeWord(std::string line, int pos, bool previous)
{
    bool end = false;
    bool first_white = false;
    bool last_white = false;

    int long_word = 0;

    if (pos < 0 || pos > line.size() || line.size() == 0)
    {
        std::cout << "Error en getRelativeWord, pos se sale de los limites o la línea está vacia\n";
        exit(1);
    }


    while (!end)
    {
        char charac = line[pos];
        //Acaba la palabra en la que estabamos
        if (charac == ' ') first_white = true;

        
        if (first_white)
            //Encuentra el inicio de la siguiente palabra
            if (charac != ' ' && std::isalpha(charac)){
                last_white = true;
                long_word++; //Empieza a contar el tamaño de la palabra
            } 

        
        if (last_white)
        {
            //Acaba la nueva palabra
            if (charac == ' ') end = true;
        }

        //Control de errores
        if (!previous && (pos-1) > 0)
            pos--;
        else if (previous && (pos+1) < line.size())
            pos++;
        else
            end = true;
        
    }

    if (!previous)
            return line.substr(pos + 2, long_word);
        else
            return line.substr(pos - long_word - 1, long_word);
    
}


/*********************************************************************************
 * 
 * Nombre de la función: getLineByPosition
 *
 *
 * Descripción de la función: Devuelve el contenido de la linea indicada de un fichero
 * 
 * Argumentos utilizados: 'path' ruta del archivo, 'position' entero indicando la posicion
 * 
 * Valor de regreso: string retorna la linea a la que pertence la posion
 *
*********************************************************************************/
std::string getLineByPosition(std::string path, int position)
{
    //Comprobamos si el fichero se abre
    std::fstream inputFile = openFileRead(path);
    if (!inputFile)
        exit(1);

    std::string line;
    int currentPos = 1;
    while (std::getline(inputFile, line))
    {
        if (currentPos == position) // si se encuentra la posición buscada
        {
            return line; // devuelve la línea actual
        }

        currentPos++;
    }
    std::cout << "Line does not exist";
    return ""; // si no se encuentra la posición, devuelve una cadena vacía
}

/*********************************************************************************
 * 
 * Nombre de la función: getFileWords
 *
 *
 * Descripción de la función: Devuelve un vector con las palabras de un archivo
 * 
 * Argumentos utilizados: 'path' ruta del archivo, 'position' entero indicando la posicion
 * 
 * Valor de regreso: string retorna la linea a la que pertence la posion
 *
*********************************************************************************/
std::vector<std::string> getFileWords(std::string path) {
    std::vector<std::string> palabras;
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: el archivo no existe\n";
        return palabras;
    }
    std::ifstream archivo(path);
    
    if (archivo.is_open()) {
        std::string palabra;
        while (archivo >> palabra) {
            palabras.push_back(palabra);
        }
        archivo.close();
    }

    return palabras;
}

/*Descripcion: Introduce texto (text) a un archivo (path). Si no existe, se crea*/
void writeFile(const std::string path, const std::string text)
{
    FILE *p_src_file = fopen(path.c_str(), "a");
    if (p_src_file == NULL)
    {
        printf("Error opening source file: %s\n", path.c_str());
    }

    fprintf(p_src_file, "%s\n", text.c_str());
    fclose(p_src_file);
}

#endif
