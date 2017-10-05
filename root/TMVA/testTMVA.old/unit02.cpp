// per compilare su mac devo modificare setup.sh
// togliendo il controllo su ROOTSYS

// prepare a TMVAFactory
/*
 c++ -o unit02 `root-config --glibs --cflags` \
     -lTMVA -L/Users/govoni/private/job/learn/root/TMVA/TMVA/lib \
     -lm unit02.cpp 
*/

#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

#include "TMVA/Factory.h"


int main ()
{
  TFile * outputfile = TFile::Open ("outputTMVA.root","RECREATE");
  TMVA::Factory * TMVAtest = new TMVA::Factory ("TMVAtest", outputfile, "S") ;

  //PG get the signal and deliver it to the TMVA factory
  TFile signalFile ("/Users/govoni/data/ntuplesHWW/H160/NTUPLE_H160_output_0.root") ;
  TTree * signalTree = (TTree *) signalFile.Get ("ntpla/VBFSimpleTree") ;
  std::cout << "READ " << signalTree->GetEntries () << " signal events\n" ;
  TMVAtest->AddSignalTree (signalTree,1) ;  

  //PG get the bkg and deliver it to the TMVA factory
  TFile bkg1File ("/Users/govoni/data/ntuplesHWW/WW_incl/NTUPLE_WW_incl_output_0.root") ;
  TTree * bkg1Tree = (TTree *) bkg1File.Get ("ntpla/VBFSimpleTree") ;
  std::cout << "READ " << bkg1Tree->GetEntries () << " bkg1 events\n" ;
  TMVAtest->AddBackgroundTree (bkg1Tree,1) ;  

  delete TMVAtest ;
  delete outputfile ;
}