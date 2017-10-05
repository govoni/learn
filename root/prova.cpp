#include<iostream>

#include "TH1.h"

#include "CLHEP/Matrix/GenMatrix.h"
//#include "CLHEP/Matrix/Matrix.h"

int main ()
{
    std::cout << "Hello World\n" ;
    TH1F pippo ("pippo","pippo",10,0,10) ;
    pippo.Fill (4) ;
    std::cout << "entries: "
              << pippo.GetEntries () 
              << std::endl ;
}