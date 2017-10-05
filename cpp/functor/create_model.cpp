#include "pulse.h"
#include <fstream>
#include <string>
//#include <stdio.h>

//#include <cassert>

// questo funziona con la pulse-shape

int main(int argc, char* argv[])
{

  // apertura file
  
  string testo;

  if (argc > 1) testo = argv[1];
  else testo = "prova_test.dat";
  
  // nota la conversione da string a char*
  ofstream test(testo.c_str());

//  assert(test != NULL);
  test.seekp(0,ios::beg); 
     	
  double Tmax = 5.;   // massimo della funzione
  double Amax = 10.;   // ampiezza massima segnale
  double samplings = 25;  // numero di sampling 
  
  test << samplings << "\n";
  test << Tmax << "\n";
  for (int i=0; i<samplings; i++)
    {
      test << Amax * NNpulse()(i - Tmax) << "\n";
    }
  
  test.close();

}
