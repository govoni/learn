/*COMPILO 
c++ -o generaSegnale -lm `root-config --cflags --libs --glibs` generaSegnale.cpp 
*/

#include <iostream>
#include <ctime>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TRandom.h"


int main (int argc, char** argv)
{
  gRandom->SetSeed (time (0)) ;

   typedef struct {
      Float_t x,y ;
   } VARS;

  static VARS vars ;
  TTree al ("sample","signal test sample") ;
  al.Branch ("vars",&vars,"x/F:y/f") ;

  for (int i = 0 ; i < 10000 ; ++i)
    {
      vars.x = gRandom->Gaus (5,0.5) ;
      vars.y = gRandom->Gaus (4,0.8) ;
      al.Fill () ;
    }
  TFile out (argv[1],"recreate") ;
  al.Write () ;
  out.Close () ;
  
  return 0 ;

}

