#ifndef readFile_h
#define readFile_h

/*
 per leggere un file e saltare le linee che iniziano per #:
 in output una stringa con la riga del file

 siccome l'ifstream va inizializzato nel costruttore di lecture non 
 so come utilizzare assert.
 percio' sara' necessario, in qualunque codice, fare un assert prima di lecture,
 nel caso si voglia controllare.
*/

//#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int extract (std::vector<int> * output , const std::string & dati) 
  {
    std::ifstream _dati (dati.c_str ()) ;
    // loop over the file
    while (!_dati.eof())
      {
        // get the line
        std::string dataline ;
        do { getline (_dati, dataline,'\n') ; } 
        while (*dataline.begin () == '#') ;
        std::stringstream linea (dataline) ;
        // loop over the line
        while (!linea.eof ())
          {
            int buffer = -1 ;
            linea >> buffer ;
            if (buffer != -1) output->push_back (buffer) ;
          } // loop over the line
      } // loop over the file     
    return output->size () ;
  }

#endif