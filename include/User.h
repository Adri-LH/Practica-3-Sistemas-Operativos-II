#ifndef USER_H
#define USER_H

#include <mutex>
#include <shared_mutex>
#include "../include/Request.h"
#include "../include/Global_Vars.h"

class User{//Clase usuario
    public:

        User(int _id, int _balance, User_Type _type){ //Constructor usuario
            id = _id;
            *balance = _balance;
            type = _type;
        }

        User(){};

        void balanceUp(double _balance){
            *balance = _balance;
        }

        int getID(){
            return id;
        }

        std::shared_ptr<int> getBalance(){
            return balance;
    
        }

        void setType(User_Type _type){
            type = _type;
        }

        User_Type getType(){
            return type;
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
            request -> setUserId(id);
            request -> setUserBalance(balance);
            request -> setUserType(type);
        }



    private:
        int id;
        std::shared_ptr<int> balance = std::make_shared<int>(0);
        User_Type type;

        std::shared_ptr<std::mutex> sem = std::make_shared<std::mutex>();
        std::shared_ptr<Request> request = std::make_shared<Request>();
        std::shared_ptr<Result> result = std::make_shared<Result>();

};

#endif