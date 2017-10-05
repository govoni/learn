/*COMPILO 
c++ -o generaFondo -lm `root-config --cflags --libs --glibs` generaFondo.cpp 
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

  TF1 pol2pie ("pol2pie", "-1*(x-5)*(x-5) + 70", 0., 10.) ;
  TF1 flatFunc ("flatFunc", "3", 0., 10.) ;

   typedef struct {
      Float_t x,y ;
   } VARS;

  static VARS vars ;
  TTree al ("sample","bkg test sample") ;
  al.Branch ("vars",&vars,"x/F:y/f") ;

  for (int i = 0 ; i < 10000 ; ++i)
    {
      vars.x = pol2pie.GetRandom () ;
      vars.y = flatFunc.GetRandom () ;
      al.Fill () ;
    }
  TFile out (argv[1],"recreate") ;
  al.Write () ;
  out.Close () ;
  
  return 0 ;

}

