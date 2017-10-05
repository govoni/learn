/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o testDoubleExp testDoubleExp.cpp
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
  gStyle->SetPalette(1);


  RooRealVar m ("m", "higgs mass", 0., 1000.) ;
  
  RooRealVar N1 ("N1", "rel amplitude first exp",  0., 1.) ;
  RooRealVar N2 ("N2", "rel amplitude second exp", 0., 1.) ;
  RooRealVar L1 ("L1", "rel attentuation first exp",  0., 1.) ;
  RooRealVar L2 ("L2", "rel attentuation first exp",  0., 1.) ;
  
  RooGenericPdf RFdoubleExp ("RFdoubleExp", "double exponential", 
  //                           "N1 * exp (-1 * L1 * x) + N2 * exp (-1 * L2 * x)",
                             "@1 * exp (-1 * @2 * @0) + @3 * exp (-1 * @4 * @0)",
                             RooArgSet (m, N1, L1, N2, L2)) ;
  
  N1.setVal (10.) ;
  L1.setVal (0.012) ;
  N2.setVal (5.) ;
  L2.setVal (0.005) ;
  
  
  
  // plot the distributions
  RooPlot *xplot = m.frame () ;
  RFdoubleExp.plotOn (xplot) ;
  TCanvas c1 ;
  xplot->Draw () ;
  c1.Print ("testDoubleExp.gif","gif") ;

  return 0 ;
}
