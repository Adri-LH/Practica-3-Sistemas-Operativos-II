/************************************************************************************
*
* Nombre: Pay_Sys.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO2 2023
*
* Descripción: Sistema de pago. Su unica funcion acepta 3 parametros:
*    - sem_system_pay_queue -> Puntero inteligente al semaforo que controla el 
*    acceso al sistema de pago.
*
*    - cond_var_system_pay_queue -> Puntero inteligente a la variable de condicion que
*    avisa al sistema de pago de que hay una peticion en la system_pay_queue.
*
*    - system_pay_queue -> Puntero inteligente a una cola de tuplas. cada tupla es una
*    peticion que incluye un punt.i. al saldo de un usuario, un punt.i. al semaforo
*    del buscador y un entero id del usuario.
*
************************************************************************************/

#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <fstream>
#include <thread>
#include <sstream>
#include <string>
#include <tuple>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <shared_mutex>
#include "User.h"

class Pay_Sys{
public:    

    void paySysWorking(std::shared_ptr<std::mutex>  p_sem_system_pay_queue,
    std::shared_ptr<std::condition_variable> p_cond_var_system_pay_queue, 
    std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int>>> p_system_pay_queue){
        
        while(true){
        //Seccion critica
        std::unique_lock<std::mutex> lock(*p_sem_system_pay_queue);
        p_cond_var_system_pay_queue->wait(lock, [&p_system_pay_queue] {return !(p_system_pay_queue->empty());});

            //Se obtiene peticion y se elimina de la cola
            std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int> tuple_balance_sem = p_system_pay_queue->front();
            p_system_pay_queue->pop();
            
            //Se recarga el saldo
            std::shared_ptr<int> p_user_balance = std::get<0>(tuple_balance_sem);
            *p_user_balance = RELOAD_BALANCE;

            //Se obtiene id de usuario
            int user_id = std::get<2>(tuple_balance_sem);

            //Se desbloquea el semaforo del buscador para que pueda continuar
            std::shared_ptr<std::mutex> p_searcher_sem = std::get<1>(tuple_balance_sem);
            p_searcher_sem->unlock();

            //Se imprime informacion
            std::cout << color_pay_sys << "[Sistema Pago] saldo de usuario " << std::to_string(user_id) << " recargado.\n";
        }
    
    }

private:

    std::string color_pay_sys =  "\033[93m";    // Color de impresion. Amarillo claro
    
};