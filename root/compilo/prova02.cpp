#include <iostream>
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"

int main () 
{
  // incipit
  std::cout << "CIAO" << std::endl ;
  // creo isto
  TH1F fuffa ("FUFFA","FUFFA",10,0,10) ;
  // riempio isto
  double value = 0 ;
  while (value != -1)
    {
      std::cout << "inserisici un valore: " << std::endl ;
      std::cin >> value ;
      if (value != -1) fuffa.Fill (value) ;
    }
  // disegno su file gif
  TCanvas c1 ("c1","fuffa",20,10,200,300) ;
  fuffa.Draw () ;
  c1.Print ("fuffa.gif","gif") ;
  // salvo su file
  TFile file ("fuffa.root","RECREATE") ;
  fuffa.Write () ;
  file.Close () ;
  return 0 ;
}

/* COMPILO
c++ -o prova02.exe `root-config --cflags --libs --glibs` prova02.cpp       
*/