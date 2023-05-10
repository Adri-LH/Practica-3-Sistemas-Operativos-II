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
#include <atomic>
#include "User.h"

#include "../include/Result.h"
#include "../include/myfiles.h"
#include "../include/Request.h"

void searchWords(std::string file, std::string word, int id_searcher, int id_thread);
void prepareSubThreads(std::vector<std::string> files, std::string word, int id_searcher);

std::shared_mutex mutex_results;

class Searcher{
    public:
        int id_searcher;
        std::string color;
        std::shared_ptr<Result> current_result; //Resultado equivalente al del usuario
        std::shared_ptr<Request> current_request;

        Searcher(int _id_searcher, std::string _color){
            id_searcher = _id_searcher;
            color = _color;
        }

        void searcherWorking(std::mutex& sem_request_queue, std::condition_variable& cond_var_request_queue, std::queue<std::shared_ptr<Request>>& request_queue){
 
            while (true){
            
            //Espera entrada seccion critica con semáforo y variable de concición
            std::unique_lock<std::mutex> lock(sem_request_queue);
            cond_var_request_queue.wait(lock, [&request_queue] {return !(request_queue.empty());});
            
            //Seccion critica

            //Obtenemos por referencia la peticion del usuario
            current_request = std::move(request_queue.front());

            //Imprimimos información
            std::cout << color << "Buscador " << id_searcher << ": procesará la petición del Usuario " << current_request->getUserId() << std::endl;

            //Obtenemos por referencia el resultado del usuario
            current_result = current_request->getResult();
            request_queue.pop();
            lock.unlock();

            //Se preparan los hilos para la búsqueda de palabra
            prepareSubThreads(current_request->GetFiles(), current_request->getWord(), id_searcher);
            
            //Desbloqueamos el semaforo del usuario. Su peticion ha sido atendida
            current_request-> getSemUser() -> unlock();

            //Imprimimos información
            std::cout << color << "Buscador " << id_searcher << ": ha finalizado la petición del Usuario " << current_request->getUserId()
            << ", se desbloqueará su semáforo" << std::endl;

            }
            
        }

        void prepareSubThreads(std::vector<std::string> files, std::string word, int id_searcher){
            std::vector<std::thread> subthreads;
            for(int i = 0; i < files.size(); i++){
                //Imprimimos información
                std::cout << color << "Buscador " << id_searcher << " buscará la palabra :" << word << ": en el archivo :" << files[i]
                << ": para el Usuario " << current_request->getUserId() << std::endl;
                
                //Se crean los hilos
                subthreads.push_back(std::thread([this, &files, &word, id_searcher, i]() {searchWords(files[i], word, id_searcher, i);})); //Debido a funcion de miembro no estatica
            }

            std::for_each(subthreads.begin(), subthreads.end(), std::mem_fn(&std::thread::join));

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
    void searchWords(std::string file, std::string word, int id_searcher, int id_thread) {

            Searcher_Result_Info searcher_result (id_searcher, id_thread, file);

            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
                
            //Comprobamos si el fichero se abre
            std::fstream archivo = openFileRead(file);
            if (!archivo)
                exit(1);

            std::string linea;
            int num_linea = 0;

            while (std::getline(archivo, linea)) {

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

                        //Si encuentra palabra:
                        std::unique_lock<std::shared_mutex> lock(mutex_results);
                        Word_Found_Info word_found(num_linea+1, previous, last);
                        searcher_result.addWordFound(word_found);

                    }
            
                num_linea++;
            
            }

            std::unique_lock<std::shared_mutex> lock(mutex_results);
            current_result -> addSearcherResultInfo(searcher_result);

            archivo.close();
            
    }
        
};


    

#endif



