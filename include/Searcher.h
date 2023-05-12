#ifndef SEARCHER_H
#define SEARCHER_H

/************************************************************************************
*
* Nombre: Searcher.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO2 2023
*
* Descripción: Clase buscador encargada de obtener y procesar peticiones de usuarios.
* Un buscador solo puede atender a un usuario a la vez. Creará un hilo por cada
* fichero a buscar pedido por el usuario. Si un usuario se queda sin saldo, el buscador
* es el encargado de avisar y esperar al sistema de pago.
*
* El resultado se creará completando por referencia el objeto Resultado del usuario
* en la funcion makeResult.
*
************************************************************************************/

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

void makeResult(std::string file, int id_thread);
void prepareSubThreads();
void getCurrentUserData();
void acessPaySystem();
bool checkUserBalance();

class Searcher{
    private:
        //VARIABLES QUE NO SE ASIGNAN EN EL CONSTRUCTOR:
        //Semaforo que protege la zona de creacion de resultados. Compartido entre hilos de un buscador
        std::shared_mutex mutex_results;    
        //Puntero a semaforo para bloquearse y esperar al sistema de pago. Se envia al S.P.
        std::shared_ptr<std::mutex> p_searcher_sem_system_pay = std::make_shared<std::mutex>(); 
        bool userHasBalance = true; //boolean para saber si un usuario tiene saldo.
        

        //VARIABLES QUE SI ASIGNAN EN EL CONSTRUCTOR
        int id_searcher;                                    //ID del buscador
        std::string color;                                  //Color de impresión del buscador
        //Puntero a un semaforo para acceder a la cola del sistema de pago. Lo comparten buscadores y S.P.
        std::shared_ptr<std::mutex> p_sem_system_pay_queue;
        //Variable de condicion para notificar al sistema de pago que el buscador ha realizado una solicitud de recarga.
        std::shared_ptr<std::condition_variable> p_cond_var_system_pay_queue;
        //Puntero a la cola de peticiones del sistema de pago (cola de tuplas de saldo, semaforo de buscador y id cliente)
        std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int>>> p_system_pay_queue;


        //VARIABLES QUE SE ASIGNAN CUANDO LLEGA UNA PETICION DE UN NUEVO CLIENTE
        std::shared_ptr<Result> p_current_result;     //Punero a resultado equivalente al del usuario
        std::shared_ptr<Request> p_current_request;   //Puntero a peticion equivalente a la del usuario
        std::shared_ptr<int> p_current_user_balance;  //Puntero al saldo del actual usuario
        USER_TYPE current_user_type;                  //Tipo del actual usuario

    public:
        
        /*Constructor clase Searcher*/
        Searcher(int _id_searcher, std::string _color, std::shared_ptr<std::mutex> _p_sem_system_pay_queue,
        std::shared_ptr<std::condition_variable> _p_cond_var_system_pay_queue,
        std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int>>> _p_system_pay_queue){

            id_searcher = _id_searcher;
            color = _color;
            p_sem_system_pay_queue = _p_sem_system_pay_queue;
            p_cond_var_system_pay_queue = _p_cond_var_system_pay_queue;
            p_system_pay_queue = _p_system_pay_queue;
            p_searcher_sem_system_pay->lock(); //Se inicializa, no se bloquea
        }

        /*********************************************************************************
         * 
         * Nombre de la función: searcherWorking
         *
         * Descripción de la función: Espera a que llegue una peticion a la cola p_request_queue.
         * Cuando llega, la procesa y desbloquea el semaforo del usuario para que continue.
         * 
         * Argumentos utilizados:
         * - p_sem_request_queue: Puntero a un semaforo que protege la cola de peticiones.
         * - p_cond_var_request_queue: Puntero a variable de condicion para recibir un notify
         *   de un usuario cuando este coloque una peticion en la cola.
         * - p_request_queue: Puntero a la cola de peticiones.
         * Todos los argumentos con compartidos entre todos los buscadores y clientes.
         *
        *********************************************************************************/
        void searcherWorking(std::shared_ptr<std::mutex> p_sem_request_queue, std::shared_ptr<std::condition_variable> p_cond_var_request_queue,
        std::shared_ptr<std::queue<std::shared_ptr<Request>>> p_request_queue){
 
            while (true){
            //Espera entrada seccion critica (cola peticiones) con semáforo y variable de concición
            std::unique_lock<std::mutex> lock(*p_sem_request_queue);
            p_cond_var_request_queue->wait(lock, [&p_request_queue] {return !(p_request_queue->empty());});

                //Obtenemos por referencia la peticion del usuario
                p_current_request = std::move(p_request_queue->front());
            
                //Obtenemos los datos del nuevo cliente a traves de la peticion actual
                getCurrentUserData();  

                //Imprimimos información
                std::cout << color << "[Buscador " << id_searcher << "] procesará la petición del Usuario " << UserTypeToString(current_user_type) 
                 << " " << p_current_request->getUserId() << std::endl;

                //Se saca la peticion de la cola
                p_request_queue->pop();

            //Desbloqueo de semaforo para que otros buscadores atiendan peticiones
            lock.unlock();

            //Se preparan los hilos para la búsqueda de palabra y se espera a que acaben
            prepareSubThreads();
            
            //Desbloqueamos el semaforo del usuario. Su peticion ha sido atendida
            p_current_request-> getSemUser() -> unlock();

            //Imprimimos información
            std::cout << color << "[Buscador " << id_searcher << "] ha finalizado la petición del Usuario " << UserTypeToString(current_user_type)
            << " " << p_current_request->getUserId() << ", se desbloqueará su semáforo" << std::endl;

            }
            
        }

        /*Descripcion: A traves de la peticion actual se obtienen los datos del nuevo usuario a
        servir Y SE RELLENAN DATOS DEL RESULTADO*/
        void getCurrentUserData(){

                //Obtenemos por referencia el resultado del usuario
                p_current_result = p_current_request->getResult();

                //Obtenemos el saldo y el tipo de usuario
                p_current_user_balance = p_current_request->getUserBalance();
                current_user_type = p_current_request->getUserType();

                //Le indicamos al resultado que buscador le ha atendido
                p_current_result->setIdSearcher(id_searcher);

                //Se determina si el usuario tiene saldo
                if(*p_current_user_balance <= 0 && current_user_type == USER_TYPE::FREE)
                    userHasBalance = false;
                
        }

        /*Descripcion: Crea y espera subhilos. A cada uno se le asigna un fichero donde buscar*/
        void prepareSubThreads(){
            //Creeacion de vector de subhilos
            std::vector<std::thread> subthreads;
            //Para mayor claridad creamos un vector de ficheros
            std::vector<std::string> files = p_current_request->GetFiles();

            //Recorremos el vector de ficheros donde buscar palabras
            for(int i = 0; i < files.size(); i++){
                //Imprimimos información
                std::cout << color << "[Buscador " << id_searcher << "] buscará la palabra :" << p_current_request->getWord()
                << ": en el archivo :" << files[i] << ": para el Usuario " << UserTypeToString(current_user_type) << " "
                << p_current_request->getUserId() << std::endl;
                
                //Se crean los hilos
                subthreads.push_back(std::thread([this, &files, i]() {makeResult(files[i], i);}));
            }

            //Espera a todos los hilos
            std::for_each(subthreads.begin(), subthreads.end(), std::mem_fn(&std::thread::join));

            //Si el usuario se ha quedado sin saldo, lo mostramos
            if(*p_current_user_balance <= 0 && current_user_type == USER_TYPE::FREE)
                std::cout << color << "[Buscador " << id_searcher << "] IMPORTANTE: el Usuario " << UserTypeToString(current_user_type)
                << " " << p_current_request->getUserId() << " se ha quedado sin saldo." << std::endl;

        }

    /*Descripcion: El subhilo busca en un archivo. Se crea el resultado de busqueda
    de un ARCHIVO. Si encuentra palabras, las añadira a este resultado*/
    void makeResult(std::string file_name, int id_thread) {
            //Para mayor claridad creamos un string word
            std::string word = p_current_request->getWord();

            //CREACION DE RESULTADO: Creamos resultado de busqueda de un ARCHIVO
            File_Result_Info searcher_result (id_thread, file_name);

            //Convertimos palabra a minúscula
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
                
            //Comprobamos si el fichero se abre
            std::fstream  file = openFileRead(file_name);
            if (!file){std::cout << "Error de lectura"; exit(1);}

            std::string line;
            int num_line = 0;

            //Busqueda en una linea
            while (std::getline(file, line) && userHasBalance) {

                // Convertir la línea a minúsculas
                std::string line_lower = line;
                std::transform(line_lower.begin(), line_lower.end(), line_lower.begin(), [](unsigned char c){ return std::tolower(c); });

                
                int pos = 0;
                // Si el buscador encuentra una palabra
                while ((pos = line_lower.find(word, pos)) != std::string::npos) {
                    //Si el buscador encuentra una palabra:
                    pos += word.length();
                    std::string previous = getRelativeWord(line, pos, false);  //Palabra anterior
                    std::string last = getRelativeWord(line, pos, true);       //Palabra posterior

                    //Semaforo para acceder a resultados
                    std::unique_lock<std::shared_mutex> lock(mutex_results);

                        //Comprobamos el saldo del usuario y si puede recargar. Si no, se corta la busqueda 
                        if (!checkUserBalance()){
                            userHasBalance = false;
                            break;
                        }

                        //AÑADIMOS PALABRA ENCONTRADA AL RESULTADO DE ARCHIVO
                        Word_Found_Info word_found(num_line+1, previous, last);
                        searcher_result.addWordFound(word_found);
                            
                        //Si el usuario es FREE o PREMIUMLIMITED, su saldo se resta
                        if(current_user_type == USER_TYPE::FREE || current_user_type == USER_TYPE::PREMIUMLIMITED)
                            *p_current_user_balance = *p_current_user_balance -1;
                    }
                
                num_line++;
            }

            //Cada subhilo cierra su archivo
            file.close();

            //AÑADIMOS RESULTADO DEL FICHERO AL RESULTADO TOTAL DEL BUSCADOR
            std::unique_lock<std::shared_mutex> lock(mutex_results);
                p_current_result -> addSearcherResultInfo(searcher_result);

                 
    }
    
    /*Comprueba si el usuario tiene saldo. Si puede hacer recarga, la hace*/
    bool checkUserBalance(){
        bool correct = true;
        //Si el usuario no tiene saldo y es premium limitado, se recarga su saldo con el sistema de pago 
        if(*p_current_user_balance <= 0 && current_user_type == USER_TYPE::PREMIUMLIMITED)
            acessPaySystem();
                                    
        //Si el usuario no tiene saldo y es gratuito, dejamos de buscar palabras en la línea del archivo
        if(*p_current_user_balance <= 0 && current_user_type == USER_TYPE::FREE)
            correct = false;
        
        return correct;
    }

    void acessPaySystem(){
        //Acceso a la cola del sistema de pago
        std::unique_lock<std::mutex> lock_pay_sys(*p_sem_system_pay_queue);
            p_system_pay_queue->push(std::make_tuple(p_current_user_balance, p_searcher_sem_system_pay, p_current_request->getUserId()));
            p_cond_var_system_pay_queue->notify_one();
        lock_pay_sys.unlock();
        p_searcher_sem_system_pay->lock(); //Se bloquea hasta que el sistema de pago termine
        p_current_result -> increaseTotalBalanceReload(); //Se incrementa el numero de recargas de saldo del usuario
    }
        
};


    

#endif



