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

class File_Result_Info {

    public:

        int thread_number = 0;
        int line_i = 0;
        int line_f = 0;
        std::vector<Word_Found_Info> word_found_info;

        File_Result_Info(int _thread_number, int _line_i, int _line_f){
            thread_number = _thread_number;
            line_i = _line_i;
            line_f = _line_f;
        }

        File_Result_Info(){};

        void setThreadNumber(int  _thread_number){
            thread_number = _thread_number;
        }

        void setLineI(int _line_i){
            line_i = _line_i;
        }

        void setLineF(int _line_f){
            line_f = _line_f;
        }

        void addWordFoundInfo(Word_Found_Info _word){
            word_found_info.push_back(_word);
        }
  
};

class Searcher_Result_Info{

    public:
        int id_searcher = 0;
        std::string file;
        std::vector<File_Result_Info> file_result_info;

        Searcher_Result_Info(int _id_searcher, std::string _file){
            id_searcher = _id_searcher;
            file = _file;
        }

        void addFileResultInfo(File_Result_Info file){
            file_result_info.push_back(file);
        }
        
};

class Result{

    public:
        std::vector<Searcher_Result_Info> searcher_result_info;
        Result(){};
        void addSearcherResultInfo(Searcher_Result_Info searcher){
            searcher_result_info.push_back(searcher);
        }

        void printResult(){
            for (int i = 0; i < searcher_result_info.size(); i++){
                for (int j = 0; j < searcher_result_info[i].file_result_info.size(); j++)
                    //for (int z = 0; z < searcher_result_info[i].file_result_info[j].word_found_info.size(); z++){
                        std::cout << "Searcher: " << searcher_result_info[i].id_searcher << " Archivo " << searcher_result_info[i].file <<
                        " Hilo: " << searcher_result_info[i].file << " Inicio: " << searcher_result_info[i].file_result_info[j].line_i
                        << " Final: " << searcher_result_info[i].file_result_info[j].line_f << " Linea: " << std::endl;
                        //searcher_result_info[i].file_result_info[j].word_found_info[z].line << std::endl;
                    //}
                
            }
        }
};


        //     Word_found word(_file,_line, _previous_word, _later_word);
        //     results.push_back(word);
        // }


#endif
