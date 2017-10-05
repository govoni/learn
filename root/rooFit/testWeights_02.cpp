/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore  \
    -o testWeights_02 testWeights_02.cpp
*/

#include "TH1.h"
#include "TH2.h"
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
  RooRealVar mean_1 ("mean_1", "mean of gaussian _1", 5., -10., 10.) ; 
  RooRealVar sigma_1 ("sigma_1", "width of gaussian _1", 2, 0., 5.) ; 
  RooGaussian gauss_1 ("gauss_1", "gaussian PDF", x, mean_1, sigma_1) ; 

  // first gaussian definition
  RooRealVar mean_2 ("mean_2", "mean of gaussian _2", 0., -10., 10.) ; 
  RooRealVar sigma_2 ("sigma_2", "width of gaussian _2", 2, 0., 5.) ; 
  RooGaussian gauss_2 ("gauss_2", "gaussian PDF", x, mean_2, sigma_2) ; 

  // first gaussian definition
  RooRealVar mean_3 ("mean_3", "mean of gaussian _3", -5., -10., 10.) ; 
  RooRealVar sigma_3 ("sigma_3", "width of gaussian _3", 2, 0., 5.) ; 
  RooGaussian gauss_3 ("gauss_3", "gaussian PDF", x, mean_3, sigma_3) ; 

  // plot the distributions
  RooPlot *xplot = x.frame () ;
  gauss_1.plotOn (xplot, LineColor (kRed)) ;
  gauss_2.plotOn (xplot, LineColor (kBlue)) ;
  gauss_3.plotOn (xplot, LineColor (kGreen)) ;
  TCanvas c1 ;
  xplot->Draw () ;
  c1.Print ("WT_distros.gif","gif") ;
  
// generate the three datasets
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * data_1 = gauss_1.generate (RooArgSet (x), 10000) ;
  RooDataSet * data_2 = gauss_2.generate (RooArgSet (x), 10000) ;
  RooDataSet * data_3 = gauss_3.generate (RooArgSet (x), 10000) ;

  // plot the distributions
  RooPlot *xplot_1 = x.frame () ;
  data_1->plotOn (xplot_1, MarkerColor (kRed)) ;
  gauss_1.plotOn (xplot_1, LineColor (kRed)) ;
  data_2->plotOn (xplot_1, MarkerColor (kBlue)) ;
  gauss_2.plotOn (xplot_1, LineColor (kBlue)) ;
  data_3->plotOn (xplot_1, MarkerColor (kGreen)) ;
  gauss_3.plotOn (xplot_1, LineColor (kGreen)) ;
  xplot_1->Draw () ;
  c1.Print ("WT_distros_and_data.gif","gif") ;
  

// calculate the weights for each event and put it into the diagram
// start with the three cross-sections equal to each other
// for some reasons I have to do it all new cause roofit is crap
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  TF1 fg_1 ("fgaus_1","TMath::Gaus(x,[0],[1],[2])",-10.,10.) ; 
  fg_1.SetParameters (5., 2.) ;
  TF1 fg_2 ("fgaus_2","TMath::Gaus(x)",-10.,10.) ; 
  fg_2.SetParameters (0., 2.) ;
  TF1 fg_3 ("fgaus_3","TMath::Gaus(x)",-10.,10.) ; 
  fg_3.SetParameters (-5., 2.) ;

  c1.cd () ;
  fg_1.SetLineColor (kRed) ; fg_1.Draw () ;
  fg_2.SetLineColor (kBlue) ; fg_2.Draw ("same") ;
  fg_3.SetLineColor (kGreen) ; fg_3.Draw ("same") ;
  c1.Print ("WT_functions.gif","gif") ;

  return 0 ;

}