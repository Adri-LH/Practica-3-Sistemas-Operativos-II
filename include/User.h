#ifndef USER_H
#define USER_H

#include <mutex>
#include <shared_mutex>
#include "../include/Request.h"

class User{//Clase usuario
    public:
        User(int _id, int _balance, bool premium, bool limited){ //Constructor usuario
            id = _id;
            balance = _balance;
            
        }

        User(){};

        void balanceUp(double _balance){
            balance = _balance;
        }

        void mostrarSaldo(){
            std::cout << balance << "\n";
        }

        void setPremium(bool _premium){
            premium = premium;
        }

        void setLimited(bool _limited){
            limited = _limited;
        }

        int getID(){
            return id;
        }

        void saludar(){
            std::unique_lock<std::mutex> lock(*sem);
            std::cout << "Hola" << std::endl;
        }

        std::shared_ptr<Request> getRequest(){
            return request;
        }

        void setResult(std::shared_ptr<Result> _result){
            result = _result;
        }

        std::shared_ptr<std::mutex> getSemUser(){
            return sem;
        }

        std::shared_ptr<Result> getResult(){
            return result;
        }

        void makeRequest(std::string _word, std::vector<std::string> _files){
            request -> setWord(_word);
            request -> setFiles(_files);
            request -> setSemUser(sem);
            request -> setResult(result);
        }



    private:
        int id;
        int balance;
        bool premium, limited;

        std::shared_ptr<std::mutex> sem = std::make_shared<std::mutex>();
        std::shared_ptr<Request> request = std::make_shared<Request>();
        std::shared_ptr<Result> result = std::make_shared<Result>();

};

#endif