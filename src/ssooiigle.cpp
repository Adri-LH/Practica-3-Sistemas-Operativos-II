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

void createPaySys();
void createUsersThreads(int num_users);
void createRequest(int user_id);
void createSearchersThreads(int num_searchers);

//Memorias compartidas (no todas lo tendran que ser)
std::vector<std::thread> user_threads;
std::vector<std::thread> searcher_threads;

//Para el sistema de pago
std::shared_ptr<std::mutex> sem_system_pay_queue = std::make_shared<std::mutex>(); //a los dos
std::shared_ptr<std::condition_variable> cond_var_system_pay_queue = std::make_shared<std::condition_variable>();
std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>>>> system_pay_queue = std::make_shared<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>>>>();


//Para el sistema de búsqueda
std::queue<std::shared_ptr<Request>> request_queue;

//Para los buscadores
std::condition_variable cond_var_request_queue;
std::mutex sem_request_queue;
//Son los users

//std::shared_ptr<UserPremiumLimited> user = std::make_shared<UserPremiumLimited>(1, 100);
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
    
    T_Pay_Sys.join();
    std::for_each(user_threads.begin(), user_threads.end(), std::mem_fn(&std::thread::join));
    std::for_each(searcher_threads.begin(), searcher_threads.end(), std::mem_fn(&std::thread::join));

}

void createPaySys(){
    Pay_Sys Paysystem;
    Paysystem.paySysWorking(sem_system_pay_queue, cond_var_system_pay_queue, system_pay_queue);
    
}

std::string getRandomWordDictionary() {
    //Obtenemos una palabra aleatoria
    int random_number = getRandomNumber(0, dictionary.size()-1);
    //return dictionary[random_number];
    return "David";
}

std::vector<std::string> getRandomFiles(){
    //Numero archivos a buscar
    int num_files = getRandomNumber(MIN_FILES, MAX_FILES);

    //Archivos aleatorios a buscar
    std::vector<std::string> files = getFilesInDirectory(FILES_PATH);
    if(files.size() == 0) std::cout << " Files directory is empty" << std::endl;

    //Archivos seleccionados
    std::vector<std::string> selected_files;
    
    // for (int i = 0; i < num_files; i++){
    //     //Obtenemos archivo aleatorio
    //     std::string file = files[getRandomNumber(0, files.size() - 1)];
    //     selected_files.push_back((std::string(FILES_PATH) + file));

    // }

    selected_files.push_back((std::string(FILES_PATH) + "prueba.txt"));
    selected_files.push_back((std::string(FILES_PATH) + "prueba2.txt"));

    return selected_files;
}

//Cada usuario será un hilo
void createRandomUser(int user_id){ 
    //Creacion usuario, por defecto Free
    std::shared_ptr<User> user = std::make_shared<User>(user_id, USER_BALANCE, User_Type::FREE);
    //int random_number = getRandomNumber(0, 2);
    int random_number = 1;
    switch (random_number){
        case 0:
            //Usuario Free
            break;
        case 1:
            //Usuario PremiumLimited
            user->setType(User_Type::PREMIUMLIMITED);
            break;
        
        case 2:
            //Usuario Premium
            user->setType(User_Type::PREMIUM);
            break;
    }

    std::string word = getRandomWordDictionary();
    std::vector<std::string> files = getRandomFiles();

    user->makeRequest(word, files);

    //Colocamos la peticion en la cola de peticiones, bloqueando el semaforo de peticiones
    std::unique_lock<std::mutex> lock_request_queue(sem_request_queue);
    request_queue.push(user->getRequest());
    cond_var_request_queue.notify_one(); //Avisamos a un buscador de que hay una peticion
    lock_request_queue.unlock();

    //El usuario se bloquea, cuando su peticion sea atendida se desbloqueará (se hace 2 veces porque la primera es para inicializarlo)
    user->getSemUser()->lock();
    user->getSemUser()->lock();

    //Creamos el archivo txt que guarda la petición y el resultado
    std::string path = std::string(RESULTS_PATH) + std::to_string(user_id) + ".txt";
    writeFile(path, user-> getRequest()->requestToString());
    writeFile(path, user-> getResult()->resultToString(*(user->getBalance())));
    
}

void createUsersThreads(int num_users){
    //Borramos el contenido de la carpeta user_results.
    deleteDirectoryFiles(RESULTS_PATH);

    for(int i = 0; i < num_users; i++){
        user_threads.push_back(std::thread(createRandomUser, i));
    }

}

void createSearchers(int searcher_id, std::string color){
    Searcher searcher = Searcher(searcher_id, color, sem_system_pay_queue, cond_var_system_pay_queue, system_pay_queue);
    searcher.searcherWorking(std::ref(sem_request_queue), std::ref(cond_var_request_queue), request_queue);
}

void createSearchersThreads(int num_searchers){
    int color_index = 0;
    for(int i = 0; i < num_searchers; i++){
        std::string color = colors[color_index];

        searcher_threads.push_back(std::thread(createSearchers, i,color));
        color_index++;

        if(color_index == colors.size())
            color_index = 0;
    }

}
