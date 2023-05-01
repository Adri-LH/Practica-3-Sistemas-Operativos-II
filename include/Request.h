#ifndef REQUEST_H
#define REQUEST_H

#include "User.h"
#include <vector>
#include <string>


class Request
{
private:
    std::shared_ptr<User> user;
    std::string word; //Palabras a buscar
    std::vector<std::string> files; //Archivos a buscar
    
    std::string search_result; //resultado de la búsqueda

public:
    Request(std::shared_ptr<User> _user, std::string _word, std::vector<std::string> _files)
    {
        user = _user;
        word = _word;
        files = _files;
    }

    std::string getWord(){
        return word;
    }

    std::vector<std::string> GetFiles(){
        return files;
    }

    std::string requestToString(){
        std::string request_string = "Usuario con ID " + std::to_string(user->getID()) + ", has buscado la palabra '" + word + "' en los archivos ";
        for (int i = 0; i < files.size(); i++)
            request_string += (files[i] + " ");

        return request_string;
    }

    std::shared_ptr<User> getUser(){
        return user;
    }

    std::string setSearchResult(std::string _search_result){
        search_result = _search_result;
    }

};

#endif