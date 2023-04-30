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
#include <condition_variable>
#include <queue>
#include <unistd.h>


#include "../include/Result.h"
#include "../include/myfiles.h"
#include "../include/User.h"
#include "../include/Pay_Sys.h"


#define WORD_FINDER 4;  //Numero de buscadores
#define USERS_NUM 50;   //Numero de usuarios. Sus perfiles serán aleatorios.

void createPaySys();

void buscarPalabra(std::string ruta, std::string palabra, int linea_inicial, int linea_final, int id_thread);

//Memorias compartidas (no todas lo tendran que ser)
std::vector<std::string> dictionary;
std::vector<Result> results;
std::vector<std::thread> vhilos;
std::shared_mutex buffer_mutex;

//Para el sistema de pago
std::mutex sem_userpl_queue;
std::queue<std::shared_ptr<UserPremiumLimited>> userpl_queue;
std::condition_variable cond_var_userpl_queue;

std::shared_ptr<UserPremiumLimited> user = std::make_shared<UserPremiumLimited>(1, 100);
int main(int argc, char *argv[])
{
    //Control de argumentos
    if (argc != 1)
    {
        std::cerr << "Use: /program\n";
        return EXIT_FAILURE;
    }

    //Creamos el diccionario
    dictionary = getFileWords("../resources/diccionario.txt"); 

    //Creamos hilo del Sistema de Pago
    std::thread T_Pay_Sys(createPaySys);
    T_Pay_Sys.join();
    user->mostrarSaldo();
    user->Saludar();

    
    
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
    Result result(id_thread, linea_inicial, linea_final, palabra);

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

                std::unique_lock<std::shared_mutex> lock(buffer_mutex);
                result.add_Result(num_linea + 1, previous, last);

                lock.unlock();
                
            }
            
        }
        
        num_linea++;
        
    }

    //Reciclamos semaforo
    std::unique_lock<std::shared_mutex> lock(buffer_mutex);
    results.push_back(result);
    lock.unlock();
    
    // Cerrar el archivo
    archivo.close();

}

void createPaySys(){
    Pay_Sys Paysystem;
    userpl_queue.push(user);
    Paysystem.esperar(std::ref(sem_userpl_queue), std::ref(cond_var_userpl_queue), userpl_queue);
    
}
