#ifndef REQUEST_H
#define REQUEST_H

#include "User.h"
#include <vector>


class Request
{
private:
    int user_id;
    std::string word; //Palabras a buscar
    std::vector<std::string> files; //Archivos a buscar

public:
    Request(int _user_id, std::string _word, std::vector<std::string> _files)
    {
        user_id = _user_id;
        word = _word;
        files = _files;
    }

    std::string getWord(){
        return word;
    }

    std::vector<std::string> GetFiles(){
        return files;
    }

};

#endif