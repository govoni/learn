#include "Utilities/Configuration/interface/Architecture.h"

#include "Examples/myRoot/interface/myRootTree.h"

//---------------
// C++ include --
//---------------

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>

//---------------
// Root include -
//---------------

#include "TFile.h"
#include "TTree.h"


#include "CARF/G3Event/interface/G3EventProxy.h"
#include "CARF/SimEvent/interface/SimEvent.h"
#include "CARF/BaseSimEvent/interface/SimVertex.h"
#include "CARF/Reco/interface/RecCollection.h"
#include "CARF/Reco/interface/AutoRecCollection.h"

myRootTree::myRootTree(const char * filename) 
{

  m_File = new TFile(filename,"RECREATE") ;
  m_Tree = new TTree("T1","My Root Tree") ;

  // GENERAL block
  m_Tree->Branch("Run", &m_Run, "Run/I") ;
  m_Tree->Branch("Event", &m_Event, "Event/I") ;

  m_Tree->Branch("m_p_trak",   &m_p_trak,   "p_trak/F");    
  m_Tree->Branch("m_px_trak",  &m_px_trak,  "px_trak/F");   
  m_Tree->Branch("m_py_trak",  &m_py_trak,  "py_trak/F");   
  m_Tree->Branch("m_pz_trak",  &m_pz_trak,  "pz_trak/F");   
  m_Tree->Branch("m_eta_trak", &m_eta_trak, "eta_trak/F");  
  m_Tree->Branch("m_phi_trak", &m_phi_trak, "phi_trak/F");  
  m_Tree->Branch("m_p_egam",   &m_p_egam,   "p_egam/F");
  m_Tree->Branch("m_px_egam",  &m_px_egam,  "px_egam/F");
  m_Tree->Branch("m_py_egam",  &m_py_egam,  "py_egam/F");
  m_Tree->Branch("m_pz_egam",  &m_pz_egam,  "pz_egam/F");
  m_Tree->Branch("m_eta_egam", &m_eta_egam, "eta_egam/F");
  m_Tree->Branch("m_phi_egam", &m_phi_egam, "phi_egam/F");
  m_Tree->Branch("EneMC", &m_EneMC, "EneMC/F") ;
  m_Tree->Branch("PhiMC", &m_PhiMC, "PhiMC/F") ;
  m_Tree->Branch("EtaMC", &m_EtaMC, "EtaMC/F") ;
  m_Tree->Branch("ChargeMC", &m_ChargeMC, "ChargeMC/F") ; 
  m_Tree->Branch("Radius1Brem", &m_Radius1Brem, "Radius1Brem/F") ;
  m_Tree->Branch("EneBream", &m_EneBream, "EneBream/F") ; 
}

myRootTree::~myRootTree() 
{
  m_File->cd() ;
  m_Tree->Write() ;
  m_File->Close() ;
  
//PG  delete m_File ;

}

void myRootTree::store()
{
  m_Tree->Fill();
} 

void myRootTree::fillGeneral(int run, int evt) 
{

  m_Run = run ;
  m_Event = evt ;
  
}


void myRootTree::load(float p_trak,  float p_egam,
                      float px_trak, float px_egam,
                      float py_trak, float py_egam,
                      float pz_trak, float pz_egam,
                      float eta_trak, float eta_egam,
                      float phi_trak, float phi_egam)
  {
    m_p_trak   = p_trak;  
    m_px_trak  = px_trak; 
    m_py_trak  = py_trak; 
    m_pz_trak  = pz_trak; 
    m_eta_trak = eta_trak;
    m_phi_trak = phi_trak;
    m_p_egam   = p_egam; 
    m_px_egam  = px_egam; 
    m_py_egam  = py_egam; 
    m_pz_egam  = pz_egam;
    m_eta_egam = eta_egam;
    m_phi_egam = phi_egam;
  }

void myRootTree::addMCInformation(float EneMC ,float PhiMC ,float EtaMC, 
float ChargeMC)
{

  m_EneMC = EneMC ; 
  m_PhiMC = PhiMC ; 
  m_EtaMC = EtaMC ;
  m_ChargeMC = ChargeMC; 
}


void myRootTree::addBremInformation(float Radius1Brem, float EneBream){

  m_Radius1Brem = Radius1Brem ;
  m_EneBream  =  EneBream ;
}
