#ifndef USER_H
#define USER_H

#include <mutex>
#include <shared_mutex>

class User{//Clase usuario
    public:
        User(int _id, int _balance, bool premium, bool limited){ //Constructor usuario
            id = _id;
            balance = _balance;
        }

        User();

        void balanceUp(double _balance){
            balance = _balance;
        }

        void mostrarSaldo(){
            std::cout << balance << "\n";
        }

        void Saludar(){
            sem.lock();
            std::cout << "Hola\n";
        }

        void Unlock(){
            sem.unlock();
        }

        void setPremium(bool _premium){
            premium = premium;
        }

        void setLimited(bool _limited){
            limited = _limited;
        }

    private:
        int id;
        int balance;
        std::mutex sem;
        bool premium, limited;

};

#endif