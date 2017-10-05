/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o createDataset createDataset.cpp
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
  w.factory ("PROD::bkg(Px,Ey)") ;
  w.factory ("SUM::model(sigf[0.01, 0., 1.] * Gxy,bkg)") ;

  TCanvas c1 ;
  TH2 * modelH2 = (TH2*) w.pdf ("model")->createHistogram ("x,y", 100, 100) ;
  modelH2->SetStats (0) ;
  modelH2->Draw ("surf") ;
  c1.Print ("model.pdf","pdf") ;

  w.var ("y")->setRange ("signaly", 2.6, 2.9) ;
  w.var ("y")->setRange ("controly", 0.5, 1.) ;
  w.var ("x")->setRange ("signalx", 0., 6.) ;
  w.var ("x")->setRange ("controlx", 25., 30.) ;

// build the dataset      ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * data = w.pdf ("model")->generate (
      RooArgSet (*w.var("x"), *w.var("y")),
      10000
    ) ;

  TNtuple saving ("dataNT","dataNT","x:y") ;
  for (int iEvent = 0 ; iEvent < data->numEntries () ; ++iEvent)
    {
      double x = data->get (iEvent)->getRealValue ("x") ;
      double y = data->get (iEvent)->getRealValue ("y") ;
      saving.Fill (x,y) ;
    }
  
  TFile f ("dataset.root", "recreate") ;
  saving.Write () ;
  data->Write () ;
  f.Close () ; 

  return 0 ;
}
