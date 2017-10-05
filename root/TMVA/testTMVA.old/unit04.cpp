// per compilare su mac devo modificare setup.sh
// togliendo il controllo su ROOTSYS

// prepare a TMVAFactory
/*
 c++ -o unit04 `root-config --glibs --cflags` \
     -lTMVA -L/Users/govoni/private/job/learn/root/TMVA/TMVA/lib \
     -lm unit04.cpp 
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
  
  TChain signalTree ("ntpla/VBFSimpleTree") ;
  signalTree.Add ("/Users/govoni/data/ntuplesHWW/ZZ_incl/NTUPLE_ZZ_incl_output_0.root") ;
  signalTree.Add ("/Users/govoni/data/ntuplesHWW/ZZ_incl/NTUPLE_ZZ_incl_output_1.root") ;
  std::cout << "READ " << signalTree.GetEntries () << " signal events\n" ;
  TMVAtest->AddSignalTree (&signalTree, 1) ;  

  //PG get the bkg and deliver it to the TMVA factory
  
  TChain bkgTree ("ntpla/VBFSimpleTree") ;
  bkgTree.Add ("/Users/govoni/data/ntuplesHWW/WW_incl/NTUPLE_WW_incl_output_?.root") ;
  std::cout << "READ " << bkgTree.GetEntries () << " bkg events\n" ;
  TMVAtest->AddBackgroundTree (&bkgTree, 1) ;  

  //PG get the training and test samples and deliver them to the TMVA factory

  TChain signalTrainTree ("ntpla/VBFSimpleTree") ;
  signalTrainTree.Add ("/Users/govoni/data/ntuplesHWW/ZZ_incl/NTUPLE_ZZ_incl_output_2.root") ;
  signalTrainTree.Add ("/Users/govoni/data/ntuplesHWW/ZZ_incl/NTUPLE_ZZ_incl_output_3.root") ;
  std::cout << "READ " << signalTrainTree.GetEntries () << " signal train events\n" ;
  
  TChain bkgTrainTree ("ntpla/VBFSimpleTree") ;
  bkgTrainTree.Add ("/Users/govoni/data/ntuplesHWW/WW_incl/NTUPLE_WW_incl_output_1?.root") ;
  std::cout << "READ " << bkgTrainTree.GetEntries () << " bkg train events\n" ;
  
  TMVAtest->SetInputTrees (signalTrainTree.GetTree (), bkgTrainTree.GetTree (), 1., 1.) ;  

  //PG variables to be used for the selection
  //PG must be defined in the TTrees
  
  TMVAtest->AddVariable ("nEle", 'I') ;
  TMVAtest->AddVariable ("nMu" , 'I') ;

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

  delete TMVAtest ;
  delete outputfile ;
}