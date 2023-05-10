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
std::vector<std::string> dictionary;
std::vector<std::thread> user_threads;
std::vector<std::thread> searcher_threads;

//Para el sistema de pago
std::mutex sem_system_pay_queue;
std::queue<std::shared_ptr<User>> system_pay_queue;
std::condition_variable cond_var_system_pay_queue;

//Para el sistema de búsqueda
std::queue<std::shared_ptr<Request>> request_queue;

//Para los buscadores
std::condition_variable cond_var_request_queue;
std::mutex sem_request_queue;



//std::shared_ptr<UserPremiumLimited> user = std::make_shared<UserPremiumLimited>(1, 100);
int main(int argc, char *argv[])
{
    //Control de argumentos
    if (argc != 1)
    {
        std::cerr << "Use: /program\n";
        return EXIT_FAILURE;
    }

    //Creamos el diccionario
    dictionary = getFileWords(std::string(FILES_PATH) + "Diccionario.txt");

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
    Paysystem.paySysWorking(std::ref(sem_system_pay_queue), std::ref(cond_var_system_pay_queue), system_pay_queue);
    
}

std::string getRandomWordDictionary() {
    static std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 34);

    int random_number = dist(mt);
    return dictionary[random_number];
}

std::vector<std::string> getRandomFiles(){
    //Archivos a buscar
    std::vector<std::string> files;
    files.push_back(std::string(FILES_PATH) + "prueba.txt"); //Cambiar por algo más aleatorio, y que puedan ser varios
    files.push_back(std::string(FILES_PATH) + "VIVE-TU-SUEÑO.txt"); //Cambiar por algo más aleatorio, y que puedan ser varios
    return files;
}

//Cada usuario será un hilo
void createRandomUser(int user_id){ 
    //Creacion usuario
    std::shared_ptr<User> user = std::make_shared<User>(user_id, 500, false, false);
    
    srand(time(nullptr));
    int random_number = rand() % 2 + 0;

    switch (random_number){
        case 0:
            //Usuario gratuito
            break;
        
        case 1:
            //Usuario PremiumLimited
            user->setLimited(true);
            break;
        
        case 2:
            //Usuario Premium
            user->setPremium(true);
            break;
    }

    std::string word = getRandomWordDictionary();
    std::vector<std::string> files = getRandomFiles();

    //std::shared_ptr<Request> request = std::make_shared<Request>(word, files, user->getSemUser(), user->getResult());
    user->makeRequest(word, files);
    request_queue.push(user->getRequest());
    cond_var_request_queue.notify_all(); //Avisamos a un buscador de que hay una peticion


    user->getSemUser()->lock(); //El usuario se bloquea, cuando su peticion sea atendida se desbloqueará
    //user->saludar();

    std::unique_lock<std::mutex> lock(*(user->getSemUser()));
    //Creamos el archivo txt que guarda la petición y el resultado
    std::string path = std::string(RESULTS_PATH) + std::to_string(user_id) + ".txt";
    writeFile(path, user-> getRequest()->requestToString());
    writeFile(path, user-> getResult()->resultToString());
    
}

void createUsersThreads(int num_users){
    
    for(int i = 0; i < num_users; i++){
        user_threads.push_back(std::thread(createRandomUser, i));
    }

}


void createSearchers(int searcher_id, std::string color){
    Searcher searcher = Searcher(searcher_id, color);
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
