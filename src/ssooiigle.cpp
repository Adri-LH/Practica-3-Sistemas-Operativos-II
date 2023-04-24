/************************************************************************************
*
* Nombre: main.cpp
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: 13/04/2023

* Descripción: Utilizada para la creacion de hilos y su tarea

************************************************************************************/

#include <iostream>
#include <thread>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <shared_mutex>
#include <regex>

#include "../include/word.h"
#include "../include/myfiles.h"

void buscarPalabra(std::string ruta, std::string palabra, int linea_inicial, int linea_final, int id_thread);

std::vector<Word> words_found;
std::vector<std::thread> vhilos;
std::shared_mutex buffer_mutex;



int main(int argc, char *argv[])
{
    //Control de argumentos
    if (argc != 4)
    {
        std::cerr << "Use: /program file_name word thread_number\n";
        return EXIT_FAILURE;
    }

    //Asignamos parametros del programa a variables
    std::string file_path = argv[1];
    std::string word = argv[2];
    int num_threads = std::stoi(argv[3]);

    //Lineas totales del fichero
    int total_lines = countLines(file_path);

    int thread_work = total_lines / num_threads;                    //Trabajo de cada hilo
    int last_thread = total_lines - (thread_work * num_threads);    //Trabajo extra del ultimo hilo
    
    int inicio = 0;                 //Linea inicial de un hilo
    int final = thread_work - 1;    //Linea final de un hilo

    //Creamos los hilos, repartiendo las tareas
    for (int i = 0; i < num_threads; i++)
    {
        //Si es el ultimo hilo, le asignamos la carga extra
        if (i == num_threads - 1)
            final = final + last_thread;

        vhilos.push_back(std::thread(buscarPalabra, file_path, word, inicio,final, i));

        //Actualizamos tareas para el siguiente hilo
        inicio = inicio + thread_work;
        final = final + (thread_work);

    }

    //Espera de hilos
    std::for_each(vhilos.begin(), vhilos.end(), std::mem_fn(&std::thread::join));

    //Ordenamos vector de palabras encontradas
    std::sort(words_found.begin(), words_found.end());

    //Finalmente imprimimos el vector
    for (auto word : words_found) {
         std::cout << word;
     }

}

/*********************************************************************************
 * 
 * Nombre de la función: buscarPalabra
 *
 *
 * Descripción de la función: Este metodo buscara la palabra facilitada por el usuario y 
 * facilitando la palabra anterior y posterior
 *
 * Argumentos utilizados: 'ruta' ruta del archivo en el que buscaremos, 'palabra' sera la 
 * palabra a buscar 'linea_inicial' sera la linea por la que empezara a buscar cada hilo 
 * 'linea_final' sera la linea en la que termine la busqueda de palabras 'id_thread' 
 * identificador del hilo
 * 
 * Valor de regreso: void (no retorna nada )
 *
*********************************************************************************/
void buscarPalabra(std::string ruta, std::string palabra, int linea_inicial, int linea_final, int id_thread) {

    //Convertir palabra a minuscula
    std::transform(palabra.begin(), palabra.end(), palabra.begin(), [](unsigned char c){ return std::tolower(c); });
    
    //Comprobamos si el fichero se abre
    std::fstream archivo = openFileRead(ruta);
    if (!archivo)
        exit(1);

    std::string linea;
    int num_linea = 0;

    while (std::getline(archivo, linea)) {
        if (num_linea >= linea_inicial && num_linea <= linea_final) {

            // Convertir la línea a minúsculas
            std::string line_lower = linea;
            std::transform(line_lower.begin(), line_lower.end(), line_lower.begin(), [](unsigned char c){ return std::tolower(c); });

            // Buscar la palabra en la línea
            int pos = 0;
            while ((pos = line_lower.find(palabra, pos)) != std::string::npos) {
                
               

               

                
                std::string previous = getRelativeWord(linea, pos, false);  //Palabra anterior
                std::string last = getRelativeWord(linea, pos, true);       //Palabra posterior

                if (previous.empty() || previous==" ") {
                   previous="no_word";
                } else if(last.empty()|| last==" "){
                    last="no_word";
                }
                
                pos += palabra.length();

                Word word(id_thread,linea_inicial,linea_final, num_linea + 1, previous, last, pos); //Borrar el + 1 en caso de que sea necesario (linea 0 o linea 1)
                
                //Actualizamos vector con la ayuda del mutex
                std::unique_lock<std::shared_mutex> lock(buffer_mutex);
                words_found.emplace_back(word);
                lock.unlock();
                
            }
            
        }
        
        num_linea++;
        
    }
    
    // Cerrar el archivo
    archivo.close();

}


    
   


