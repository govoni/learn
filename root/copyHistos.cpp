#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include <iostream>

int main ()
{
  TDirectory * initial = gDirectory ;

  TDirectory * dire = new TDirectory ("prova","prova") ;
  dire->cd () ;
  TH1F histo ("histo","histo",10,0,0) ;
  histo.Fill (5) ;
  std::cout << histo.GetEntries () << std::endl ;
  initial->cd () ;
  TH1F * histo2 = (TH1F*) dire->Get ("histo") ;
  delete dire ;
  std::cout << histo2->GetEntries () << std::endl ;
}



/*

 c++ -o copyHistos `root-config --glibs --libs --cflags` copyHistos.cpp 

*/