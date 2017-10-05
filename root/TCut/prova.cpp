/*
  c++ -o prova `root-config --glibs --cflags` prova.cpp
*/


#include "TCut.h"
#include <iostream>

int main ()
{
  TCut taglio = "m>5" ;
  taglio += "h<4" ;
  std::cout << taglio << std::endl ;
  std::cout << taglio.GetName () << std::endl ;
  std::cout << taglio.GetTitle () << std::endl ;

  TCut CutExtended = Form ("(%s) * autoWeight(numPUMC)", taglio.GetTitle ());    
  std::cout << CutExtended << std::endl ;

  return 0 ;
}
