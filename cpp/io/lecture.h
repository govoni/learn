#ifndef lecture.h
#define lecture.h

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
#include <fstream>
#include <vector>

class lecture
{
  public:
    lecture (string dati) : _dati (dati.c_str ()) {} ;
    ~lecture () {} ;  
// FIXME metti un &
    std::string & GetLine ()
      {
        string dataline ;
        do {
            getline (_dati, dataline,'\n') ;
          } while (*dataline.begin () == '#') ;
	    return dataline ;
      }

  private:
    ifstream _dati ;
} ;


template <typename T>
class leggoFile
{
  public:
    leggoFile (string dati, vector<T>* v_out):_dati (dati.c_str ()) 
      {
//        char* sbuffer ;
        string sbuffer ;
        T Tbuffer ;
        while (!_dati.eof ()) v_out->push_back (Tbuffer) ;
      } ;
    ~leggoFile () {} ;  

  private:
    ifstream _dati ;
} ;


template <typename T>
int leggiFile (string dati, vector<T>* v_out)
  {
    ifstream _dati (dati.c_str ()) ;
    T Tbuffer ;
    while (!_dati.eof ()) v_out->push_back (Tbuffer) ;
  }

// la funzione che segue non funziona.
/*

template <typename T>
int leggiRiga (ifstream& _dati, vector<T>* v_out)
  {
    T Tbuffer ;
    string line ;
    int i = 0 ;

    getline (_dati,line,'\n') ;
    for (i=1 ; line >> Tbuffer ; ++i)
      {
	v_out->push_back (Tbuffer) ;
      }
    return i ;
  }

*/


/*

// IN FUTURO, NEL TENTATIVO DI LEGGERE I COMMENTI

        while (!_dati.eof ())
	  { 
	    if (_dati >> Tbuffer) 
	      {
		 v_out->push_back (Tbuffer) ;
		 cout << "leggo" << endl ;
	      }
            else 
	      {
		 getline (_dati,sbuffer,'\n') ;
		 cout << "non leggo" << endl ;
	      }
	  }

*/

#endif