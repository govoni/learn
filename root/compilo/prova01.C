#include "TH1F.h"

int prova01 () 
{
//  std::cout << "CIAO" << std::endl ;
  TH1F fuffa ("FUFFA","FUFFA",10,0,10) ;
  fuffa.Fill (5) ;
//  std::cout << "CIAO : " << fuffa.GetEntries ()
//            << std::endl ;
}
