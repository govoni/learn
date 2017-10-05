//////////////////////////////////////////////////////////////////////////
//
// RooStats tutorial macro #501
// 2009/08 - Kyle Cranmer
//
// Show how to run the RooStats classes to perform specific tasks. The
// ROOT file containing a workspace holding the models, data and other
// objects needed to run can be prepared with any of the rs500e.C
// tutorial macros.
//
// Compute with ProfileLikelihoodCalculator a 95% CL upper limit on
// the parameter of interest for the given data.
//
//////////////////////////////////////////////////////////////////////////

#include "RooRealVar.h"
#include "RooProdPdf.h"
#include "RooWorkspace.h"

#include "TH2F.h"
#include "RooDataHist.h"
#include "TFile.h"

#include "RooStats/ProfileLikelihoodCalculator.h"
#include "RooStats/FeldmanCousins.h"
#include "RooStats/MCMCCalculator.h"
#include "RooStats/MCMCInterval.h"
#include "RooStats/UniformProposal.h"
#include "RooStats/LikelihoodIntervalPlot.h"
#include "RooStats/MCMCIntervalPlot.h"
#include "RooDataHist.h"

using namespace RooFit;
using namespace RooStats;


void rs501_ThreeTypesOfLimits( const char* fileName="WS_GaussOverFlat_withSystematics_floatingMass.root" )
{
  
  // Open the ROOT file and import from the workspace the objects needed for this tutorial
  TFile* file = new TFile(fileName);
  RooWorkspace* myWS = (RooWorkspace*) file->Get("myWS");
  RooAbsPdf* modelTmp = myWS->pdf("model");
  RooAbsData* data = myWS->data("data");
  RooAbsPdf* priorNuisance = myWS->pdf("priorNuisance");
  const RooArgSet* POI = myWS->set("POI");
  RooRealVar* parameterOfInterest = dynamic_cast<RooRealVar*>(POI->first());
  assert(parameterOfInterest);

  // If there are nuisance parameters, multiply their prior distribution to the full model
  RooAbsPdf* model = modelTmp;
  if( priorNuisance!=0 ) model = new RooProdPdf("constrainedModel","Model with nuisance parameters",*modelTmp,*priorNuisance);

  /////////////////////////////////////////////////////
  // M A K E   C O N F I D E N C E   I N T E R V A L S
  /////////////////////////////////////////////////////
  double size = 0.1; // 10% size == 90% confidence interval

  // Set up the ProfileLikelihoodCalculator
  ProfileLikelihoodCalculator plc(*data, *model, *POI);
  // ProfileLikelihoodCalculator usually make intervals: the 95% CL one-sided upper-limit is the same as the two-sided upper-limit of a 90% CL interval  
  plc.SetTestSize(size);


  // Pointer to the confidence interval
  //  model->fitTo(*data,SumW2Error(kFALSE)); // <-- problem
  LikelihoodInterval* interval = plc.GetInterval();

  // Compute the upper limit: a fit is needed first in order to locate the minimum of the -log(likelihood) and ease the upper limit computation
  //  model->fitTo(*data,SumW2Error(kFALSE)); // <-- problem
  //  const double upperLimit = interval->UpperLimit(*parameterOfInterest); // <-- to simplify


  
  FeldmanCousins fc;
  fc.SetPdf(*model);
  fc.SetData(*data); 
  fc.SetParameters( *POI );
  fc.UseAdaptiveSampling(true);
  fc.SetNBins(5); // number of points to test per parameter
  fc.SetTestSize(size);
  //  fc.SaveBeltToFile(true); // optional
  ConfInterval* fcint = NULL;
  cout << "\n\nRunning FC (with nuisance parameters a few minute" << endl;
  fcint = fc.GetInterval();  // that was easy.

  /////////////////////////////////////////
  // Third, use a Calculator based on Markov Chain monte carlo
  UniformProposal up;
  MCMCCalculator mc;
  myWS->defineSet("POI_rev","trueMass,S");
  const RooArgSet* POIreversed = myWS->set("POI_rev");
  mc.SetPdf(*model);
  mc.SetData(*data);
  mc.SetParameters(*POIreversed);
  mc.SetProposalFunction(up);
  mc.SetNumIters(100000); // steps in the chain
  mc.SetTestSize(size); // 90% CL
  mc.SetNumBins(20); // used in posterior histogram
  mc.SetNumBurnInSteps(40); // ignore first steps in chain due to "burn in"
  ConfInterval* mcmcint = NULL;
  cout << "\n\nRunning MCMC, this will take about a minute" << endl;
  mcmcint = mc.GetInterval();

  //////////////////////////////////////////////////
  // M A K E    P L O T S
  //////////////////////////////////////////////////

  cout << "making plots" << endl;
  // Make a plot of the profile-likelihood and confidence interval
  LikelihoodIntervalPlot plot(interval);
  plot.Draw();

  MCMCIntervalPlot* mcPlot = new MCMCIntervalPlot(*((MCMCInterval*)mcmcint));
  mcPlot->Draw("same");
  mcPlot->SetLineColor(kGreen);
  mcPlot->SetLineWidth(2);

  // we don't have a nice class for plotting FC plots
  // first plot a small dot for every point tested
  RooDataHist* parameterScan = (RooDataHist*) fc.GetPointsToScan();
  TH2F* hist = (TH2F*) parameterScan->createHistogram("trueMass:S",10,10);
  //  hist->Draw();
  TH2F* forContour = (TH2F*)hist->Clone();
  forContour->SetLineColor(kRed);
  forContour->SetLineWidth(2);

  // now loop through the points and put a marker if it's in the interval
  RooArgSet* tmpPoint;
  // loop over points to test
  for(Int_t i=0; i<parameterScan->numEntries(); ++i){
     // get a parameter point from the list of points to test.
    tmpPoint = (RooArgSet*) parameterScan->get(i)->clone("temp");
    //    tmpPoint->Print("v");
    if (fcint){
      if (fcint->IsInInterval( *tmpPoint ) ) {
	forContour->SetBinContent( hist->FindBin(tmpPoint->getRealValue("trueMass"), 
						  tmpPoint->getRealValue("S")),	 1);
      }else{
	forContour->SetBinContent( hist->FindBin(tmpPoint->getRealValue("trueMass"), 
						  tmpPoint->getRealValue("S")),	 0);
      }
    }


    delete tmpPoint;
  }
  if(fcint)
    forContour->Draw("box,same");
  

  delete model;
}
