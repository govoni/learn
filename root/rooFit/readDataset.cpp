/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o readDataset readDataset.cpp
*/

#include "TProfile2D.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include <fstream>

#include "RooFormulaVar.h"
#include "RooKeysPdf.h"
#include "RooExponential.h"
#include "RooArgusBG.h"
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
  gStyle->SetStatFont (42) ;
  gStyle->SetStatFontSize (0.1) ;
  gStyle->SetStatTextColor (1) ;
  gStyle->SetStatFormat ("6.4g") ;
  gStyle->SetStatBorderSize (1) ;
  gStyle->SetStatH (0.06) ;
  gStyle->SetStatW (0.3) ;
  gStyle->SetPalette (1) ;

  // Create variables to load from the ntuple.
  RooRealVar x ("x", "x", 0, 30) ; 
  RooRealVar y ("y", "y", 0, 3.14) ; 
  RooArgSet ntupleVarSet (x, y) ;
  TFile* file1 = new TFile ("dataset.root") ; 
//  TTree* data = (TTree*) file1->Get ("dataNT") ; 
//  RooDataSet * dataSet = new RooDataSet ("data", "data", data, ntupleVarSet) ;

  RooDataSet * dataSet = (RooDataSet *) file1->Get ("modelData") ;

  // plot the distributions
  RooPlot *xplot1 = x.frame () ;
  dataSet->plotOn (xplot1, MarkerColor (kRed)) ;
  TCanvas c1 ;
  xplot1->Draw () ;
  c1.Print ("plot1.pdf","pdf") ;
   
  TH2F * histo2D = (TH2F*) dataSet->createHistogram ("x,y", 100, 100) ;
  histo2D->SetStats (0) ;
  histo2D->Draw ("col") ;
  c1.Print ("plot2.pdf","pdf") ;
  

  return 0 ;
  
}