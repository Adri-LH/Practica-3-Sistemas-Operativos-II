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


class Word_found{
            public:
                int line;
                std::string previous_word;
                std::string later_word;

            Word_found(int _line, std::string _previous_word, std::string _later_word){
                line = _line;
                previous_word = _previous_word;
                later_word = _later_word;
            }
};

class Result {
    
    private:

        int thread_number = 0;
        int line_i = 0;
        int line_f = 0;
        std::string word;
        std::vector<Word_found> results;

    public:

        Result(int _thread_number, int _line_i, int _line_f, std::string _word)
        {
            thread_number = _thread_number;
            line_i = _line_i;
            line_f = _line_f;
            word = _word;
        }

        void PrintResults(){
            for (int i = 0; i < results.size(); i++){
                std::cout << "[Hilo: " << thread_number << " inicio: " << line_i << " final: " << line_f <<
                "] :: línea " << results[i].line << " :: ... " << results[i].previous_word << " " << word << " " << results[i].later_word << " ...\n";
            
            }
        }

        void add_Result(int _line, std::string _previous_word, std::string _later_word){
            Word_found word(_line, _previous_word, _later_word);
            results.push_back(word);
        }
            
             
};


#endif
