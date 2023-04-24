/************************************************************************************
*
* Nombre: word.h
* 
* Nombre de Autor/es:Adrian Carrasco Espinosa y Javier de la Concepcion Dorado 
* 
* Fecha de creación/actualización: 13/04/2023

* Descripción: Clase de la palabra

************************************************************************************/
#ifndef WORD_H
#define WORD_H

#include <string>

class Word {
private:
        class Fragment {
        public:
            int thread_number = 0;
            int line_i = 0;
            int line_f = 0;
        };

        Fragment fragment;
        int line;
        int pos;
        std::string previous_word;
        std::string later_word;

public:

/*********************************************************************************
 * 
 * Nombre de la función: operator<
 *
 *
 * Descripción de la función: Para ordenar al revés, cambiar < por >
 * 
 * Argumentos utilizados: 'file_path' que sera la ruta del archivo
 * 
 * Valor de regreso: bool retorna el orden a llevar a cabo
 *
*********************************************************************************/
    bool operator<(const Word& other) const{
        if (fragment.thread_number != other.fragment.thread_number)
            return fragment.thread_number < other.fragment.thread_number;

        else if (line != other.line)
            return line < other.line;
        else
            return pos < other.pos;
                 
    }

/*********************************************************************************
 * 
 * Nombre de la función: Word
 *
 *
 * Descripción de la función: Constructor de la clase
 * 
 *
*********************************************************************************/
    Word(int thread_number, int line_i, int line_f, int line, std::string previous_word, std::string later_word, int pos)
    {
        this->fragment.thread_number = thread_number;
        this->fragment.line_i = line_i;
        this->fragment.line_f = line_f;
        this->line = line;
        this->previous_word = previous_word;
        this->later_word = later_word;
        this->pos = pos;
    }
    
/*********************************************************************************
 * 
 * Nombre de la función: operator<<
 *
 *
 * Descripción de la función: Permite ver por pantalla al usuario la el hilo con el
 *  inicio y final de sus lineas la palabra anterior y posterior
 * 
 * Argumentos utilizados: 'file_path' que sera la ruta del archivo
 * 
 * Valor de regreso: bool retorna el orden a llevar a cabo
 *
*********************************************************************************/
    friend std::ostream& operator<<(std::ostream& os, const Word& p) {
        os << "[Hilo: " << p.fragment.thread_number << " inicio: " << p.fragment.line_i << " final: " << p.fragment.line_f <<
         "] :: línea " << p.line << " :: Anterior: " << p.previous_word << " :: Posterior: " << p.later_word << "\n";
        return os;
    }

};

#endif
