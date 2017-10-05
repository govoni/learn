#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
#include "TFoamIntegrand.h"
#include "TFoamCell.h"
#include "TFoamVect.h"
#include "TFoam.h"
#else

// @(#)root/foam:$Id: TFoamWrapper.h,v 1.2 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#ifndef ROOT_TMVA_TFoamCell
#define ROOT_TMVA_TFoamCell

////////////////////////////////////////////////////////////////////////////////////
// Class TFoamCell  used in TFoam                                                 //
//                                                                                //
// Objects of this class are hyperrectangular cells organized in the binary tree. //
// Special algoritm for encoding relalive positioning of the cells                //
// saves total memory allocation needed for the system of cells.                  //
////////////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TRef
#include "TRef.h"
#endif

namespace TMVA {

class TFoamVect;


class TFoamCell : public TObject {
   //   static, the same for all cells!
private:
   Short_t  fDim;                   // Dimension of the vector space
   //   MEMBERS
private:
   //--- linked tree organization ---
   Int_t    fSerial;                // Serial number
   Int_t    fStatus;                // Status (active, inactive)
   TRef     fParent;                // Pointer to parent cell
   TRef     fDaught0;               // Pointer to daughter 1
   TRef     fDaught1;               // Pointer to daughter 2
   //--- M.C. sampling and choice of the best edge ---
private:
   Double_t fXdiv;                  // Factor for division
   Int_t    fBest;                  // Best Edge for division
   //--- Integrals of all kinds ---
   Double_t fVolume;                // Cartesian Volume of cell
   Double_t fIntegral;              // Integral over cell (estimate from exploration)
   Double_t fDrive;                 // Driver  integral, only for cell build-up
   Double_t fPrimary;               // Primary integral, only for MC generation
   //----------  working space for the user --------------
   TObject *fElement;               // may set by the user to save some data in this cell
   //////////////////////////////////////////////////////////////////////////////////////
   //                           METHODS                                                //
   //////////////////////////////////////////////////////////////////////////////////////
public:
   TFoamCell();                          // Default Constructor for ROOT streamers
   TFoamCell(Int_t);                     // User Constructor
   TFoamCell(TFoamCell &);               // Copy Constructor
   virtual ~TFoamCell();                 // Destructor
   void  Fill(Int_t, TFoamCell*, TFoamCell*, TFoamCell*);    // Assigns values of attributes
   TFoamCell&  operator=(const TFoamCell&);       // Substitution operator (never used)
   //--------------- Geometry ----------------------------------
   Double_t  GetXdiv() const { return fXdiv;}          // Pointer to Xdiv
   Int_t     GetBest() const { return fBest;}          // Pointer to Best
   void      SetBest(Int_t    Best){ fBest =Best;}     // Set Best edge candidate
   void      SetXdiv(Double_t Xdiv){ fXdiv =Xdiv;}     // Set x-division for best edge cand.
   void      GetHcub(  TFoamVect&, TFoamVect&) const;  // Get position and size vectors (h-cubical subspace)
   void      GetHSize( TFoamVect& ) const;             // Get size only of cell vector  (h-cubical subspace)
   //--------------- Integrals/Volumes -------------------------
   void      CalcVolume();                             // Calculates volume of cell
   Double_t  GetVolume() const { return fVolume;}      // Volume of cell
   Double_t  GetIntg() const { return fIntegral;}      // Get Integral
   Double_t  GetDriv() const { return fDrive;}         // Get Drive
   Double_t  GetPrim() const { return fPrimary;}       // Get Primary
   void      SetIntg(Double_t Intg){ fIntegral=Intg;}  // Set true integral
   void      SetDriv(Double_t Driv){ fDrive   =Driv;}  // Set driver integral
   void      SetPrim(Double_t Prim){ fPrimary =Prim;}  // Set primary integral
   //--------------- linked tree organization ------------------
   Int_t     GetStat() const { return fStatus;}        // Get Status
   void      SetStat(Int_t Stat){ fStatus=Stat;}       // Set Status
   TFoamCell* GetPare() const { return (TFoamCell*) fParent.GetObject(); }  // Get Pointer to parent cell
   TFoamCell* GetDau0() const { return (TFoamCell*) fDaught0.GetObject(); } // Get Pointer to 1-st daughter vertex
   TFoamCell* GetDau1() const { return (TFoamCell*) fDaught1.GetObject(); } // Get Pointer to 2-nd daughter vertex
   void      SetDau0(TFoamCell* Daug){ fDaught0 = Daug;}  // Set pointer to 1-st daughter
   void      SetDau1(TFoamCell* Daug){ fDaught1 = Daug;}  // Set pointer to 2-nd daughter
   void      SetSerial(Int_t Serial){ fSerial=Serial;}    // Set serial number
   Int_t     GetSerial() const { return fSerial;}         // Get serial number
   //--- other ---
   void Print(Option_t *option) const ;                   // Prints cell content
   //--- getter and setter for user variable ---
   void SetElement(TObject* fobj){ fElement = fobj; };    // Set user variable
   TObject* GetElement(){ return fElement; };             // Get pointer to user varibale
   ////////////////////////////////////////////////////////////////////////////
   ClassDef(TFoamCell,1)  //Single cell of FOAM
};
/////////////////////////////////////////////////////////////////////////////
} // namespace TMVA
#endif


// @(#)root/foam:$Id: TFoamWrapper.h,v 1.2 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#ifndef ROOT_TMVA_TFoam
#define ROOT_TMVA_TFoam

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TFoam is the main class of the multi-dimensional general purpose         //
// Monte Carlo event generator (integrator) FOAM.                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

#include "TString.h"

class TH1D;
class TRefArray;
class TMethodCall;
class TRandom;

namespace TMVA {

//class TH1D;
//class TRefArray;
//class TMethodCall;
//class TRandom;
class TFoamIntegrand;
class TFoamMaxwt;
class TFoamVect;
class TFoamCell;

class TFoam : public TObject {
protected:
  // COMPONENTS //
    //-------------- Input parameters
   TString fName;             // Name of a given instance of the FOAM class
   TString fVersion;          // Actual version of the FOAM like (1.01m)
   TString fDate;             // Release date of FOAM
   Int_t   fDim;              // Dimension of the integration/simulation space
   Int_t   fNCells;           // Maximum number of cells
   Int_t   fRNmax;            // Maximum No. of the rand. numb. requested at once
   //-------------------
   Int_t   fOptDrive;         // Optimization switch =1,2 for variance or maximum weight optimization
   Int_t   fChat;             // Chat=0,1,2 chat level in output, Chat=1 normal level
   Int_t   fOptRej;           // Switch =0 for weighted events; =1 for unweighted events in MC
   //-------------------
   Int_t   fNBin;             // No. of bins in the edge histogram for cell MC exploration
   Int_t   fNSampl;           // No. of MC events, when dividing (exploring) cell
   Int_t   fEvPerBin;         // Maximum number of effective (wt=1) events per bin
   //-------------------  MULTI-BRANCHING ---------------------
   Int_t  *fMaskDiv;          //! [fDim] Dynamic Mask for  cell division
   Int_t  *fInhiDiv;          //! [fDim] Flags for inhibiting cell division
   Int_t   fOptPRD;           //  Option switch for predefined division, for quick check
   TFoamVect **fXdivPRD;      //! Lists of division values encoded in one vector per direction
   //-------------------  GEOMETRY ----------------------------
   Int_t   fNoAct;            // Number of active cells
   Int_t   fLastCe;           // Index of the last cell
   TFoamCell **fCells;           // [fNCells] Array of ALL cells
   //------------------ M.C. generation----------------------------
   TFoamMaxwt   *fMCMonit;    // Monitor of the MC weight for measuring MC efficiency
   Double_t   fMaxWtRej;      // Maximum weight in rejection for getting wt=1 events
   TRefArray *fCellsAct;      // Array of pointers to active cells, constructed at the end of foam build-up
   Double_t  *fPrimAcu;       // [fNoAct] Array of cumulative probability of all active cells
   TObjArray *fHistEdg;       // Histograms of wt, one for each cell edge
   TObjArray *fHistDbg;       // Histograms of wt, for debug
   TH1D      *fHistWt;        // Histogram of the MC wt

   Double_t *fMCvect;         // [fDim] Generated MC vector for the outside user
   Double_t  fMCwt;           // MC weight
   Double_t *fRvec;           // [fRNmax] random number vector from r.n. generator fDim+1 maximum elements
   //----------- Procedures
   TFoamIntegrand *fRho;      //! Pointer to the user-defined integrand function/distribution
   TMethodCall *fMethodCall;  //! ROOT's pointer to user-defined global distribution function
   TRandom         *fPseRan;  // Pointer to user-defined generator of pseudorandom numbers
   //----------- Statistics and MC results
   Long_t   fNCalls;          // Total number of the function calls
   Long_t   fNEffev;          // Total number of effective events (wt=1) in the foam buildup
   Double_t fSumWt, fSumWt2;  // Total sum of wt and wt^2
   Double_t fSumOve;          // Total Sum of overveighted events
   Double_t fNevGen;          // Total number of the generated MC events
   Double_t fWtMax, fWtMin;   // Maximum/Minimum MC weight
   Double_t fPrime;           // Primary integral R' (R=R'<wt>)
   Double_t fMCresult;        // True Integral R from MC series
   Double_t fMCerror;         // and its error
   //----------  working space for CELL exploration -------------
   Double_t *fAlpha;          // [fDim] Internal parameters of the hyperrectangle
   //////////////////////////////////////////////////////////////////////////////////////////////
   //                                     METHODS                                              //
   //////////////////////////////////////////////////////////////////////////////////////////////
protected:
   virtual void OutputGrow(Bool_t finished = false );

public:
   TFoam();                          // Default constructor (used only by ROOT streamer)
   TFoam(const Char_t*);             // Principal user-defined constructor
   virtual ~TFoam();                 // Default destructor
   TFoam(const TFoam&);              // Copy Constructor  NOT USED
   // Initialization
   virtual void Initialize();                // Initialization of the FOAM (grid, cells, etc), mandatory!
   virtual void Initialize(TRandom *, TFoamIntegrand *); // Alternative initialization method, backward compatibility
   virtual void InitCells();                 // Initializes first cells inside original cube
   virtual Int_t  CellFill(Int_t, TFoamCell*);  // Allocates new empty cell and return its index
   virtual void Explore(TFoamCell *Cell);       // Exploration of the new cell, determine <wt>, wtMax etc.
   virtual void Carver(Int_t&,Double_t&,Double_t&);// Determines the best edge, wt_max reduction
   virtual void Varedu(Double_t [], Int_t&, Double_t&,Double_t&); // Determines the best edge, variace reduction
   virtual void MakeAlpha();                 // Provides random point inside hyperrectangle
   virtual void Grow();                      // Adds new cells to FOAM object until buffer is full
   virtual Long_t PeekMax();                 // Choose one active cell, used by Grow and also in MC generation
   virtual Int_t  Divide(TFoamCell *);       // Divide iCell into two daughters; iCell retained, taged as inactive
   virtual void MakeActiveList();            // Creates table of active cells
   virtual void GenerCel2(TFoamCell *&);     // Chose an active cell the with probability ~ Primary integral
   // Generation
   virtual Double_t Eval(Double_t *);        // Evaluates value of the distribution function
   virtual void     MakeEvent();             // Makes (generates) single MC event
   virtual void     GetMCvect(Double_t *);   // Provides generated randomly MC vector
   virtual void     GetMCwt(Double_t &);     // Provides generated MC weight
   virtual Double_t GetMCwt();               // Provides generates MC weight
   virtual Double_t MCgenerate(Double_t *MCvect);// All three above function in one
   // Finalization
   virtual void GetIntegMC(Double_t&, Double_t&);// Provides Integrand and abs. error from MC run
   virtual void GetIntNorm(Double_t&, Double_t&);// Provides normalization Inegrand
   virtual void GetWtParams(Double_t, Double_t&, Double_t&, Double_t&);// Provides MC weight parameters
   virtual void Finalize(  Double_t&, Double_t&);  // Prints summary of MC integration
   virtual TFoamIntegrand  *GetRho(){return fRho;} // Gets pointer of the distribut. (after restoring from disk)
   virtual TRandom *GetPseRan() const {return fPseRan;}   // Gets pointer of r.n. generator (after restoring from disk)
   virtual void SetRhoInt(void *Rho);              // Set new integrand distr. in interactive mode
   virtual void SetRho(TFoamIntegrand *Rho);       // Set new integrand distr. in compiled mode
   virtual void ResetRho(TFoamIntegrand *Rho);                // Set new distribution, delete old
   virtual void SetPseRan(TRandom *PseRan){fPseRan=PseRan;}   // Set new r.n. generator
   virtual void ResetPseRan(TRandom *PseRan);                 // Set new r.n.g, delete old
   // Getters and Setters
   virtual void SetkDim(Int_t kDim){fDim = kDim;}            // Sets dimension of cubical space
   virtual void SetnCells(Long_t nCells){fNCells =nCells;}  // Sets maximum number of cells
   virtual void SetnSampl(Long_t nSampl){fNSampl =nSampl;}  // Sets no of MC events in cell exploration
   virtual void SetnBin(Int_t nBin){fNBin = nBin;}          // Sets no of bins in histogs in cell exploration
   virtual void SetChat(Int_t Chat){fChat = Chat;}          // Sets option Chat, chat level
   virtual void SetOptRej(Int_t OptRej){fOptRej =OptRej;}   // Sets option for MC rejection
   virtual void SetOptDrive(Int_t OptDrive){fOptDrive =OptDrive;}  // Sets optimization switch
   virtual void SetEvPerBin(Int_t EvPerBin){fEvPerBin =EvPerBin;}  // Sets max. no. of effective events per bin
   virtual void SetMaxWtRej(Double_t MaxWtRej){fMaxWtRej=MaxWtRej;}  // Sets max. weight for rejection
   virtual void SetInhiDiv(Int_t, Int_t );            // Set inhibition of cell division along certain edge
   virtual void SetXdivPRD(Int_t, Int_t, Double_t[]); // Set predefined division points
   // Getters and Setters
   virtual const char *GetVersion() const {return fVersion.Data();}// Get version of the FOAM
   virtual Int_t    GetTotDim() const { return fDim;}              // Get total dimension
   virtual Double_t GetPrimary() const {return fPrime;}            // Get value of primary integral R'
   virtual void GetPrimary(Double_t &prime) {prime = fPrime;}      // Get value of primary integral R'
   virtual Long_t GetnCalls() const {return fNCalls;}            // Get total no. of the function calls
   virtual Long_t GetnEffev() const {return fNEffev;}            // Get total no. of effective wt=1 events
   // Debug
   virtual void CheckAll(Int_t);     // Checks correctness of the entire data structure in the FOAM object
   virtual void PrintCells();        // Prints content of all cells
   virtual void RootPlot2dim(Char_t*);   // Generates C++ code for drawing foam
   virtual void LinkCells(void);         // Void function for backward compatibility
   // Inline
private:
   Double_t Sqr(Double_t x) const { return x*x;}      // Square function
   //////////////////////////////////////////////////////////////////////////////////////////////
   ClassDef(TFoam,1);   // General purpose self-adapting Monte Carlo event generator
};
} // namespace TMVA
#endif


// @(#)root/foam:$Id: TFoamWrapper.h,v 1.2 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#ifndef ROOT_TMVA_TFoamIntegrand
#define ROOT_TMVA_TFoamIntegrand

//_________________________________________
// Class TFoamIntegrand
// =====================
// Abstract class representing n-dimensional real positive integrand function

#ifndef ROOT_TObject
#include "TObject.h"
#endif

namespace TMVA {

class TFoamIntegrand : public ::TObject  {
public:
   TFoamIntegrand();
   virtual ~TFoamIntegrand() { };
   virtual Double_t Density(Int_t ndim, Double_t *) = 0;

   ClassDef(TFoamIntegrand,1); //n-dimensional real positive integrand of FOAM
};
} // namespace TMVA
#endif


// @(#)root/foam:$Id: TFoamWrapper.h,v 1.2 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#ifndef ROOT_TMVA_TFoamMaxwt
#define ROOT_TMVA_TFoamMaxwt

//////////////////////////////////////////////////////////////////
//                                                              //
// Small auxiliary class for controlling MC weight.             //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

//class TH1D;

namespace TMVA {

class TFoamMaxwt : public TObject {
private:
   Double_t  fNent;      // No. of MC events
   Int_t     fnBin;      // No. of bins on the weight distribution
   Double_t  fwmax;      // Maximum analyzed weight
public:
   TH1D   *fWtHst1;      // Histogram of the weight wt
   TH1D   *fWtHst2;      // Histogram of wt filled with wt

public:
   TFoamMaxwt();                            // NOT IMPLEMENTED (NEVER USED)
   TFoamMaxwt(Double_t, Int_t);             // Principal Constructor
   TFoamMaxwt(TFoamMaxwt &From);            // Copy constructor
   virtual ~TFoamMaxwt();                   // Destructor
   void Reset();                            // Reset
   TFoamMaxwt& operator=(const TFoamMaxwt &);    // operator =
   void Fill(Double_t);
   void Make(Double_t, Double_t&);
   void GetMCeff(Double_t, Double_t&, Double_t&);  // get MC efficiency= <w>/wmax

   ClassDef(TFoamMaxwt,1); //Controlling of the MC weight (maximum weight)
};
} // namespace TMVA
#endif


// @(#)root/foam:$Id: TFoamWrapper.h,v 1.2 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#ifndef ROOT_TMVA_TFoamVect
#define ROOT_TMVA_TFoamVect

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Auxiliary class TFoamVect of n-dimensional vector, with dynamic allocation //
// used for the cartesian geometry of the TFoam cells                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

namespace TMVA {
///////////////////////////////////////////////////////////////////////////////
class TFoamVect : public TObject {
   // constructor
private:
   Int_t       fDim;                     // Dimension
   Double_t   *fCoords;                  // [fDim] Coordinates
   TFoamVect  *fNext;                    // pointer for tree construction
   TFoamVect  *fPrev;                    // pointer for tree construction
public:
   TFoamVect();                          // Constructor
   TFoamVect(Int_t);                     // USER Constructor
   TFoamVect(const TFoamVect &);         // Copy constructor
   virtual ~TFoamVect();                 // Destructor
//////////////////////////////////////////////////////////////////////////////
//                     Overloading operators                                //
//////////////////////////////////////////////////////////////////////////////
   TFoamVect& operator =(const TFoamVect&);  // = operator; Substitution
   Double_t &operator[](Int_t);              // [] provides POINTER to coordinate
   TFoamVect& operator =(Double_t []);       // LOAD IN entire double vector
   TFoamVect& operator =(Double_t);          // LOAD IN double number
//////////////////////////   OTHER METHODS    //////////////////////////////////
   TFoamVect& operator+=(const  TFoamVect&); // +=; add vector u+=v  (FAST)
   TFoamVect& operator-=(const  TFoamVect&); // +=; add vector u+=v  (FAST)
   TFoamVect& operator*=(const Double_t&);   // *=; mult. by scalar v*=x (FAST)
   TFoamVect  operator+( const  TFoamVect&); // +;  u=v+s, NEVER USE IT, SLOW!!!
   TFoamVect  operator-( const  TFoamVect&); // -;  u=v-s, NEVER USE IT, SLOW!!!
   void Print(Option_t *option) const;   // Prints vector
   void PrintList();                         // Prints vector and the following linked list
   Int_t    GetDim() const { return fDim; }  // Returns dimension
   Double_t GetCoord(Int_t i) const {return fCoords[i];};   // Returns coordinate

   ClassDef(TFoamVect,1) //n-dimensional vector with dynamical allocation
};
} // namespace TMVA
#endif

#endif
