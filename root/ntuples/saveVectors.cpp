/*
rootcint -f dict.cpp -c LinkDef.h   
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o saveVectors dict.cpp saveVectors.cpp

and how to test it:

root [4] ntu->Draw ("value")
root [6] ntu->Draw ("value","2")
root [7] ntu->Draw ("value","pesi[0]")
root [8] ntu->Draw ("value","pesi[1]")

*/

#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <string>

using namespace std ;

struct vars
{
    // tree definition
    TFile* m_outputRootFile;
    TTree* m_reducedTree;

	float value ;
	vector<float> pesi ;
	vector<float> * p_pesi ;

} ;

void initNtuple (vars& v, const std::string& outputRootFileName)
{
  v.m_outputRootFile = new TFile (outputRootFileName.c_str(), "RECREATE") ;  
  
  v.m_reducedTree = new TTree("ntu", "ntu");
  v.m_reducedTree -> SetDirectory(v.m_outputRootFile);
  
  v.m_reducedTree -> Branch("value", &v.value, "value/F");
  v.m_reducedTree -> Branch("pesi", "std::vector<float>", &v.p_pesi);

}  


void FillTree (vars & v)
{
  v.m_reducedTree -> Fill();
}



void Close (vars & v)
{
  // save tree
  v.m_outputRootFile -> cd();
  v.m_reducedTree -> Write();
  v.m_outputRootFile -> Close();
}




int main ()
    {
      vars v ;
      initNtuple (v, "saveVectors.root") ; 	
      for (int i = 0 ; i < 5 ; ++i)
        {
            v.value = i ;
            vector<float> * l_pesi = new vector<float>; 
            l_pesi->push_back (1.) ;
            l_pesi->push_back (2.) ;
            v.pesi = *l_pesi ;
            v.p_pesi = &(v.pesi) ;
            FillTree (v) ;
        }
      Close (v) ;	
	
	}