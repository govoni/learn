// @(#)root/foam:$Name: V03-09-06-01 $:$Id: PDEFoam.h,v 1.6 2008/08/08 16:35:37 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#ifndef ROOT_TMVA_PDEFoam
#define ROOT_TMVA_PDEFoam

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TFDISTR is a child class of TFoamIntegrand and contains Methods to       //
// create a probability density by filling in events.                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// set option "noTMVA" when compiling alonestanding lib
// set option "isTMVA" when compiling as part of TMVA
#define isTMVA

#ifndef isTMVA
#include "TFoamIntegrand.h"
#include "TFoamCell.h"
#include "TFoamVect.h"
#include "TFoam.h"
#else
#include "TFoamWrapper.h"
#endif

#include "TMath.h"
#include "TH2D.h"
#include "TObjArray.h"
#include "TVectorT.h"
#include <iostream>

#ifndef ROOT_TMVA_BinarySearchTree
#include "TMVA/BinarySearchTree.h"
#endif
#ifndef ROOT_TMVA_VariableInfo
#include "TMVA/VariableInfo.h"
#endif
#ifndef ROOT_TMVA_Timer
#include "TMVA/Timer.h"
#endif

namespace TMVA {

//    class TFDISTR;
//    std::ostream& operator<< ( std::ostream& os, const TFDISTR& distr );
//    std::istream& operator>> ( std::istream& istr,     TFDISTR& distr );

  // class definition of underlying density
#ifndef isTMVA
  class TFDISTR : public     ::TFoamIntegrand { // load root foam
#else
#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
  class TFDISTR : public     ::TFoamIntegrand { // load root foam
#else
  class TFDISTR : public TFoamIntegrand { // load intern TMVA-Foam
#endif
#endif
     
//    friend std::ostream& operator<< ( std::ostream& os, const TFDISTR& distr );
//    friend std::istream& operator>> ( std::istream& istr,     TFDISTR& distr );

  private:
    Int_t kDim;                // number of dimensions 
    Float_t *xmin;            //[kDim] minimal value of phase space in all dimension 
    Float_t *xmax;            //[kDim] maximal value of phase space in all dimension 
    char *filename;            // name of file containing event
    char *treename;            // name of tree containing variable
    char *varname;             // Variable name in tree defining the phase space point
    // is assumed to be array here with 6-dimensions
    Int_t ilay;                // index of varname to be used
    Float_t volfraction;      // volume fraction (with respect to total phase space
    // where events are searched to define Density for a given point
    //TTree *newtree;
    //  Float_t Eig0, Eig1, Eig2, Eig3, Eig4, Eig5, Eig6, weight;
    Float_t eigenvector[7];
    Float_t weight;

    Bool_t filldiscr;          // fill discriminator to tree, instead of event density (see TFDISTR::Density())
    int maxpoints;             // Maximum points to average over to calculate weight

    // for new TMVA-Framework
    std::vector<Float_t> vars;   //! Don't save

  public:
    TFDISTR(){
      filename="";
      treename="";
      varname="";
      ilay=-1;
      volfraction=-1.;
      kDim=-1;
      bst=NULL;
      maxpoints=9;
      filldiscr = false; // default: fill event density to BinarySearchTree
    };
    virtual ~TFDISTR(){
      delete bst;
      delete xmin; delete xmax;
    };

    BinarySearchTree * bst; // Binary tree to find events within a volume

    void SetLayer(Int_t ilayer){ilay=ilayer; return;};
    Int_t GetLayer() {return ilay;};

    void SetVolumeFraction(Float_t vfr){volfraction=vfr; return;};
    Float_t GetVolumeFraction(){return volfraction;};

    void SetDim(Int_t idim){ // Memory leak, fix me
      kDim=idim; 
      xmin=new Float_t[kDim];
      xmax=new Float_t[kDim];
      return;
    };

    void SetXmin(Int_t idim,Float_t wmin){xmin[idim]=wmin; return;};
    void SetXmax(Int_t idim,Float_t wmax){xmax[idim]=wmax; return;};

    void SetMaxpoints(Int_t maxpoints){this->maxpoints = maxpoints; return;};

    void PrintDensity();

    void Initialize(Int_t ndim = 2);
    void InputEvent(std::vector<Double_t>& eigvecprojs, Bool_t, Double_t weight, Double_t);

    // DD 10.Jan.2008
    // new method to fill edge histograms directly, without MC sampling
    void FillEdgeHist(Int_t nDim, TObjArray *myfHistEdg , Double_t *cellPosi, Double_t *cellSize, Double_t *ceSum, Double_t ceVol);

    Double_t Density(int nDim, Double_t *Xarg);
    void FillDiscriminator(Bool_t val){ filldiscr=val; };

    TH2D* MakeHistogram(Int_t nbinsx, Int_t nbinsy);

    ClassDef(TFDISTR,3); //Class for Event density
  };  //end of TFDISTR

}  // namespace TMVA

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// NodeAndDistance calculates the squared distance of an event 'Xarg' to    //
// the event given in a node 'thenode' of a binary search tree              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

namespace TMVA{

   class NodeAndDistance {
   public:
      const TMVA::BinarySearchTreeNode* node;
      double distance;

      NodeAndDistance(const TMVA::BinarySearchTreeNode* thenode, int nDim, Double_t *Xarg);
      virtual ~NodeAndDistance(){};

      ClassDef(NodeAndDistance,1); //Class for Event density
   };  // end of NodeAndDistance

} // namespace TMVA

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// PDEFoam is the child class of the multi-dimensional general purpose      //
// Monte Carlo event generator (integrator) TFoam.                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

namespace TMVA {

   class PDEFoam;
   std::ostream& operator<< ( std::ostream& os, const PDEFoam& pdefoam );
   std::istream& operator>> ( std::istream& istr,     PDEFoam& pdefoam );

#ifndef isTMVA
  class PDEFoam : public     ::TFoam { // load root foam
#else
#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
  class PDEFoam : public     ::TFoam { // load root foam
#else
  class PDEFoam : public TMVA::TFoam { // load TMVA foam
#endif
#endif

  protected:

    Double_t *xmin;          // [fDim] minimum for variable transform
    Double_t *xmax;          // [fDim] maximum for variable transform
    TFDISTR *fdistr;         //! density from extern
    Timer *timer;            // timer for graphical output

    //////////////////////////////////////////////////////////////////////////////////////////////
    //                                     METHODS                                              //
    //////////////////////////////////////////////////////////////////////////////////////////////
  protected:
    virtual void OutputGrow(Bool_t finished = false ); // override  TFoam::OutputGrow(Bool_t)

  public:
    PDEFoam();                          // Default constructor (used only by ROOT streamer)
    PDEFoam(const Char_t*);             // Principal user-defined constructor
    virtual ~PDEFoam();                 // Default destructor

    virtual void Explore(TFoamCell *Cell);       // Exploration of the new cell, determine <wt>, wtMax etc.

    friend std::ostream& operator<< ( std::ostream& os, const PDEFoam& pdefoam );
    friend std::istream& operator>> ( std::istream& istr,     PDEFoam& pdefoam );

    void ReadStream(istream &);         // read foam from stream
    void PrintStream(ostream  &) const; // write foam from stream

    // Getters and Setters
    virtual void SetkDim(Int_t kDim){            // override TFoam::SetkDim(Int_t)
      TFoam::SetkDim(kDim);
      xmin=new Double_t[GetTotDim()];
      xmax=new Double_t[GetTotDim()];
    } // Sets dimension of cubical space

    virtual TH2D* Project(Int_t idim1, Int_t idim2, char *opt="mean");                       // Project foam (version of Carli and Dannheim)
    virtual TH2D* Project2(Int_t idim1, Int_t idim2, char *opt="mean", Int_t nbin=100);      // Project foam (verison of Alexander Voigt)
    virtual TH2D* ProjectMC(Int_t idim1, Int_t idim2, char *opt, Int_t nevents, Int_t nbin); // Project foam by creating MC events
    virtual TH1D* Draw1Dim(char *opt, Int_t nbin);                                           // Draw a 1-dim histogram
    virtual void RootPlot2dim(Char_t*, std::string what, Bool_t);      // Generates C++ code for drawing foam
    virtual void RootPlot2dim(Char_t*){};

    void ResetCellIntegrals(void);
    void DisplayCellContent(void);
    void SetCell(Double_t *xvec, Double_t value, Double_t weight=1.);
    void AddCellEvents(Double_t *xvec, Bool_t IsSignal=true, Double_t SigWeight=1., Double_t BgWeight=1.);
    void CalcCellDiscr();
    Double_t GetCellMean(Double_t *xvec);
    Double_t GetCellRMS(Double_t *xvec);
    Double_t GetCellEntries(Double_t *xvec);
    Double_t GetCellDiscr(Double_t *xvec);
    Double_t GetCellDiscrError(Double_t *xvec);

    // DD 28.11.2007
    Double_t GetCellDensity(Double_t *xvec);

    TFoamCell* FindCell(Double_t *xvec); //!

    Double_t GetCellMean(TFoamCell* cell);
    Double_t GetCellRMS(TFoamCell* cell);
    Double_t GetCellEntries(TFoamCell* cell);
    Double_t GetCellDiscr(TFoamCell* cell);
    Double_t GetCellDiscrError(TFoamCell* cell);

    void SetXmin(Int_t idim, Double_t wmin){ 
      xmin[idim]=wmin;
      fdistr->SetXmin(idim, wmin);
      return; 
    };
    void SetXmax(Int_t idim, Double_t wmax){ 
      xmax[idim]=wmax;
      fdistr->SetXmax(idim, wmax);
      return; 
    };
    Double_t GetXmin(Int_t idim){return xmin[idim];};
    Double_t GetXmax(Int_t idim){return xmax[idim];};

    //AV>>   testfunctions to set fElement of cells
    void InitCellVariables();
    TVectorD* GetFirstCellElement();
    //AV<<

    Double_t VarTransform(Int_t idim,Double_t x){
      Double_t b=xmax[idim]-xmin[idim]; 
      return (x-xmin[idim])/b;
    };

    //AV>>   to include TFDISTR
    void SetVolumeFraction(Double_t);  // set VolFrac to TFDISTR
    void InputEvent(std::vector<Double_t>&, Bool_t, Double_t, Double_t); // input event to TFDISTR
    void Create();             // create TFoam
    void Init();         // initialize TFDISTR
    void PrintDensity();
    void FillDiscriminator(Bool_t); // fill foam cells with discriminator, instead of number of events
    //AV<<

    //////////////////////////////////////////////////////////////////////////////////////////////
    ClassDef(PDEFoam,3);   // General purpose self-adapting binning
  }; // end of PDEFoam 

}  // namespace TMVA

#endif
