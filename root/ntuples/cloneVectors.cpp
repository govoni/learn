/*
rootcint -f dict.cpp -c LinkDef.h   
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o cloneVectors dict.cpp cloneVectors.cpp

and how to test it:

root [4] ntu->Draw ("value")
root [6] ntu->Draw ("value","2")
root [7] ntu->Draw ("value","pesi[0]")
root [8] ntu->Draw ("value","pesi[1]")

*/

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std ;

struct clonedVars
{
    // tree definition
    TFile* m_outputRootFile;
    TTree* m_reducedTree;

    int newVal ;
	vector<float> pesi ;
	vector<float> * p_pesi ;

} ;

void SetOriginalBranches (clonedVars & v, TTree * t)
    {
      v.p_pesi = new std::vector<float> ;
      t->SetBranchAddress ("pesi", &v.p_pesi) ;
	}

void AddNewBranches (clonedVars & v, TTree * t)
    {
      t->Branch ("newVal", &v.newVal, "newVal/I") ;
	}

void InitializeNewTree (clonedVars & v)
    {
      v.newVal = -1. ;
	}

void Close (clonedVars & v)
{
  // save tree
  v.m_outputRootFile->cd () ;
  v.m_reducedTree->Write () ;
  v.m_outputRootFile->Close () ;
}


int main ()
    {
	  // Open tree
	  std::string treeName = "ntu";
	  TChain * chain = new TChain (treeName.c_str ()) ;
	  chain->Add ("saveVectors.root") ;

	  TFile* outputRootFile = new TFile("cloneVectors.root", "RECREATE");
      outputRootFile->cd () ;

	  clonedVars v ;
	  SetOriginalBranches (v, chain) ;
  
      TTree * clonedTree = chain->CloneTree (0) ;
      clonedTree->SetName ("ntu_1") ;
      AddNewBranches (v, clonedTree) ;   
  

	  for (int entry = 0 ; entry < chain->GetEntries () ; ++entry)
	    {
	      chain -> GetEntry (entry) ;
	      cout << "reading entry " << entry << endl ;   
          InitializeNewTree (v) ;
          v.newVal = 3 * entry ;
          clonedTree->Fill () ;
	    }  

      clonedTree->AutoSave () ;
	  outputRootFile->Close () ;
	  delete outputRootFile ;

	}