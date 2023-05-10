#ifndef REQUEST_H
#define REQUEST_H

#include "User.h"
#include <vector>
#include <string>


class Request
{
private:
    std::string word; //Palabras a buscar
    std::vector<std::string> files; //Archivos a buscar
    
    std::shared_ptr<std::mutex> sem_user;
    std::shared_ptr<Result> result; //resultado de la búsqueda

public:
    Request(std::string _word, std::vector<std::string> _files, std::shared_ptr<std::mutex> _sem_user, std::shared_ptr<Result> _result)
    {
        word = _word;
        files = _files;
        sem_user = _sem_user;
        result = _result;
    }

    Request(){}

    void setWord(std::string _word){
        word = _word;
    }

    void setFiles(std::vector<std::string> _files){
        files = _files;
    }

    void setSemUser(std::shared_ptr<std::mutex> _sem_user){
        sem_user = _sem_user;
    }

    void setResult(std::shared_ptr<Result> _result){
        result = _result;
    }

    std::string getWord(){
        return word;
    }

    std::vector<std::string> GetFiles(){
        return files;
    }

    std::shared_ptr<std::mutex> getSemUser(){
        return sem_user;
    }

    std::shared_ptr<Result> getResult(){
        return result;
    }

    std::string requestToString(){
        std::string request_string = "***** Saludos usuario, has buscado la palabra '" + word + "' en los archivos ";
        for (int i = 0; i < files.size(); i++)
            request_string += (files[i] + " ");

        request_string += " *****";
        return request_string;
    }

    // std::shared_ptr<User> getUser(){
    //     return user;
    // }

    // std::string setSearchResult(std::string _search_result){
    //     search_result = _search_result;
    // }

};

#endif