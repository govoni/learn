#ifndef myRootTree_h
#define myRootTree_h

class TFile;
class TTree;

class G3EventProxy;

class myRootTree {

public:

   myRootTree(const char * filename = "myroottree.root") ;

   ~myRootTree() ;

   void store() ;

   void fillGeneral(int run, int evt) ;
  
   void load(float p_trak,  float p_egam,
             float px_trak, float px_egam,
             float py_trak, float py_egam,
             float pz_trak, float pz_egam,
             float eta_trak, float eta_egam,
             float phi_trak, float phi_egam);

   void addMCInformation(float EneMC ,float PhiMC ,float EtaMC, float 
ChargeMC ) ;

   void addBremInformation(float Radius1Brem, float EneBream) ;

private:
  
  TFile* m_File ; 
  TTree* m_Tree ;
  
  // gen info
  int m_Run , m_Event ; 

  // momenta
  float m_p_trak, m_p_egam;
  float m_px_trak, m_px_egam;
  float m_py_trak, m_py_egam;
  float m_pz_trak, m_pz_egam;

  // directions
  float m_eta_trak, m_eta_egam;
  float m_phi_trak, m_phi_egam;

  //MC information
  float m_EneMC , m_PhiMC , m_EtaMC, m_ChargeMC;

  //Brem information
  float m_Radius1Brem;
  float m_EneBream;

};

#endif
