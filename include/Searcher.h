#ifndef SEARCHER_H
#define SEARCHER_H

#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <fstream>
#include <thread>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <shared_mutex>
#include "User.h"

#include "../include/Result.h"
#include "../include/myfiles.h"
#include "../include/Request.h"

#define SUBTHREADS 2//Numero de subhilos que buscan en un mismo documento

void searchWords(std::string file, std::string word, int linea_inicial, int linea_final, int id_searcher, int id_thread);
void prepareSubThreads(std::vector<std::string> files, std::string word, int num_threads, int id_searcher);

std::vector<Result> results;
std::vector<std::thread> subhilos;
std::shared_mutex mutex_results;

class Searcher{
    public:
        Searcher(int _id_searcher){
            id_searcher = _id_searcher;
        }

        void searcherWorking(std::mutex& sem_request_queue, std::condition_variable& cond_var_request_queue, std::queue<std::shared_ptr<Request>>& request_queue){

            std::unique_lock<std::mutex> lock(sem_request_queue); //Seccion critica
            cond_var_request_queue.wait(lock, [&request_queue] {return !(request_queue.empty());});

            std::shared_ptr<Request>& request = request_queue.front();
            prepareSubThreads(request->GetFiles(), request->getWord(), SUBTHREADS, id_searcher);
        
        }
        
        int id_searcher;
        
};

void prepareSubThreads(std::vector<std::string> files, std::string word, int num_threads, int id_searcher){
        for(int i = 0; i < files.size(); i++){
        
            //Es posible que haya problemas si los archivos no existen
            int total_lines = countLines(files[i]);
            int thread_work = total_lines / num_threads;                    //Trabajo de cada hilo
            int last_thread = total_lines - (thread_work * num_threads);    //Trabajo extra del ultimo hilo
            int inicio = 0;                 //Linea inicial de un hilo
            int final = thread_work - 1;    //Linea final de un hilo

            //Creamos los hilos, repartiendo las tareas
            for (int j = 0; j < num_threads; j++)
            {
                //Si es el ultimo hilo, le asignamos la carga extra
                if (j == num_threads - 1)
                    final = final + last_thread;
                
                subhilos.push_back(std::thread(searchWords, files[i], word, inicio, final, id_searcher, j));

                //Actualizamos tareas para el siguiente hilo
                inicio = inicio + thread_work;
                final = final + (thread_work);

            }

        }

        //Espera de subhilos
            std::for_each(subhilos.begin(), subhilos.end(), std::mem_fn(&std::thread::join));

        //Finalmente imprimimos el vector
            for (int i = 0; i < results.size(); i++) {
                results[i].PrintResults();
            }

}

    /*********************************************************************************
     * 
     * Nombre de la función: searchWords
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
    void searchWords (std::string file, std::string word, int linea_inicial, int linea_final, int id_searcher, int id_thread) {
            Result result(id_searcher, id_thread, linea_inicial, linea_final,word);
        
            //Convertir palabra a minuscula
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
                
            //Comprobamos si el fichero se abre
            std::fstream archivo = openFileRead(file);
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
                    while ((pos = line_lower.find(word, pos)) != std::string::npos) {
                        
                        std::string previous = getRelativeWord(linea, pos, false);  //Palabra anterior
                        std::string last = getRelativeWord(linea, pos, true);       //Palabra posterior

                        if (previous.empty() || previous==" ") {
                            previous="no_word";
                        }else if(last.empty()|| last==" "){
                            last="no_word";
                        }
                        
                        pos += word.length();

                        std::unique_lock<std::shared_mutex> lock(mutex_results);
                        result.add_Result(file, num_linea + 1, previous, last);
                        
                        lock.unlock();
                        
                    }
                    
                }

            
                num_linea++;
            
            }

            //Reciclamos semaforo
            std::unique_lock<std::shared_mutex> lock(mutex_results);
            results.push_back(result);
            lock.unlock();

            // Cerrar el archivo
            archivo.close();

        }
    

#endif



