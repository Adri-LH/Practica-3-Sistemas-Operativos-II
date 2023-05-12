#ifndef REQUEST_H
#define REQUEST_H

/************************************************************************************
*
* Nombre: Request.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO2 2023
*
* Descripción: Clase peticion de un usuario a un buscador para realizar la busqueda
* de una sola palabra en uno o varios archivos.
*
************************************************************************************/
#include "../include/User.h"
#include "../include/Global_Vars.h"
#include <vector>
#include <string>

class Request
{
private:
    std::string word;                       //Palabra a buscar
    std::vector<std::string> files;         //Archivos a buscar
    
    std::shared_ptr<std::mutex> p_sem_user;   //Puntero al semaforo del usuario
    std::shared_ptr<Result> p_result;         //Puntero al resultado de la búsqueda
    int user_id;                              //ID del usuario
    std::shared_ptr<int> p_user_balance;      //Puntero al saldo usuario
    USER_TYPE user_type;                      //Tipo usuario

public:

    Request(std::string _word, std::vector<std::string> _files, std::shared_ptr<std::mutex> _p_sem_user,
    std::shared_ptr<Result> _p_result, int _user_id, std::shared_ptr<int> _p_user_balance, USER_TYPE _user_type)
    {
        word = _word;
        files = _files;
        p_sem_user = _p_sem_user;
        p_result = _p_result;
        user_id = _user_id;
        p_user_balance = _p_user_balance;
        user_type = _user_type;

    }

    Request(){}

    void setWord(std::string _word){
        word = _word;
    }

    void setFiles(std::vector<std::string> _files){
        files = _files;
    }

    void setSemUser(std::shared_ptr<std::mutex> _p_sem_user){
        p_sem_user = _p_sem_user;
    }

    void setResult(std::shared_ptr<Result> _p_result){
        p_result = _p_result;
    }

    void setUserId(int _user_id){
        user_id = _user_id;
    }

    void setUserBalance(std::shared_ptr<int> _p_user_balance){
        p_user_balance = _p_user_balance;
    }

    void setUserType(USER_TYPE _user_type){
        user_type = _user_type;
    }

    std::shared_ptr<int> getUserBalance(){
        return p_user_balance;
    }

    USER_TYPE getUserType(){
        return user_type;
    }

    std::string getWord(){
        return word;
    }

    std::vector<std::string> GetFiles(){
        return files;
    }

    std::shared_ptr<std::mutex> getSemUser(){
        return p_sem_user;
    }

    std::shared_ptr<Result> getResult(){
        return p_result;
    }

    int getUserId(){
        return user_id;
    }

    /*Descripcion: Retorna informacion de la peticion en forma legible al usuario*/
    std::string requestToString(){

        std::string request_string = "Saludos usuario " + UserTypeToString(user_type) + " con ID "
        + std::to_string(user_id) + ", has buscado la palabra '" + word + "' en los archivos:\n";

        for (int i = 0; i < files.size(); i++)
            request_string += files[i] + "\n";

        request_string += "\n";

        return request_string;
    }

};

#endif