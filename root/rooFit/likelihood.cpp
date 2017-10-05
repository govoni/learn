/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o likelihood likelihood.cpp
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
  w.factory ("SUM::model(sigf[0.1, 0., 1.] * Gxy,bkg)") ;
  RooPlot * xframe = w.var ("x")->frame () ;

  TCanvas c1 ;                                                        // plotting
  RooPlot * framex = w.var ("x")->frame () ;                          // plotting
  w.pdf ("Gx")->plotOn (framex, LineColor (kBlue)) ;                  // plotting
  w.pdf ("Px")->plotOn (framex, LineColor (kRed)) ;                   // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("xdistros.gif","gif") ;                                   // plotting
  delete framex ;                                                     // plotting
  RooPlot * framey = w.var ("y")->frame () ;                          // plotting
  w.pdf ("Gy")->plotOn (framey, LineColor (kBlue)) ;                  // plotting
  w.pdf ("Ey")->plotOn (framey, LineColor (kRed)) ;                   // plotting
  framey->Draw () ;                                                   // plotting
  c1.Print ("ydistros.gif","gif") ;                                   // plotting
  delete framey ;                                                     // plotting

  TH2 * modelH2 = (TH2*) w.pdf ("model")->createHistogram ("x,y", 100, 100) ;
  modelH2->Draw ("surf") ;
  c1.Print ("likelihoodPlot.gif","gif") ;

  w.var ("y")->setRange ("signal", 2.6, 2.9) ;
  w.var ("y")->setRange ("controly", 0.5, 1.) ;
  framex = w.var ("x")->frame () ;                                    // plotting
  w.pdf ("model")->plotOn (framex, LineColor (kRed + 2),              // plotting
     ProjectionRange ("signal")) ;                                    // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("signalRegion.gif","gif") ;                               // plotting
  delete framex ;                                                     // plotting

// build the datasets     ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * data = w.pdf ("model")->generate (
      RooArgSet (*w.var("x"), *w.var("y")),
      10000
    ) ;

  framex = w.var ("x")->frame () ;                                    // plotting
  data->plotOn (framex, MarkerColor (kRed), CutRange ("signal")) ;    // plotting
  data->plotOn (framex, MarkerColor (kBlue), CutRange ("controly")) ; // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("signalRegionWData.gif","gif") ;                          // plotting
  delete framex ;                                                     // plotting

// build bkg pdf from data and signal one from MC  ---- ---- ---- ---- ---- ----

  // signal pdfs
  w.factory ("Gaussian::GxSig(x, meanSigx[5., 0., 30.], sigmaSigx[ 2., 0., 8.])") ;
  w.var ("meanSigx")->setConstant (kTRUE) ;
  w.var ("sigmaSigx")->setConstant (kTRUE) ;

  framex = w.var ("x")->frame () ;                                    // plotting
  w.pdf ("GxSig")->plotOn (framex, LineColor (kOrange)) ;             // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("sigModel_x.gif","gif") ;                                 // plotting
  delete framex ;                                                     // plotting

  w.factory ("Gaussian::GySig(y, meanSigy[2.8, 0., 3.14], sigmaSigy[0.3, 0., 3.14])") ;
  w.var ("meanSigy")->setConstant (kTRUE) ;
  w.var ("sigmaSigy")->setConstant (kTRUE) ;

  framex = w.var ("y")->frame () ;                                    // plotting
  w.pdf ("GySig")->plotOn (framex, LineColor (kOrange)) ;             // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("sigModel_y.gif","gif") ;                                 // plotting
  delete framex ;                                                     // plotting

  // bkg pdfs
  //   Unlike RooAbsArgs (RooAbsPdf,RooFormulaVar,....) datasets are not attached to 
  //   the variables they are constructed from. Instead they are attached to an internal 
  //   clone of the supplied set of arguments
  RooDataSet * data_controly = dynamic_cast<RooDataSet*> (
    data->reduce (RooArgSet (*w.var ("x")), "y>0.5 && y<1.")) ; //PG controly
  RooKeysPdf bkg_pdf_x ("bkg_pdf_x","bkg_pdf_x", *w.var ("x"), *data_controly, RooKeysPdf::MirrorBoth) ;  

  framex = w.var ("x")->frame () ;                                    // plotting
  data_controly->plotOn (framex) ;                                    // plotting
  bkg_pdf_x.plotOn (framex, LineColor (kOrange)) ;                    // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("bkgModel_x.gif","gif") ;                                 // plotting
  delete framex ;                                                     // plotting

  RooDataSet * data_controlx = dynamic_cast<RooDataSet*> (
    data->reduce (RooArgSet (*w.var ("y")), "x>25. && y<30.")) ; //PG controlx
  RooKeysPdf bkg_pdf_y ("bkg_pdf_y","bkg_pdf_y", *w.var ("y"), *data_controlx, RooKeysPdf::MirrorBoth) ;  

  framex = w.var ("y")->frame () ;                                    // plotting
  data_controlx->plotOn (framex) ;                                    // plotting
  bkg_pdf_y.plotOn (framex, LineColor (kOrange)) ;                    // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("bkgModel_y.gif","gif") ;                                 // plotting
  delete framex ;                                                     // plotting

// build log likelihood ratio and add it to the data    ---- ---- ---- ---- ----

  RooFormulaVar llratio ("llratio", "log(@0)+log(@1)-log(@2)-log(@3)", 
          RooArgList (*w.pdf ("GxSig"), *w.pdf ("GySig"), 
                      bkg_pdf_x, bkg_pdf_y)) ;

  // Calculate likelihood ratio for each event, define subset of events with high signal likelihood
  RooRealVar * stupid_dummy = (RooRealVar*) data->addColumn (llratio) ;
  
  double xmin = 0. ; double xmax = 0. ;
  data->getRange (*w.var ("x"), xmin, xmax) ;
  double ymin = 0. ;
  double ymax = 0. ;
  data->getRange (*w.var ("y"), ymin, ymax) ;
  double llratiomin = 0. ;
  double llratiomax = 0. ;
  data->getRange (*stupid_dummy, llratiomin, llratiomax) ;

  std::cout << "ranges | " 
            << "(" << xmin << "," << xmax << "), "
            << "(" << ymin << "," << ymax << "), "
            << "(" << llratiomin << "," << llratiomax << "), " 
            << "\n" ;

  framex = stupid_dummy->frame (llratiomin, llratiomax) ;             // plotting
  data->plotOn (framex, MarkerColor (kRed), CutRange ("signal")) ;    // plotting
  data->plotOn (framex, MarkerColor (kBlue), CutRange ("controlx")) ; // plotting
  framex->Draw () ;                                                   // plotting
  c1.Print ("llrPlot.gif","gif") ;                                    // plotting
  delete framex ;                                                     // plotting

  TTree * al = (TTree*) data->tree ()->Clone ("al") ;
  TH3F llr_3D ("llr_3D","llr_3D", 100, xmin, xmax, 100, ymin, ymax, 100, llratiomin, llratiomax) ;
  llr_3D.SetStats (0) ;
  al->Draw ("x:y:llratio>>llr_3D") ;
  llr_3D.Draw () ;
  c1.Print ("llr3D.gif","gif") ;
  
  TH2F llr_2Dx ("llr_2Dx","llr_2Dx", 100, xmin, xmax, 100, llratiomin, llratiomax) ;
  llr_2Dx.SetStats (0) ;
  al->Draw ("x:llratio>>llr_2Dx") ;
  llr_2Dx.Draw ("COLZ") ;
  c1.Print ("llr2Dx.gif","gif") ;

  TH2F llr_2Dy ("llr_2Dy","llr_2Dy", 100, ymin, ymax, 100, llratiomin, llratiomax) ;
  llr_2Dy.SetStats (0) ;
  al->Draw ("y:llratio>>llr_2Dy") ;
  llr_2Dy.Draw ("COLZ") ;
  c1.Print ("llr2Dy.gif","gif") ;

  TProfile2D * pippo = llr_3D.Project3DProfile () ;
  pippo->SetStats (0) ;
  pippo->SetMarkerStyle (8) ;
  pippo->Draw ("SURF") ;
  c1.Print ("llr3D_2.gif","gif") ;

/*
COME SI FA A PLOTTARE LA VARIABILE LLRATIO VS (X,Y) CON STI RooDataSet DI MERDA???
*/



//  TH2 * llratioH2 = (TH2*) //w.pdf ("model")->createHistogram ("x,y", 100, 100) ;
  //createHistogram (*w.var ("x"), *w.var ("y")) ;
//  llratioH2->Draw ("surf") ;
//  c1.Print ("LLRPlot.gif","gif") ;


//  RooDataSet * data_signal = dynamic_cast<RooDataSet*> (
//    data->reduce (RooArgSet (*w.var ("x")), "y>2.6 && y<2.9")) ;
//  RooRealVar * stupid_dummy = (RooRealVar*) data_controly->addColumn (llratio) ;
//  data_signal->addColumn (llratio) ;
//  data_signal->Print () ;
//
//  framex = stupid_dummy->frame (-80.,10.) ;                           // plotting
//  data_signal->plotOn (framex, MarkerColor (kBlue)) ;                 // plotting
//  data_controly->plotOn (framex, MarkerColor (kRed)) ;                // plotting
//  framex->Draw () ;                                                   // plotting
//  c1.Print ("llrPlots.gif","gif") ;                                   // plotting
//  delete framex ;                                                     // plotting
  
  // applico lo stimatore

}