#ifndef myDataTree_h
#define myDataTree_h
#include <string>

class TFile;
class TTree;

class G3EventProxy;

class myDataTree {

public:

   myDataTree(const string filename = "myDataTree.root") ;

   ~myDataTree() ;

   void Fill() ;

   void loadGeneral(int loop) ;
  
   void loadMC(float EneMC,
               float EtaMC, 
               float PhiMC,
               float ChargeMC ) ;

   void loadTrack(float PTk,
                  float EtaTk, 
                  float PhiTk,
                  int validTKHits) ;
                  
   void loadTrackFit(float chiSq,
                     float normChiSq) ;

   void loadECALInfo(float mostEneCRHEne, 
                     int mostEneCRHEta,
                     int mostEneCRHPhi,
                     float SCEoverTKP,
                     float SCE, 
                     float SCEt) ;

  void loadBremInfo(float McEBrem,
                    float McRBrem) ;


   void loadECALArray(float crystal[87][24]) ;

private:
  
  TFile* m_File ; 
  TTree* m_Tree ;
  std::string m_fileName ;
  int m_giro ;
  
  int m_loop ;

  float m_EneMC ;
  float m_PhiMC ;
  float m_EtaMC ; 
  float m_ChargeMC ;

  float m_PTk ;
  float m_PhiTk ;
  float m_EtaTk ; 
  int m_validTKHits ;
  float m_chiSq ;
  float m_normChiSq ;

  float m_mostEneCRHEne ;
  int m_mostEneCRHEta ;
  int m_mostEneCRHPhi ;

  float m_SCEoverTKP ;
  float m_SCE ;
  float m_SCEt ;

  float m_McEBrem ;
  float m_McRBrem ;

  float m_crystal[87][24] ;

};

#endif
