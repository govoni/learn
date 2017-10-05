/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o createSamples createSamples.cpp
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


// create pdfs  ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooWorkspace w ("w", kTRUE) ;

  w.factory ("Gaussian::Gx(x[0.,30.], meanx[5., 0., 30.], sigmax[ 2., 0., 8.])") ;
  w.factory ("Gaussian::Gy(y[0., 3.14], meany[2.8, 0., 3.14], sigmay[0.3, 0., 3.14])") ;
  w.factory ("PROD::Gxy(Gx, Gy)") ;

  w.factory ("Polynomial::Px(x, {p0[1], p1[-0.1], p2[0.1]})") ;
  w.factory ("Exponential::Ey(y, fact[-1.])") ;
  w.factory ("PROD::bkg_1(Px,Ey)") ;

  w.factory ("Polynomial::Pf(x)") ;
  w.factory ("Polynomial::Py(y, {py0[1.7], py1[1.0]})") ;
  w.factory ("PROD::bkg_2(Pf,Py)") ;

// build the dataset      ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * signal = w.pdf ("Gxy")->generate (
      RooArgSet (*w.var("x"), *w.var("y")),
      10000
    ) ;

  RooDataSet * bkg_1 = w.pdf ("bkg_1")->generate (
      RooArgSet (*w.var("x"), *w.var("y")),
      10000
    ) ;

  RooDataSet * bkg_2 = w.pdf ("bkg_2")->generate (
      RooArgSet (*w.var("x"), *w.var("y")),
      10000
    ) ;

  TNtuple signalNT ("signalNT","signalNT","x:y") ;
  TNtuple bkg_1NT ("bkg_1NT","bkg_1NT","x:y") ;
  TNtuple bkg_2NT ("bkg_2NT","bkg_2NT","x:y") ;
  
  for (int i = 0 ; i < 10000 ; ++i)
    {
      double x = signal->get (i)->getRealValue ("x") ;
      double y = signal->get (i)->getRealValue ("y") ;
      signalNT.Fill (x,y) ;
    
      x = bkg_1->get (i)->getRealValue ("x") ;
      y = bkg_1->get (i)->getRealValue ("y") ;
      bkg_1NT.Fill (x,y) ;    
    
      x = bkg_2->get (i)->getRealValue ("x") ;
      y = bkg_2->get (i)->getRealValue ("y") ;
      bkg_2NT.Fill (x,y) ;    
    }
  
  TFile f ("samples.root","recreate") ;
  signalNT.Write () ;
  bkg_1NT.Write () ;
  bkg_2NT.Write () ;
  f.Close () ;
  return 0 ;
}