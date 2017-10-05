/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore  -o modelBuilding modelBuilding.cpp
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


// create pdfs  ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  // variables definition
  RooRealVar x ("x", "x", -10, 10) ; 
  RooRealVar y ("y", "y", -10, 10) ; 
  
  // 2D gaussian definition
  RooRealVar meanx ("meanx", "mean of gaussian x", 5., -10., 10.) ; 
  RooRealVar sigmax ("sigmax", "width of gaussian x", 2, 0., 5.) ; 
  RooGaussian gaussx ("gaussx", "gaussian PDF", x, meanx, sigmax) ; 

  RooRealVar meany ("meany", "mean of gaussian y", 5., -10., 10.) ; 
  RooRealVar sigmay ("sigmay", "width of gaussian y", 2, 0., 5.) ; 
  RooGaussian gaussy ("gaussy", "gaussian PDF", y, meany, sigmay) ; 

  RooProdPdf gaussxy ("gaussxy","gaussx*gaussy",RooArgList (gaussx, gaussy)) ;

  // Define shape parameters  f (x) = 1 + c1 x + c2 x^2
  RooRealVar poly_c1x ("poly_c1x","coeff of 1 order", 2., 1., 5.) ;
  RooRealVar poly_c2x ("poly_c2x","coeff of 2 order", 1., 0.5, 2.5) ;
  
  // Define cubic polynomial PDF
  RooPolynomial polix ("polix", "quadratic polynomial in x", x, RooArgList (poly_c1x, poly_c2x)) ;
  RooPolynomial poliy ("poliy", "flat line in y", y) ;
  
  RooProdPdf bkg ("bkg","polix*poliy",RooArgList (polix, poliy)) ;

  RooRealVar gfrac ("gfrac","fraction of gauss",0.5,0.,1.) ; 
  RooAddPdf model ("model","gaussxy + bkg", RooArgList (gaussxy,bkg), RooArgList (gfrac)) ;
  
  // representations  
  model.graphVizTree ("modelScheme.dot") ;

  TCanvas c1 ;
  RooPlot * frame = x.frame () ; 
  model.plotOn (frame, LineColor (kRed + 2)) ;
  poly_c2x = 0. ;
  poly_c1x = 0.5 ;
  model.plotOn (frame, LineColor (kBlue + 2)) ;
  frame->Draw () ;	
  c1.Print ("modelPlot.gif","gif") ;

}