#ifndef USER_H
#define USER_H

/************************************************************************************
* Nombre: User.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO2 2023
*
* Descripción: Clase usuario. Contiene un objeto puntero peticion y un objeto
* puntero resultado. Cuando se crea un usuario, se rellenan los datos de la peticion
* y se envia a la cola de peticiones. Tambien se rellenan algunos datos del resultado
************************************************************************************/

#include <mutex>
#include <shared_mutex>
#include "../include/Request.h"
#include "../include/Global_Vars.h"

class User{//Clase usuario
    public:

        User(int _id, int _balance, USER_TYPE _type){ //Constructor usuario
            id = _id;
            *p_balance = _balance;
            type = _type;

            p_sem->lock(); //Se inicializa el semaforo (no se bloquea)
        
        }

        User(){};

        void balanceUp(double _balance){
            *p_balance = _balance;
        }

        int getID(){
            return id;
        }

        std::shared_ptr<int> getBalance(){
            return p_balance;
    
        }

        void setType(USER_TYPE _type){
            type = _type;
        }

        USER_TYPE getType(){
            return type;
        }

        std::shared_ptr<Request> getRequest(){
            return p_request;
        }

        void setResult(std::shared_ptr<Result> _result){
            p_result = _result;
        }

        std::shared_ptr<std::mutex> getSemUser(){
            return p_sem;
        }

        std::shared_ptr<Result> getResult(){
            return p_result;
        }

        /*Descripcion: Completa los datos de la peticion*/
        void makeRequest(std::string _word, std::vector<std::string> _files){
            p_request -> setWord(_word);
            p_request -> setFiles(_files);
            p_request -> setSemUser(p_sem);
            p_request -> setResult(p_result);
            p_request -> setUserId(id);
            p_request -> setUserBalance(p_balance);
            p_request -> setUserType(type);

            //Se le informa al resultado de la palabra a buscar y el tipo de usuario
            p_result->setWordSearched(_word);
            p_result->setUserType(type);
        }


    private:
        int id;         //ID del usuario
        //Puntero al saldo del usuario
        std::shared_ptr<int> p_balance = std::make_shared<int>(0);
        USER_TYPE type; //Tipo de usuario

        //Puntero al semaforo del usuario
        std::shared_ptr<std::mutex> p_sem = std::make_shared<std::mutex>();
        //Puntero a la peticion del usuario
        std::shared_ptr<Request> p_request = std::make_shared<Request>();
        //Puntero al resultado del usuario
        std::shared_ptr<Result> p_result = std::make_shared<Result>();

};

#endif