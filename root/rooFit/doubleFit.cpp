/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o doubleFit doubleFit.cpp
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
#include "RooFFTConvPdf.h"
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif 

using namespace RooFit ;
using namespace std ;


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

  // variables definition
  RooRealVar x ("x", "x", -10, 10) ; 

  // first gaussian definition
  RooRealVar mean_1 ("mean_1", "mean of gaussian _1", 5., -10., 10.) ; 
  RooRealVar sigma_1 ("sigma_1", "width of gaussian _1", 0.7, 0., 5.) ; 
  RooGaussian gauss_1 ("gauss_1", "gaussian PDF", x, mean_1, sigma_1) ; 

  // first gaussian definition
  RooRealVar mean_2 ("mean_2", "mean of gaussian _2", 3., -10., 10.) ; 
  RooRealVar sigma_2 ("sigma_2", "width of gaussian _2", 0.5, 0., 5.) ; 
  RooGaussian gauss_2 ("gauss_2", "gaussian PDF", x, mean_2, sigma_2) ; 

  RooRealVar frac ("frac", "frac", 0.25, 0., 1.) ; 

  RooAddPdf tot ("tot","frac * gauss_1 + gauss_2",  
                 RooArgList (gauss_1,gauss_2) , 
                 RooArgList (frac) ,
                 kTRUE) ;

  RooDataSet * data = tot.generate (RooArgSet (x), 10000) ;

  // plot the distributions
  RooPlot *xplot = x.frame () ;
  data->plotOn (xplot) ;
  gauss_1.plotOn (xplot, LineColor (kRed)) ;
  gauss_2.plotOn (xplot, LineColor (kBlue)) ;
  tot.plotOn (xplot, LineColor (kGreen + 2)) ;
  TCanvas c1 ;
  xplot->Draw () ;
  c1.Print ("doubleFit_distr.gif","gif") ;
  
  // first gaussian definition
  RooRealVar mean_gen ("mean_gen", "mean of gaussian _gen", 3., -10., 10.) ; 
  RooRealVar sigma_gen ("sigma_gen", "width of gaussian _gen", 0.5, 0., 5.) ; 
  RooGaussian gauss_gen ("gauss_gen", "gaussian PDF", x, mean_gen, sigma_gen) ; 

  gauss_gen.fitTo (*data, RooFit::Minos (1)) ;

  x.setRange ("left", mean_gen.getVal () - 2 * sigma_gen.getVal (), mean_gen.getVal ()) ;

  RooRealVar mean_3 ("mean_3", "mean of gaussian _3", 3., -10., 10.) ; 
  RooRealVar sigma_3 ("sigma_3", "width of gaussian _3", 0.5, 0., 5.) ; 
  RooGaussian gauss_3 ("gauss_3", "gaussian PDF", x, mean_3, sigma_3) ; 
  gauss_3.fitTo (*data, RooFit::Minos (1), Range ("left")) ;

  x.setRange ("right", mean_gen.getVal () + 0.5 * sigma_gen.getVal (), mean_gen.getVal () + 3 * sigma_gen.getVal ()) ;

  RooRealVar mean_4 ("mean_4", "mean of gaussian _4", 3., -10., 10.) ; 
  RooRealVar sigma_4 ("sigma_4", "width of gaussian _4", 0.5, 0., 5.) ; 
  RooGaussian gauss_4 ("gauss_4", "gaussian PDF", x, mean_4, sigma_4) ; 
  gauss_4.fitTo (*data, RooFit::Minos (1), Range ("right")) ;

  RooPlot *xplot_1 = x.frame () ;
  TLegend leg_bkg (0.5, 0.6, 0.9, 0.9, NULL, "brNDC") ;
  leg_bkg.SetBorderSize (0) ;
  leg_bkg.SetTextFont (42) ;
  leg_bkg.SetTextSize (0.04) ;
  leg_bkg.SetLineColor (1) ;
  leg_bkg.SetLineStyle (1) ;
  leg_bkg.SetLineWidth (1) ;
  leg_bkg.SetFillColor (0) ;
  leg_bkg.SetFillStyle (0) ;

  data->plotOn (xplot_1) ;
  leg_bkg.AddEntry (xplot_1->FindObject (xplot_1->nameOf (xplot_1->numItems () - 1)), "data", "lp") ;
  gauss_gen.plotOn (xplot_1, LineColor (kGreen + 2)) ;
  leg_bkg.AddEntry (xplot_1->FindObject (xplot_1->nameOf (xplot_1->numItems () - 1)), "gaussGen", "lp") ;
  gauss_3.plotOn (xplot_1, LineColor (kGreen)) ;
  leg_bkg.AddEntry (xplot_1->FindObject (xplot_1->nameOf (xplot_1->numItems () - 1)), "gauss 3", "lp") ;
  gauss_4.plotOn (xplot_1, LineColor (kBlue)) ;
  leg_bkg.AddEntry (xplot_1->FindObject (xplot_1->nameOf (xplot_1->numItems () - 1)), "gauss 4", "lp") ;
  xplot_1->Draw () ;
//  leg_bkg.Draw () ;
 
  for (int i = 0 ; i < xplot_1->numItems () ; ++i) 
    {
      TString obj_name = xplot_1->nameOf (i) ; 
//      if (obj_name=="") continue ;
      cout << Form ("%d. '%s'\n",i,obj_name.Data ()) ;
    }
  
  c1.Print ("doubleFit_fit.gif","gif") ;

  return 0 ;
}
