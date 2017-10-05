// @(#)Root/tmva $Id: Factory.cxx,v 1.10 2008/07/30 15:18:21 alexvoigt Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Factory                                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <stelzer@cern.ch>        - DESY, Germany                  *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *      Kai Voss        <Kai.Voss@cern.ch>       - U. of Victoria, Canada         *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      U. of Victoria, Canada                                                    * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_______________________________________________________________________
//                                                                      
// This is the main MVA steering class: it creates all MVA methods,     
// and guides them through the training, testing and evaluation         
// phases
//_______________________________________________________________________

#ifndef ROOT_TMVA_Factory
#include "TMVA/Factory.h"
#endif
#ifndef ROOT_TMVA_ClassifierFactory
#include "TMVA/ClassifierFactory.h"
#endif

#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TEventList.h"
#include "TH1.h"
#include "TH2.h"
#include "TText.h"
#include "TTreeFormula.h"
#include "TStyle.h"
#include "TMatrixF.h"
#include "TMatrixDSym.h"
#include "TPaletteAxis.h"
#include "TPrincipal.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Ranking.h"
#include "TMVA/DataSet.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"
#include "TMVA/DataInputHandler.h"
#include "TMVA/DataSetManager.h"
#include "TMVA/DataSetInfo.h"
#include "TMVA/MethodBoost.h"

#include "TMVA/VariableIdentityTransform.h"
#include "TMVA/VariableDecorrTransform.h"
#include "TMVA/VariablePCATransform.h"
#include "TMVA/VariableGaussTransform.h"
#include "TMVA/VariableNormalizeTransform.h"





const Bool_t DEBUG_TMVA_Factory = kFALSE;

const int MinNoTrainingEvents = 10;
const int MinNoTestEvents     = 1;
const long int basketsize     = 1280000;

TFile* TMVA::Factory::fgTargetFile = 0;

ClassImp(TMVA::Factory)

//_______________________________________________________________________
TMVA::Factory::Factory( TString jobName, TFile* theTargetFile, TString theOption )
   : Configurable          ( theOption ),
     fDataInputHandler     ( new DataInputHandler ),
     fTransformations      ( "" ),
     fVerbose              ( kFALSE ),
     fJobName              ( jobName )
{  
   // standard constructor
   //   jobname       : this name will appear in all weight file names produced by the MVAs
   //   theTargetFile : output ROOT file; the test tree and all evaluation plots 
   //                   will be stored here
   //   theOption     : option string; currently: "V" for verbose

   fgTargetFile = theTargetFile;

   DataSetManager::createInstance(*fDataInputHandler);

   // histograms are not automatically associated with the current
   // directory and hence don't go out of scope when closing the file
   // TH1::AddDirectory(kFALSE);
   DeclareOptionRef( fVerbose, "V", "verbose flag" );
   DeclareOptionRef( fColor=!gROOT->IsBatch(), "Color", "color flag (default on)" );
   DeclareOptionRef( fTransformations, "Transformations", "List of transformations to test" );

   ParseOptions( kFALSE );

   fLogger.SetMinType( Verbose() ? kVERBOSE : kINFO );

   gConfig().SetUseColor( fColor );

   



   Greetings();

}

//_______________________________________________________________________
void TMVA::Factory::Greetings() 
{
   // print welcome message
   // options are: kLogoWelcomeMsg, kIsometricWelcomeMsg, kLeanWelcomeMsg

   gTools().TMVAWelcomeMessage( fLogger, gTools().kLogoWelcomeMsg );
   gTools().TMVAVersionMessage( fLogger ); fLogger << Endl;
}

//_______________________________________________________________________
TMVA::Factory::~Factory( void )
{
   // destructor
   std::vector<TMVA::VariableTransformBase*>::iterator trfIt = fDefaultTrfs.begin();
   for(;trfIt != fDefaultTrfs.end(); trfIt++)
      delete (*trfIt);

   this->DeleteAllMethods();
}

//_______________________________________________________________________
void TMVA::Factory::DeleteAllMethods( void )
{
   // delete methods
   MVector::iterator itrMethod = fMethods.begin();
   for (; itrMethod != fMethods.end(); itrMethod++) {
      fLogger << kVERBOSE << "Delete method: " << (*itrMethod)->GetName() << Endl;    
      delete (*itrMethod);
   }
   fMethods.clear();
}

//_______________________________________________________________________
void 
TMVA::Factory::SetVerbose( Bool_t v ) {
   fVerbose = v; //DataInput().SetVerbose(Verbose());
}


//_______________________________________________________________________
TMVA::DataSetInfo &
TMVA::Factory::AddDataSet( DataSetInfo &dsi )
{
   return DataSetManager::Instance().AddDataSetInfo(dsi);
}

//_______________________________________________________________________
TMVA::DataSetInfo &
TMVA::Factory::AddDataSet( const TString& dsiName )
{
   DataSetInfo* dsi = DataSetManager::Instance().GetDataSetInfo(dsiName);
   if(dsi!=0)
      return *dsi;
   
   dsi = new DataSetInfo(dsiName);
   return DataSetManager::Instance().AddDataSetInfo(*dsi);
}

//_______________________________________________________________________
void TMVA::Factory::AddSignalTree( TTree* signal, Double_t weight, Types::ETreeType treetype )
{
   // number of signal events (used to compute significance)
   DataInput().AddSignalTree(signal, weight, treetype);
}

//_______________________________________________________________________
void TMVA::Factory::AddSignalTree( TTree* signal, Double_t weight, const TString& treetype )
{
   // number of signal events (used to compute significance)
   Types::ETreeType tt = Types::kMaxTreeType;
   TString tmpTreeType = treetype; tmpTreeType.ToLower();
   if      (tmpTreeType.Contains( "train" ) && tmpTreeType.Contains( "test" )) tt = Types::kMaxTreeType;
   else if (tmpTreeType.Contains( "train" ))                                   tt = Types::kTraining;
   else if (tmpTreeType.Contains( "test" ))                                    tt = Types::kTesting;
   else {
      fLogger << kFATAL << "<AddSignalTree> cannot interpret tree type: \"" << treetype 
              << "\" should be \"Training\" or \"Test\" or \"Training and Testing\"" << Endl;
   }

   DataInput().AddSignalTree(signal, weight, tt);
}

//_______________________________________________________________________
void TMVA::Factory::SetSignalTree( TTree* signal, Double_t weight )
{
   // number of signal events (used to compute significance)
   DataInput().ClearSignalTreeList(); 
   DataInput().AddSignalTree(signal, weight);
}

//_______________________________________________________________________
void TMVA::Factory::AddBackgroundTree( TTree* background, Double_t weight, Types::ETreeType treetype )
{
   // add background tree to input
   DataInput().AddBackgroundTree( background, weight, treetype );
}

//_______________________________________________________________________
void TMVA::Factory::AddBackgroundTree( TTree* background, Double_t weight, const TString& treetype )
{
   // add background tree to input
   Types::ETreeType tt = Types::kMaxTreeType;
   TString tmpTreeType = treetype; tmpTreeType.ToLower();
   if      (tmpTreeType.Contains( "train" ) && tmpTreeType.Contains( "test" )) tt = Types::kMaxTreeType;
   else if (tmpTreeType.Contains( "train" ))                                   tt = Types::kTraining;
   else if (tmpTreeType.Contains( "test" ))                                    tt = Types::kTesting;
   else {
      fLogger << kFATAL << "<AddBackgroundTree> cannot interpret tree type: \"" << treetype 
              << "\" should be \"Training\" or \"Test\" or \"Training and Testing\"" << Endl;
   }

   DataInput().AddBackgroundTree( background, weight, tt );
}

//_______________________________________________________________________
void TMVA::Factory::SetBackgroundTree( TTree* background, Double_t weight )
{
   // set background tree
   DataInput().ClearBackgroundTreeList();
   DataInput().AddBackgroundTree( background, weight );
}

//_______________________________________________________________________
void TMVA::Factory::SetInputTrees(TTree* signal, TTree* background, 
                                    Double_t signalWeight, Double_t backgroundWeight)
{
   // define the input trees for signal and background; no cuts are applied
   SetSignalTree    ( signal,     signalWeight );
   SetBackgroundTree( background, backgroundWeight );
}

//_______________________________________________________________________
void TMVA::Factory::SetInputTrees( const TString& datFileS, const TString& datFileB, 
                                   Double_t signalWeight, Double_t backgroundWeight )
{
   DataInput().AddSignalTree(datFileS, signalWeight);
   DataInput().AddBackgroundTree(datFileB, backgroundWeight);
}

//_______________________________________________________________________
void TMVA::Factory::SetInputTrees(TTree* inputTree, const TCut& SigCut, const TCut& BgCut)
{
   // define the input trees for signal and background from single input tree,
   // containing both signal and background events distinguished by the type 
   // identifiers: SigCut and BgCut
   DataInput().ClearSignalTreeList();
   DataInput().ClearBackgroundTreeList();
   DataInput().AddInputTrees(inputTree, SigCut, BgCut);
}

//_______________________________________________________________________
void TMVA::Factory::AddVariable( const TString& expression, char type,
                                 Double_t min, Double_t max )
{
   DefaultDataSetInfo().AddVariable( expression, min, max, type ); 
}

//_______________________________________________________________________
TMVA::DataSetInfo& TMVA::Factory::DefaultDataSetInfo() { 
   return AddDataSet( "Default" );
}

//_______________________________________________________________________
void TMVA::Factory::SetInputVariables( vector<TString>* theVariables ) 
{ 
   // fill input variables in data set
   for(std::vector<TString>::iterator it=theVariables->begin();
       it!=theVariables->end(); it++) AddVariable(*it);
}

//_______________________________________________________________________
void TMVA::Factory::SetSignalWeightExpression( const TString& variable)  { 
   DefaultDataSetInfo().SetSignalWeightExpression(variable);
}

//_______________________________________________________________________
void TMVA::Factory::SetBackgroundWeightExpression( const TString& variable) {
   DefaultDataSetInfo().SetBackgroundWeightExpression(variable);
}

//_______________________________________________________________________
void TMVA::Factory::SetCut( const TString& cut, Types::ESBType tt ) {
   DefaultDataSetInfo().SetCut(cut, tt);
}

//_______________________________________________________________________
void TMVA::Factory::SetCut( const TCut& cut, Types::ESBType tt ) {
   DefaultDataSetInfo().SetCut(cut, tt);
}

//_______________________________________________________________________
void TMVA::Factory::PrepareTrainingAndTestTree( const TCut& cut, 
                                                Int_t NsigTrain, Int_t NbkgTrain, Int_t NsigTest, Int_t NbkgTest,
                                                const TString& otherOpt )
{
   // prepare the training and test trees
   SetCut(cut);
   DefaultDataSetInfo().SetSplitOptions( Form("NsigTrain=%i:NbkgTrain=%i:NsigTest=%i:NbkgTest=%i:%s", 
                                              NsigTrain, NbkgTrain, NsigTest, NbkgTest, otherOpt.Data()) );
}

//_______________________________________________________________________
void TMVA::Factory::PrepareTrainingAndTestTree( const TCut& cut, Int_t Ntrain, Int_t Ntest )
{
   // prepare the training and test trees 
   // kept for backward compatibility
   SetCut(cut);
   DefaultDataSetInfo().SetSplitOptions( Form("NsigTrain=%i:NbkgTrain=%i:NsigTest=%i:NbkgTest=%i:SplitMode=Random:EqualTrainSample:!V", 
                                              Ntrain, Ntrain, Ntest, Ntest) );
}

//_______________________________________________________________________
void TMVA::Factory::PrepareTrainingAndTestTree( const TCut& cut, const TString& opt )
{ 
   // prepare the training and test trees
   SetCut(cut);
   DefaultDataSetInfo().SetSplitOptions( opt );
}

//_______________________________________________________________________
void TMVA::Factory::PrepareTrainingAndTestTree( TCut sigcut, TCut bkgcut, const TString& splitOpt )
{ 
   // prepare the training and test trees

   // if event-wise data assignment, add local trees to dataset first
   if (fDataAssignType == kAssignEvents) SetInputTreesFromEventAssignTrees();

   fLogger << kINFO << "Preparing trees for training and testing..." << Endl;
   DefaultDataSetInfo().SetCut(sigcut, Types::kSignal);
   DefaultDataSetInfo().SetCut(bkgcut, Types::kBackground);

   DefaultDataSetInfo().SetSplitOptions( splitOpt );
}

//_______________________________________________________________________
Bool_t TMVA::Factory::BookMethod( TString theMethodName, TString methodTitle, TString theOption ) 
{
   Int_t BoostNum;
   IMethod* method;

   TMVA::Configurable* conf=new TMVA::Configurable(theOption);

   // booking via name; the names are translated into enums and the 
   // corresponding overloaded BookMethod is called
   if (GetMethod( methodTitle ) != 0) {
      fLogger << kFATAL << "Booking failed since method with title <"
              << methodTitle <<"> already exists"
              << Endl;
   }

   fLogger << kINFO << "Booking method: " << methodTitle << Endl;

   //reading from whether this is a boosted classifier
   
   conf->DeclareOptionRef( BoostNum = 0, "BoostNum",
                           "Number of times the classifier will be boosted");
   conf->ParseOptions();
   // initialize methods
   fLogger << "Boost NUM : " << BoostNum << " <= " << theOption << Endl;
   
   if ( BoostNum == 0 )
      method = ClassifierFactory::Instance().Create(std::string(theMethodName), 
                                                    fJobName,
                                                    methodTitle,
                                                    DefaultDataSetInfo(),
                                                    theOption );
   else
   {
      //boosted classifier, requires a specific definition, making it transparent for the user
      method = ClassifierFactory::Instance().Create(std::string("Boost"), 
                                                    fJobName,
                                                    methodTitle,
                                                    DefaultDataSetInfo(),
                                                    theOption );
      (dynamic_cast<MethodBoost*>(method))->SetBoostedMethodName(theMethodName);
   }
   fMethods.push_back( method );
   delete conf;

   return kTRUE;
}

//_______________________________________________________________________
Bool_t TMVA::Factory::BookMethod( Types::EMVA theMethod, TString methodTitle, TString theOption ) 
{
   // books MVA method; the option configuration string is custom for each MVA
   // the TString field "theNameAppendix" serves to define (and distringuish) 
   // several instances of a given MVA, eg, when one wants to compare the 
   // performance of various configurations

   return BookMethod( Types::Instance().GetMethodName( theMethod ), methodTitle, theOption );
}

//_______________________________________________________________________
TMVA::IMethod*
TMVA::Factory::GetMethod( const TString &methodTitle ) const
{
   // returns pointer to MVA that corresponds to given method title
   MVector::const_iterator itrMethod    = fMethods.begin();
   MVector::const_iterator itrMethodEnd = fMethods.end();
   //
   for (; itrMethod != itrMethodEnd; itrMethod++) {
      MethodBase* mva = dynamic_cast<MethodBase*>(*itrMethod);    
      if ( (mva->GetMethodTitle())==methodTitle ) return mva;
   }
   return 0;
}

//_______________________________________________________________________
void TMVA::Factory::WriteDataInformation() {
   // put correlations of input data and a few (default + user
   // selected) transformations into the root file

   RootBaseDir()->cd();

   // correlation matrix of the default DS
   const TMatrixD* m(0);
   const TH2* h(0);
   m = DefaultDataSetInfo().CorrelationMatrix(Types::kSignal);
   h = DefaultDataSetInfo().createCorrelationMatrixHist(m, "CorrelationMatrixS", "Correlation Matrix (signal)");
   if (h!=0){ 
      h->Write();
      delete h;
   }

   m = DefaultDataSetInfo().CorrelationMatrix(Types::kBackground);
   h = DefaultDataSetInfo().createCorrelationMatrixHist(m, "CorrelationMatrixB", "Correlation Matrix (background)");
   if (h!=0){ 
      h->Write();
      delete h;
   }
   
   // some default transformations to evaluate
   TString processTrfs = "I;D;P;G,D;";
   // plus some user defined transformations
   processTrfs += fTransformations;

   std::vector<TMVA::TransformationHandler*> trfs;
   
   std::vector<TString> trfsDef = gTools().SplitString(processTrfs,';');
   std::vector<TString>::iterator trfsDefIt = trfsDef.begin();
   for(; trfsDefIt!=trfsDef.end(); trfsDefIt++) {
      trfs.push_back(new TMVA::TransformationHandler(DefaultDataSetInfo()));
      std::vector<TString> trfDef = gTools().SplitString(*trfsDefIt,',');
      std::vector<TString>::iterator trfDefIt = trfDef.begin();
      for(; trfDefIt!=trfDef.end(); trfDefIt++) {
         TString trfS = (*trfDefIt);
         if(trfS=='I') {
            trfs.back()->AddTransformation(new VariableIdentityTransform(DefaultDataSetInfo().GetVariableInfos()));
         } else if (trfS=='D') {
            trfs.back()->AddTransformation(new VariableDecorrTransform(DefaultDataSetInfo().GetVariableInfos()));
         } else if (trfS=='P') {
            trfs.back()->AddTransformation(new VariablePCATransform(DefaultDataSetInfo().GetVariableInfos()));
         } else if (trfS=='G') {
            trfs.back()->AddTransformation(new VariableGaussTransform(DefaultDataSetInfo().GetVariableInfos()));
         } else if (trfS=='N') {
            trfs.back()->AddTransformation(new VariableNormalizeTransform(DefaultDataSetInfo().GetVariableInfos()));
         } else {
            fLogger << kINFO << "The transformation " << *trfsDefIt << " definition is not valid, the \n"
                    << "transformation " << trfS << " is not known!" << Endl;
         }
      }
   }

   const std::vector<Event*>& inputEvents = DefaultDataSetInfo().GetDataSet()->GetEventCollection();

   std::vector<TMVA::TransformationHandler*>::iterator trfIt = trfs.begin();
   for(;trfIt != trfs.end(); trfIt++) {
      // setting a Root dir causes the variables distributions to be saved to the root file
      (*trfIt)->SetRootDir(RootBaseDir());
      (*trfIt)->CalcTransformations(inputEvents);
      delete (*trfIt);
   }

}

//_______________________________________________________________________
void TMVA::Factory::TrainAllMethods() 
{  
   // iterates over all MVAs that have been booked, and calls their training methods

   // first write some information about the default dataset
   WriteDataInformation();

   // here the training starts
   fLogger << kINFO << "Training all methods..." << Endl;

   MVector::iterator itrMethod;

   // iterate over methods and train
   for (itrMethod = fMethods.begin(); itrMethod != fMethods.end(); itrMethod++) {

      MethodBase* mva = dynamic_cast<MethodBase*>(*itrMethod);
      if (mva->Data()->GetNTrainingEvents() >= MinNoTrainingEvents) {
         fLogger << kINFO << "Train method: " << mva->GetMethodTitle() << Endl;
         mva->TrainMethod();
      }
      else {
         fLogger << kWARNING << "Method " << mva->GetMethodName() 
                 << " not trained (training tree has less entries ["
                 << mva->Data()->GetNTrainingEvents() 
                 << "] than required [" << MinNoTrainingEvents << "]" << Endl; 
      }
   }

   // variable ranking 
   fLogger << Endl;
   fLogger << kINFO << "Begin ranking of input variables..." << Endl;
   for (itrMethod = fMethods.begin(); itrMethod != fMethods.end(); itrMethod++) {
      MethodBase* mva = dynamic_cast<MethodBase*>(*itrMethod);
      if (mva->Data()->GetNTrainingEvents() >= MinNoTrainingEvents) {

         // create and print ranking
         const Ranking* ranking = (*itrMethod)->CreateRanking();
         if (ranking != 0) ranking->Print();
         else fLogger << kINFO << "No variable ranking supplied by classifier: " 
                      << dynamic_cast<MethodBase*>(*itrMethod)->GetMethodTitle() << Endl;
      }
   }
   fLogger << Endl;
}

//_______________________________________________________________________
void TMVA::Factory::TestAllMethods( void )
{
   // iterates over all MVAs that have been booked, and calls their testing methods
   fLogger << kINFO << "Testing all classifiers..." << Endl;

   // iterate over methods and test
   MVector::iterator itrMethod    = fMethods.begin();
   MVector::iterator itrMethodEnd = fMethods.end();
   for (; itrMethod != itrMethodEnd; itrMethod++) {
      MethodBase* mva = dynamic_cast<MethodBase*>(*itrMethod);
      fLogger << kINFO << "Test method: " << mva->GetMethodTitle() << Endl;
      mva->AddClassifierOutput(Types::kTesting);
      if(mva->HasMVAPdfs())
         mva->AddClassifierOutputProb(Types::kTesting);
   }
}

//_______________________________________________________________________
void TMVA::Factory::MakeClass( const TString& methodTitle ) const
{
   // Print predefined help message of classifier
   // iterate over methods and test
   if (methodTitle != "") {
      IMethod* method = GetMethod( methodTitle );
      if (method) method->MakeClass();
      else {
         fLogger << kWARNING << "<MakeClass> Could not find classifier \"" << methodTitle 
                 << "\" in list" << Endl;
      }
   }
   else {

      // no classifier specified, print all hepl messages
      MVector::const_iterator itrMethod    = fMethods.begin();
      MVector::const_iterator itrMethodEnd = fMethods.end();
      for (; itrMethod != itrMethodEnd; itrMethod++) {
         MethodBase* method = dynamic_cast<MethodBase*>(*itrMethod);
         fLogger << kINFO << "Make response class for classifier: " << method->GetMethodTitle() << Endl;
         method->MakeClass();
      }
   }
}

//_______________________________________________________________________
void TMVA::Factory::PrintHelpMessage( const TString& methodTitle ) const
{
   // Print predefined help message of classifier
   // iterate over methods and test
   if (methodTitle != "") {
      IMethod* method = GetMethod( methodTitle );
      if (method) method->PrintHelpMessage();
      else {
         fLogger << kWARNING << "<PrintHelpMessage> Could not find classifier \"" << methodTitle 
                 << "\" in list" << Endl;
      }
   }
   else {

      // no classifier specified, print all hepl messages
      MVector::const_iterator itrMethod    = fMethods.begin();
      MVector::const_iterator itrMethodEnd = fMethods.end();
      for (; itrMethod != itrMethodEnd; itrMethod++) {
         MethodBase* method = dynamic_cast<MethodBase*>(*itrMethod);
         fLogger << kINFO << "Print help message for classifier: " << method->GetMethodTitle() << Endl;
         method->PrintHelpMessage();
      }
   }
}

//_______________________________________________________________________
void TMVA::Factory::EvaluateAllVariables( TString options )
{
   // iterates over all MVA input varables and evaluates them
   fLogger << kINFO << "Evaluating all variables..." << Endl;

   for (UInt_t i=0; i<DefaultDataSetInfo().GetNVariables(); i++) {
      TString s = DefaultDataSetInfo().GetVariableInfos()[i].GetInternalVarName();
      if (options.Contains("V")) s += ":V";
      this->BookMethod( "Variable", s );
   }
}

//_______________________________________________________________________
void TMVA::Factory::EvaluateAllMethods( void )
{
   // iterates over all MVAs that have been booked, and calls their evaluation methods

   fLogger << kINFO << "Evaluating all classifiers..." << Endl;

   // -----------------------------------------------------------------------
   // First part of evaluation process
   // --> compute efficiencies, and other separation estimators
   // -----------------------------------------------------------------------

   // although equal, we now want to seperate the outpuf for the variables
   // and the real methods
   Int_t isel;                  // will be 0 for a Method; 1 for a Variable
   Int_t nmeth_used[2] = {0,0}; // 0 Method; 1 Variable

   vector<vector<TString> >  mname(2);
   vector<vector<Double_t> > sig(2), sep(2);
   vector<vector<Double_t> > eff01(2), eff10(2), eff30(2), effArea(2);
   vector<vector<Double_t> > eff01err(2), eff10err(2), eff30err(2);
   vector<vector<Double_t> > trainEff01(2), trainEff10(2), trainEff30(2);

   // following vector contains all methods - with the exception of Cuts, which are special
   MVector methodsNoCuts; 

   // iterate over methods and evaluate
   MVector::iterator itrMethod    = fMethods.begin();
   MVector::iterator itrMethodEnd = fMethods.end();
   for (; itrMethod != itrMethodEnd; itrMethod++) {
      MethodBase* theMethod = dynamic_cast<MethodBase*>(*itrMethod);
      if (theMethod->GetMethodType() != Types::kCuts) methodsNoCuts.push_back( *itrMethod );

      fLogger << kINFO << "Evaluate classifier: " << theMethod->GetMethodTitle() << Endl;
      isel = (theMethod->GetMethodName().Contains("Variable")) ? 1 : 0;
      
      // perform the evaluation
      theMethod->Test();
 
      // evaluate the classifier
      mname[isel].push_back( theMethod->GetMethodTitle() );
      sig[isel].push_back  ( theMethod->GetSignificance() );
      sep[isel].push_back  ( theMethod->GetSeparation() );
      Double_t err;
      eff01[isel].push_back( theMethod->GetEfficiency("Efficiency:0.01", Types::kTesting, err) );
      eff01err[isel].push_back( err );
      eff10[isel].push_back( theMethod->GetEfficiency("Efficiency:0.10", Types::kTesting, err) );
      eff10err[isel].push_back( err );
      eff30[isel].push_back( theMethod->GetEfficiency("Efficiency:0.30", Types::kTesting, err) );
      eff30err[isel].push_back( err );
      effArea[isel].push_back( theMethod->GetEfficiency("",              Types::kTesting, err)  ); // computes the area (average)

      trainEff01[isel].push_back( theMethod->GetTrainingEfficiency("Efficiency:0.01") ); // the first pass takes longer
      trainEff10[isel].push_back( theMethod->GetTrainingEfficiency("Efficiency:0.10") );
      trainEff30[isel].push_back( theMethod->GetTrainingEfficiency("Efficiency:0.30") );

      nmeth_used[isel]++;
      theMethod->WriteEvaluationHistosToFile();

   }

   // now sort the variables according to the best 'eff at Beff=0.10'
   for (Int_t k=0; k<2; k++) {
      vector< vector<Double_t> > vtemp;
      vtemp.push_back( effArea[k] );
      vtemp.push_back( eff10[k] ); // this is the vector that is ranked
      vtemp.push_back( eff01[k] );
      vtemp.push_back( eff30[k] );
      vtemp.push_back( eff10err[k] ); // this is the vector that is ranked
      vtemp.push_back( eff01err[k] );
      vtemp.push_back( eff30err[k] );
      vtemp.push_back( trainEff10[k] );
      vtemp.push_back( trainEff01[k] );
      vtemp.push_back( trainEff30[k] );
      vtemp.push_back( sig[k] );
      vtemp.push_back( sep[k] );
      vector<TString> vtemps = mname[k];
      gTools().UsefulSortDescending( vtemp, &vtemps );
      effArea[k]    = vtemp[0];
      eff10[k]      = vtemp[1];
      eff01[k]      = vtemp[2];
      eff30[k]      = vtemp[3];
      eff10err[k]   = vtemp[4];
      eff01err[k]   = vtemp[5];
      eff30err[k]   = vtemp[6];
      trainEff10[k] = vtemp[7];
      trainEff01[k] = vtemp[8];
      trainEff30[k] = vtemp[9];
      sig[k]        = vtemp[10];
      sep[k]        = vtemp[11];
      mname[k]      = vtemps;
   }

   // -----------------------------------------------------------------------
   // Second part of evaluation process
   // --> compute correlations among MVAs
   // --> count overlaps
   // -----------------------------------------------------------------------
   
   const Int_t nvar = methodsNoCuts.size();
   if (nvar > 1) {

      // needed for correlations
      Float_t  *fvec = new Float_t[nvar];
      Double_t *dvec = new Double_t[nvar];
      vector<Double_t> rvec;
      // for correlations
      TPrincipal* tpSig = new TPrincipal( nvar, "" );   
      TPrincipal* tpBkg = new TPrincipal( nvar, "" );   

      // set required tree branch references
      Int_t ivar = 0;
      vector<TString>* theVars = new vector<TString>;
      for (itrMethod = methodsNoCuts.begin(); itrMethod != methodsNoCuts.end(); itrMethod++, ivar++) {
         theVars->push_back( dynamic_cast<MethodBase*>(*itrMethod)->GetTestvarName() );
         rvec.push_back( dynamic_cast<MethodBase*>(*itrMethod)->GetSignalReferenceCut() );
         theVars->back().ReplaceAll( "MVA_", "" );
      }

      // for overlap study
      TMatrixD* overlapS = new TMatrixD( nvar, nvar );
      TMatrixD* overlapB = new TMatrixD( nvar, nvar );
      (*overlapS) *= 0; // init...
      (*overlapB) *= 0; // init...

      // loop over test tree
      // ANDREAS: how do you want to handle the overlap for methods with different datasets
      
      // also, I don't understand this loop
      DataSet * defDs = DefaultDataSetInfo().GetDataSet();
      defDs->SetCurrentType(Types::kTesting);
      for (Int_t ievt=0; ievt<defDs->GetNEvents(); ievt++) {
         Event * ev = defDs->GetEvent(ievt);

         // for correlations
         TMatrixD* theMat = 0;
         for (Int_t im=0; im<nvar; im++) dvec[im] = (Double_t)fvec[im];
         if (ev->IsSignal()) { tpSig->AddRow( dvec ); theMat = overlapS; }
         else           { tpBkg->AddRow( dvec ); theMat = overlapB; }

         // count overlaps
         for (Int_t im=0; im<nvar; im++) {
            for (Int_t jm=im; jm<nvar; jm++) {
               if ((dvec[im] - rvec[im])*(dvec[jm] - rvec[jm]) > 0) { 
                  (*theMat)(im,jm)++; 
                  if (im != jm) (*theMat)(jm,im)++;
               }
            }
         }
      }

      // renormalise overlap matrix
      (*overlapS) *= (1.0/defDs->GetNEvtSigTest());  // init...
      (*overlapB) *= (1.0/defDs->GetNEvtBkgdTest()); // init...
   
      tpSig->MakePrincipals();
      tpBkg->MakePrincipals();

      const TMatrixD* covMatS = tpSig->GetCovarianceMatrix();
      const TMatrixD* covMatB = tpBkg->GetCovarianceMatrix();
   
      const TMatrixD* corrMatS = gTools().GetCorrelationMatrix( covMatS );
      const TMatrixD* corrMatB = gTools().GetCorrelationMatrix( covMatB );

      // print correlation matrices
      if (corrMatS != 0 && corrMatB != 0) {

         fLogger << kINFO << Endl;
         fLogger << kINFO << "Inter-MVA correlation matrix (signal):" << Endl;
         gTools().FormattedOutput( *corrMatS, *theVars, fLogger );
         fLogger << kINFO << Endl;

         fLogger << kINFO << "Inter-MVA correlation matrix (background):" << Endl;
         gTools().FormattedOutput( *corrMatB, *theVars, fLogger );
         fLogger << kINFO << Endl;   
      }
      else fLogger << kWARNING << "<TestAllMethods> cannot compute correlation matrices" << Endl;

      // print overlap matrices
      fLogger << kINFO << "The following \"overlap\" matrices contain the fraction of events for which " << Endl;
      fLogger << kINFO << "the MVAs 'i' and 'j' have returned conform answers about \"signal-likeness\"" << Endl;
      fLogger << kINFO << "An event is signal-like, if its MVA output exceeds the following value:" << Endl;
      gTools().FormattedOutput( rvec, *theVars, "Method" , "Cut value", fLogger );
      fLogger << kINFO << "which correspond to the working point: eff(signal) = 1 - eff(background)" << Endl;

      // give notice that cut method has been excluded from this test
      if (nvar != (Int_t)fMethods.size()) 
         fLogger << kINFO << "Note: no correlations and overlap with cut method are provided at present" << Endl;

      fLogger << kINFO << Endl;
      fLogger << kINFO << "Inter-MVA overlap matrix (signal):" << Endl;
      gTools().FormattedOutput( *overlapS, *theVars, fLogger );
      fLogger << kINFO << Endl;
      
      fLogger << kINFO << "Inter-MVA overlap matrix (background):" << Endl;
      gTools().FormattedOutput( *overlapB, *theVars, fLogger );

      // cleanup
      delete tpSig;
      delete tpBkg;
      delete corrMatS;
      delete corrMatB;
      delete theVars;
      delete overlapS;
      delete overlapB;
      delete [] fvec;
      delete [] dvec;
   }

   // -----------------------------------------------------------------------
   // Third part of evaluation process
   // --> output
   // ----------------------------------------------------------------------- 

   fLogger << Endl;
   TString hLine = "-----------------------------------------------------------------------------";
   fLogger << kINFO << "Evaluation results ranked by best signal efficiency and purity (area)" << Endl;
   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << "MVA              Signal efficiency at bkg eff. (error):  |  Sepa-    Signifi-"   << Endl;
   fLogger << kINFO << "Methods:         @B=0.01    @B=0.10    @B=0.30    Area   |  ration:  cance:  "   << Endl;
   fLogger << kINFO << hLine << Endl;
   for (Int_t k=0; k<2; k++) {
      if (k == 1 && nmeth_used[k] > 0) {
         fLogger << kINFO << hLine << Endl;
         fLogger << kINFO << "Input Variables: " << Endl << hLine << Endl;
      }
      for (Int_t i=0; i<nmeth_used[k]; i++) {
         if (k == 1) mname[k][i].ReplaceAll( "Variable_", "" );
         fLogger << kINFO << Form("%-15s: %1.3f(%02i)  %1.3f(%02i)  %1.3f(%02i)  %1.3f  |  %1.3f    %1.3f",
                                  (const char*)mname[k][i], 
                                  eff01[k][i], Int_t(1000*eff01err[k][i]), 
                                  eff10[k][i], Int_t(1000*eff10err[k][i]), 
                                  eff30[k][i], Int_t(1000*eff30err[k][i]), 
                                  effArea[k][i], 
                                  sep[k][i], sig[k][i]) << Endl;
      }
   }
   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << Endl;
   fLogger << kINFO << "Testing efficiency compared to training efficiency (overtraining check)" << Endl;
   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << "MVA           Signal efficiency: from test sample (from traing sample) "   << Endl;
   fLogger << kINFO << "Methods:         @B=0.01             @B=0.10            @B=0.30   "   << Endl;
   fLogger << kINFO << hLine << Endl;
   for (Int_t k=0; k<2; k++) {
      if (k == 1 && nmeth_used[k] > 0) {
         fLogger << kINFO << hLine << Endl;
         fLogger << kINFO << "Input Variables: " << Endl << hLine << Endl;
      }
      for (Int_t i=0; i<nmeth_used[k]; i++) {
         if (k == 1) mname[k][i].ReplaceAll( "Variable_", "" );
         fLogger << kINFO << Form("%-15s: %1.3f (%1.3f)       %1.3f (%1.3f)      %1.3f (%1.3f)",
                                  (const char*)mname[k][i], 
                                  eff01[k][i],trainEff01[k][i], 
                                  eff10[k][i],trainEff10[k][i],
                                  eff30[k][i],trainEff30[k][i]) << Endl;
      }
   }
   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << Endl; 
   // JOERG
   fLogger << kINFO << "TODO Implement writing of Test Tree to file" << Endl;
   //    Data().BaseRootDir()->cd();
   //    Data().GetTestTree()->Write("",TObject::kOverwrite);
}

