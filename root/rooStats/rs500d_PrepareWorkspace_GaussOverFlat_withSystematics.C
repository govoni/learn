//////////////////////////////////////////////////////////////////////////
//
// RooStats tutorial macro #500d
// 2009/08 - Nils Ruthmann, Gregory Schott
//
// Prepare a workspace (stored in a ROOT file) containing a models,
// data and other objects needed to run statistical classes in
// RooStats.
//
// In this macro a PDF model is built assuming signal has a Gaussian
// PDF and the background a flat PDF.  The parameter of interest is
// the signal yield and we assume for it a flat prior.  It is shown
// how two types of systematics uncertainties can be expressed ; those
// are a sytematic uncertainty on the background yield and another on
// one of the parameters (sigma) of the signal shape.  All needed
// objects are stored in a ROOT file (within a RooWorkspace
// container) ; this ROOT file can then be fed as input to various
// statistical methods.
//
// root -q -x -l 'rs500d_PrepareWorkspace_GaussOverFlat_withSystematics.C ()'
//
//////////////////////////////////////////////////////////////////////////

#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooDataHist.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooWorkspace.h"

#include "TFile.h"

using namespace RooFit ;


// Three types of toy-MC data generation is made available as option passed to this macro ; 
// generation is made from the S+B model:
//  type = 0 : unbinned data with total number of events fluctuating using Poisson probability
//  type = 1 : binned data with total number of events fluctuating using Poisson probability
//  type = 2 : binned data without any bin-by-bin fluctuation (expected data)

void rs500d_PrepareWorkspace_GaussOverFlat_withSystematics ( 
    TString fileName = "WS_GaussOverFlat_withSystematics.root", 
    int type = 0 )
{

  // Use a RooWorkspace to store the PDF models, prior informations, list of parameters, ...
  RooWorkspace myWS ("myWS") ;

  // Number of signal and background events
  myWS.factory ("S[10,0,50]") ; // default value 10 and range [0,50]
  myWS.factory ("B[100,0,200]") ; // value fixed to 100 and range [0,200]

  // Observable
  myWS.factory ("mass[0,500]") ; // range [0,500]

  // Signal and background distribution of the observable
  myWS.factory ("Gaussian::sigPdf (mass,250,sigSigma[15,0,30])") ;
  myWS.factory ("Uniform::bkgPdf (mass)") ;

  // S+B and B-only models (both extended PDFs)
  myWS.factory ("SUM::model (S*sigPdf,B*bkgPdf") ;
  myWS.factory ("ExtendPdf::modelBkg (bkgPdf,B)") ;

  // Prior on the parameter of interest
  myWS.factory ("Uniform::priorPOI (S)") ;

  // Other prior PDFs
  myWS.factory ("Gaussian::prior_sigSigma (sigSigma,15,5)") ;
  myWS.factory ("Gaussian::prior_B (B,100,50)") ;
  myWS.factory ("PROD::priorNuisance (prior_sigSigma,prior_B)") ;

  // Define list of observables, parameters of interest, ...
  myWS.defineSet ("observables","mass") ;
  myWS.defineSet ("nuisanceParameters","B,sigSigma") ;
  myWS.defineSet ("POI","S") ;

  // Set random seed to make the generation reproducible
  (RooRandom::randomGenerator ())->SetSeed (4357) ;

  // Generate one of the dataset type
  RooAbsData* data = 0 ;
  // type = 0 : unbinned data with total number of events fluctuating using Poisson probability
  if (type==0) data = (RooAbsData*) myWS.pdf ("model")->generate (
     *myWS.set ("observables"), 
     Extended (), 
     Name ("data")) ;
  // type = 1 : binned data with total number of events fluctuating using Poisson probability
  if (type==1) data = myWS.pdf ("model")->generateBinned (*myWS.set ("observables"),Extended (),Name ("data")) ;
  // type = 2 : binned data without any bin-by-bin fluctuation (expected data)
  if (type==2) data = myWS.pdf ("model")->generateBinned (*myWS.set ("observables"),Name ("data"),ExpectedData ()) ;
  myWS.import (*data) ;
  
  myWS.writeToFile (fileName) ;  
  std::cout << "\nRooFit model initialized and stored in " << fileName << std::endl ;

  // control plot of the generated data
  RooPlot* plot = myWS.var ("mass")->frame (25) ;
  data->plotOn (plot) ;
  myWS.pdf ("model")->plotOn (plot,Components ("bkgPdf"),LineStyle (kDotted)) ;
  myWS.pdf ("model")->plotOn (plot) ;
  plot->DrawClone () ;

  myWS.Print ("v") ;

}
