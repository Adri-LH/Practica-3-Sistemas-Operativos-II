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

std::shared_ptr<std::mutex> sem_provisional; //Deberá pasarse por pipe
std::shared_ptr<std::condition_variable> cond_var_provisional; //Deberá pasarse por pipe
std::shared_ptr<std::vector<std::shared_ptr<User>>> cond_var_provisional; //Deberá pasarse por pipe. Quiza es mejor que sea una cola
//La solucion consiste en un puntero a una cola de punteros de tipo usuario


void topUpBalance(){
    while(true){
        std::unique_lock<std::mutex> lock(*sem_provisional); //Seccion critica
        (*cond_var_provisional).wait(lock, [] {});
    }
}

int main(int argc, char *argv[]){
    if (argc != 1)
    {
        std::cerr << "Use: /program\n";
        return EXIT_FAILURE;
    }

    topUpBalance();

}