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

    void paySysWorking(std::mutex& sem, std::condition_variable& cond_var, std::queue<std::shared_ptr<User>>& userpl_queue){

         
        // std::unique_lock<std::mutex> lock(sem); //Seccion critica
        // cond_var.wait(lock, [&userpl_queue] {return !(userpl_queue.empty());}); //Hay que añadir this

        // std::shared_ptr<User>& user = userpl_queue.front();
        // user->balanceUp(500);
        // std::cout << "Saldo recargado\n";
        // user->Unlock();
    
    }
    
};