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

#define balance 4 //Cantidad de recargo

class Pay_Sys{
public:    

    void paySysWorking(std::shared_ptr<std::mutex>  sem_system_pay_queue,
    std::shared_ptr<std::condition_variable> cond_var_system_pay_queue, 
    std::shared_ptr<std::queue<std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int>>> system_pay_queue){
        
        while(true){
        std::unique_lock<std::mutex> lock(*sem_system_pay_queue); //Seccion critica
        cond_var_system_pay_queue->wait(lock, [&system_pay_queue] {return !(system_pay_queue->empty());});

        std::tuple<std::shared_ptr<int>, std::shared_ptr<std::mutex>, int> tuple_balance_sem = system_pay_queue->front();
        system_pay_queue->pop();
        
        std::shared_ptr<int> user_balance = std::get<0>(tuple_balance_sem);
        *user_balance = balance;

        int user_id = std::get<2>(tuple_balance_sem);

        std::shared_ptr<std::mutex> searcher_sem = std::get<1>(tuple_balance_sem);
        searcher_sem->unlock();

        std::cout << color << "[Sistema Pago] saldo de usuario " << std::to_string(user_id) << " recargado.\n";
        }
    
    }

private:
    std::string color =  "\033[93m";    // Amarillo claro
    
};