//////////////////////////////////////////////////////////////////////////
//
// RooStats tutorial macro #502
// 2009/08 - Nils Ruthmann, Gregory Schott
//
// Show how to run the RooStats classes to perform specific tasks. The
// ROOT file containing a workspace holding the models, data and other
// objects needed to run can be prepared with any of the rs500*.C
// tutorial macros.
//
// Compute with HybridCalculator the signal significance for the given
// data. Also quote the CL_s ratio.
//
//////////////////////////////////////////////////////////////////////////

#include "RooRandom.h"
#include "RooRealVar.h"
#include "RooProdPdf.h"
#include "RooWorkspace.h"
#include "RooStats/HybridCalculator.h"
#include "RooStats/HybridResult.h"
#include "RooStats/HybridPlot.h"

#include "TFile.h"
#include "TStopwatch.h"

using namespace RooFit ;
using namespace RooStats ;


void rs505_HybridCalculator_significance (const char* fileName="WS_GaussOverFlat_withSystematics.root",
                                          int ntoys=2000)
{
  // Run a TStopwatch to evaluate running time
  //TStopwatch t ;
  //t.Start () ;

  // Open the ROOT file and import from the workspace the objects needed for this tutorial
  TFile file (fileName) ;
  RooWorkspace* myWS = (RooWorkspace*) file.Get ("myWS") ;
  RooAbsPdf* model = myWS->pdf ("model") ;
  RooAbsData* data = myWS->data ("data") ;
  RooAbsPdf* priorNuisance = myWS->pdf ("priorNuisance") ;
  RooAbsPdf* modelBkg = myWS->pdf ("modelBkg") ;
  const RooArgSet* nuisanceParameters = myWS->set ("nuisanceParameters") ;
  

  HybridCalculator hc ("hc","HybridCalculator",*data,*model,*modelBkg) ;
  hc.SetNumberOfToys (ntoys) ;
  hc.SetTestStatistics (1) ;

  if ( priorNuisance!=0 ) {
    hc.UseNuisance (kTRUE) ;
    hc.SetNuisancePdf (*priorNuisance) ;
    cout << "The following nuisance parameters are taken into account:\n" ;
    nuisanceParameters->Print () ;
    hc.SetNuisanceParameters (*nuisanceParameters) ;
  } else {
    hc.UseNuisance (kFALSE) ;
  }
  
  HybridResult* hcResult = hc.GetHypoTest () ;
  double p_value_sb = hcResult->AlternatePValue () ;
  double p_value_b = hcResult->NullPValue () ;
  double cl_s = hcResult->CLs () ;
  double significance = hcResult->Significance () ;
  
  cout << "p-value S+B:" << p_value_sb << endl ;
  cout << "p-value  B :" << p_value_b << endl ;
  cout << "CL_s:" << cl_s << endl ;
  cout << "significance:" << significance << endl ;

  HybridPlot* hcPlot = hcResult->GetPlot ("hcPlot","p-Values plot",100) ;
  hcPlot->Draw () ;

  //t.Stop () ;
  //t.Print () ;

  // Closing the file would delete the workspace
  //file.Close () ;
}
