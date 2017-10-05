/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o testKeysFunc testKeysFunc.cpp
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

// build the dataset      ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * data = w.pdf ("model")->generate (
      RooArgSet (*w.var("x"), *w.var("y")),
      10000
    ) ;

  RooKeysPdf pdf_x ("pdf_x","pdf_x", *w.var ("x"), *data, RooKeysPdf::MirrorBoth) ;  
  TF1 * pdf_x_tf1 = pdf_x.asTF (RooArgList (*w.var ("x"))) ;

  RooPlot * frame = w.var ("x")->frame () ;                                   
  data->plotOn (frame, MarkerColor (kRed)) ;
  pdf_x.plotOn (frame, LineColor (kBlue)) ;
  TCanvas c1 ;
  frame->Draw () ;                                                  
  c1.Print ("pdfKeysTest.pdf","pdf") ;                                
  pdf_x_tf1->Draw () ;
//  pdf_x_tf1->DrawIntegral () ;
  c1.Print ("pdfKeysTestTF1.pdf","pdf") ;                                

  std::cout << "INTEGRAL : " << pdf_x_tf1->Integral (0., 30.) << "\n" ;
  //PG non viene normalizzata!
  return 0 ;
}