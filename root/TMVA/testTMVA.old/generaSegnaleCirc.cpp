/*COMPILO 
c++ -o generaSegnaleCirc -lm `root-config --cflags --libs --glibs` generaSegnaleCirc.cpp 
*/

#include <iostream>
#include <ctime>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TRandom.h"


int main (int argc, char** argv)
{
  gRandom->SetSeed (time (0)) ;

// http://root.cern.ch/root/html/TMath.html
  TF2 circ ("circ","TMath::Gaus (TMath::Sqrt(x*x+y*y),3,0.5)",0,10,0,10) ;

   typedef struct {
      Float_t x,y ;
   } VARS;

  static VARS vars ;
  TTree al ("sample","signal test sample") ;
  al.Branch ("vars",&vars,"x/F:y/f") ;

  for (int i = 0 ; i < 10000 ; ++i)
    {
      Double_t x,y ;
      circ.GetRandom2 (x,y) ;
      vars.x = x ;
      vars.y = y ;
      al.Fill () ;
    }
  TFile out (argv[1],"recreate") ;
  al.Write () ;
  out.Close () ;
  
  return 0 ;

}

