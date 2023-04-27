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

std::mutex sem;
std::condition_variable cond_var;
bool var = false;

void algo(){
    
    std::unique_lock<std::mutex> lock(sem); //Seccion critica
    cond_var.wait(lock, [] {return var;});
    std::cout << "He pasado \n";
}

void otro(){
    std::this_thread::sleep_for(std::chrono::seconds(2));
    var = false;
    cond_var.notify_one(); //hace que el primer hilo compruebe la variable de condicion
}

int main(){
    std::thread h(algo);
    std::thread h2(otro);
    h.join();
    h2.join();
}