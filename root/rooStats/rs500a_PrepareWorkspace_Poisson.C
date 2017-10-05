//////////////////////////////////////////////////////////////////////////
//
// RooStats tutorial macro #500a
// 2009/08 - Nils Ruthmann, Gregory Schott
//
// Prepare a workspace (stored in a ROOT file) containing a models,
// data and other objects needed to run statistical classes in
// RooStats.
//
// In this macro a PDF model is built for a counting analysis.  A
// certain number of events are observed (this can be enforced or left
// free) while a number of background events is expected.  In this
// macro, no systematic uncertainty is considered (see
// rs500b_PrepareWorkspace_Poisson_withSystematics.C).  The parameter
// of interest is the signal yield and we assume for it a flat prior.
// All needed objects are stored in a ROOT file (within a RooWorkspace
// container) ; this ROOT file can then be fed as input to various
// statistical methods.
//
// root -q -x -l 'rs500a_PrepareWorkspace_Poisson.C ()'
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
//  type = 0 : binned data with fixed number of events
//  type = 1 : binned data with total number of events fluctuating using Poisson probability
//  type = 2 : binned data without any bin-by-bin fluctuation (expected data)

void rs500a_PrepareWorkspace_Poisson ( TString fileName = "WS_Poisson.root", int type = 0 )
{

  // Use a RooWorkspace to store the PDF models, prior informations, list of parameters, ...
  RooWorkspace myWS ("myWS") ;

  // Number of signal and background events
  myWS.factory ("S[2,0,10]") ; // default value 2 and range [0,10]
  myWS.factory ("B[1]") ; // value fixed to 1

  // Observable (dummy observable used in case of Poisson likelihood function)
  myWS.factory ("x[0,1]") ; // arbitrary range [0,1] 
  myWS.var ("x")->setBins (1) ;

  // Signal and background distribution of the observable
  myWS.factory ("Uniform::sigPdf (x)") ;
  myWS.factory ("Uniform::bkgPdf (x)") ;

  // S+B and B-only models (both extended PDFs)
  myWS.factory ("SUM::model (S*sigPdf,B*bkgPdf") ;
  myWS.factory ("ExtendPdf::modelBkg (bkgPdf,B)") ;

  // Prior on the parameter of interest
  myWS.factory ("Uniform::priorPOI (S)") ;

  // Define list of observables and parameters of interest
  myWS.defineSet ("observables","x") ;
  myWS.defineSet ("POI","S") ;
  
  // Set random seed to make the generation reproducible
  (RooRandom::randomGenerator ())->SetSeed (4357) ;

  // Generate one of the dataset type
  RooAbsData* data = 0 ;
  // type = 0 : binned data with fixed number of events
  if (type==0) 
    data = myWS.pdf ("model")->generateBinned (
        *myWS.set ("observables"),                             // use only obs for generation
        myWS.var ("S")->getVal () + myWS.var ("B")->getVal (), // number of events, 
        Name ("data")                                          // name of the generation
    ) ;
  // type = 1 : binned data with total number of events fluctuating using Poisson probability
  if (type==1) 
    data = myWS.pdf ("model")->generateBinned (
      *myWS.set ("observables"), // same as before
      Extended (),               // added a poisson fluctuation bin by bin
      Name ("data")              // same as before
    ) ;
  // type = 2 : binned data without any bin-by-bin fluctuation (expected data)
  // I could use it 4 bkg eval from sidebands
  if (type==2) 
    data = myWS.pdf ("model")->generateBinned (
      *myWS.set ("observables"), // same as before
      Name ("data"),             // same as before
      ExpectedData ()            // add a global poisson fluctuation to the whole set of bins
    ) ;
  myWS.import (*data) ;
  
  myWS.writeToFile (fileName) ;  
  std::cout << "\nRooFit model initialized and stored in " << fileName << std::endl ;

  // control plot of the generated data
  RooPlot* plot = myWS.var ("x")->frame () ;
  data->plotOn (plot) ;
  myWS.pdf ("model")->plotOn (plot,Components ("bkgPdf"),LineStyle (kDotted)) ;
  myWS.pdf ("model")->plotOn (plot) ;
  plot->DrawClone () ;

  myWS.Print ("v") ;

}
