/*COMPILO 
c++ -o prepare4VBF -lm `root-config --cflags --libs --glibs` prepare4VBF.cpp 
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "TNtuple.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TGraph.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TText.h"
#include "TLegend.h"


struct lepton 
{
    lepton () : m_flav (-1) , m_index (-1) {} ;
    lepton (TLorentzVector * kine, int flav, int index) :
      m_kine (kine),
      m_flav (flav),
      m_index (index)
      {}

    TLorentzVector * m_kine ;
    int m_flav ;
    int m_index ;
} ;           


//  ------------------------------------------------------------


struct lessThan : public std::binary_function<lepton, lepton, bool>
{
  bool operator ()(const lepton & lep1, const lepton & lep2)
    {
      return lep1.m_kine->Pt () < lep2.m_kine->Pt () ;
    }
} ;


//  ------------------------------------------------------------


double deltaPhi (double phi1, double phi2) ;


//  ========== M A I N    P R O G R A M =========================


int main (int argc, char *argv[])
{ 
  if (argc < 3)
    {
      std::cout << "usage: "
                << argv[0] 
                << " prepare4VBF input output"
                << std::endl ;
      return 1 ;
    }
  std::string inputRootFile = argv[1] ;

   typedef struct {
      Float_t j1eta, j1phi, j1p ;
      Float_t j2eta, j2phi, j2p ;
      Float_t Deta, Dphi, Minv ;
   } VARS;

  static VARS vars ;

  TTree * shrinked = new TTree ("shrinked","variables for TMVA analysis") ;
  shrinked->Branch ("vars",&vars,"j1eta/F:j1phi/F:j1p/F:j2eta/F:j2phi/F:j2p/F:Deta/F:Dphi/F:Minv/F") ;

  TChain tree ("ntpla/VBFSimpleTree") ;
  tree.Add (argv[1]) ;

  TClonesArray * tagJets = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("tagJets", &tagJets) ;
  
  int nentries = (int) tree.GetEntries () ;

  //PG loop over the events
  for (int evt = 0 ; evt < nentries ; ++evt)
    {
      tree.GetEntry (evt) ;
      if (evt % 1000 == 0) std::cout << "reading evt " << evt << "\n" ;

      if (tagJets->GetEntries () < 2) continue ;
      TLorentzVector * primoTAG = (TLorentzVector*) (tagJets->At (0)) ;
      TLorentzVector * secondoTAG = (TLorentzVector*) (tagJets->At (1)) ; 
      //PG get the first two in pt
      if (primoTAG->Pt () < secondoTAG->Pt ())
        {
          primoTAG = (TLorentzVector*) (tagJets->At (1)) ;
          secondoTAG = (TLorentzVector*) (tagJets->At (0)) ; 
        }

      TLorentzVector sumTAG = *primoTAG + *secondoTAG ;

      vars.j1eta = primoTAG->Eta () ;
      vars.j1phi = primoTAG->Phi () ;
      vars.j1p = primoTAG->Mag () ;
      vars.j2eta = secondoTAG->Eta () ;
      vars.j2phi = secondoTAG->Phi () ;
      vars.j2p = secondoTAG->Mag () ;
      vars.Deta = fabs (primoTAG->Eta () - secondoTAG->Eta ()) ;
      vars.Dphi = deltaPhi (primoTAG->Phi (), secondoTAG->Phi ()) ;
      vars.Minv = sumTAG.M () ;
      
      shrinked->Fill () ;

    } //PG loop over the events

  TFile output (argv[2],"recreate") ;
  output.cd () ;
  shrinked->Write () ;
  output.Close () ;
  return 0 ;

}


//  ------------------------------------------------------------


double 
deltaPhi (double phi1, double phi2)
  { 
    double deltaphi = fabs (phi1 - phi2) ;
    if (deltaphi > 6.283185308) deltaphi -= 6.283185308 ;
    if (deltaphi > 3.141592654) deltaphi = 6.283185308 - deltaphi ;
    return deltaphi ;
  }


