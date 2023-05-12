/************************************************************************************
*
* Nombre: Result.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: SSOO2 2023
*
* Descripción: Clase resultado. El usuario tiene un puntero resultado. Lo incluye en
* la peticion y manda la peticion. El buscador completa el resultado.
*
*
* Esta clase se divide en 3 subclases: Result, File_Result_Info y Word_Found_Info
* - Result: Resultado final de un buscador. Contiene el id del buscador y la palabra
* a buscar. Contiene un vector de File_Result_Info
*
* - File_Result_Info: Es el resultado de un solo archivo. Contiene el id del hilo
* que ha hecho la busqueda y el nombre del respectivo archivo. Tambien contiene
* un vector de Word_Found_Info

* - Word_Found_Info: Es una palabra encontrada. Contiene la linea donde se ha encontrado,
* la palabra anterior y la palabra posterior.
************************************************************************************/

#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <vector>
#include <iostream>
#include "../include/Global_Vars.h"

/*Descripcion: palabra encontrada*/
class Word_Found_Info{

    public:
        //Para simplificar resultToString, los atributos son publicos
        int line;                   //Linea de la palabra
        std::string previous_word;  //Palabra anterior
        std::string later_word;     //Palabra posterior
        
        Word_Found_Info(int _line,  std::string _previous_word, std::string _later_word){
            line = _line;
            previous_word = _previous_word;
            later_word = _later_word;
        };

};

/*Descripcion: resultado de un archivo*/
class File_Result_Info{

    public:
        //Para simplificar resultToString, los atributos son publicos
        int id_thread;                              //Hilo encargado de la busqueda del fichero
        std::string file;                           //Nombre archivo
        std::vector<Word_Found_Info> words_found;   //vector de palabras encontradas

        File_Result_Info(int _id_thread, std::string _file){
            id_thread = _id_thread;
            file = _file;
        }

        //Añadir palabra al vector
        void addWordFound(Word_Found_Info word_found){
            words_found.push_back(word_found);
        }

};

/*Descripcion: resultado de un buscador*/
class Result{
    private:
        int id_searcher;                                    //ID del buscador encargado de la busqueda del usuario
        int total_balance_reload;                           //Recargos de saldo que se han hecho durante la busqueda
        std::string word_searched;                          //Palabra que el usuario ha buscado
        USER_TYPE user_type;                                //Tipo de usuario
        float total_search_time;
        std::vector<File_Result_Info> searcher_result_info; //Vector de resultados ficheros

    public:
        
        Result(){};

        void addSearcherResultInfo(File_Result_Info file_result){
            searcher_result_info.push_back(file_result);
        }

        void setIdSearcher(int _id_searcher){
            id_searcher = _id_searcher;
        }

        void setTotalSearchTime(float _total_search_time){
            total_search_time = _total_search_time;
        }

        void setWordSearched(std::string _word_searched){
            word_searched = _word_searched;
        }

        void setUserType(USER_TYPE _user_type){
            user_type = _user_type;
        }

        void setTotalBalanceReload(int _total_balance_reload){
            total_balance_reload = _total_balance_reload;
        }

        /*Descripcion: Retorna informacion del resultado en forma legible al usuario*/
        std::string resultToString(int balance){
            std::string result = "RESULTADOS:\n\n";
            for(int i = 0; i < searcher_result_info.size(); i++){
                //Si el buscador no ha encontrado resultados en un archivo
                if (searcher_result_info[i].words_found.size() == 0) 
                    result+= "Buscador: " + std::to_string(id_searcher) 
                    + " Hilo: " + std::to_string(searcher_result_info[i].id_thread) + " no pudo encontrar ningun resultado en el archivo"
                    + searcher_result_info[i].file + "\n";

                else
                for (int j = 0; j < searcher_result_info[i].words_found.size(); j++)
                    result += "Buscador: " + std::to_string(id_searcher) + " Hilo: " + std::to_string(searcher_result_info[i].id_thread)
                    + " Archivo: " + searcher_result_info[i].file + " Linea: " + std::to_string(searcher_result_info[i].words_found[j].line)
                    + " --> " + searcher_result_info[i].words_found[j].previous_word + " " + word_searched +" " 
                    + searcher_result_info[i].words_found[j].later_word +  "\n";
                
                result += "\n";
                  
            }

            //Si el usuario se ha quedado sin saldo
            if (balance <= 0 && user_type == USER_TYPE::FREE)
                result += "AVISO: Durante tu busqueda te has quedado sin saldo. Es posible que falten palabras.\n\n";
            
            //Si el usuario ha realizado recargos
            if (total_balance_reload > 0)
                result += "AVISO: Durante tu busqueda se han realizado " + std::to_string(total_balance_reload) + " recargas de saldo.\n\n";

            result += "El tiempo total de la busqueda ha sido  " + std::to_string(total_search_time) + " segundos.\n";

            return result;
    }
        
};

#endif
