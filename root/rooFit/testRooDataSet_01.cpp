/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore  \
    -o testRooDataSet_01 testRooDataSet_01.cpp
*/

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>

#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooFFTConvPdf.h"
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif 

using namespace RooFit ;



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


// create pdfs for the three models
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  // variables definition
  RooRealVar x ("x", "x", -10, 10) ; 

  // first gaussian definition
  RooRealVar mean_1 ("mean_1", "mean of gaussian _1", -5., -10., 10.) ; 
  RooRealVar sigma_1 ("sigma_1", "width of gaussian _1", 2, 0., 5.) ; 
  RooGaussian gauss_1 ("gauss_1", "gaussian PDF", x, mean_1, sigma_1) ; 
  RooDataSet * data_1 = gauss_1.generate (RooArgSet (x), 10000) ;

  TH1F test ("test", "test", 100, -10., 10.) ;
  for (int iEvent = 0 ; iEvent < data_1->numEntries () ; ++iEvent)
    {
      test.Fill (data_1->get (iEvent)->getRealValue ("x")) ;
    }
  
  TCanvas c1 ;
  test.Draw () ;
  test.Fit ("gaus") ;
  c1.Print ("testRooDataSet_01.gif","gif") ;
  
  return 0 ;

}