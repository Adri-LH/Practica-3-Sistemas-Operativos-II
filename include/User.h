#ifndef USER_H
#define USER_H

#include <mutex>
#include <shared_mutex>

class User{//Clase usuario
    public:
        User(int _id){ //Constructor usuario
        id = _id;
        }
    private:
        int id;
};

class UserFree : protected User{ //Clase UserFree. Hereda de User
    public:
        UserFree(int _id, int _balance) : User(_id){ //Constructor UserFree
        balance = _balance;
        }

    protected:
        int balance;

};


class UserPremiumLimited : protected UserFree{ //Clase UserPremiumLimited. Hereda de User
    public:
        UserPremiumLimited(int _id, int _balance) : UserFree(_id, _balance){
            sem.lock(); //Se inicializa quedandose a 0. Con otro lock, se bloquea (-1)
        }
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

    private:
        std::mutex sem;

};

class UserPremium : protected User{
    public:
        UserPremium(int _id) : User(_id){}

};

#endif




                  