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
#include "../include/Request.h"
#include "../include/Searcher.h"


#define SEARCHERS_NUM 4  //Numero de buscadores
#define USERS_NUM 1   //Numero de usuarios. Sus perfiles serán aleatorios.

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
    dictionary = getFileWords("../resources/Diccionario.txt");

    //Creamos hilo del Sistema de Pago
    std::thread T_Pay_Sys(createPaySys);

    //Creamos buscadores
    createSearchersThreads(SEARCHERS_NUM);
    //std::vector<std::string> vectorr;
    //vectorr.push_back("../resources/prueba.txt");
    //vectorr.push_back("../resources/21-LEYES-DEL-LIDERAZGO.txt");
    //std::shared_ptr<Request> request = std::make_shared<Request>(1, "David", vectorr);
    //request_queue.push(request);
    
    //Creamos usuarios y peticiones
    createUsersThreads(USERS_NUM);
    
    
    
    T_Pay_Sys.join();
    std::for_each(user_threads.begin(), user_threads.end(), std::mem_fn(&std::thread::join));
    std::for_each(searcher_threads.begin(), searcher_threads.end(), std::mem_fn(&std::thread::join));


}

void createPaySys(){
    Pay_Sys Paysystem;
    //userpl_queue.push(user);
    Paysystem.paySysWorking(std::ref(sem_system_pay_queue), std::ref(cond_var_system_pay_queue), system_pay_queue);
    
}

std::string getRandomWordDictionary(){
    srand(time(nullptr));
    int random_number = rand() % 35 + 0;

    //return dictionary[random_number];
    return "David";
}

std::vector<std::string> getRandomFiles(){
    //Archivos a buscar
    std::vector<std::string> files;
    files.push_back("../resources/prueba.txt"); //Cambiar por algo más aleatorio, y que puedan ser varios
    files.push_back("../resources/VIVE-TU-SUEÑO.txt"); //Cambiar por algo más aleatorio, y que puedan ser varios
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

    std::shared_ptr<Request> request = std::make_shared<Request>(user, word, files);
    request_queue.push(request);
    cond_var_request_queue.notify_all(); //Avisamos a un buscador de que hay una peticion


    user->lock(); //El usuario se bloquea, cuando su peticion sea atendida se desbloqueará
    //user->saludar();

    //Creamos el archivo txt que guarda la petición y el resultado
    std::string path = "../user_results/" + std::to_string(user_id) + ".txt";
    writeFile(path, request->requestToString());
    
}

void createUsersThreads(int num_users){
    
    for(int i = 0; i < num_users; i++){
        user_threads.push_back(std::thread(createRandomUser, i));
    }

}


void createSearchers(int searcher_id){
    Searcher searcher = Searcher(searcher_id);
    searcher.searcherWorking(std::ref(sem_request_queue), std::ref(cond_var_request_queue), request_queue);
}

void createSearchersThreads(int num_searchers){
    
    for(int i = 0; i < num_searchers; i++){
        searcher_threads.push_back(std::thread(createSearchers, i));
    }

}
