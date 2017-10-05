// @(#)root/tmva $Id: MethodTMlpANN.cxx,v 1.8 2008/07/12 18:14:27 stelzer Exp $ 
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 
/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodTMlpANN                                                         *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Xavier Prudent  <prudent@lapp.in2p3.fr>  - LAPP, France                   *
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
/* Begin_Html

  This is the TMVA TMultiLayerPerceptron interface class. It provides the 
  training and testing the ROOT internal MLP class in the TMVA framework.<be>

  Available learning methods:<br>
  <ul>
  <li>Stochastic      </li> 
  <li>Batch           </li>
  <li>SteepestDescent </li>
  <li>RibierePolak    </li>
  <li>FletcherReeves  </li>
  <li>BFGS            </li>
  </ul>
End_Html */
//
//  See the TMultiLayerPerceptron class description
//  for details on this ANN.
//
//_______________________________________________________________________

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "TLeaf.h"
#include "TEventList.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TMultiLayerPerceptron.h"
#include "Config.h"

#include "TMVA/MethodTMlpANN.h"

#include "TMVA/ClassifierFactory.h"
#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif

// some additional TMlpANN options
const Bool_t EnforceNormalization__=kTRUE;
#if ROOT_VERSION_CODE > ROOT_VERSION(5,13,06)
const TMultiLayerPerceptron::ELearningMethod LearningMethod__= TMultiLayerPerceptron::kStochastic;
// const TMultiLayerPerceptron::ELearningMethod LearningMethod__= TMultiLayerPerceptron::kBatch;
#else
const TMultiLayerPerceptron::LearningMethod LearningMethod__= TMultiLayerPerceptron::kStochastic;
// const TMultiLayerPerceptron::LearningMethod LearningMethod__= TMultiLayerPerceptron::kBatch;
#endif

REGISTER_METHOD(TMlpANN)

ClassImp(TMVA::MethodTMlpANN)

//_______________________________________________________________________
TMVA::MethodTMlpANN::MethodTMlpANN( TString jobName, TString methodTitle, DataSetInfo& theData, 
                                    TString theOption, TDirectory* theTargetDir)
   : TMVA::MethodBase(jobName, methodTitle, theData, theOption, theTargetDir  ),
     fMLP(0),
     fLearningMethod( "" )
{
   // standard constructor 
   InitTMlpANN();
   
   // interpretation of configuration option string
   SetConfigName( TString("Method") + GetMethodName() );
   DeclareOptions();
   ParseOptions();
   ProcessOptions();  
}

//_______________________________________________________________________
TMVA::MethodTMlpANN::MethodTMlpANN( DataSetInfo& theData, 
                                    TString theWeightFile,  
                                    TDirectory* theTargetDir )
   : TMVA::MethodBase( theData, theWeightFile, theTargetDir ),
     fMLP(0),
     fLearningMethod( "" )
{
   // constructor to calculate the TMlpANN-MVA from previously generatad 
   // weigths (weight file)
   InitTMlpANN();

   DeclareOptions();
}

//_______________________________________________________________________
void TMVA::MethodTMlpANN::InitTMlpANN( void )
{
   // default initialisations
   SetMethodName( "TMlpANN" );
   SetMethodType( TMVA::Types::kTMlpANN );
   SetTestvarName();
}

//_______________________________________________________________________
TMVA::MethodTMlpANN::~MethodTMlpANN( void )
{
   // destructor
//   std::cout << "DEL: " << fMLP << std::endl;
//   if (fMLP != 0) { delete fMLP; fMLP = 0; }
}
 
//_______________________________________________________________________
void TMVA::MethodTMlpANN::CreateMLPOptions( TString layerSpec )
{
   // translates options from option string into TMlpANN language

   fHiddenLayer = ":";

   while (layerSpec.Length()>0) {
      TString sToAdd="";
      if (layerSpec.First(',')<0) {
         sToAdd = layerSpec;
         layerSpec = "";
      } 
      else {
         sToAdd = layerSpec(0,layerSpec.First(','));
         layerSpec = layerSpec(layerSpec.First(',')+1,layerSpec.Length());
      }
      int nNodes = 0;
      if (sToAdd.BeginsWith("N")) { sToAdd.Remove(0,1); nNodes = GetNvar(); }
      nNodes += atoi(sToAdd);
      fHiddenLayer = Form( "%s%i:", (const char*)fHiddenLayer, nNodes );
   }

   // set input vars
   std::vector<TString>::iterator itrVar    = (*fInputVars).begin();
   std::vector<TString>::iterator itrVarEnd = (*fInputVars).end();
   fMLPBuildOptions = "";
   for (; itrVar != itrVarEnd; itrVar++) {
      if (EnforceNormalization__) fMLPBuildOptions += "@";
      TString myVar = *itrVar; ;
      fMLPBuildOptions += myVar;
      fMLPBuildOptions += ",";
   }
   fMLPBuildOptions.Chop(); // remove last ","

   // prepare final options for MLP kernel
   fMLPBuildOptions += fHiddenLayer;
   fMLPBuildOptions += "type";

   fLogger << kINFO << "Use " << fNcycles << " training cycles" << Endl;
   fLogger << kINFO << "Use configuration (nodes per hidden layer): " << fHiddenLayer << Endl;  
}

//_______________________________________________________________________
void TMVA::MethodTMlpANN::DeclareOptions() 
{
   // define the options (their key words) that can be set in the option string 
   // know options:
   // NCycles       <integer>    Number of training cycles (too many cycles could overtrain the network) 
   // HiddenLayers  <string>     Layout of the hidden layers (nodes per layer)
   //   * specifiactions for each hidden layer are separated by commata
   //   * for each layer the number of nodes can be either absolut (simply a number)
   //        or relative to the number of input nodes to the neural net (N)
   //   * there is always a single node in the output layer 
   //   example: a net with 6 input nodes and "Hiddenlayers=N-1,N-2" has 6,5,4,1 nodes in the 
   //   layers 1,2,3,4, repectively 
   DeclareOptionRef( fNcycles    = 3000,      "NCycles",      "Number of training cycles" );
   DeclareOptionRef( fLayerSpec  = "N-1,N-2", "HiddenLayers", "Specification of hidden layer architecture (N stands for number of variables; any integers may also be used)" );
   
   DeclareOptionRef( fValidationFraction = 0.5, "ValidationFraction", 
                     "Fraction of events in training tree used for cross validation" );

   DeclareOptionRef( fLearningMethod = "Stochastic", "LearningMethod", "Learning method" );
   AddPreDefVal( TString("Stochastic") );
   AddPreDefVal( TString("Batch") );
   AddPreDefVal( TString("SteepestDescent") );
   AddPreDefVal( TString("RibierePolak") );
   AddPreDefVal( TString("FletcherReeves") );
   AddPreDefVal( TString("BFGS") );
}

//_______________________________________________________________________
void TMVA::MethodTMlpANN::ProcessOptions() 
{
   // builds the neural network as specified by the user

   MethodBase::ProcessOptions();

   CreateMLPOptions(fLayerSpec);

   // Here we create a dummy tree necessary to create 
   // a minimal NN
   // this NN gets recreated before training
   // but can be used for testing (see method GetMvaVal() )
   static Double_t* d = new Double_t[Data()->GetNVariables()] ;
   static Int_t   type;

   gROOT->cd();
   TTree * dummyTree = new TTree("dummy","Empty dummy tree", 1);
   for (UInt_t ivar = 0; ivar<Data()->GetNVariables(); ivar++) {
      TString vn = DataInfo().GetInternalVarName(ivar);
      dummyTree->Branch(Form("%s",vn.Data()), d+ivar, Form("%s/D",vn.Data()));
   }
   dummyTree->Branch("type", &type, "type/I");

   if (fMLP != 0) { delete fMLP; fMLP = 0; }
   fMLP = new TMultiLayerPerceptron( fMLPBuildOptions.Data(), dummyTree );
}

//_______________________________________________________________________
Double_t TMVA::MethodTMlpANN::GetMvaValue()
{
   // calculate the value of the neural net for the current event 
   const Event * ev = GetEvent();
   static Double_t* d = new Double_t[Data()->GetNVariables()];
   for (UInt_t ivar = 0; ivar<Data()->GetNVariables(); ivar++) {
      d[ivar] = (Double_t)ev->GetVal(ivar);
   }
   Double_t mvaVal = fMLP->Evaluate(0,d);
   return mvaVal;
}

//_______________________________________________________________________
void TMVA::MethodTMlpANN::Train( void )
{
   // performs TMlpANN training
   // available learning methods:
   //
   //       TMultiLayerPerceptron::kStochastic      
   //       TMultiLayerPerceptron::kBatch           
   //       TMultiLayerPerceptron::kSteepestDescent 
   //       TMultiLayerPerceptron::kRibierePolak    
   //       TMultiLayerPerceptron::kFletcherReeves  
   //       TMultiLayerPerceptron::kBFGS            
   //
//   if (!CheckSanity()) fLogger << kFATAL << "<Train> sanity check failed" << Endl;
  
   fLogger << kVERBOSE << "Option string: " << GetOptions() << Endl;

   // TMultiLayerPerceptron wants test and training tree at once
   // so merge the training and testing trees from the MVA factory first:



   Int_t type;
   Float_t weight;
   const Long_t basketsize = 128000;
   Float_t* vArr = new Float_t[GetNvar()];

   TTree *localTrainingTree = new TTree("TMLPtrain","local training tree for TMLP");
   localTrainingTree->Branch( "type",       &type,        "type/I",        basketsize );
   localTrainingTree->Branch( "weight",     &weight,      "weight/F",      basketsize );
   
  for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      const char* myVar = GetInternalVarName(ivar).Data();
      localTrainingTree->Branch( myVar, &vArr[ivar], Form("Var%02i/F", ivar), basketsize );
   }

   for (UInt_t ievt=0; ievt<Data()->GetNEvents(); ievt++ ){
      const Event *ev = GetEvent(ievt);
      for( UInt_t i=0; i<GetNvar(); i++ ){
         vArr[i] = ev->GetVal( i );
      }
      type = ev->Type();
      weight = ev->GetWeight();
      localTrainingTree->Fill();
   }

   // These are the event lists for the mlp train method
   // first events in the tree are for training
   // the rest for internal testing (cross validation)...
   // NOTE: the training events are ordered: first part is signal, second part background
   TString trainList = "Entry$<";
   trainList += 1.0-fValidationFraction;
   trainList += "*";
   trainList += (Int_t)Data()->GetNEvtSigTrain();
   trainList += " || (Entry$>";
   trainList += (Int_t)Data()->GetNEvtSigTrain();
   trainList += " && Entry$<";
   trainList += (Int_t)(Data()->GetNEvtSigTrain() + (1.0 - fValidationFraction)*Data()->GetNEvtBkgdTrain());
   trainList += ")";
   TString testList  = TString("!(") + trainList + ")";

   // print the requirements
   fLogger << kINFO << "Requirement for training   events: \"" << trainList << "\"" << Endl;
   fLogger << kINFO << "Requirement for validation events: \"" << testList << "\"" << Endl;

   // localTrainingTree->Print();

   // create NN 
   if (fMLP != 0) { delete fMLP; fMLP = 0; }
   fMLP = new TMultiLayerPerceptron( fMLPBuildOptions.Data(), 
                                     localTrainingTree,
                                     trainList,
                                     testList );
  
   // set learning method
#if ROOT_VERSION_CODE > ROOT_VERSION(5,13,06)
   TMultiLayerPerceptron::ELearningMethod learningMethod = TMultiLayerPerceptron::kStochastic; 
#else
   TMultiLayerPerceptron::LearningMethod  learningMethod = TMultiLayerPerceptron::kStochastic; 
#endif

   fLearningMethod.ToLower();
   if      (fLearningMethod == "stochastic"      ) learningMethod = TMultiLayerPerceptron::kStochastic;
   else if (fLearningMethod == "batch"           ) learningMethod = TMultiLayerPerceptron::kBatch;
   else if (fLearningMethod == "steepestdescent" ) learningMethod = TMultiLayerPerceptron::kSteepestDescent;
   else if (fLearningMethod == "ribierepolak"    ) learningMethod = TMultiLayerPerceptron::kRibierePolak;
   else if (fLearningMethod == "fletcherreeves"  ) learningMethod = TMultiLayerPerceptron::kFletcherReeves;
   else if (fLearningMethod == "bfgs"            ) learningMethod = TMultiLayerPerceptron::kBFGS;
   else {
      fLogger << kFATAL << "Unknown Learning Method: \"" << fLearningMethod << "\"" << Endl;
   }
   fMLP->SetLearningMethod( learningMethod );

   // train NN
   fMLP->Train(fNcycles, "text,update=50" );

   // write weights to File;
   // this is not nice, but fMLP gets deleted at the end of Train()
   delete localTrainingTree;

}

//_______________________________________________________________________
void  TMVA::MethodTMlpANN::WriteWeightsToStream( ostream & o ) const
{
   // write weights to stream

   // since the MLP can not write to stream and provides no access to its content
   // except through DumpWeights(filename), we 
   // 1st: dump the weights
   fMLP->DumpWeights( "weights/TMlp.nn.weights.temp" );
   // 2nd: read them back
   std::ifstream inf( "weights/TMlp.nn.weights.temp" );
   // 3rd: write them to the stream
   o << inf.rdbuf();
   inf.close();
   // here we can delete the temporary file
   // how?
}
  
//_______________________________________________________________________
void  TMVA::MethodTMlpANN::ReadWeightsFromStream( istream & istr )
{
   // read weights from stream
   // since the MLP can not read from the stream, we
   // 1st: write the weights to temporary file
   std::ofstream fout( "weights/TMlp.nn.weights.temp" );
   fout << istr.rdbuf();
   fout.close();
   // 2nd: load the weights from the temporary file into the MLP
   // the MLP is already build
   fLogger << kINFO << "Load TMLP weights" << Endl;
   fMLP->LoadWeights( "weights/TMlp.nn.weights.temp" );
   // here we can delete the temporary file
   // how?
}

//_______________________________________________________________________
void TMVA::MethodTMlpANN::MakeClassSpecific( std::ostream& fout, const TString& className ) const
{
   // write specific classifier response
   fout << "   // not implemented for class: \"" << className << "\"" << std::endl;
   fout << "};" << std::endl;
}

//_______________________________________________________________________
void TMVA::MethodTMlpANN::GetHelpMessage() const
{
   // get help message text
   //
   // typical length of text line: 
   //         "|--------------------------------------------------------------|"
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Short description:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "This feed-forward multilayer perceptron neural network is the " << Endl;
   fLogger << "standard implementation distributed with ROOT (class TMultiLayerPerceptron)." << Endl;
   fLogger << Endl;
   fLogger << "Detailed information is available here:" << Endl;
   if (gConfig().WriteOptionsReference()) {
      fLogger << "<a href=\"http://root.cern.ch/root/html/TMultiLayerPerceptron.html\">";
      fLogger << "http://root.cern.ch/root/html/TMultiLayerPerceptron.html</a>" << Endl;
   }
   else fLogger << "http://root.cern.ch/root/html/TMultiLayerPerceptron.html" << Endl;
   fLogger << Endl;
}
