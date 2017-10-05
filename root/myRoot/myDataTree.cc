#include "Utilities/Configuration/interface/Architecture.h"

#include "Examples/myRoot/interface/myDataTree.h"

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

myDataTree::myDataTree(const std::string filename) :
            m_fileName(filename),
            m_giro(0)
{

//  m_File = new TFile(m_fileName,"RECREATE") ;
  m_Tree = new TTree("myData","Personal Selected Data Tree") ;

  // GENERAL block

  m_Tree->Branch("loop", &m_loop, "loop/I") ;          

  m_Tree->Branch("EneMC", &m_EneMC, "EneMC/F") ;         
  m_Tree->Branch("PhiMC", &m_PhiMC, "PhiMC/F") ;         
  m_Tree->Branch("EtaMC", &m_EtaMC, "EtaMC/F") ;         
  m_Tree->Branch("ChargeMC", &m_ChargeMC, "ChargeMC/F") ;      

  m_Tree->Branch("PTk", &m_PTk, "PTk/F") ;           
  m_Tree->Branch("PhiTk", &m_PhiTk, "PhiTk/F") ;         
  m_Tree->Branch("EtaTk", &m_EtaTk, "EtaTk/F") ;         
  m_Tree->Branch("validTKHits", &m_validTKHits, "validTKHits/I") ;   
  m_Tree->Branch("chiSq", &m_chiSq, "chiSq/F") ;   
  m_Tree->Branch("normChiSq", &m_normChiSq, "normChiSq/F") ;   

  m_Tree->Branch("mostEneCRHEne", &m_mostEneCRHEne, "mostEneCRHEne/I") ; 
  m_Tree->Branch("mostEneCRHEta", &m_mostEneCRHEta, "mostEneCRHEta/I") ; 
  m_Tree->Branch("mostEneCRHPhi", &m_mostEneCRHPhi, "mostEneCRHPhi/I") ; 

  m_Tree->Branch("SCEoverTKP", &m_SCEoverTKP, "SCEoverTKP/F") ;    
  m_Tree->Branch("SCE", &m_SCE, "SCE/F") ;    
  m_Tree->Branch("SCEt", &m_SCEt, "SCEt/F") ;    

  m_Tree->Branch("McEBrem", &m_McEBrem, "McEBrem/F") ;    
  m_Tree->Branch("McRBrem", &m_McRBrem, "McRBrem/F") ;    

  m_Tree->Branch("crystal", m_crystal, "Array[87][24]/F") ;
    
}

myDataTree::~myDataTree() 
{
  m_File = new TFile(m_fileName.c_str(),"RECREATE") ;
  m_File->cd() ;
  m_Tree->Write() ;
  m_File->Close() ;
  
//PG  delete m_File ;

}

void myDataTree::Fill()
{
  m_Tree->Fill() ;
  ++m_giro ;

  //PG to save often (tks GF)
  if ( m_giro % 50 == 0)
    {
      m_File = new TFile(m_fileName.c_str(),"RECREATE") ;
      m_File -> cd() ;
      m_Tree -> Write() ;
      m_File -> Close() ;
      std::cout << "[myDataTree][store] WRITING TO FILE\n" ;
    }
} 


void myDataTree::loadGeneral(int loop)
{
  m_loop = loop ;
}


void myDataTree::loadMC(float EneMC,
                        float EtaMC, 
                        float PhiMC,
                        float ChargeMC ) 
{
  m_EneMC    = EneMC ;
  m_PhiMC    = PhiMC ;
  m_EtaMC    = EtaMC ; 
  m_ChargeMC = ChargeMC ;
}


void myDataTree::loadTrack(float PTk,
                           float EtaTk, 
                           float PhiTk,
                           int validTKHits)
{
  m_PTk         = PTk ;
  m_PhiTk       = PhiTk ;
  m_EtaTk       = EtaTk ; 
  m_validTKHits = validTKHits ;
}


void myDataTree::loadTrackFit(float chiSq,
                              float normChiSq) 
{
  m_chiSq       = chiSq ;
  m_normChiSq   = normChiSq ;
}


void myDataTree::loadECALInfo(float mostEneCRHEne,
                              int mostEneCRHEta,
                              int mostEneCRHPhi,
                              float SCEoverTKP,
                              float SCE, 
                              float SCEt)                               
{
  m_mostEneCRHEne = mostEneCRHEne ;
  m_mostEneCRHEta = mostEneCRHEta ;
  m_mostEneCRHPhi = mostEneCRHPhi ;
  m_SCEoverTKP    = SCEoverTKP ;
  m_SCE           = SCE ;
  m_SCEt          = SCEt ;
}


void myDataTree::loadBremInfo(float McEBrem,
                              float McRBrem)
{
  m_McEBrem = McEBrem ;
  m_McRBrem = McRBrem ;
}


void myDataTree::loadECALArray(float crystal[87][24]) 
{
  for (int eta =0; eta<87; ++eta)    
    {
      for (int phi =0; phi<24; ++phi) 
        {
          m_crystal[eta][phi] = crystal[eta][phi];  
        }   
    }
}



