/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore  -o readAndPlot_rf readAndPlot_rf.cpp
*/

#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include <fstream>

#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooGaussian.h"
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


// read the dataset  ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  // Create dataset variables
  RooRealVar x ("x", "x", -3, 3) ; 
  RooDataSet data ("data", "data", RooArgSet (x)) ;
  // Fill data 
  std::ifstream leggo (argv[1], std::ios::in) ;
  while (!leggo.eof ())
    {
      double var ;
      leggo >> var ;
      x = var ;
      data.add (RooArgSet (x)) ;
    }
  leggo.close () ;
  data.Print ("v") ;

// prepare functions ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  // Build two Gaussian PDFs
//  RooRealVar x ("x","x",-10,10) ; 
  RooRealVar mean1 ("mean1", "mean of gaussian 1",-1.,-2.,0.) ; 
  RooRealVar sigma1 ("sigma1", "width of gaussian 1", 0.5,0.,2.) ; 
  RooGaussian gauss1 ("gauss1", "gaussian PDF", x, mean1, sigma1) ; 
  RooRealVar mean2 ("mean2", "mean of gaussian 2",0.5,0.,2.) ; 
  RooRealVar sigma2 ("sigma2", "width of gaussian 2", 0.5,0.,2.) ; 
  RooGaussian gauss2 ("gauss2", "gaussian PDF", x, mean2, sigma2) ;

  // Add the components
  RooRealVar g1frac ("g1frac","fraction of gauss1",0.5,0.,1.) ; 
  RooAddPdf sum ("sum","g1+g2", RooArgList (gauss1,gauss2), RooArgList (g1frac)) ;

// fit data w/ function ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooFitResult * fitRes = sum.fitTo (data, Minos (1), Save (1)) ;
  if (fitRes != 0) fitRes->Print () ;
  else std::cerr << "NO FIT RESULTS AVAILABLE\n" ;

  // Make a plot of the data and PDF
  TCanvas c1 ;
  RooPlot * dtframe = x.frame (-3,3,100) ; 
  data.plotOn (dtframe) ; 
  sum.plotOn (dtframe) ; 
  sum.paramOn (dtframe) ;
  dtframe->Draw () ;	
  c1.Print ("rf.gif","gif") ;

// lo stesso con la factory  ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
  
  //PG roofit workspace
  RooWorkspace w ("w", kTRUE) ;
  w.import (data) ;

  //PG roofit gaussian added
  w.factory ("Gaussian::G1(x, meanA[-3,3], sigmaA[3, 0.1, 10])") ;
  //PG roofit gaussian added
  w.factory ("Gaussian::G2(x, meanB[-3,3], sigmaB[3, 0.1, 10])") ;
  // sum the two
  w.factory ("SUM::GG(g1fract[0., 1.] * G1, G2") ;
  
  fitRes = w.pdf ("GG")->fitTo (data, Minos (1), Save (1)) ;
  if (fitRes != 0) fitRes->Print () ;
  else std::cerr << "NO FIT RESULTS AVAILABLE\n" ;

  RooPlot * dtframe2 = w.var ("x")->frame (-3,3,100) ; 
  data.plotOn (dtframe) ; 
  w.pdf ("GG")->plotOn (dtframe, LineColor (2)) ; 
  w.pdf ("GG")->plotOn (dtframe, LineColor (52), LineStyle (kDashed), Components ("G1")) ; 
  w.pdf ("GG")->plotOn (dtframe, LineColor (55), LineStyle (kDashed), Components ("G2")) ; 
  w.pdf ("GG")->paramOn (dtframe) ;
  dtframe->Draw () ;	
  c1.Print ("rf2.gif","gif") ;
  
}