/************************************************************************************
*
* Nombre: ssooiigle.cpp
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO 2023

* Descripción: Clase encargada de creacion de usuarios, buscadores y sistema de pago.
  Interesante mirar Global_Vars.h

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
#include <tuple>
#include <mutex>
#include <shared_mutex>
#include <regex>
#include <condition_variable>
#include <queue>
#include <unistd.h>
#include <random>
#include <chrono>


#include "../include/Result.h"
#include "../include/myfiles.h"
#include "../include/User.h"
#include "../include/Pay_Sys.h"
#include "../include/Request.h"
#include "../include/Searcher.h"
#include "../include/Global_Vars.h"

//Declaracion de funciones
void createPaySys();
void createSearchers(int searcher_id, std::string color);
void createSearchersThreads(int num_searchers);
void createUsersThreads(int num_users);
void createRandomUser(int user_id);
void createUserRequest(std::shared_ptr<User> user);
void createTxtResults(std::string path, std::string request, std::string result);
void addUserRequestWithPreference(std::shared_ptr<User> user);
std::string getRandomWordDictionary();
std::vector<std::string> getRandomFiles();

//Variables globales
std::vector<std::thread> g_user_threads;      //Vector de hilos de usuarios
std::vector<std::thread> g_searcher_threads;  //Vector de hilos de buscadores
int g_user_id_counter = 0;                    //Contador de usuarios creados

//PARA EL SISTEMA DE PAGO

//Puntero a semaforo que controla el acceso a la cola del sistema de pago
std::shared_ptr<std::mutex> p_sem_system_pay_queue = std::make_shared<std::mutex>();
//Puntero a la variable de condicion del sistema de pago. Buscadores la usaran para el notify en una solicitud.
//El sistema de pago lo usara para el wait.
std::shared_ptr<std::condition_variable> p_cond_var_system_pay_queue = std::make_shared<std::condition_variable>();
//Puntero a cola de peticiones del sistema de pago. Es una tupla que contiene 3 elementos:
//Puntero a saldo de un usuario, puntero a semaforo del buscador y id del usuario.
std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int>>> p_system_pay_queue 
= std::make_shared<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int>>>();

//PARA EL SISTEMA DE BUSQUEDA

//Puntero a semaforo que controla el acceso a la cola de peticiones de buscadores
std::shared_ptr<std::mutex> p_sem_request_queue = std::make_shared<std::mutex>();
//Puntero a variable de condicion del sistema de busqueda. Los usuarios lo usaran para el notify en una solicitud.
//Los buscadores lo usaran para el wait.
std::shared_ptr<std::condition_variable> p_cond_var_request_queue = std::make_shared<std::condition_variable>();
//Puntero a una cola de punteros de peticiones de busqueda.
std::shared_ptr<std::queue<std::shared_ptr<Request>>> p_request_queue = std::make_shared<std::queue<std::shared_ptr<Request>>>();

int main(int argc, char *argv[])
{
    //Control de argumentos
    if (argc != 1)
    {
        std::cerr << "Use: /program\n";
        return EXIT_FAILURE;
    }

    //Creamos hilo del Sistema de Pago
    std::thread T_Pay_Sys(createPaySys);

    //Creamos buscadores
    createSearchersThreads(SEARCHERS_NUM);
    
    //Creamos usuarios y peticiones
    createUsersThreads(USERS_NUM);

    //Volvemos a crear usurios y peticiones a los 5 segundos
    std::for_each(g_user_threads.begin(), g_user_threads.end(), std::mem_fn(&std::thread::join));
    std::cout << "\n\033[0m" << "[SSOOIIGLE] en 5 segundos se volveran a enviar " << std::to_string(USERS_NUM) << " peticiones.\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    createUsersThreads(USERS_NUM);

    //Espera a todas las entidades
    T_Pay_Sys.join();
    std::for_each(g_user_threads.begin(), g_user_threads.end(), std::mem_fn(&std::thread::join));
    std::for_each(g_searcher_threads.begin(), g_searcher_threads.end(), std::mem_fn(&std::thread::join));

}

/*Descripcion: Crea el sistema de pago*/
void createPaySys(){
    Pay_Sys Paysystem;
    Paysystem.paySysWorking(p_sem_system_pay_queue, p_cond_var_system_pay_queue, p_system_pay_queue);
    
}

/*Descripcion: Crea los buscadores*/
void createSearchers(int searcher_id, std::string color){
    Searcher searcher = Searcher(searcher_id, color, p_sem_system_pay_queue, p_cond_var_system_pay_queue, p_system_pay_queue);
    searcher.searcherWorking(p_sem_request_queue, p_cond_var_request_queue, p_request_queue);
}

/*Descripcion: Crea los hilos de los buscadores*/
void createSearchersThreads(int num_searchers){
    //Se obtiene un color para la impresión del buscador
    int color_index = 0;
    for(int i = 0; i < num_searchers; i++){
        std::string color = COLORS[color_index];

        g_searcher_threads.push_back(std::thread(createSearchers, i,color));
        color_index++;

        //Los colores son limitados, hay que reiniciar el index
        if(color_index == COLORS.size())
            color_index = 0;
    }

}

/*Descripcion: Crea los hilos de los usuarios*/
void createUsersThreads(int num_users){
    //Borramos el contenido de la carpeta user_results (resultados antiguos)
    deleteDirectoryFiles(RESULTS_PATH);

    for(int i = 0; i < num_users; i++){
        g_user_threads.push_back(std::thread(createRandomUser, g_user_id_counter));
        g_user_id_counter++;
    }

}

/*Descripcion: Creacion de usuarios y creacion de peticion*/
void createRandomUser(int user_id){ 
    //Creacion usuario, por defecto Free
    std::shared_ptr<User> user = std::make_shared<User>(user_id, USER_BALANCE, USER_TYPE::FREE);
    int random_number = getRandomNumber(0, 2);

    switch (random_number){
        case 0:
            //Usuario Free
            break;
        case 1:
            //Usuario PremiumLimited
            user->setType(USER_TYPE::PREMIUMLIMITED);
            break;
        
        case 2:
            //Usuario Premium
            user->setType(USER_TYPE::PREMIUM);
            break;
    }

    //El usuario hace la peticion
    createUserRequest(user);

}

/*Descripcion: Creacion, envio y recepcion de peticion y resultado del usuario*/
void createUserRequest(std::shared_ptr<User> user){
    //Se generan los datos de la busqueda
    std::string word = getRandomWordDictionary();
    std::vector<std::string> files = getRandomFiles();

    //Se hace la peticion
    user->makeRequest(word, files);

    //Colocamos la peticion en la cola de peticiones, bloqueando el semaforo de peticiones
    std::unique_lock<std::mutex> lock_request_queue(*p_sem_request_queue);
    addUserRequestWithPreference(user);

    //Avisamos a un buscador de que hay una peticion
    p_cond_var_request_queue->notify_one(); 
    lock_request_queue.unlock();

    //El usuario se bloquea, cuando su peticion sea atendida se desbloqueará.
    user->getSemUser()->lock();

    //Creamos resultados
    std::string path = std::string(RESULTS_PATH) + std::to_string(user->getID()) + ".txt";
    createTxtResults(path, user-> getRequest()->requestToString(), user-> getResult()->resultToString(*(user->getBalance())));
}

/*Descripcion: */
void addUserRequestWithPreference(std::shared_ptr<User> user){
    int randomnumber = getRandomNumber(0, 9);

    //Usuarios no premium
    if(user->getType() == USER_TYPE::FREE){
        if (randomnumber <= 1)
            p_request_queue->push(user->getRequest());      // 20% Principio de la cola
        else
            p_request_queue->emplace(user->getRequest());   //80% Final de la cola
    }
    //Usuarios premium
    else{
        if (randomnumber <= 1)
            p_request_queue->emplace(user->getRequest());   //20% Final de la cola
        else
            p_request_queue->push(user->getRequest());      //80% Principio de la cola
    }
}

/*Descripcion: Crea un txt de resultado de busqueda y le añade informacion*/
void createTxtResults(std::string path, std::string request, std::string result){
    writeFile(path, request);
    writeFile(path, result);
}

/*Descripcion: Obtiene una palabra aleatoria del diccionario*/
std::string getRandomWordDictionary() {
    //Obtenemos una palabra aleatoria
    int random_number = getRandomNumber(0, DICTIONARY.size()-1);
    return DICTIONARY[random_number];
}

/*Descripcion: Obtiene un numero aleatorio de archivos aleatorios*/
std::vector<std::string> getRandomFiles(){
    //Numero archivos a buscar
    int num_files = getRandomNumber(MIN_FILES, MAX_FILES);

    //Archivos aleatorios a buscar
    std::vector<std::string> files = getFilesInDirectory(FILES_PATH);
    if(files.size() == 0) std::cout << " Files directory is empty" << std::endl;

    //Archivos seleccionados
    std::vector<std::string> selected_files;
    
    for (int i = 0; i < num_files; i++){
        //Obtenemos archivo aleatorio
        std::string file = files[getRandomNumber(0, files.size() - 1)];
        selected_files.push_back((std::string(FILES_PATH) + file));
    }

    return selected_files;
}

