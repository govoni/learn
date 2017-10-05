/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore  -o signalBkg signalBkg.cpp
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
  TH2 * modelH2 = (TH2*) model.createHistogram ("x,y", 100, 100) ;
  TCanvas c1 ;
  modelH2->Draw ("surf") ;
  c1.Print ("modelPlot.gif","gif") ;

// create a dataset w/ 2 deg of freedom  ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * data = model.generate (RooArgSet (x,y), 10000) ;

// plot the dataset  ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  y.setRange ("signal", 2., 8.) ;
  y.setRange ("control", -8., -2.) ;

  RooPlot * frame = x.frame () ; 
  data->plotOn (frame) ;
  data->plotOn (frame, MarkerColor (kRed), CutRange ("signal")) ;
  data->plotOn (frame, MarkerColor (kBlue), CutRange ("control")) ;
  model.plotOn (frame, LineColor (kRed + 2),  ProjectionRange ("signal")) ;
  model.plotOn (frame, LineColor (kBlue + 2), ProjectionRange ("control")) ;
  frame->Draw () ;	
  c1.Print ("dataPlot.gif","gif") ;

// fit the dataset   ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


//  RooDataSet * data_control = dynamic_cast<RooDataSet*> (data->reduce (SelectVars (RooArgSet (x)), CutRange ("control"))) ;
  RooAbsData * data_control = data->reduce (RooArgSet (x), "y<-2") ;
  // fit the background area
  polix.fitTo (*data_control, Minos (1), Save (1)) ;
  
  poly_c1x.setConstant (kTRUE) ;
  poly_c2x.setConstant (kTRUE) ;
  // fit the signal w/ fixed background
//  RooAbsData * data_signal = data->reduce (SelectVars (RooArgSet (x)), CutRange ("signal")) ;
  RooAbsData * data_signal = data->reduce (SelectVars (RooArgSet (x)), "y>2") ;
  
  RooRealVar gfrac1D ("gfrac1D","fraction of gauss",0.5,0.,1.) ; 
  RooAddPdf model1D ("model1D","gaussx + polix", RooArgList (gaussx,polix), RooArgList (gfrac1D)) ;
  RooFitResult * fitRes = model1D.fitTo (*data_signal, Minos (1), Save (1)) ;
  fitRes->Print () ;

  RooPlot * frame2 = x.frame () ; 
  data_control->plotOn (frame2, MarkerColor (kGray)) ;
  polix.plotOn (frame2, LineColor (kGray + 2)) ;
  data_signal->plotOn (frame2, MarkerColor (kOrange)) ;
  model1D.plotOn (frame2, LineColor (kOrange + 2)) ;
  frame2->Draw () ;
  c1.Print ("dataFit.gif","gif") ;


// read the dataset  ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


}