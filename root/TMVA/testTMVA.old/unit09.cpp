// per compilare su mac devo modificare setup.sh
// togliendo il controllo su ROOTSYS

// prepare a TMVAFactory
/*
 c++ -o unit09 `root-config --glibs --cflags` \
     -lTMVA -L/Users/govoni/private/job/learn/root/TMVA/TMVA/lib \
     -lm unit09.cpp 
*/


#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TChain.h"

#include "TMVA/Factory.h"


int main ()
{
  TFile * outputfile = TFile::Open ("outputTMVA.root","RECREATE");
  TMVA::Factory * TMVAtest = new TMVA::Factory ("TMVAtest", outputfile, "S") ;

  //PG get the signal and deliver it to the TMVA factory
  
  TChain signalTree ("shrinked") ;
  signalTree.Add ("data/H160_VBF.root") ;
  std::cout << "READ " << signalTree.GetEntries () << " signal events\n" ;
  TMVAtest->AddSignalTree (&signalTree, 1) ;  

  //PG get the bkg and deliver it to the TMVA factory
  
  TChain bkgTree ("shrinked") ;
  bkgTree.Add ("data/H160_NOVBF.root") ;
  std::cout << "READ " << bkgTree.GetEntries () << " bkg events\n" ;
  TMVAtest->AddBackgroundTree (&bkgTree, 1) ;  

  //PG get the training and test samples and deliver them to the TMVA factory

  TChain signalTrainTree ("shrinked") ;
  signalTrainTree.Add ("data/H150_VBF.root") ;
  std::cout << "READ " << signalTrainTree.GetEntries () << " signal train events\n" ;
  
  TChain bkgTrainTree ("shrinked") ;
  bkgTrainTree.Add ("data/H150_NOVBF.root") ;
  std::cout << "READ " << bkgTrainTree.GetEntries () << " bkg train events\n" ;

  // PG this is useless!!  
  TMVAtest->SetInputTrees (signalTrainTree.GetTree (), bkgTrainTree.GetTree (), 1., 1.) ;  

  //PG variables to be used for the selection
  //PG must be defined in the TTrees
  
  TMVAtest->AddVariable ("vars.Deta", 'F') ;
  TMVAtest->AddVariable ("vars.Dphi" , 'F') ;
  TMVAtest->AddVariable ("vars.Minv" , 'F') ;

  int signalNumTrain = signalTrainTree.GetEntries () * 4 / 5 ;
  int bkgNumTrain = bkgTrainTree.GetEntries () * 4 / 5 ;
  int signalNumTest = signalTrainTree.GetEntries () - signalNumTrain ;
  int bkgNumTest = bkgTrainTree.GetEntries () - bkgNumTrain ;
  char trainOptions[120] ;
  sprintf (trainOptions,"NSigTrain=%d:NBkgTrain=%d:NSigTest=%d:NBkgTest=%d",
           signalNumTrain, bkgNumTrain,
           signalNumTest, bkgNumTest) ;
  sprintf (trainOptions,"NSigTrain=%d:NBkgTrain=%d:NSigTest=%d:NBkgTest=%d",
           0,0,0,0) ;
  std::cout << "TRAINING CONFIGURATION : " << trainOptions << "\n" ;
  TMVAtest->PrepareTrainingAndTestTree ("",trainOptions) ;
  
  //PG prepare the classifiers
  //PG ------------------------

  //PG cut-based, default params
  TMVAtest->BookMethod (TMVA::Types::kCuts, "Cuts") ;
  
  //PG multi layer perceptron
  TMVAtest->BookMethod (TMVA::Types::kMLP, "MLP", 
                        "H:!V:!Normalise:NeuronType=tanh:NCycles=200:HiddenLayers=N+1,N:TestRate=5");
  
  TMVAtest->TrainAllMethods () ;
  TMVAtest->TestAllMethods () ;
  TMVAtest->EvaluateAllMethods () ;
 
  delete TMVAtest ;
  delete outputfile ;
}
