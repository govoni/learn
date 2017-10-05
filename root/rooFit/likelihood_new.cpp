/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o likelihood_new likelihood_new.cpp
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

  TH2F * dataH2 = data->createHistogram (*w.var ("x"), *w.var ("y")) ;
  dataH2->SetStats (0) ;
  dataH2->SetMarkerStyle (4) ;
  dataH2->Draw ("P") ;
  c1.Print ("dataPlot.pdf","pdf") ;

// build bkg pdf from data and signal one from MC  ---- ---- ---- ---- ---- ----

  // signal pdfs
  w.factory ("Gaussian::GxSig(x, meanSigx[5., 0., 30.], sigmaSigx[ 2., 0., 8.])") ;
  w.var ("meanSigx")->setConstant (kTRUE) ;
  w.var ("sigmaSigx")->setConstant (kTRUE) ;

  w.factory ("Gaussian::GySig(y, meanSigy[2.8, 0., 3.14], sigmaSigy[0.3, 0., 3.14])") ;
  w.var ("meanSigy")->setConstant (kTRUE) ;
  w.var ("sigmaSigy")->setConstant (kTRUE) ;

  // bkg pdfs
  RooDataSet * data_controly = dynamic_cast<RooDataSet*> (
    data->reduce (RooArgSet (*w.var ("x")), "y>0.5 && y<1.")) ; //PG controly
  RooKeysPdf bkg_pdf_x ("bkg_pdf_x","bkg_pdf_x", *w.var ("x"), *data_controly, RooKeysPdf::MirrorBoth) ;  

  RooDataSet * data_controlx = dynamic_cast<RooDataSet*> (
    data->reduce (RooArgSet (*w.var ("y")), "x>25. && x<30.")) ; //PG controlx
  RooKeysPdf bkg_pdf_y ("bkg_pdf_y","bkg_pdf_y", *w.var ("y"), *data_controlx, RooKeysPdf::MirrorBoth) ;  

// SOME PLOTS   ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooPlot * frame = 0 ;
  
  frame = w.var ("x")->frame () ;                                   
  data->plotOn (frame, MarkerColor (kRed), CutRange ("signaly")) ;  
  w.pdf ("GxSig")->plotOn (frame, LineColor (kRed)) ;               
  data->plotOn (frame, MarkerColor (kBlue), CutRange ("controly")) ;
  bkg_pdf_x.plotOn (frame, LineColor (kBlue)) ;
  frame->Draw () ;                                                  
  c1.Print ("xProjection.pdf","pdf") ;                                
  delete frame ;                                                    

  frame = w.var ("y")->frame () ;                                   
  data->plotOn (frame, MarkerColor (kRed+2), CutRange ("signalx")) ;  
  w.pdf ("GySig")->plotOn (frame, LineColor (kRed+2)) ;               
  data->plotOn (frame, MarkerColor (kBlue+2), CutRange ("controlx")) ;
  bkg_pdf_y.plotOn (frame, LineColor (kBlue+2)) ;
  frame->Draw () ;                                                  
  c1.Print ("yProjection.pdf","pdf") ;                                
  delete frame ;                                                    

// build log likelihood ratio and add it to the data    ---- ---- ---- ---- ----

  RooFormulaVar llratio ("llratio", "log(@0)+log(@1)-log(@2)-log(@3)", 
          RooArgList (*w.pdf ("GxSig"), *w.pdf ("GySig"), 
                      bkg_pdf_x, bkg_pdf_y)) ;

  // Calculate likelihood ratio for each event, define subset of events with high signal likelihood
  RooRealVar * stupid_dummy = (RooRealVar*) data->addColumn (llratio) ;
  
// SOME PLOTS   ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  double llratiomin = 0. ;
  double llratiomax = 0. ;
  data->getRange (*stupid_dummy, llratiomin, llratiomax) ;

  frame = stupid_dummy->frame (llratiomin, llratiomax) ;             
  data->plotOn (frame, MarkerColor (kRed), CutRange ("signaly")) ;  
  data->plotOn (frame, MarkerColor (kBlue), CutRange ("controly")) ;
  data->plotOn (frame, MarkerColor (kRed+2), CutRange ("signalx")) ;  
  data->plotOn (frame, MarkerColor (kBlue+2), CutRange ("controlx")) ;
  frame->Draw () ;                                                  
  c1.Print ("llrPlots.pdf","pdf") ;                                   
  delete frame ;                                                    

  TH2F * dataH2_cut = data->createHistogram (*w.var ("x"), *w.var ("y"),"llratio>0.") ;
  dataH2_cut->SetStats (0) ;
  dataH2_cut->SetMarkerStyle (4) ;
  dataH2_cut->SetMarkerColor (kOrange) ;
  dataH2->Draw ("P") ;
  dataH2_cut->Draw ("Psame") ;
  c1.Print ("dataPlot_Sel.pdf","pdf") ;
  
  return 0 ;
}