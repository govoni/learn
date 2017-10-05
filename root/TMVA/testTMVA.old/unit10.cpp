
// prepare a TMVAFactory starting from a tntuple
/*
 c++ -o unit10 `root-config --glibs --cflags` \
     -lTMVA -L/Users/govoni/private/job/learn/root/TMVA/TMVA/lib \
     -lm unit10.cpp 
*/


#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TChain.h"

#include "TMVA/Factory.h"
#include "TNtuple.h"


void fillNtuple (TNtuple * nt, float mean) 
{
  float x = 0. ;
  for (int i = 0 ; i < 500 ; ++i) 
    {
      x = gRandom->Gaus (mean, 1) ;
      
      nt->Fill (x) ;
    }
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int main ()
{
  TFile * outputfile = TFile::Open ("outputTMVA.root","RECREATE");
  TMVA::Factory * TMVAtest = new TMVA::Factory ("TMVAtest", outputfile, "V") ;

  //PG get the signal and deliver it to the TMVA factory
  TNtuple * nt_sig = new TNtuple ("NT", "signal", "x") ;
  fillNtuple (nt_sig, 1.) ;
  TMVAtest->AddSignalTree (nt_sig, 1.) ;  

  TNtuple * nt_bkg = new TNtuple ("NT", "backgr", "") ;
  fillNtuple (nt_bkg, 3.) ;
  TMVAtest->AddBackgroundTree (nt_bkg, 1.) ;  

  TMVAtest->AddVariable ("x", 'F') ;

  char trainOptions[120] ;
  sprintf (trainOptions,"nTrain_Signal=%d:nTrain_Background=%d:nTest_Signal=%d:nTest_Background=%d:SplitMode=Random",
           0,0,0,0) ;
  TMVAtest->PrepareTrainingAndTestTree ("",trainOptions) ;
  
  //PG prepare the classifiers
  //PG ------------------------

  //PG cut-based, default params
  TMVAtest->BookMethod (TMVA::Types::kCuts, "Cuts") ;
  
  TMVAtest->TrainAllMethods () ;
  TMVAtest->TestAllMethods () ;
  TMVAtest->EvaluateAllMethods () ;
 
  delete TMVAtest ;
  delete outputfile ;
}
