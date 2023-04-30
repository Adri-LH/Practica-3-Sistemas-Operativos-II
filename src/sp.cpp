#include <iostream>
#include <thread>
#include <shared_mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <memory>
#include <algorithm>
#include <ctime>
#include "struct.h"

/*
Crear sistema de pago sencillo
(singleton) que atienda peticiones
de una cola. Cada petición debe
incluir id del hilo solicitante,
cantidad a pagar, saldo y
semáforo que bloquee al
pagador. El sistema de pago
debe restar la cantidad del saldo
y enviar notificación.
*/

std::queue<std::shared_ptr<peticion>> petitions_queue;
std::condition_variable payment_cv;
std::vector<std::thread> v_threads;
std::mutex mtx;

void client(int i);
void payment_sys();

int main()
{
    for (int i = 0; i < 5; i++)
    {
        v_threads.push_back(std::thread(client, i));
    }
    std::thread payment_sys_thread(payment_sys);

    std::for_each(v_threads.begin(), v_threads.end(), [](std::thread &t)
                  { t.join(); });

    payment_sys_thread.join();

    return EXIT_SUCCESS;
}

void client(int i)
{

    srand(time(NULL));
    std::shared_ptr<peticion> p = std::make_shared<peticion>();

    p->th_id = i;
    p->payment = rand() % 20;
    p->quantity = 100;
    p->mtx.lock();

    std::unique_lock<std::mutex> lock(mtx);
    petitions_queue.push(p);
    lock.unlock();

    payment_cv.notify_one();
    std::cout << "Esperando pago: " << i << std::endl;
    p->mtx.lock();
    std::cout << "Pago realizado: " << i << std::endl;
}

void payment_sys()
{
    int cont = 5;
    while (cont > 0)
    {
        

        std::unique_lock<std::mutex> lock(mtx);
        payment_cv.wait(lock, []()
                        { return !petitions_queue.empty(); });
        int id = petitions_queue.front()->th_id;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        petitions_queue.front()->quantity = petitions_queue.front()->quantity - petitions_queue.front()->payment;
        std::cout << "Thread id: " << id << std::endl;
        std::cout << "Total: " << petitions_queue.front()->quantity << std::endl;
        petitions_queue.front()->mtx.unlock();
        petitions_queue.pop();
        cont--;
    }
}