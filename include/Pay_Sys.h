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

#define balance 200 //Cantidad de recargo

class Pay_Sys{
public:

    std::shared_ptr<std::mutex> sem; //Deberá asignarse antes de asociar el objeto a un hilo. Semaforo para cola de recargos
    std::shared_ptr<std::condition_variable> cond_var; //Deberá asignarse antes de asociar el objeto a un hilo
    std::shared_ptr<std::queue<std::shared_ptr<User>>> user_queue; //Deberá asignarse antes de asociar el objeto a un hilo

    void operator ()(){

        while(true){
        std::unique_lock<std::mutex> lock(*sem); //Seccion critica
        (*cond_var).wait(lock, [this] {return ((*user_queue).empty());}); //Hay que añadir this

        User user = (*(*user_queue).front()); //Ni de coña funciona
        (*user_queue).pop();
        user.setBalance(balance);

    }

    }
};