//////////////////////////////////////////////////////////////////////////
//
// RooStats tutorial macro #500c
// 2009/08 - Nils Ruthmann, Gregory Schott
//
// Prepare a workspace (stored in a ROOT file) containing a models,
// data and other objects needed to run statistical classes in
// RooStats.
//
// In this macro a PDF model is built assuming signal has a Gaussian
// PDF and the background a flat PDF.  The parameter of interest is
// the signal yield and we assume for it a flat prior.  In this macro,
// no systematic uncertainty is considered (see
// rs500d_PrepareWorkspace_GaussOverFlat_withSystematics.C).  All
// needed objects are stored in a ROOT file (within a RooWorkspace
// container); this ROOT file can then be fed as input to various
// statistical methods.
//
// root -q -x -l 'rs500c_PrepareWorkspace_GaussOverFlat.C()'
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

using namespace RooFit;


// Three types of toy-MC data generation is made available as option passed to this macro; 
// generation is made from the S+B model:
//  type = 0 : unbinned data with total number of events fluctuating using Poisson probability
//  type = 1 : binned data with total number of events fluctuating using Poisson probability
//  type = 2 : binned data without any bin-by-bin fluctuation (expected data)

void rs500c_PrepareWorkspace_GaussOverFlat( TString fileName = "WS_GaussOverFlat.root", int type = 0 )
{

  // Use a RooWorkspace to store the PDF models, prior informations, list of parameters, ...
  RooWorkspace myWS("myWS");

  // Number of signal and background events
  myWS.factory("S[10,0,50]"); // default value 10 and range [0,50]
  myWS.factory("B[100]"); // value fixed to 100

  // Observable
  myWS.factory("mass[0,500]"); // range [0,500]

  // Signal and background distribution of the observable
  myWS.factory("Gaussian::sigPdf(mass,250,15)");
  myWS.factory("Uniform::bkgPdf(mass)");

  // S+B and B-only models (both extended PDFs)
  myWS.factory("SUM::model(S*sigPdf,B*bkgPdf");
  myWS.factory("ExtendPdf::modelBkg(bkgPdf,B)");

  // Prior on the parameter of interest
  myWS.factory("Uniform::priorPOI(S)");

  // Define list of observables and parameters of interest
  myWS.defineSet("observables","mass");
  myWS.defineSet("POI","S");
  
  // Set random seed to make the generation reproducible
  (RooRandom::randomGenerator())->SetSeed(4357);

  // Generate one of the dataset type
  RooAbsData* data = 0;
  // type = 0 : unbinned data with total number of events fluctuating using Poisson probability
  if (type==0) data = (RooAbsData*) myWS.pdf("model")->generate(*myWS.set("observables"),Extended(),Name("data"));
  // type = 1 : binned data with total number of events fluctuating using Poisson probability
  if (type==1) data = myWS.pdf("model")->generateBinned(*myWS.set("observables"),Extended(),Name("data"));
  // type = 2 : binned data without any bin-by-bin fluctuation (expected data)
  if (type==2) data = myWS.pdf("model")->generateBinned(*myWS.set("observables"),Name("data"),ExpectedData());
  myWS.import(*data) ;
  
  myWS.writeToFile(fileName);  
  std::cout << "\nRooFit model initialized and stored in " << fileName << std::endl;

  // control plot of the generated data
  RooPlot* plot = myWS.var("mass")->frame(25);
  data->plotOn(plot);
  myWS.pdf("model")->plotOn(plot,Components("bkgPdf"),LineStyle(kDotted));
  myWS.pdf("model")->plotOn(plot);
  plot->DrawClone();

  myWS.Print("v");

}
