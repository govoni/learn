/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o test test.cpp
*/

#include "TProfile2D.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TLegend.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TNtuple.h"
#include <fstream>

#include "RooFormulaVar.h"
#include "RooKeysPdf.h"
#include "RooExponential.h"
#include "RooArgusBG.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooGenericPdf.h"
#include "RooFFTConvPdf.h"
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif 

using namespace RooFit ;


int main ()
{
  TH1F * h = new TH1F ("h", "h", 10, 0, 10) ;
  h->Fill (5) ;

  RooRealVar x ("x", "x", -10, 10) ;   
  RooDataHist rdh ("rdh", "rdh", RooArgList (x), h) ;
  
  RooPlot *xplot = x.frame () ;
  rdh.plotOn (xplot, MarkerColor (kRed)) ;
  TCanvas c1 ;
  xplot->Draw () ;
  c1.Print ("test.png", "png") ;
  return 0 ;

}