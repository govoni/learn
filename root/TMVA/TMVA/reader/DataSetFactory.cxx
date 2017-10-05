// @(#)root/tmva $Id: DataSetFactory.cxx,v 1.12 2008/07/15 16:02:16 speckmayer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSetFactory                                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#include <vector>
#include <iomanip>
#include <iostream>

#include "TMVA/DataSetFactory.h"

#include "TEventList.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TMatrixF.h"
#include "TVectorF.h"
#include "TMath.h"
#include "TROOT.h"

#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif
#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif
#ifndef ROOT_TMVA_VariableIdentityTransform
#include "TMVA/VariableIdentityTransform.h"
#endif
#ifndef ROOT_TMVA_VariableDecorrTransform
#include "TMVA/VariableDecorrTransform.h"
#endif
#ifndef ROOT_TMVA_VariablePCATransform
#include "TMVA/VariablePCATransform.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_DataSetInfo
#include "TMVA/DataSetInfo.h"
#endif
#ifndef ROOT_TMVA_DataInputHandler
#include "TMVA/DataInputHandler.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif



TMVA::DataSetFactory* TMVA::DataSetFactory::fgInstance = 0;


namespace TMVA {
   // calculate the largest common divider
   // this function is not happy if numbers are negative!
   Int_t largestCommonDivider(Int_t a, Int_t b) 
   {
      if (a<b) {Int_t tmp = a; a=b; b=tmp; } // achieve a>=b
      if (b==0) return a;
      Int_t fullFits = a/b;
      return largestCommonDivider(b,a-b*fullFits);
   }
}

//_______________________________________________________________________
TMVA::DataSetFactory::DataSetFactory() :
   fVerbose(kFALSE),
   fCurrentTree(0),
   fCurrentEvtIdx(0),
   fInputVarFormulas(0),
   fLogger( "DataSetFactory", kINFO )
{
   // constructor

   fWeightFormula[0] = fWeightFormula[1] = 0;
   fCutFormula[0]    = fCutFormula[1]    = 0;
}

//_______________________________________________________________________
TMVA::DataSetFactory::~DataSetFactory() 
{
   // destructor
}

//_______________________________________________________________________
TMVA::DataSet*
TMVA::DataSetFactory::createDataSet(TMVA::DataSetInfo& dsi, TMVA::DataInputHandler& dataInput) {
   // steering the creation of a new dataset

   // build the first dataset from the data input
   DataSet * ds = buildInitialDataSet(dsi, dataInput);

//   ds->SetCurrentType(Types::kTraining); <======== taken out

   if( ds->GetNEvents() > 1 ){
      CalcMinMax(ds,dsi);

      // from the the final dataset build the correlation matrix
      dsi.SetCorrelationMatrix( CalcCorrelationMatrix( ds, kTRUE ), Types::kSignal );
      dsi.SetCorrelationMatrix( CalcCorrelationMatrix( ds, kFALSE ), Types::kBackground );

      dsi.PrintCorrelationMatrix( Types::kSignal );
      dsi.PrintCorrelationMatrix( Types::kBackground );
   }
   return ds;
}

//_______________________________________________________________________
TMVA::DataSet*
TMVA::DataSetFactory::buildInitialDataSet(TMVA::DataSetInfo& dsi, TMVA::DataInputHandler& dataInput) {
   DataSet* ds = new DataSet(dsi);

   if(dataInput.GetEntries()==0) {
      std::vector<VariableInfo>& varinfos = dsi.GetVariableInfos();
      std::vector<Float_t*>* evdyn = new std::vector<Float_t*>(0);
      std::vector<VariableInfo>::iterator it = varinfos.begin();
      for(;it!=varinfos.end();it++){
	 evdyn->push_back( (Float_t*)(*it).GetExternalLink() );
	 //	 std::cout << "it ext " << (Float_t*)(*it).GetExternalLink() << std::endl;
      }
      TMVA::Event * ev = new Event((const std::vector<Float_t*>*&)evdyn);
      std::vector<Event*>* newEventVector = new std::vector<Event*>;
      newEventVector->push_back(ev);
      ds->SetEventCollection(newEventVector, Types::kTraining);
      ds->SetCurrentType( Types::kTraining );
      ds->SetCurrentEvent( 0 );
   } else {
      // the dataset splitting
      Configurable splitSpecs( dsi.GetSplitOptions() );
      splitSpecs.SetConfigName("DataSetFactory");

      TString splitMode( "Random" );    // the splitting mode
      splitSpecs.DeclareOptionRef( splitMode, "SplitMode",
				   "Method of picking training and testing events (default: random)" );
      splitSpecs.AddPreDefVal(TString("Random"));
      splitSpecs.AddPreDefVal(TString("Alternate"));
      splitSpecs.AddPreDefVal(TString("Block"));

      UInt_t  splitSeed(100);
      splitSpecs.DeclareOptionRef( splitSeed, "SplitSeed",
				   "Seed for random event shuffling" );   

      TString normMode( "NumEvents" );  // the weight normalisation modes
      splitSpecs.DeclareOptionRef( normMode, "NormMode",
				   "Type of renormalisation of event-by-event weights" );
      splitSpecs.AddPreDefVal(TString("None"));
      splitSpecs.AddPreDefVal(TString("NumEvents"));
      splitSpecs.AddPreDefVal(TString("EqualNumEvents"));

      // the number of events
      Int_t nSigTrainEvents(0); // number of signal training events, 0 - all available
      Int_t nBkgTrainEvents(0); // number of backgd training events, 0 - all available
      Int_t nSigTestEvents (0); // number of signal testing events, 0 - all available
      Int_t nBkgTestEvents (0); // number of backgd testing events, 0 - all available
      splitSpecs.DeclareOptionRef( nSigTrainEvents, "NSigTrain",
				   "Number of signal training events (default: 0 - all)" );
      splitSpecs.DeclareOptionRef( nBkgTrainEvents, "NBkgTrain",
				   "Number of background training events (default: 0 - all)" );
      splitSpecs.DeclareOptionRef( nSigTestEvents,  "NSigTest",
				   "Number of signal testing events (default: 0 - all)" );
      splitSpecs.DeclareOptionRef( nBkgTestEvents,  "NBkgTest",
				   "Number of background testing events (default: 0 - all)" );

      splitSpecs.DeclareOptionRef( fVerbose, "V", "Verbosity (default: true)" );
      splitSpecs.ParseOptions();

      if (Verbose()) fLogger.SetMinType( kVERBOSE );

      // put all to upper case
      splitMode.ToUpper(); normMode.ToUpper();
   
      // loop over signal events first, then over background events
      const char* typeName[2] = { "signal", "background" };

      // ============================================================
      // create training and test tree
      // ============================================================

      Float_t weight;

      // the sum of weights should be renormalised to the number of events
      Double_t sumOfWeights[2] = { 0, 0 };
      Double_t nTempEvents[2]  = { 0, 0 };
      Double_t renormFactor[2] = { -1, -1 };

      // create the type, weight and boostweight branches
      std::vector<Event*> tmpEventVector[2];

      const UInt_t nvars = dsi.GetNVariables();
      std::vector<Float_t> vArr(nvars);

      // number of signal and background events passing cuts
      Int_t nInitialEvents[2]; nInitialEvents[0] = nInitialEvents[1] = 0;
      Int_t nEvBeforeCut[2];   nEvBeforeCut[0]   = nEvBeforeCut[1]   = 0;
      Int_t nEvAfterCut[2];    nEvAfterCut[0]    = nEvAfterCut[1]    = 0;
      Float_t nWeEvBeforeCut[2]; nWeEvBeforeCut[0] = nWeEvBeforeCut[1] = 0;
      Float_t nWeEvAfterCut[2];  nWeEvAfterCut[0]  = nWeEvAfterCut[1]  = 0;

      Bool_t haveArrayVariable = kFALSE;
      Bool_t *varIsArray = new Bool_t[nvars];
      Float_t *varAvLength[2]; 
      varAvLength[0] = new Float_t[nvars];
      varAvLength[1] = new Float_t[nvars];
      for(UInt_t ivar=0; ivar<nvars; ivar++) {
	 varIsArray[ivar] = kFALSE;
	 varAvLength[0][ivar] = 0;
	 varAvLength[1][ivar] = 0;
      }

      Double_t nNegWeights[2] = { 0, 0 };

      // if we work with chains we need to remember the current tree
      // if the chain jumps to a new tree we have to reset the formulas
      for (Int_t sb=0; sb<2; sb++) { // sb=0 - signal, sb=1 - background

	 fLogger << kINFO << "Create training and testing trees: looping over " << typeName[sb] 
		 << " events ..." << Endl;

	 // used for chains only
	 TString currentFileName("");

	 std::vector<TreeInfo>::const_iterator treeIt(dataInput.begin(sb));
	 for (;treeIt!=dataInput.end(sb); treeIt++) {

	    TreeInfo currentInfo = *treeIt;

	    Bool_t isChain = (TString("TChain") == currentInfo.GetTree()->ClassName());
	    currentInfo.GetTree()->LoadTree(0);
	    ChangeToNewTree( currentInfo.GetTree()->GetTree(), dsi );

	    // count number of events in tree before cut
	    nInitialEvents[sb] += currentInfo.GetTree()->GetEntries();

	    // loop over events in ntuple
	    for (Long64_t evtIdx = 0; evtIdx < currentInfo.GetTree()->GetEntries(); evtIdx++) {
	       currentInfo.GetTree()->LoadTree(evtIdx);

	       // may need to reload tree in case of chains
	       if (isChain) {
		  if (currentInfo.GetTree()->GetTree()->GetDirectory()->GetFile()->GetName() != currentFileName) {
		     currentFileName = currentInfo.GetTree()->GetTree()->GetDirectory()->GetFile()->GetName();
		     TTree* pCurrentTree = currentInfo.GetTree()->GetTree();
		     ChangeToNewTree( pCurrentTree, dsi );
		  }
	       }
	       currentInfo.GetTree()->GetEntry(evtIdx);
	       Int_t sizeOfArrays = 1;
	       Int_t prevArrExpr = 0;
	       for (UInt_t ivar=0; ivar<nvars; ivar++) {
		  Int_t ndata = fInputVarFormulas[ivar]->GetNdata();
		  varAvLength[sb][ivar] += ndata;
		  if (ndata==1) continue;
		  haveArrayVariable = kTRUE;
		  varIsArray[ivar] = kTRUE;
		  if (sizeOfArrays==1) {
		     sizeOfArrays = ndata;
		     prevArrExpr = ivar;
		  } 
		  else if (sizeOfArrays!=ndata) {
		     fLogger << kERROR << "ERROR while preparing training and testing trees:" << Endl;
		     fLogger << "   multiple array-type expressions of different length were encountered" << Endl;
		     fLogger << "   location of error: event " << evtIdx 
			     << " in tree " << currentInfo.GetTree()->GetName()
			     << " of file " << currentInfo.GetTree()->GetCurrentFile()->GetName() << Endl;
		     fLogger << "   expression " << fInputVarFormulas[ivar]->GetTitle() << " has " 
			     << ndata << " entries, while" << Endl;
		     fLogger << "   expression " << fInputVarFormulas[prevArrExpr]->GetTitle() << " has "
			     << fInputVarFormulas[prevArrExpr]->GetNdata() << " entries" << Endl;
		     fLogger << kFATAL << "Need to abort" << Endl;
		  }
	       }


	       for (Int_t idata = 0;  idata<sizeOfArrays; idata++) {
		  Bool_t containsNaN = kFALSE;

		  // the cut expression
		  Float_t cutVal = 1;
		  TTreeFormula* sbCut = fCutFormula[sb];
		  if(sbCut) {
		     Int_t ndata = sbCut->GetNdata();
		     if(ndata==1) {
			cutVal = sbCut->EvalInstance(0);
		     } else {
			cutVal = sbCut->EvalInstance(idata);
		     }
		     if (TMath::IsNaN(cutVal)) {
			containsNaN = kTRUE;
			fLogger << kWARNING << "Cut expression resolves to infinite value (NaN): " 
				<< sbCut->GetTitle() << Endl;
		     }
		  }

		  // read the variables
		  for (UInt_t ivar=0; ivar<nvars; ivar++) {
		     Int_t ndata = fInputVarFormulas[ivar]->GetNdata();
		     vArr[ivar] = (ndata == 1 ? 
				   fInputVarFormulas[ivar]->EvalInstance(0) : 
				   fInputVarFormulas[ivar]->EvalInstance(idata));
		     if (TMath::IsNaN(vArr[ivar])) {
			containsNaN = kTRUE;
			fLogger << kWARNING << "Expression resolves to infinite value (NaN): " 
				<< fInputVarFormulas[ivar]->GetTitle() << Endl;
		     }
		  }

		  // the weight (can also be an array)
		  weight = currentInfo.GetWeight(); // multiply by tree weight

		  if (fWeightFormula[sb]!=0) {               
		     Int_t ndata = fWeightFormula[sb]->GetNdata();
		     weight *= ndata==1?fWeightFormula[sb]->EvalInstance():fWeightFormula[sb]->EvalInstance(idata);
		     if (TMath::IsNaN(weight)) {
			containsNaN = kTRUE;
			fLogger << kWARNING << "Weight expression resolves to infinite value (NaN): " 
				<< fWeightFormula[sb]->GetTitle() << Endl;
		     }
		  }

		  // global flag if negative weights exist -> can be used by classifiers who may 
		  // require special data treatment (also print warning)
		  if (weight < 0) nNegWeights[sb]++;

		  // Count the events before rejection due to cut or NaN value
		  // (weighted and unweighted)
		  nEvBeforeCut[sb] ++;
		  if(!TMath::IsNaN(weight))
		     nWeEvBeforeCut[sb] += weight;

		  // apply the cut
		  if(cutVal<0.5) continue;

		  if (containsNaN) {
		     fLogger << kWARNING << "Event " << evtIdx;
		     if (sizeOfArrays>1) fLogger << kWARNING << "[" << idata << "]";
		     fLogger << " rejected" << Endl;
		     continue;
		  }

		  // Count the events after rejection due to cut or NaN value
		  // (weighted and unweighted)
		  nEvAfterCut[sb] ++;
		  nWeEvAfterCut[sb] += weight;

		  // event accepted, fill temporary ntuple
		  tmpEventVector[sb].push_back(new Event(vArr,sb==0,weight));

		  // add up weights
		  sumOfWeights[sb] += weight;
		  nTempEvents[sb]  += 1;
	       }
	    }
      
	    currentInfo.GetTree()->ResetBranchAddresses();
	 }

	 // compute renormalisation factors
	 renormFactor[sb] = nTempEvents[sb]/sumOfWeights[sb];
      }


      fLogger << kINFO << "Prepare training and Test samples:" << Endl;
      fLogger << kINFO << "  Signal tree     - total number of events     : " << dataInput.GetSignalEntries()     << Endl;
      fLogger << kINFO << "  Background tree - total number of events     : " << dataInput.GetBackgroundEntries() << Endl;

      if (haveArrayVariable) {
	 fLogger << kINFO << "Some variables are arrays:" << Endl;
	 for(UInt_t ivar=0; ivar<nvars; ivar++) {
	    if(varIsArray[ivar]) {
	       fLogger << kINFO << "    Variable " << dsi.GetExpression(ivar) << " : array with average length "
		       << varAvLength[0][ivar] << " (S) and " << varAvLength[1][ivar] << "(B)" << Endl;
	    } else {
	       fLogger << kINFO << "    Variable " << dsi.GetExpression(ivar) << " : single" << Endl;
	    }
	 }
	 fLogger << kINFO << "Number of events after serialization:" << Endl;
	 fLogger << kINFO << "    Signal          - number of events :  "
		 << std::setw(5) << nEvBeforeCut[0] << "    weighted : " << std::setw(5) << nWeEvBeforeCut[0] << Endl;
	 fLogger << kINFO << "    Background      - number of events :  "
		 << std::setw(5) << nEvBeforeCut[1] << "    weighted : " << std::setw(5) << nWeEvBeforeCut[1] << Endl;
      }

      fLogger << kINFO << "Preselection:" << Endl;
      if (dsi.HasCuts()) {
	 fLogger << kINFO << "    On signal input       : " << dsi.CutS(Types::kSignal) << Endl;
	 fLogger << kINFO << "    On background input   : " << dsi.CutS(Types::kBackground) << Endl;
	 fLogger << kINFO << "    Signal          - number of events :  "
		 << std::setw(5) << nEvAfterCut[0] << "    weighted : " << std::setw(5) << nWeEvAfterCut[0] << Endl;
	 fLogger << kINFO << "    Background      - number of events :  "
		 << std::setw(5) << nEvAfterCut[1] << "    weighted : " << std::setw(5) << nWeEvAfterCut[1] << Endl;
	 fLogger << kINFO << "    Signal          - efficiency       :                 "
		 << std::setw(10) << nWeEvAfterCut[0]/nWeEvBeforeCut[0] << Endl;
	 fLogger << kINFO << "    Background      - efficiency       :                 " 
		 << std::setw(10) << nWeEvAfterCut[1]/nWeEvBeforeCut[1] << Endl;
      }
      else fLogger << kINFO << "    No preselection cuts applied on signal or background" << Endl;



      // print rescaling info
      if (normMode == "NONE") {
	 fLogger << kINFO << "No weight renormalisation applied: use original event weights" << Endl;
	 renormFactor[0] = renormFactor[1] = 1;
      }
      else if (normMode == "NUMEVENTS") {
	 fLogger << kINFO << "Weight renormalisation mode: \"NumEvents\": renormalise signal and background" << Endl;
	 fLogger << kINFO << "... weights independently so that Sum[i=1..N_j]{w_i} = N_j, j=signal, background" << Endl;
	 fLogger << kINFO << "... (note that N_j is the sum of training and test events)" << Endl;
	 for (Int_t sb=0; sb<2; sb++) { // sb=0 - signal, sb=1 - background
	    fLogger << kINFO << "Rescale " << typeName[sb] << " event weights by factor: " << renormFactor[sb] << Endl;
	 }
      }
      else if (normMode == "EQUALNUMEVENTS") {
	 fLogger << kINFO << "Weight renormalisation mode: \"EqualNumEvents\": renormalise signal and background" << Endl;
	 fLogger << kINFO << "   weights so that Sum[i=1..N_j]{w_i} = N_signal, j=signal, background" << Endl;
	 fLogger << kINFO << "   (note that N_j is the sum of training and test events)" << Endl;
	 // sb=0 - signal, sb=1 - background
	 renormFactor[1] *= nTempEvents[0]/nTempEvents[1];
	 for (Int_t sb=0; sb<2; sb++) { 
	    fLogger << kINFO << "Rescale " << typeName[sb] << " event weights by factor: " << renormFactor[sb] << Endl;
	 }
      }
      else {
	 fLogger << kFATAL << "<PrepareForTrainingAndTesting> Unknown NormMode: " << normMode << Endl;
      }

      // ============================================================
      // now the training tree needs to be pruned
      // ============================================================

      Long64_t origSize[2];

      for (Int_t sb = 0; sb<2; sb++ ) {
	 origSize[sb] = tmpEventVector[sb].size();
      }

      Long64_t finalNEvents[2][2] = { {nSigTrainEvents, nSigTestEvents},
				      {nBkgTrainEvents, nBkgTestEvents} };

      fLogger << kVERBOSE << "Number of available training events:" << Endl
	      << "  Signal    : " << origSize[Types::kSignal] << Endl
	      << "  Background: " << origSize[Types::kBackground] << Endl;

      for (Int_t sb = 0; sb<2; sb++) { // sb: 0-signal, 1-background
      
	 if (finalNEvents[sb][Types::kTraining]>origSize[sb])
	    fLogger << kFATAL << "More training events requested than available for the " << typeName[sb] << Endl;
      
	 if (finalNEvents[sb][Types::kTesting]>origSize[sb])
	    fLogger << kFATAL << "More testing events requested than available for the " << typeName[sb] << Endl;
      
	 if (finalNEvents[sb][Types::kTraining] + finalNEvents[sb][Types::kTesting] > origSize[sb])
	    fLogger << kFATAL << "More testing and training events requested than available for the " << typeName[sb] << Endl;

	 if (finalNEvents[sb][Types::kTraining]==0 || finalNEvents[sb][Types::kTesting]==0) {
	    if (finalNEvents[sb][Types::kTraining]==0 && finalNEvents[sb][Types::kTesting]==0) {
	       finalNEvents[sb][Types::kTraining] = finalNEvents[sb][Types::kTesting] = origSize[sb]/2;
	    } 
	    else if (finalNEvents[sb][Types::kTesting]==0) {
	       finalNEvents[sb][Types::kTesting]  = origSize[sb] - finalNEvents[sb][Types::kTraining];
	    } 
	    else {
	       finalNEvents[sb][Types::kTraining]  = origSize[sb] - finalNEvents[sb][Types::kTesting];
	    }
	 }
      }

      TRandom rndm( splitSeed ); 
      TEventList* evtList[2][2];

      evtList[Types::kSignal]    [Types::kTraining] = new TEventList();
      evtList[Types::kBackground][Types::kTraining] = new TEventList();
      evtList[Types::kSignal]    [Types::kTesting]  = new TEventList();
      evtList[Types::kBackground][Types::kTesting]  = new TEventList();

      for (Int_t sb = 0; sb<2; sb++ ) { // signal, background
   
	 const Long64_t size = origSize[sb];

	 if (splitMode == "RANDOM") {

	    fLogger << kINFO << "Randomly shuffle events in training and testing trees for " << typeName[sb] << Endl;

	    // the index array
	    Long64_t* idxArray = new Long64_t[size];
	    Bool_t*   allPickedIdxArray = new Bool_t[size];
	    for (Int_t i=0; i<size; i++) { idxArray[i]=i; allPickedIdxArray[i] = kFALSE; }
      
	    for (Int_t itype=Types::kTraining; itype<Types::kMaxTreeType; itype++) {

	       // the selected events
	       Bool_t* thisPickedIdxArray = new Bool_t[size];
	       for (Int_t i=0; i<size; i++) thisPickedIdxArray[i] = kFALSE;
         
	       Long64_t pos = 0;
	       for (Long64_t i=0; i<finalNEvents[sb][itype]; i++) {
		  do { pos = Long64_t(size * rndm.Rndm()); } while (allPickedIdxArray[idxArray[pos]]);
		  thisPickedIdxArray[idxArray[pos]] = kTRUE;
		  allPickedIdxArray [idxArray[pos]] = kTRUE;
	       }
	       for (Long64_t i=0; i<size; i++) if (thisPickedIdxArray[i]) evtList[sb][itype]->Enter(i); 

	       delete [] thisPickedIdxArray;
	    }

	    delete [] idxArray;
	    delete [] allPickedIdxArray;
	 }
	 else if (splitMode == "ALTERNATE") {

	    fLogger << kINFO << "Pick alternating training and test events from input tree for " << typeName[sb] << Endl;
      
	    Int_t ntrain = finalNEvents[sb][Types::kTraining];
	    Int_t ntest  = finalNEvents[sb][Types::kTesting];

	    Int_t lcd       = largestCommonDivider(ntrain,ntest);
	    Int_t trainfrac = ntrain/lcd;
	    Int_t modulo    = (ntrain+ntest)/lcd;

	    for (Long64_t i=0; i<finalNEvents[sb][Types::kTraining]+finalNEvents[sb][Types::kTesting]; i++) {
	       Bool_t isTrainingEvent = (i%modulo)<trainfrac;
	       evtList[sb][isTrainingEvent ? Types::kTraining:Types::kTesting]->Enter( i );
	    }
	 }
	 else if (splitMode == "BLOCK") {

	    fLogger << kINFO << "Pick block-wise training and test events from input tree for " << typeName[sb] << Endl;
      
	    for (Long64_t i=0; i<finalNEvents[sb][Types::kTraining]; i++)
	       evtList[sb][Types::kTraining]->Enter( i );
	    for (Long64_t i=0; i<finalNEvents[sb][Types::kTesting]; i++)
	       evtList[sb][Types::kTesting]->Enter( i + finalNEvents[sb][Types::kTraining]);

	 }
	 else fLogger << kFATAL << "Unknown type: " << splitMode << Endl;
      }

      //    gROOT->cd();

      // merge signal and background trees, and renormalise the event weights in this step   
      for (Int_t itreeTypeTmp=0; itreeTypeTmp<2; itreeTypeTmp++) {

	 Types::ETreeType itreeType = (itreeTypeTmp == 0) ? Types::kTraining : Types::kTesting;

	 fLogger << kINFO << "Create " << (itreeType == Types::kTraining ? "training" : "testing") << " tree" << Endl;        

	 std::vector<Event*>* newEventVector = new std::vector<Event*>;
	 ds->SetEventCollection(newEventVector, itreeType); 

	 newEventVector->reserve(evtList[0][itreeType]->GetN()+evtList[1][itreeType]->GetN());

	 for (Int_t sb=0; sb<2; sb++) {

	    // renormalise only if non-trivial renormalisation factor
	    for (UInt_t ievt=0; ievt<tmpEventVector[sb].size(); ievt++) {
	       if (!evtList[sb][itreeType]->Contains(ievt)) continue;

	       newEventVector->push_back(tmpEventVector[sb][ ievt ]);
	       newEventVector->back()->ScaleWeight( renormFactor[sb] );
	    }
	 }
      }

      tmpEventVector[Types::kSignal].clear(); tmpEventVector[Types::kSignal].resize(0);
      tmpEventVector[Types::kBackground].clear(); tmpEventVector[Types::kBackground].resize(0);

   
      fLogger << kINFO << "Collected:" << Endl;
      fLogger << kINFO << "  Training signal entries     : " << ds->GetNEvtSigTrain()  << Endl;
      fLogger << kINFO << "  Training background entries : " << ds->GetNEvtBkgdTrain() << Endl;
      fLogger << kINFO << "  Testing  signal entries     : " << ds->GetNEvtSigTest()   << Endl;
      fLogger << kINFO << "  Testing  background entries : " << ds->GetNEvtBkgdTest()  << Endl;

      // sanity check
      if (ds->GetNEvtSigTrain() == 0 || ds->GetNEvtBkgdTrain() == 0 ||
	  ds->GetNEvtSigTest()  == 0 || ds->GetNEvtBkgdTest()  == 0) {
	 fLogger << kFATAL << "Zero events encountered for training and/or testing in signal and/or "
		 << "background sample" << Endl;
      }
   }

   return ds;
}


//_______________________________________________________________________
void TMVA::DataSetFactory::ChangeToNewTree( TTree* tr, const DataSetInfo & dsi)
{ 
   // While the data gets copied into the local training and testing
   // trees, the input tree can change (for intance when changing from
   // signal to background tree, or using TChains as input) The
   // TTreeFormulas, that hold the input expressions need to be
   // reassociated with the new tree, which is done here

   tr->SetBranchStatus("*",1);

   std::vector<TTreeFormula*>::const_iterator varFIt = fInputVarFormulas.begin();
   for (;varFIt!=fInputVarFormulas.end();varFIt++) if(*varFIt) delete *varFIt;
   fInputVarFormulas.clear();
   for (UInt_t i=0; i<dsi.GetNVariables(); i++) {
      TTreeFormula* ttf = new TTreeFormula( Form( "Formula%s", dsi.GetInternalVarName(i).Data() ),
                                            dsi.GetExpression(i).Data(), tr );
      fInputVarFormulas.push_back( ttf );
      // sanity check
      if (ttf->GetNcodes() == 0) {
         fLogger << kFATAL << "Expression: " << dsi.GetExpression(i) 
                 << " does not appear to depend on any TTree variable --> please check spelling" << Endl;
      }
   }

   // the cuts
   for(Int_t sb=0; sb<2; sb++) {
      Types::ESBType tt = sb==0?Types::kSignal:Types::kBackground;
      if(TString(dsi.CutS(tt)) != "-") {
         if(fCutFormula[sb]) delete fCutFormula[sb];
         fCutFormula[sb] = new TTreeFormula( "Cut", dsi.CutS(tt), tr );
         // sanity check
         if (fCutFormula[sb]->GetNcodes() == 0)
            fLogger << kFATAL << "Signal cut '" << dsi.CutS(tt)
                    << "' does not appear to depend on any TTree variable --> please check spelling" << Endl;
      }
   }


   // recreate all formulas associated with the new tree
   // clear the old Formulas, if there are any
   //    vector<TTreeFormula*>::const_iterator varFIt = fInputVarFormulas.begin();
   //    for (;varFIt!=fInputVarFormulas.end();varFIt++) delete *varFIt;

   for(Int_t sb=0; sb<2; sb++) {
      if (fWeightFormula[sb]!=0) { delete fWeightFormula[sb]; fWeightFormula[sb]=0; }
      if (dsi.GetWeightExpression(sb)!=TString(""))
         fWeightFormula[sb] = new TTreeFormula("FormulaWeight",dsi.GetWeightExpression(sb),tr);
   }

   tr->SetBranchStatus("*",0);

   for (varFIt = fInputVarFormulas.begin(); varFIt!=fInputVarFormulas.end(); varFIt++) {
      TTreeFormula * ttf = *varFIt;
      for (Int_t bi = 0; bi<ttf->GetNcodes(); bi++)
         tr->SetBranchStatus( ttf->GetLeaf(bi)->GetBranch()->GetName(), 1 );
   }

   for(Int_t sb=0; sb<2; sb++) {
      if (fWeightFormula[sb]!=0)
         for (Int_t bi = 0; bi<fWeightFormula[sb]->GetNcodes(); bi++)
            tr->SetBranchStatus( fWeightFormula[sb]->GetLeaf(bi)->GetBranch()->GetName(), 1 );
   }
   return;
}

//_______________________________________________________________________
void TMVA::DataSetFactory::CalcMinMax( DataSet* ds, TMVA::DataSetInfo& dsi )
{
   // compute covariance matrix
   const UInt_t nvar = ds->GetNVariables();

   Float_t *min = new Float_t[nvar];
   Float_t *max = new Float_t[nvar];

   for (UInt_t ivar=0; ivar<nvar; ivar++) {
     min[ivar] = 1e20;;
     max[ivar] = -1e20;
     //     min[ivar] = dsi.GetVariableInfos()[ivar].GetMin();
     //     max[ivar] = dsi.GetVariableInfos()[ivar].GetMax();
   }

   // perform event loop

   for (Int_t i=0; i<ds->GetNEvents(); i++) {
      Event * ev = ds->GetEvent(i);
      for (UInt_t ivar=0; ivar<nvar; ivar++) {
         Double_t v = ev->GetVal(ivar);
         if(v<min[ivar]) min[ivar] = v;
         if(v>max[ivar]) max[ivar] = v;
      }
   }

   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      dsi.GetVariableInfos()[ivar].SetMin(min[ivar]);
      dsi.GetVariableInfos()[ivar].SetMax(max[ivar]);
   }
}



//_______________________________________________________________________
TMatrixD*
TMVA::DataSetFactory::CalcCorrelationMatrix( DataSet* ds, Bool_t isSignal )
{
   // computes correlation matrix for variables "theVars" in tree;
   // "theType" defines the required event "type" 
   // ("type" variable must be present in tree)

   // first compute variance-covariance
   TMatrixD* mat = CalcCovarianceMatrix( ds, isSignal );

   // now the correlation
   UInt_t nvar = ds->GetNVariables(), ivar, jvar;

   for (ivar=0; ivar<nvar; ivar++) {
      for (jvar=0; jvar<nvar; jvar++) {
         if (ivar != jvar) {
            Double_t d = (*mat)(ivar, ivar)*(*mat)(jvar, jvar);
            if (d > 0) (*mat)(ivar, jvar) /= sqrt(d);
            else {
               fLogger << kWARNING << "<GetCorrelationMatrix> Zero variances for variables "
                       << "(" << ivar << ", " << jvar << ") = " << d                   
                       << Endl;
               (*mat)(ivar, jvar) = 0;
            }
         }
      }
   }

   for (UInt_t ivar=0; ivar<nvar; ivar++) (*mat)(ivar, ivar) = 1.0;

   return mat;
}

//_______________________________________________________________________
TMatrixD*
TMVA::DataSetFactory::CalcCovarianceMatrix( DataSet * ds, Bool_t isSignal )
{
   // compute covariance matrix

   UInt_t nvar = ds->GetNVariables();
   UInt_t ivar = 0, jvar = 0;

   TMatrixD* mat = new TMatrixD( nvar, nvar );

   // init matrices
   TVectorD vec(nvar);
   TMatrixD mat2(nvar, nvar);      
   for (ivar=0; ivar<nvar; ivar++) {
      vec(ivar) = 0;
      for (jvar=0; jvar<nvar; jvar++) mat2(ivar, jvar) = 0;
   }

   // perform event loop
   Double_t ic = 0;
   for (Int_t i=0; i<ds->GetNTrainingEvents(); i++) {

      // fill the event
      Event * ev = ds->GetTrainingEvent(i);

      if (ev->IsSignal() == isSignal) {

         Double_t weight = ev->GetWeight();
         ic += weight; // count used events

         for (ivar=0; ivar<nvar; ivar++) {

            Double_t xi = ev->GetVal(ivar);
            vec(ivar) += xi*weight;
            mat2(ivar, ivar) += (xi*xi*weight);

            for (jvar=ivar+1; jvar<nvar; jvar++) {
               Double_t xj =  ev->GetVal(jvar);
               mat2(ivar, jvar) += (xi*xj*weight);
               mat2(jvar, ivar) = mat2(ivar, jvar); // symmetric matrix
            }
         }
      }
   }

   // variance-covariance
   for (ivar=0; ivar<nvar; ivar++) {
      for (jvar=0; jvar<nvar; jvar++) {
         (*mat)(ivar, jvar) = mat2(ivar, jvar)/ic - vec(ivar)*vec(jvar)/(ic*ic);
      }
   }

   return mat;
}

