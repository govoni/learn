// per compilare su mac devo modificare setup.sh
// togliendo il controllo su ROOTSYS

// prepare a TMVAFactory
/*
 c++ -o unit05 `root-config --glibs --cflags` \
     -lTMVA -L/Users/govoni/private/job/learn/root/TMVA/TMVA/lib \
     -lm unit05.cpp 
*/

  //PG FIXME usa TChain


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
  signalTree.Add ("data/Zee_1.root") ;
  std::cout << "READ " << signalTree.GetEntries () << " signal events\n" ;
  TMVAtest->AddSignalTree (&signalTree, 1) ;  

  //PG get the bkg and deliver it to the TMVA factory
  
  TChain bkgTree ("shrinked") ;
  bkgTree.Add ("data/WW_1.root") ;
  std::cout << "READ " << bkgTree.GetEntries () << " bkg events\n" ;
  TMVAtest->AddBackgroundTree (&bkgTree, 1) ;  

  //PG get the training and test samples and deliver them to the TMVA factory

  TChain signalTrainTree ("shrinked") ;
  signalTrainTree.Add ("data/Zee_0.root") ;
  std::cout << "READ " << signalTrainTree.GetEntries () << " signal train events\n" ;
  
  TChain bkgTrainTree ("shrinked") ;
  bkgTrainTree.Add ("data/WW_0.root") ;
  std::cout << "READ " << bkgTrainTree.GetEntries () << " bkg train events\n" ;
  
  TMVAtest->SetInputTrees (signalTrainTree.GetTree (), bkgTrainTree.GetTree (), 1., 1.) ;  

  //PG variables to be used for the selection
  //PG must be defined in the TTrees
  
  TMVAtest->AddVariable ("vars.nele", 'I') ;
  TMVAtest->AddVariable ("vars.nmu" , 'I') ;

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
  
  //PG prepare the classifier
  
  //PG cut-based, default params
  TMVAtest->BookMethod (TMVA::Types::kCuts, "Cuts") ;
  
  TMVAtest->TrainAllMethods () ;
  TMVAtest->TestAllMethods () ;
  TMVAtest->EvaluateAllMethods () ;
 


  delete TMVAtest ;
  delete outputfile ;
}