
/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o fitWithHistos fitWithHistos.cpp
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
#include "TString.h"
#include <fstream>
#include <vector>

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
using namespace std ;


int main (int argc, char ** argv)
{
//  gROOT->SetStyle ("Plain") ;	
//  gStyle->SetOptStat ("mr") ;
  gStyle->SetOptFit (1111) ;
//  gStyle->SetStatFont(42);
//  gStyle->SetStatFontSize(0.1);
//  gStyle->SetStatTextColor(1);
//  gStyle->SetStatFormat("6.4g");
//  gStyle->SetStatBorderSize(1);
//  gStyle->SetStatH(0.06);
//  gStyle->SetStatW(0.3);
//  gStyle->SetPalette(1);

  bool ALLPLOTS = false ;

  // the variable of interest
  RooRealVar x ("x", "x", -10, 10) ;   
  RooPlot * xplot = x.frame () ;
  TCanvas c1 ;
  
  // the sig model
  RooRealVar meanx ("meanx", "mean of gaussian x", 0., -10., 10.) ; 
  RooRealVar sigmax ("sigmax", "width of gaussian x", 2.5, 0., 5.) ; 
  RooGaussian gaussx ("gaussx", "gaussian PDF", x, meanx, sigmax) ; 

  // the background model
  RooRealVar N1 ("N1", "rel amplitude first exp",  0., 1.) ;
  RooRealVar L1 ("L1", "rel attentuation first exp",  0., 1.) ;
  RooGenericPdf bkg ("RFExp", "exponential", 
                     "@1 * exp (-1 * @2 * @0)",
                     RooArgSet (x, N1, L1)) ;
  
  // the sum of the two
  RooRealVar gfrac ("gfrac","fraction of gauss",0.1,0.,1.) ; 
  RooAddPdf model ("model","sig + bkg", RooArgList (gaussx, bkg), RooArgList (gfrac)) ;

  // the fitting function
  RooDataSet * sigDS = gaussx.generate (RooArgSet (x), 100000) ;
  RooDataHist sigDH ("sigDH", "sigDH", RooArgList (x), *sigDS) ;
  RooHistPdf sigPdf_forModeling ("sigPdf_forModeling", "sigPdf_forModeling", RooArgSet (x), sigDH) ;
  RooHistPdf sigPdf ("sigPdf", "sigPdf", RooArgSet (x), sigDH) ;

  sigPdf.plotOn (xplot, MarkerColor (kRed)) ;

  RooDataSet * bkgDS = bkg.generate (RooArgSet (x), 100000) ;
  RooDataHist bkgDH ("bkgDH", "bkgDH", RooArgList (x), *bkgDS) ;
  RooHistPdf bkgPdf_forModeling ("bkgPdf_forModeling", "bkgPdf_forModeling", RooArgSet (x), bkgDH) ;
  RooHistPdf bkgPdf ("bkgPdf", "bkgPdf", RooArgSet (x), bkgDH) ;

  bkgPdf.plotOn (xplot, LineColor (kRed)) ;

  RooRealVar coef_sig ("coef_sig", "coef_sig", 0.5, 0., 100.) ;   
  RooRealVar coef_bkg ("coef_bkg", "coef_bkg", 0.5, 0., 100.) ;   
//  RooAddPdf totalModel ("totalModel", "totalModel", sigPdf, bkgPdf, coef1) ;
  RooAddPdf fittingFunction ("fittingFunction", "fittingFunction", 
      RooArgList (sigPdf, bkgPdf),
      RooArgList (coef_sig, coef_bkg)
    ) ;  
  
  model.plotOn (xplot, LineColor (kBlack)) ;
  xplot->Draw () ;
  c1.Print ("fitWithHistos_models.png", "png") ;
  

  // generate N diffent toy experiments
  unsigned int Ntoys = 1000 ;
  unsigned int EventsPerToy = 50 ;
  TH1F fitResults ("fitResults", "fitResults", 100, -1., 1.) ;
  
  // loop over toy exp
  for (int i = 0 ; i < Ntoys ; ++i)
    {

      RooDataSet * data = model.generate (RooArgSet (x), EventsPerToy) ;
      fittingFunction.fitTo (*data) ;
//      fitResults.Fill (coef1.getVal ()) ;
      fitResults.Fill (coef_sig.getVal () / EventsPerToy) ;

// questo ritorna sempre il numero di entry dei RooDataHist di partenza, 
// anzi di uno solo (perche' immagino faccia frazioni)
// tuttavia, la somma dei coefficienti da' la normalizzazione totale dei dati,
// quindi (fors'anche a prescindere dal fatto che gli isto iniziali siano normalizzati)
// ciascuno e' la sezione d'urto del sample.
//      RooAbsReal* fracInt = fittingFunction.createIntegral (x) ;
//      cout << " INTEGRAL INTEGRAL INTEGRAL " << fracInt->getVal () << endl ;

      if (ALLPLOTS)
        {
          RooPlot * localxplot = x.frame () ;
          data->plotOn (localxplot, MarkerColor (kRed)) ;
          model.plotOn(localxplot, LineColor (kGray + 2)) ;
          fittingFunction.plotOn(localxplot, LineColor (kBlue + 2)) ;
          localxplot->Draw () ;
          TString fileName = "plot_" ;
          fileName += i ;
          fileName += ".png" ;
          c1.Print (fileName, "png") ;
        }
    }
  
  fitResults.Fit ("gaus") ;  
  fitResults.Draw () ;
  c1.Print ("fitWithHistos_result.png", "png") ;
  
  return 0 ;

}
