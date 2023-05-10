/************************************************************************************
* Nombre: Result.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: 13/04/2023

* Descripción: Clase de la palabra
************************************************************************************/
#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <vector>
#include <iostream>


class Word_Found_Info{

    public:

        int line;
        std::string previous_word;
        std::string later_word;

        Word_Found_Info(int _line,  std::string _previous_word, std::string _later_word){
            line = _line;
            previous_word = _previous_word;
            later_word = _later_word;
        };

};

class Searcher_Result_Info{

    public:
        int id_searcher;
        int id_thread;
        std::string file;
        std::vector<Word_Found_Info> words_found;

        Searcher_Result_Info(int _id_searcher, int _id_thread, std::string _file){
            id_searcher = _id_searcher;
            id_thread = _id_thread;
            file = _file;
        }

        void addWordFound(Word_Found_Info word_found){
            words_found.push_back(word_found);
        }

        
};

class Result{

    public:
        std::vector<Searcher_Result_Info> searcher_result_info;
        Result(){};
        void addSearcherResultInfo(Searcher_Result_Info searcher){
            searcher_result_info.push_back(searcher);
        }

    std::string resultToString(){
        std::string result;
        for(int i = 0; i < searcher_result_info.size(); i++)
            for (int j = 0; j < searcher_result_info[i].words_found.size(); j++)
                result += "Buscador: " + std::to_string(searcher_result_info[i].id_searcher) + " Hilo: " + std::to_string(searcher_result_info[i].id_thread)
                + " Archivo: " + searcher_result_info[i].file + " Linea: " + std::to_string(searcher_result_info[i].words_found[j].line)
                + " --> " + searcher_result_info[i].words_found[j].previous_word + " " + searcher_result_info[i].words_found[j].later_word +  "\n";
        return result;
    }
        
};

#endif
