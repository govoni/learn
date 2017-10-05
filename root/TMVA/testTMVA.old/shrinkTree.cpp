/*COMPILO 
c++ -o shrinkTree -lm `root-config --cflags --libs --glibs` shrinkTree.cpp 
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
                << " shrinkTree input output"
                << std::endl ;
      return 1 ;
    }
  std::string inputRootFile = argv[1] ;

   typedef struct {
      Int_t nele,nmu ;
      Float_t minv,met,dphi ;
   } VARS;

  static VARS vars ;

  TTree * shrinked = new TTree ("shrinked","variables for TMVA analysis") ;
  shrinked->Branch ("vars",&vars,"nele/I:nmu/I:minv/F:met/F:dphi/F") ;

  TChain tree ("ntpla/VBFSimpleTree") ;
  tree.Add (argv[1]) ;

  TClonesArray * tagJets = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("tagJets", &tagJets) ;
  TClonesArray * otherJets = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("otherJets", &otherJets) ;
  TClonesArray * electrons = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("electrons", &electrons) ;
  TClonesArray * muons = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("muons", &muons) ;
  TClonesArray * MET = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("MET", &MET) ;
  TClonesArray * tracks = new TClonesArray ("TLorentzVector") ;
  tree.SetBranchAddress ("tracks", &tracks) ;
  
  int EleId[100];
  float IsolEleSumPt[100];
  int nEle;
  tree.SetBranchAddress ("nEle", &nEle) ;
  tree.SetBranchAddress ("EleId",EleId ) ;
  tree.SetBranchAddress ("IsolEleSumPt",IsolEleSumPt ) ;

  float IsolMuSumPt[100];
  int nMu ;
  tree.SetBranchAddress ("nMu", &nMu) ;
  tree.SetBranchAddress ("IsolMuSumPt",IsolMuSumPt ) ;

  int nentries = (int) tree.GetEntries () ;

  //PG loop over the events
  //  int nentries = 100 ;
  for (int evt = 0 ; evt < nentries ; ++evt)
    {
      tree.GetEntry (evt) ;
      if (evt % 1000 == 0) std::cout << "reading evt " << evt << "\n" ;

      double dphi = -1 ;
  
      std::vector<lepton> leptons ;
            
      //PG pour electrons into leptons collection
      //PG ---------------------------------------

      //PG loop over electrons
      for (int iele = 0; iele < nEle ; ++iele)
        {
          TLorentzVector * theEle = (TLorentzVector*) (electrons->At (iele)) ;
          lepton dummy (theEle, 0, iele) ;
          leptons.push_back (dummy) ;
        } //PG loop over electrons

      //PG loop over muons
      for (int imu = 0 ; imu < nMu ; ++imu)
        {
          TLorentzVector * theMu = (TLorentzVector*) (muons->At (imu)) ;
          lepton dummy (theMu, 1, imu) ;
          leptons.push_back (dummy) ;
        } //PG loop over muons

      sort (leptons.rbegin (), leptons.rend (), lessThan ()) ;
      if (leptons.size () < 2) continue ;

      //PG find the two most energetic leptons
      //PG -----------------------------------
      
      deltaPhi (leptons.begin ()->m_kine->Phi (), (leptons.begin ()+1)->m_kine->Phi ()) ;
      TLorentzVector sumLEP = *(leptons.begin ()->m_kine) + *((leptons.begin ()+1)->m_kine) ;

      //PG MET
      //PG ---

      TLorentzVector* met = ((TLorentzVector*) (MET->At(0))) ;
      //correct for muons
      for (int i = 0 ; i < nMu ; i++)
        {
           TLorentzVector * mu_v = (TLorentzVector*) (muons->At (i)) ;
   
           if (mu_v->Pt () > 3)
             {
               met->SetPx (met->Px () - mu_v->Px ()) ;
               met->SetPy (met->Py () - mu_v->Py ()) ;
             }
         }      
      
      vars.nele = nEle ;
      vars.nmu  = nMu ;
      vars.minv = sumLEP.M () ;
      vars.met  = met->Pt () ;
      vars.dphi = deltaPhi (leptons.begin ()->m_kine->Phi (), 
                            (leptons.begin ()+1)->m_kine->Phi ()) ;

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


