/*
c++ `root-config --cflags --glibs` -o readAndPlot readAndPlot.cpp
*/

#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include <fstream>


int main (int argc, char ** argv)
{
  gROOT->SetStyle ("Plain") ;	
  gStyle->SetOptStat ("mr") ;
  gStyle->SetOptFit (1111) ;
  gStyle->SetStatFont(42);
  gStyle->SetStatFontSize(0.1);
  gStyle->SetStatTextColor(1);
  gStyle->SetStatFormat("6.4g");
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatH(0.06);
  gStyle->SetStatW(0.3);



  TH1F histo ("histo","",100,-3,2) ;
  std::ifstream leggo (argv[1], std::ios::in) ;
  while (!leggo.eof ())
    {
      double var ;
      leggo >> var ;
      histo.Fill (var) ;
    }
  leggo.close () ;
  
  TCanvas c1 ;
  histo.SetFillColor (5) ;
  histo.Draw () ;
  c1.Print ("isto.gif","gif") ;

  TF1 gauss ("gauss","gaus",-3,1) ;
  gauss.SetParameter (1,histo.GetMean ()) ;
  gauss.SetParameter (2,histo.GetRMS ()) ;
  
  histo.Fit ("gauss","L") ;
  histo.Draw () ;
  c1.Print ("istoFit1.gif","gif") ;

  double mean = gauss.GetParameter (1) ;
  double sigma = gauss.GetParameter (2) ;

  histo.Fit ("gauss","L","",mean-sigma, mean+sigma) ;
  histo.Draw () ;
  c1.Print ("istoFit2.gif","gif") ;
  
  TFile saving ("plots.root","recreate") ;
  histo.Write () ;
  saving.Close () ;
	
}