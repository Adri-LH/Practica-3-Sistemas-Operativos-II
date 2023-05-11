#ifndef SEARCHER_H
#define SEARCHER_H

#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <fstream>
#include <thread>
#include <sstream>
#include <tuple>
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
#include "../include/Global_Vars.h"

void searchWords(std::string file, std::string word, int id_searcher, int id_thread);
void prepareSubThreads(std::vector<std::string> files, std::string word, int id_searcher);

std::shared_mutex mutex_results;    //Semaforo para introducir reultados
std::atomic<int> user_counter(0);   //Variable que cuenta los usuarios totales

std::mutex mutex_balance;            //Semaforo para acceder al saldo
std::condition_variable cond_var_balance;   //Variable de condicion para saldo

class Searcher{
    public:
        int id_searcher;                            //ID del buscador
        std::string color;                          //Color de impresión del buscador
        std::shared_ptr<std::mutex> sem_system_pay_queue; //Semaforo para acceder a la cola del sistema de pago
        std::shared_ptr<std::condition_variable> cond_var_system_pay_queue;
        std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>>>> system_pay_queue;
        std::shared_ptr<std::mutex> searcher_sem_system_pay = std::make_shared<std::mutex>(); //Semaforo para bloquearse y esperar al sistema de pago

        std::shared_ptr<Result> current_result;     //Resultado equivalente al del usuario
        std::shared_ptr<Request> current_request;   //Peticion equivalente a la del usuario
        std::shared_ptr<int> current_user_balance;  //Referencia al saldo del actual usuario
        User_Type current_user_type;                //Tipo del actual usuario


        Searcher(int _id_searcher, std::string _color, std::shared_ptr<std::mutex> _sem_system_pay_queue,
        std::shared_ptr<std::condition_variable> _cond_var_system_pay_queue,
        std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>>>> _system_pay_queue){

            id_searcher = _id_searcher;
            color = _color;
            sem_system_pay_queue = _sem_system_pay_queue;
            cond_var_system_pay_queue = _cond_var_system_pay_queue;
            system_pay_queue = _system_pay_queue;
            searcher_sem_system_pay->lock(); //Se inicializa, no se bloquea
        }
 
        void searcherWorking(std::mutex& sem_request_queue, std::condition_variable& cond_var_request_queue, std::queue<std::shared_ptr<Request>>& request_queue){
 
            while (user_counter < USERS_NUM){
            //Espera entrada seccion critica con semáforo y variable de concición
            std::unique_lock<std::mutex> lock(sem_request_queue);
            cond_var_request_queue.wait(lock, [&request_queue] {return !(request_queue.empty());});
            
                //Seccion critica
                user_counter++;

                //Obtenemos por referencia la peticion del usuario
                current_request = std::move(request_queue.front());

                //Se saca la peticion de la cola
                request_queue.pop();

                //Imprimimos información
                std::cout << color << "[Buscador " << id_searcher << "] procesará la petición del Usuario " << current_request->getUserId() << std::endl;

                //Obtenemos por referencia el resultado del usuario e indicamos que buscador ha atendido la petición y la palabra que se ha buscado
                current_result = current_request->getResult();
                current_result-> setIdSearcher(id_searcher);
                current_result->setWordSearched(current_request->getWord());

                //Obtenemos el saldo y el tipo de usuario
                current_user_balance = current_request->getUserBalance();
                current_user_type = current_request->getUserType();

            //Desbloqueo de semaforo para que otros buscadores atiendan peticiones
            lock.unlock();

            //Se preparan los hilos para la búsqueda de palabra
            prepareSubThreads(current_request->GetFiles(), current_request->getWord(), id_searcher);
            
            //Desbloqueamos el semaforo del usuario. Su peticion ha sido atendida
            current_request-> getSemUser() -> unlock();

            //Imprimimos información
            std::cout << color << "[Buscador " << id_searcher << "] ha finalizado la petición del Usuario " << current_request->getUserId()
            << ", se desbloqueará su semáforo" << std::endl;

            }
            
        }

        void prepareSubThreads(std::vector<std::string> files, std::string word, int id_searcher){
            //Creeacion de vector de hilos
            std::vector<std::thread> subthreads;

            for(int i = 0; i < files.size(); i++){
                //Imprimimos información
                std::cout << color << "[Buscador " << id_searcher << "] buscará la palabra :" << word << ": en el archivo :" << files[i]
                << ": para el Usuario " << current_request->getUserId() << std::endl;
                
                //Se crean los hilos
                subthreads.push_back(std::thread([this, &files, &word, id_searcher, i]() {searchWords(files[i], word, id_searcher, i);})); //Debido a funcion de miembro no estatica
            }

            //Espera a todos los hilos
            std::for_each(subthreads.begin(), subthreads.end(), std::mem_fn(&std::thread::join));

            //Si el usuario se ha quedado sin saldo, lo mostramos
            if(*current_user_balance <= 0)
                std::cout << color << "[Buscador " << id_searcher << "] IMPORTANTE: el usuario " << current_request->getUserId() << " se ha quedado sin saldo." << std::endl;

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
            //Creamos resultado de busqueda de un archivo
            File_Result_Info searcher_result (id_thread, file);

            //Convertimos línea a minúscula
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
                
            //Comprobamos si el fichero se abre
            std::fstream archivo = openFileRead(file);
            if (!archivo)
                exit(1);

            std::string linea;
            int num_linea = 0;

            while (std::getline(archivo, linea)) {

                    //Si el usuario no tiene saldo, dejamos de buscar palabras en el archivo
                    // std::unique_lock<std::shared_mutex> lock(mutex_results);
                    //     if(*current_user_balance <= 0)
                    //         break;
                    // lock.unlock();

                    // Convertir la línea a minúsculas
                    std::string line_lower = linea;
                    std::transform(line_lower.begin(), line_lower.end(), line_lower.begin(), [](unsigned char c){ return std::tolower(c); });

                    // Buscar la palabra en la línea
                    int pos = 0;

                    while ((pos = line_lower.find(word, pos)) != std::string::npos) {
                        //Si el buscador encuentra una palabra:
                        pos += word.length();
                        std::string previous = getRelativeWord(linea, pos, false);  //Palabra anterior
                        std::string last = getRelativeWord(linea, pos, true);       //Palabra posterior

                        //Semaforo para acceder a resultados
                        std::unique_lock<std::shared_mutex> lock(mutex_results);

                            //Si el usuario no tiene saldo y es premium limitado, se recarga su saldo con el sistema de pago
                           
                                    
                                    std::cout << std::to_string(*current_user_balance) << "\n";
                                    
                                    if(*current_user_balance <= 0 && current_user_type == User_Type::PREMIUMLIMITED){
                                    std::unique_lock<std::mutex> lock_ps(mutex_balance);
                                    system_pay_queue->push(std::make_tuple(current_user_balance, searcher_sem_system_pay));
                                    cond_var_system_pay_queue->notify_one();
                                    searcher_sem_system_pay->lock(); //Se bloquea hasta que el sistema de pago termine
                                    lock_ps.unlock();
                                    }
                                    
                                      

                            //Si el usuario no tiene saldo y es gratuito, dejamos de buscar palabras en la línea del archivo
                            if(*current_user_balance <= 0 && current_user_type == User_Type::FREE)
                            break;

                            //Añadimos la palabra encontrada al resultado del archivo
                            Word_Found_Info word_found(num_linea+1, previous, last);
                            searcher_result.addWordFound(word_found);
                            
                            //Si el usuario es FREE o PREMIUMLIMITED, su saldo se resta
                            if(current_user_type == User_Type::FREE || current_user_type == User_Type::PREMIUMLIMITED)
                                *current_user_balance = *current_user_balance -1;


                    }
                
                num_linea++;
            
            }

            archivo.close();

            //Añadimos resultado del archivo a resultado del buscador
            std::unique_lock<std::shared_mutex> lock(mutex_results);
                current_result -> addSearcherResultInfo(searcher_result);

            
            
    }
        
};


    

#endif



