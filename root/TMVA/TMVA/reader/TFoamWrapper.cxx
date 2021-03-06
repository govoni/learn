#include "TObject.h"

#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
//#include "TFoam.h"
#else


// @(#)root/foam:$Id: TFoamWrapper.cxx,v 1.3 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

//_________________________________________________________________________________
//
// Class TFoamCell  used in TFoam
// ==============================
// Objects of this class are hyper-rectangular cells organized in the binary tree.
// Special algorithm for encoding relative positioning of the cells
// allow to save total memory allocation needed for the system of cells.
//
//_________________________________________________________________________________

#include "Riostream.h"
// #include "TFoamCell.h"
// #include "TFoamVect.h"

#include "TFoamWrapper.h"

#include "TH1.h"
#include "TRefArray.h"
#include "TMethodCall.h"
#include "TRandom.h"
#include "TMath.h"
#include "G__ci.h"
#include "TSystem.h"

//using namespace TMVA;

ClassImp(TMVA::TFoamCell);

//________________________________________________________________________________
TMVA::TFoamCell::TFoamCell()
{
// Default constructor for streamer

   fParent  = 0;
   fDaught0 = 0;
   fDaught1 = 0;
   fElement = 0;
}

//_________________________________________________________________________________
TMVA::TFoamCell::TFoamCell(Int_t kDim)
{
// User constructor allocating single empty Cell
   if (  kDim >0) {
      //---------=========----------
      fDim     = kDim;
      fStatus   = 1;
      fParent   = 0;
      fDaught0  = 0;
      fDaught1  = 0;
      fXdiv     = 0.0;
      fBest     = 0;
      fVolume   = 0.0;
      fIntegral = 0.0;
      fDrive    = 0.0;
      fPrimary  = 0.0;
      fElement  = 0;
   } else
      Error("TFoamCell","Dimension has to be >0 \n ");
}

//_________________________________________________________________________________
TMVA::TFoamCell::TFoamCell(TFoamCell &From): TObject(From)
{
// Copy constructor (not tested!)

   Error("TFoamCell", "+++++ NEVER USE Copy constructor for TFoamCell \n");
   fStatus      = From.fStatus;
   fParent      = From.fParent;
   fDaught0     = From.fDaught0;
   fDaught1     = From.fDaught1;
   fXdiv        = From.fXdiv;
   fBest        = From.fBest;
   fVolume      = From.fVolume;
   fIntegral    = From.fIntegral;
   fDrive       = From.fDrive;
   fPrimary     = From.fPrimary;
   fElement     = From.fElement;
}

//___________________________________________________________________________________
TMVA::TFoamCell::~TFoamCell()
{
// Destructor
}

//___________________________________________________________________________________
TMVA::TFoamCell& TMVA::TFoamCell::operator=(const TFoamCell &From)
{
// Substitution operator = (never used)

   Info("TFoamCell", "operator=\n ");
   if (&From == this) return *this;
   fStatus      = From.fStatus;
   fParent      = From.fParent;
   fDaught0     = From.fDaught0;
   fDaught1     = From.fDaught1;
   fXdiv        = From.fXdiv;
   fBest        = From.fBest;
   fVolume      = From.fVolume;
   fIntegral    = From.fIntegral;
   fDrive       = From.fDrive;
   fPrimary     = From.fPrimary;
   fElement     = From.fElement;
   return *this;
}


//___________________________________________________________________________________
void TMVA::TFoamCell::Fill(Int_t Status, TFoamCell *Parent, TFoamCell *Daugh1, TFoamCell *Daugh2)
{
// Fills in certain data into newly allocated cell

   fStatus  = Status;
   fParent  = Parent;
//   std::cout << "D1" << Daugh1 << std::endl;
   fDaught0 = Daugh1;
   fDaught1 = Daugh2;
}

////////////////////////////////////////////////////////////////////////////////
//              GETTERS/SETTERS
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________________
void    TMVA::TFoamCell::GetHcub( TFoamVect &cellPosi, TFoamVect &cellSize)  const
{
// Provides size and position of the cell
// These parameter are calculated by analyzing information in all parents
// cells up to the root cell. It takes time but saves memory.
   if(fDim<1) return;
   const TFoamCell *pCell,*dCell;
   cellPosi = 0.0; cellSize=1.0; // load all components
   dCell = this;
   while(dCell != 0) {
      pCell = dCell->GetPare();
      if( pCell== 0) break;
      Int_t    kDiv = pCell->fBest;
      Double_t xDivi = pCell->fXdiv;
      if(dCell == pCell->GetDau0()  ) {
         cellSize[kDiv] *=xDivi;
         cellPosi[kDiv] *=xDivi;
      } else if(   dCell == pCell->GetDau1()  ) {
         cellSize[kDiv] *=(1.0-xDivi);
         cellPosi[kDiv]  =cellPosi[kDiv]*(1.0-xDivi)+xDivi;
      } else {
         Error("GetHcub ","Something wrong with linked tree \n");
      }
      dCell=pCell;
   }//while
}//GetHcub

//______________________________________________________________________________________
void    TMVA::TFoamCell::GetHSize( TFoamVect &cellSize)  const
{
// Provides size of the cell
// Size parameters are calculated by analyzing information in all parents
// cells up to the root cell. It takes time but saves memory.
   if(fDim<1) return;
   const TFoamCell *pCell,*dCell;
   cellSize=1.0; // load all components
   dCell = this;
   while(dCell != 0) {
      pCell = dCell->GetPare();
      if( pCell== 0) break;
      Int_t    kDiv = pCell->fBest;
      Double_t xDivi = pCell->fXdiv;
      if(dCell == pCell->GetDau0() ) {
         cellSize[kDiv]=cellSize[kDiv]*xDivi;
      } else if(dCell == pCell->GetDau1()  ) {
         cellSize[kDiv]=cellSize[kDiv]*(1.0-xDivi);
      } else {
         Error("GetHSize ","Something wrong with linked tree \n");
      }
      dCell=pCell;
   }//while
}//GetHSize

//_________________________________________________________________________________________
void TMVA::TFoamCell::CalcVolume(void)
{
// Calculates volume of the cell using size params which are calculated

   Int_t k;
   Double_t volu=1.0;
   if(fDim>0) {         // h-cubical subspace
      TFoamVect cellSize(fDim);
      GetHSize(cellSize);
      for(k=0; k<fDim; k++) volu *= cellSize[k];
   }
   fVolume =volu;
}

//__________________________________________________________________________________________
void TMVA::TFoamCell::Print(Option_t *option) const
{
// Printout of the cell geometry parameters for the debug purpose

   if(!option) Error("Print", "No option set\n");

   cout <<  " Status= "<<     fStatus   <<",";
   cout <<  " Volume= "<<     fVolume   <<",";
   cout <<  " TrueInteg= " << fIntegral <<",";
   cout <<  " DriveInteg= "<< fDrive    <<",";
   cout <<  " PrimInteg= " << fPrimary  <<",";
   cout<< endl;
   cout <<  " Xdiv= "<<fXdiv<<",";
   cout <<  " Best= "<<fBest<<",";
   cout <<  " Parent=  {"<< (GetPare() ? GetPare()->GetSerial() : -1) <<"} "; // extra DEBUG
   cout <<  " Daught0= {"<< (GetDau0() ? GetDau0()->GetSerial() : -1 )<<"} "; // extra DEBUG
   cout <<  " Daught1= {"<< (GetDau1() ? GetDau1()->GetSerial()  : -1 )<<"} "; // extra DEBUG
   cout<< endl;
   //
   //
   if(fDim>0 ) {
      TFoamVect cellPosi(fDim); TFoamVect cellSize(fDim);
      GetHcub(cellPosi,cellSize);
      cout <<"   Posi= "; cellPosi.Print("1"); cout<<","<< endl;
      cout <<"   Size= "; cellSize.Print("1"); cout<<","<< endl;
   }
}
///////////////////////////////////////////////////////////////////
//        End of  class  TFoamCell                               //
///////////////////////////////////////////////////////////////////
// @(#)root/foam:$Id: TFoamWrapper.cxx,v 1.3 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

//______________________________________________________________________________
//
// FOAM  Version 1.02M
// ===================
// Authors:
//   S. Jadach and P.Sawicki
//   Institute of Nuclear Physics, Cracow, Poland
//   Stanislaw. Jadach@ifj.edu.pl, Pawel.Sawicki@ifj.edu.pl
//
// What is FOAM for?
// =================
// * Suppose you want to generate randomly points (vectors) according to
//   an arbitrary probability distribution  in n dimensions,
//   for which you supply your own subprogram. FOAM can do it for you!
//   Even if your distributions has quite strong peaks and is discontinuous!
// * FOAM generates random points with weight one or with variable weight.
// * FOAM is capable to integrate using efficient "adaptive" MC method.
//   (The distribution does not need to be normalized to one.)
// How does it work?
// =================
// FOAM is the simplified version of the multi-dimensional general purpose
// Monte Carlo event generator (integrator) FOAM.
// It creates hyper-rectangular "foam of cells", which is more dense around its peaks.
// See the following 2-dim. example of the map of 1000 cells for doubly peaked distribution:
//BEGIN_HTML <!--
/* -->
<img src="gif/foam_MapCamel1000.gif">
<!--*/
// -->END_HTML
// FOAM is now fully integrated with the ROOT package.
// The important bonus of the ROOT use is persistency of the FOAM objects!
//
// For more sophisticated problems full version of FOAM may be more appropriate:
//BEGIN_HTML <!--
/* -->
  See <A HREF="http://jadach.home.cern.ch/jadach/Foam/Index.html"> full version of FOAM</A>
<!--*/
// -->END_HTML
// Simple example of the use of FOAM:
// ==================================
// Int_t kanwa(){
//   gSystem->Load("libFoam");
//   TH2D  *hst_xy = new TH2D("hst_xy" ,  "x-y plot", 50,0,1.0, 50,0,1.0);
//   Double_t *MCvect =new Double_t[2]; // 2-dim vector generated in the MC run
//   TRandom3  *PseRan   = new TRandom3();  // Create random number generator
//   PseRan->SetSeed(4357);                // Set seed
//   TFoam   *FoamX    = new TFoam("FoamX");   // Create Simulator
//   FoamX->SetkDim(2);          // No. of dimensions, obligatory!
//   FoamX->SetnCells(500);      // No. of cells, can be omitted, default=2000
//   FoamX->SetRhoInt(Camel2);   // Set 2-dim distribution, included below
//   FoamX->SetPseRan(PseRan);   // Set random number generator
//   FoamX->Initialize();        // Initialize simulator, takes a few seconds...
//   // From now on FoamX is ready to generate events according to Camel2(x,y)
//   for(Long_t loop=0; loop<100000; loop++){
//     FoamX->MakeEvent();          // generate MC event
//     FoamX->GetMCvect( MCvect);   // get generated vector (x,y)
//     Double_t x=MCvect[0];
//     Double_t y=MCvect[1];
//     if(loop<10) cout<<"(x,y) =  ( "<< x <<", "<< y <<" )"<<endl;
//     hst_xy->Fill(x,y);           // fill scattergram
//   }// loop
//   Double_t mcResult, mcError;
//   FoamX->GetIntegMC( mcResult, mcError);  // get MC integral, should be one
//   cout << " mcResult= " << mcResult << " +- " << mcError <<endl;
//   // now hst_xy will be plotted visualizing generated distribution
//   TCanvas *cKanwa = new TCanvas("cKanwa","Canvas for plotting",600,600);
//   cKanwa->cd();
//   hst_xy->Draw("lego2");
// }//kanwa
// Double_t sqr(Double_t x){return x*x;};
// Double_t Camel2(Int_t nDim, Double_t *Xarg){
// // 2-dimensional distribution for FOAM, normalized to one (within 1e-5)
//   Double_t x=Xarg[0];
//   Double_t y=Xarg[1];
//   Double_t GamSq= sqr(0.100e0);
//   Double_t Dist=exp(-(sqr(x-1./3) +sqr(y-1./3))/GamSq)/GamSq/TMath::Pi();
//   Dist        +=exp(-(sqr(x-2./3) +sqr(y-2./3))/GamSq)/GamSq/TMath::Pi();
//   return 0.5*Dist;
// }// Camel2
// Two-dim. histogram of the MC points generated with the above program looks as follows:
//BEGIN_HTML <!--
/* -->
<img src="gif/foam_cKanwa.gif">
<!--*/
// -->END_HTML
// Canonical nine steering parameters of FOAM
// ===========================================
//------------------------------------------------------------------------------
//  Name     | default  | Description
//------------------------------------------------------------------------------
//  kDim     | 0        | Dimension of the integration space. Must be redefined!
//  nCells   | 1000     | No of allocated number of cells,
//  nSampl   | 200      | No. of MC events in the cell MC exploration
//  nBin     | 8        | No. of bins in edge-histogram in cell exploration
//  OptRej   | 1        | OptRej = 0, weighted; OptRej=1, wt=1 MC events
//  OptDrive | 2        | Maximum weight reduction, =1 for variance reduction
//  EvPerBin | 25       | Maximum number of the effective wt=1 events/bin,
//           |          | EvPerBin=0 deactivates this option
//  Chat     | 1        | =0,1,2 is the ``chat level'' in the standard output
//  MaxWtRej | 1.1      | Maximum weight used to get w=1 MC events
//------------------------------------------------------------------------------
// The above can be redefined before calling 'Initialize()' method,
// for instance FoamObject->SetkDim(15) sets dimension of the distribution to 15.
// Only kDim HAS TO BE redefined, the other parameters may be left at their defaults.
// nCell may be increased up to about million cells for wildly peaked distributions.
// Increasing nSampl sometimes helps, but it may cost CPU time.
// MaxWtRej may need to be increased for wild a distribution, while using OptRej=0.
//
// --------------------------------------------------------------------
// Past versions of FOAM: August 2003, v.1.00; September 2003 v.1.01
// Adopted starting from FOAM-2.06 by P. Sawicki
// --------------------------------------------------------------------
// Users of FOAM are kindly requested to cite the following work:
// S. Jadach, Computer Physics Communications 152 (2003) 55.
//
//______________________________________________________________________________

// #include "TFoam.h"
// #include "TFoamIntegrand.h"
// #include "TFoamMaxwt.h"
// #include "TFoamVect.h"
// #include "TFoamCell.h"
// #include "Riostream.h"
// #include "TH1.h"
// #include "TRefArray.h"
// #include "TMethodCall.h"
// #include "TRandom.h"
// #include "TMath.h"
// #include "G__ci.h"

ClassImp(TMVA::TFoam);

//FFFFFF  BoX-FORMATs for nice and flexible outputs
#define BXOPE cout<<\
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"<<endl<<\
"F                                                                              F"<<endl
#define BXTXT(text) cout<<\
"F                   "<<setw(40)<<         text           <<"                   F"<<endl
#define BX1I(name,numb,text) cout<<\
"F "<<setw(10)<<name<<" = "<<setw(10)<<numb<<" = "          <<setw(50)<<text<<" F"<<endl
#define BX1F(name,numb,text)     cout<<"F "<<setw(10)<<name<<\
          " = "<<setw(15)<<setprecision(8)<<numb<<"   =    "<<setw(40)<<text<<" F"<<endl
#define BX2F(name,numb,err,text) cout<<"F "<<setw(10)<<name<<\
" = "<<setw(15)<<setprecision(8)<<numb<<" +- "<<setw(15)<<setprecision(8)<<err<<\
                                                      "  = "<<setw(25)<<text<<" F"<<endl
#define BXCLO cout<<\
"F                                                                              F"<<endl<<\
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"<<endl
  //FFFFFF  BoX-FORMATs ends here

static const Double_t gHigh= 1.0e150;
static const Double_t gVlow=-1.0e150;

#define SW2 setprecision(7) << setw(12)

//________________________________________________________________________________________________
TMVA::TFoam::TFoam()
{
  // Default constructor for streamer, user should not use it.
   fDim      = 0;
   fNoAct    = 0;
   fNCells   = 0;
   fRNmax    = 0;
   fMaskDiv  = 0;
   fInhiDiv  = 0;
   fXdivPRD  = 0;
   fCells    = 0;
   fAlpha    = 0;
   fCellsAct = 0;
   fPrimAcu  = 0;
   fHistEdg  = 0;
   fHistWt   = 0;
   fHistDbg  = 0;
   fMCMonit  = 0;
   fRho      = 0;  // Integrand function
   fMCvect   = 0;
   fRvec     = 0;
   fPseRan   = 0;  // generator of pseudorandom numbers

}
//_________________________________________________________________________________________________
TMVA::TFoam::TFoam(const Char_t* Name)
{
// User constructor, to be employed by the user

   if(strlen(Name)  >129) {
      Error("TFoam","Name too long %s \n",Name);
   }
   fName=Name;                                            // Class name
   fDate="  Release date:  2005.04.10";                   // Release date
   fVersion= "1.02M";                                     // Release version
   fMaskDiv  = 0;             // Dynamic Mask for  cell division, h-cubic
   fInhiDiv  = 0;             // Flag allowing to inhibit cell division in certain projection/edge
   fXdivPRD  = 0;             // Lists of division values encoded in one vector per direction
   fCells    = 0;
   fAlpha    = 0;
   fCellsAct = 0;
   fPrimAcu  = 0;
   fHistEdg  = 0;
   fHistWt   = 0;
   fHistDbg  = 0;
   fDim     = 0;                // dimension of hyp-cubical space
   fNCells   = 1000;             // Maximum number of Cells,    is usually re-set
   fNSampl   = 200;              // No of sampling when dividing cell
   fOptPRD   = 0;                // General Option switch for PRedefined Division, for quick check
   fOptDrive = 2;                // type of Drive =1,2 for TrueVol,Sigma,WtMax
   fChat     = 1;                // Chat=0,1,2 chat level in output, Chat=1 normal level
   fOptRej   = 1;                // OptRej=0, wted events; OptRej=1, wt=1 events
   //------------------------------------------------------
   fNBin     = 8;                // binning of edge-histogram in cell exploration
   fEvPerBin =25;                // maximum no. of EFFECTIVE event per bin, =0 option is inactive
   //------------------------------------------------------
   fNCalls = 0;                  // No of function calls
   fNEffev = 0;                  // Total no of eff. wt=1 events in build=up
   fLastCe =-1;                  // Index of the last cell
   fNoAct  = 0;                  // No of active cells (used in MC generation)
   fWtMin = gHigh;               // Minimal weight
   fWtMax = gVlow;               // Maximal weight
   fMaxWtRej =1.10;              // Maximum weight in rejection for getting wt=1 events
   fPseRan   = 0;                // Initialize private copy of random number generator
   fMCMonit  = 0;                // MC efficiency monitoring
   fRho = 0;                     // pointer to abstract class providing function to integrate
   fMethodCall=0;                // ROOT's pointer to global distribution function
}

//_______________________________________________________________________________________________
TMVA::TFoam::~TFoam()
{
// Default destructor
//  cout<<" DESTRUCTOR entered "<<endl;
   Int_t i;

   if(fCells!= 0) {
      for(i=0; i<fNCells; i++) delete fCells[i]; // TFoamCell*[]
      delete [] fCells;
   }
   delete [] fRvec;    //double[]
   delete [] fAlpha;   //double[]
   delete [] fMCvect;  //double[]
   delete [] fPrimAcu; //double[]
   delete [] fMaskDiv; //int[]
   delete [] fInhiDiv; //int[]
 
   if( fXdivPRD!= 0) {
      for(i=0; i<fDim; i++) delete fXdivPRD[i]; // TFoamVect*[]
      delete [] fXdivPRD;
   }
   delete fMCMonit;
   delete fHistWt;
}


//_____________________________________________________________________________________________
TMVA::TFoam::TFoam(const TFoam &From): TObject(From)
{
// Copy Constructor  NOT IMPLEMENTED (NEVER USED)
   Error("TFoam", "COPY CONSTRUCTOR NOT IMPLEMENTED \n");
}

//_____________________________________________________________________________________________
void TMVA::TFoam::Initialize(TRandom *PseRan, TFoamIntegrand *fun )
{
// Basic initialization of FOAM invoked by the user. Mandatory!
// ============================================================
// This method starts the process of the cell build-up.
// User must invoke Initialize with two arguments or Initialize without arguments.
// This is done BEFORE generating first MC event and AFTER allocating FOAM object
// and reseting (optionally) its internal parameters/switches.
// The overall operational scheme of the FOAM is the following:
//BEGIN_HTML <!--
/* -->
<img src="gif/foam_schema2.gif">
<!--*/
// -->END_HTML
//
// This method invokes several other methods:
// ==========================================
// InitCells initializes memory storage for cells and begins exploration process
// from the root cell. The empty cells are allocated/filled using  CellFill.
// The procedure Grow which loops over cells, picks up the cell with the biggest
// ``driver integral'', see Comp. Phys. Commun. 152 152 (2003) 55 for explanations,
// with the help of PeekMax procedure. The chosen cell is split using Divide.
// Subsequently, the procedure Explore called by the Divide
// (and by InitCells for the root cell) does the most important
// job in the FOAM object build-up: it performs a small MC run for each
// newly allocated daughter cell.
// Explore calculates how profitable the future split of the cell will be
// and defines the optimal cell division geometry with the help of Carver or Varedu
// procedures, for maximum weight or variance optimization respectively.
// All essential results of the exploration are written into
// the explored cell object. At the very end of the foam build-up,
// Finally, MakeActiveList is invoked to create a list of pointers to
// all active cells, for the purpose of the quick access during the MC generation.
// The procedure Explore employs MakeAlpha to generate random coordinates
// inside a given cell with the uniform distribution.
// The above sequence of the procedure calls is depicted in the following figure:
//BEGIN_HTML <!--
/* -->
<img src="gif/foam_Initialize_schema.gif">
<!--*/
// -->END_HTML

   SetPseRan(PseRan);
   SetRho(fun);
   Initialize();
}

//_______________________________________________________________________________________
void TMVA::TFoam::Initialize()
{
// Basic initialization of FOAM invoked by the user.
// IMPORTANT: Random number generator and the distribution object has to be
// provided using SetPseRan and SetRho prior to invoking this initializator!

   Bool_t addStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   Int_t i;

   if(fChat>0){
      BXOPE;
      BXTXT("****************************************");
      BXTXT("******   TMVA::TFoam::Initialize  ******");
      BXTXT("****************************************");
      BXTXT(fName);
      BX1F("  Version",fVersion,  fDate);
      BX1I("     kDim",fDim,     " Dimension of the hyper-cubical space             ");
      BX1I("   nCells",fNCells,   " Requested number of Cells (half of them active)  ");
      BX1I("   nSampl",fNSampl,   " No of MC events in exploration of a cell         ");
      BX1I("     nBin",fNBin,     " No of bins in histograms, MC exploration of cell ");
      BX1I(" EvPerBin",fEvPerBin, " Maximum No effective_events/bin, MC exploration  ");
      BX1I(" OptDrive",fOptDrive, " Type of Driver   =1,2 for Sigma,WtMax            ");
      BX1I("   OptRej",fOptRej,   " MC rejection on/off for OptRej=0,1               ");
      BX1F(" MaxWtRej",fMaxWtRej, " Maximum wt in rejection for wt=1 evts");
      BXCLO;
   }

   if(fPseRan==0) Error("Initialize", "Random number generator not set \n");
   if(fRho==0 && fMethodCall==0 ) Error("Initialize", "Distribution function not set \n");
   if(fDim==0) Error("Initialize", "Zero dimension not allowed \n");

   /////////////////////////////////////////////////////////////////////////
   //                   ALLOCATE SMALL LISTS                              //
   //  it is done globally, not for each cell, to save on allocation time //
   /////////////////////////////////////////////////////////////////////////
   fRNmax= fDim+1;
   fRvec = new Double_t[fRNmax];   // Vector of random numbers
   if(fRvec==0)  Error("Initialize", "Cannot initialize buffer fRvec \n");

   if(fDim>0){
      fAlpha = new Double_t[fDim];    // sum<1 for internal parametrization of the simplex
      if(fAlpha==0)  Error("Initialize", "Cannot initialize buffer fAlpha \n" );
   }
   fMCvect = new Double_t[fDim]; // vector generated in the MC run
   if(fMCvect==0)  Error("Initialize", "Cannot initialize buffer fMCvect  \n" );

   //====== List of directions inhibited for division
   if(fInhiDiv == 0){
      fInhiDiv = new Int_t[fDim];
      for(i=0; i<fDim; i++) fInhiDiv[i]=0;
   }
   //====== Dynamic mask used in Explore for edge determination
   if(fMaskDiv == 0){
      fMaskDiv = new Int_t[fDim];
      for(i=0; i<fDim; i++) fMaskDiv[i]=1;
   }
   //====== List of predefined division values in all directions (initialized as empty)
   if(fXdivPRD == 0){
      fXdivPRD = new TFoamVect*[fDim];
      for(i=0; i<fDim; i++)  fXdivPRD[i]=0; // Artificially extended beyond fDim
   }
   //====== Initialize list of histograms
   fHistWt  = new TH1D("HistWt","Histogram of MC weight",100,0.0, 1.5*fMaxWtRej); // MC weight
   fHistEdg = new TObjArray(fDim);           // Initialize list of histograms
   TString hname;
   TString htitle;
   for(i=0;i<fDim;i++){
      hname=fName+TString("_HistEdge_");
      hname+=i;
      htitle=TString("Edge Histogram No. ");
      htitle+=i;
      //cout<<"i= "<<i<<"  hname= "<<hname<<"  htitle= "<<htitle<<endl;
      (*fHistEdg)[i] = new TH1D(hname.Data(),htitle.Data(),fNBin,0.0, 1.0); // Initialize histogram for each edge
      ((TH1D*)(*fHistEdg)[i])->Sumw2();
   }
   //======  extra histograms for debug purposes
   fHistDbg = new TObjArray(fDim);         // Initialize list of histograms
   for(i=0;i<fDim;i++){
      hname=fName+TString("_HistDebug_");
      hname+=i;
      htitle=TString("Debug Histogram ");
      htitle+=i;
      (*fHistDbg)[i] = new TH1D(hname.Data(),htitle.Data(),fNBin,0.0, 1.0); // Initialize histogram for each edge
   }

   // ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| //
   //                     BUILD-UP of the FOAM                            //
   // ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| //
   //
   //        Define and explore root cell(s)
   InitCells();
   //        PrintCells(); cout<<" ===== after InitCells ====="<<endl;
   Grow();
   //        PrintCells(); cout<<" ===== after Grow      ====="<<endl;

   MakeActiveList(); // Final Preperations for the M.C. generation

   // Preperations for the M.C. generation
   fSumWt  = 0.0;               // M.C. generation sum of Wt
   fSumWt2 = 0.0;               // M.C. generation sum of Wt**2
   fSumOve = 0.0;               // M.C. generation sum of overweighted
   fNevGen = 0.0;               // M.C. generation sum of 1d0
   fWtMax  = gVlow;               // M.C. generation maximum wt
   fWtMin  = gHigh;               // M.C. generation minimum wt
   fMCresult=fCells[0]->GetIntg(); // M.C. Value of INTEGRAL,temporary assignment
   fMCresult=fCells[0]->GetIntg(); // M.C. Value of INTEGRAL,temporary assignment
   fMCerror =fCells[0]->GetIntg(); // M.C. Value of ERROR   ,temporary assignment
   fMCMonit = new TFoamMaxwt(5.0,1000);  // monitoring M.C. efficiency
   //
   if(fChat>0){
      Double_t driver = fCells[0]->GetDriv();
      BXOPE;
      BXTXT("*** TMVA::TFoam::Initialize FINISHED!!! ***");
      BX1I("    nCalls",fNCalls,  "Total number of function calls         ");
      BX1F("    XPrime",fPrime,   "Primary total integral                 ");
      BX1F("    XDiver",driver,    "Driver  total integral                 ");
      BX1F("  mcResult",fMCresult,"Estimate of the true MC Integral       ");
      BXCLO;
   }
   if(fChat==2) PrintCells();
   TH1::AddDirectory(addStatus);
} // Initialize

//_______________________________________________________________________________________
void TMVA::TFoam::InitCells()
{
// Internal subprogram used by Initialize.
// It initializes "root part" of the FOAM of the tree of cells.

   Int_t i;

   fLastCe =-1;                             // Index of the last cell
   if(fCells!= 0) {
      for(i=0; i<fNCells; i++) delete fCells[i];
      delete [] fCells;
   }
   //
   fCells = new TFoamCell*[fNCells];
   for(i=0;i<fNCells;i++){
      fCells[i]= new TFoamCell(fDim); // Allocate BIG list of cells
      fCells[i]->SetSerial(i);
   }
   if(fCells==0) Error("InitCells", "Cannot initialize CELLS \n"  );

   /////////////////////////////////////////////////////////////////////////////
   //              Single Root Hypercube                                      //
   /////////////////////////////////////////////////////////////////////////////
   CellFill(1,   0);  //  0-th cell ACTIVE

   // Exploration of the root cell(s)
   for(Long_t iCell=0; iCell<=fLastCe; iCell++){
      Explore( fCells[iCell] );               // Exploration of root cell(s)
   }
}//InitCells

//_______________________________________________________________________________________
Int_t TMVA::TFoam::CellFill(Int_t Status, TFoamCell *parent)
{
// Internal subprogram used by Initialize.
// It initializes content of the newly allocated active cell.

   TFoamCell *cell;
   if (fLastCe==fNCells){
      Error( "CellFill", "Too many cells\n");
   }
   fLastCe++;   // 0-th cell is the first
   if (Status==1) fNoAct++;

   cell = fCells[fLastCe];

   cell->Fill(Status, parent, 0, 0);

   cell->SetBest( -1);         // pointer for planning division of the cell
   cell->SetXdiv(0.5);         // factor for division
   Double_t xInt2,xDri2;
   if(parent!=0){
      xInt2  = 0.5*parent->GetIntg();
      xDri2  = 0.5*parent->GetDriv();
      cell->SetIntg(xInt2);
      cell->SetDriv(xDri2);
   }else{
      cell->SetIntg(0.0);
      cell->SetDriv(0.0);
   }
   return fLastCe;
}

//______________________________________________________________________________________
void TMVA::TFoam::Explore(TFoamCell *cell)
{
// Internal subprogram used by Initialize.
// It explores newly defined cell with help of special short MC sampling.
// As a result, estimates of true and drive volume is defined/determined
// Average and dispersion of the weight distribution will is found along
// each edge and the best edge (minimum dispersion, best maximum weight)
// is memorized for future use.
// The optimal division point for eventual future cell division is
// determined/recorded. Recorded are also minimum and maximum weight etc.
// The volume estimate in all (inactive) parent cells is updated.
// Note that links to parents and initial volume = 1/2 parent has to be
// already defined prior to calling this routine.

   Double_t wt, dx, xBest, yBest;
   Double_t intOld, driOld;

   Long_t iev;
   Double_t nevMC;
   Int_t i, j, k;
   Int_t nProj, kBest;
   Double_t ceSum[5], xproj;

   TFoamVect  cellSize(fDim);
   TFoamVect  cellPosi(fDim);

   cell->GetHcub(cellPosi,cellSize);

   TFoamCell  *parent;

   Double_t *xRand = new Double_t[fDim];

   Double_t *volPart=0;

   cell->CalcVolume();
   dx = cell->GetVolume();
   intOld = cell->GetIntg(); //memorize old values,
   driOld = cell->GetDriv(); //will be needed for correcting parent cells


   /////////////////////////////////////////////////////
   //    Special Short MC sampling to probe cell      //
   /////////////////////////////////////////////////////
   ceSum[0]=0;
   ceSum[1]=0;
   ceSum[2]=0;
   ceSum[3]=gHigh;  //wtmin
   ceSum[4]=gVlow;  //wtmax
   //
   for(i=0;i<fDim;i++) ((TH1D *)(*fHistEdg)[i])->Reset(); // Reset histograms
   fHistWt->Reset();
   //
   // ||||||||||||||||||||||||||BEGIN MC LOOP|||||||||||||||||||||||||||||
   Double_t nevEff=0.;
   for(iev=0;iev<fNSampl;iev++){
      MakeAlpha();               // generate uniformly vector inside hypercube

      if(fDim>0){
      for(j=0; j<fDim; j++)
         xRand[j]= cellPosi[j] +fAlpha[j]*(cellSize[j]);
      }

      wt=dx*Eval(xRand);

      nProj = 0;
      if(fDim>0) {
         for(k=0; k<fDim; k++) {
            xproj =fAlpha[k];
            ((TH1D *)(*fHistEdg)[nProj])->Fill(xproj,wt);
            nProj++;
         }
      }
      //
      fNCalls++;
      ceSum[0] += wt;    // sum of weights
      ceSum[1] += wt*wt; // sum of weights squared
      ceSum[2]++;        // sum of 1
      if (ceSum[3]>wt) ceSum[3]=wt;  // minimum weight;
      if (ceSum[4]<wt) ceSum[4]=wt;  // maximum weight
      // test MC loop exit condition
      nevEff = ceSum[0]*ceSum[0]/ceSum[1];
      if( nevEff >= fNBin*fEvPerBin) break;
   }   // ||||||||||||||||||||||||||END MC LOOP|||||||||||||||||||||||||||||
   //------------------------------------------------------------------
   //---  predefine logics of searching for the best division edge ---
   for(k=0; k<fDim;k++){
      fMaskDiv[k] =1;                       // default is all
      if( fInhiDiv[k]==1) fMaskDiv[k] =0; // inhibit some...
   }
   // Note that predefined division below overrule inhibition above
   kBest=-1;
   Double_t rmin,rmax,rdiv;
   if(fOptPRD) {          // quick check
      for(k=0; k<fDim; k++) {
         rmin= cellPosi[k];
         rmax= cellPosi[k] +cellSize[k];
         if( fXdivPRD[k] != 0) {
            Int_t n= (fXdivPRD[k])->GetDim();
            for(j=0; j<n; j++) {
               rdiv=(*fXdivPRD[k])[j];
               // check predefined divisions is available in this cell
               if( (rmin +1e-99 <rdiv) && (rdiv< rmax -1e-99)) {
                  kBest=k;
                  xBest= (rdiv-cellPosi[k])/cellSize[k] ;
                  goto ee05;
               }
            }
         }
      }//k
   }
   ee05:
   //------------------------------------------------------------------
   fNEffev += (Long_t)nevEff;
   nevMC          = ceSum[2];
   Double_t intTrue = ceSum[0]/(nevMC+0.000001);
   Double_t intDriv=0.;
   Double_t intPrim=0.;

   switch(fOptDrive){
   case 1:                       // VARIANCE REDUCTION
      if(kBest == -1) Varedu(ceSum,kBest,xBest,yBest); // determine the best edge,
      //intDriv =sqrt( ceSum[1]/nevMC -intTrue*intTrue ); // Older ansatz, numerically not bad
      intDriv =sqrt(ceSum[1]/nevMC) -intTrue; // Foam build-up, sqrt(<w**2>) -<w>
      intPrim =sqrt(ceSum[1]/nevMC);          // MC gen. sqrt(<w**2>) =sqrt(<w>**2 +sigma**2)
      break;
   case 2:                       // WTMAX  REDUCTION
      if(kBest == -1) Carver(kBest,xBest,yBest);  // determine the best edge
      intDriv =ceSum[4] -intTrue; // Foam build-up, wtmax-<w>
      intPrim =ceSum[4];          // MC generation, wtmax!
      break;
   default:
      Error("Explore", "Wrong fOptDrive = \n" );
   }//switch
   //=================================================================================
   //hist_Neff_distrib.Fill( fLastCe/2.0+0.01, nevEff+0.01);  //
   //hist_kBest_distrib.Fill( kBest+0.50, 1.0 ); //  debug
   //hist_xBest_distrib.Fill( xBest+0.01, 1.0 ); //  debug
   //=================================================================================
   cell->SetBest(kBest);
   cell->SetXdiv(xBest);
   cell->SetIntg(intTrue);
   cell->SetDriv(intDriv);
   cell->SetPrim(intPrim);
   // correct/update integrals in all parent cells to the top of the tree
   Double_t  parIntg, parDriv;
   for(parent = cell->GetPare(); parent!=0; parent = parent->GetPare()){
      parIntg = parent->GetIntg();
      parDriv = parent->GetDriv();
      parent->SetIntg( parIntg   +intTrue -intOld );
      parent->SetDriv( parDriv   +intDriv -driOld );
   }
   delete [] volPart;
   delete [] xRand;
   //cell->Print();
} // TFoam::Explore

//______________________________________________________________________________________
void TMVA::TFoam::Varedu(Double_t ceSum[5], Int_t &kBest, Double_t &xBest, Double_t &yBest)
{
// Internal subrogram used by Initialize.
// In determines the best edge candidate and the position of the cell division plane
// in case of the variance reduction for future cell division,
// using results of the MC exploration run stored in fHistEdg

   Double_t nent   = ceSum[2];
   Double_t swAll  = ceSum[0];
   Double_t sswAll = ceSum[1];
   Double_t ssw    = sqrt(sswAll)/sqrt(nent);
   //
   Double_t swIn,swOut,sswIn,sswOut,xLo,xUp;
   kBest =-1;
   xBest =0.5;
   yBest =1.0;
   Double_t maxGain=0.0;
   // Now go over all projections kProj
   for(Int_t kProj=0; kProj<fDim; kProj++) {
      if( fMaskDiv[kProj]) {
         // initialize search over bins
         Double_t sigmIn =0.0; Double_t sigmOut =0.0;
         Double_t sswtBest = gHigh;
         Double_t gain =0.0;
         Double_t xMin=0.0; Double_t xMax=0.0;
         // Double loop over all pairs jLo<jUp
         for(Int_t jLo=1; jLo<=fNBin; jLo++) {
            Double_t aswIn=0;  Double_t asswIn=0;
            for(Int_t jUp=jLo; jUp<=fNBin;jUp++) {
               aswIn  +=     ((TH1D *)(*fHistEdg)[kProj])->GetBinContent(jUp);
               asswIn += Sqr(((TH1D *)(*fHistEdg)[kProj])->GetBinError(  jUp));
               xLo=(jLo-1.0)/fNBin;
               xUp=(jUp*1.0)/fNBin;
               swIn  =        aswIn/nent;
               swOut = (swAll-aswIn)/nent;
               sswIn = sqrt(asswIn)       /sqrt(nent*(xUp-xLo))     *(xUp-xLo);
               sswOut= sqrt(sswAll-asswIn)/sqrt(nent*(1.0-xUp+xLo)) *(1.0-xUp+xLo);
               if( (sswIn+sswOut) < sswtBest) {
                  sswtBest = sswIn+sswOut;
                  gain     = ssw-sswtBest;
                  sigmIn   = sswIn -swIn;  // Debug
                  sigmOut  = sswOut-swOut; // Debug
                  xMin    = xLo;
                  xMax    = xUp;
               }
            }//jUp
         }//jLo
         Int_t iLo = (Int_t) (fNBin*xMin);
         Int_t iUp = (Int_t) (fNBin*xMax);
         //----------DEBUG printout
         //cout<<"@@@@@  xMin xMax = "<<xMin   <<" "<<xMax<<"  iLo= "<<iLo<<"  iUp= "<<iUp;
         //cout<<"  sswtBest/ssw= "<<sswtBest/ssw<<"  Gain/ssw= "<< Gain/ssw<<endl;
         //----------DEBUG auxilary Plot
         for(Int_t iBin=1;iBin<=fNBin;iBin++) {
            if( ((iBin-0.5)/fNBin > xMin) && ((iBin-0.5)/fNBin < xMax) ){
               ((TH1D *)(*fHistDbg)[kProj])->SetBinContent(iBin,sigmIn/(xMax-xMin));
            } else {
               ((TH1D *)(*fHistDbg)[kProj])->SetBinContent(iBin,sigmOut/(1-xMax+xMin));
            }
         }
         if(gain>=maxGain) {
            maxGain=gain;
            kBest=kProj; // <--- !!!!! The best edge
            xBest=xMin;
            yBest=xMax;
            if(iLo == 0)     xBest=yBest; // The best division point
            if(iUp == fNBin) yBest=xBest; // this is not really used
         }
      }
   } //kProj
   //----------DEBUG printout
   //cout<<"@@@@@@@>>>>> kBest= "<<kBest<<"  maxGain/ssw= "<< maxGain/ssw<<endl;
   if( (kBest >= fDim) || (kBest<0) ) Error("Varedu", "Something wrong with kBest \n" );
}          //TFoam::Varedu

//________________________________________________________________________________________
void TMVA::TFoam::Carver(Int_t &kBest, Double_t &xBest, Double_t &yBest)
{
// Internal subrogram used by Initialize.
// Determines the best edge-candidate and the position of the division plane
// for the future cell division, in the case of the optimization of the maximum weight.
// It exploits results of the cell MC exploration run stored in fHistEdg.

   Int_t    kProj,iBin;
   Double_t carve,carvTot,carvMax,carvOne,binMax,binTot,primTot,primMax;
   Int_t    jLow,jUp,iLow,iUp;
   Double_t theBin;
   Int_t    jDivi; // TEST
   Int_t j;

   Double_t *bins  = new Double_t[fNBin];      // bins of histogram for single  PROJECTION
   if(bins==0)    Error("Carver", "Cannot initialize buffer Bins \n" );

   kBest =-1;
   xBest =0.5;
   yBest =1.0;
   carvMax = gVlow;
   primMax = gVlow;
   for(kProj=0; kProj<fDim; kProj++)
      if( fMaskDiv[kProj] ){
      //if( kProj==1 ){
      //cout<<"==================== Carver histogram: kProj ="<<kProj<<"==================="<<endl;
      //((TH1D *)(*fHistEdg)[kProj])->Print("all");
      binMax = gVlow;
      for(iBin=0; iBin<fNBin;iBin++){
         bins[iBin]= ((TH1D *)(*fHistEdg)[kProj])->GetBinContent(iBin+1);
         binMax = TMath::Max( binMax, bins[iBin]);       // Maximum content/bin
      }
      if(binMax < 0 ) {       //case of empty cell
         delete [] bins;
         return;
      }
      carvTot = 0.0;
      binTot  = 0.0;
      for(iBin=0;iBin<fNBin;iBin++){
         carvTot = carvTot + (binMax-bins[iBin]);     // Total Carve (more stable)
         binTot  +=bins[iBin];
      }
      primTot = binMax*fNBin;
      //cout <<"Carver:  CarvTot "<<CarvTot<< "    primTot "<<primTot<<endl;
      jLow =0;
      jUp  =fNBin-1;
      carvOne = gVlow;
      Double_t yLevel = gVlow;
      for(iBin=0; iBin<fNBin;iBin++) {
         theBin = bins[iBin];
         //-----  walk to the left and find first bin > theBin
         iLow = iBin;
         for(j=iBin; j>-1; j-- ) {
            if(theBin< bins[j]) break;
            iLow = j;
         }
         //iLow = iBin;
         //if(iLow>0)     while( (theBin >= bins[iLow-1])&&(iLow >0) ){iLow--;} // horror!!!
         //------ walk to the right and find first bin > theBin
         iUp  = iBin;
         for(j=iBin; j<fNBin; j++) {
            if(theBin< bins[j]) break;
            iUp = j;
         }
         //iUp  = iBin;
         //if(iUp<fNBin-1) while( (theBin >= bins[iUp+1])&&( iUp<fNBin-1 ) ){iUp++;} // horror!!!
         //
         carve = (iUp-iLow+1)*(binMax-theBin);
         if( carve > carvOne) {
            carvOne = carve;
            jLow = iLow;
            jUp  = iUp;
            yLevel = theBin;
         }
      }//iBin
      if( carvTot > carvMax) {
         carvMax   = carvTot;
         primMax   = primTot;
         //cout <<"Carver:   primMax "<<primMax<<endl;
         kBest = kProj;    // Best edge
         xBest = ((Double_t)(jLow))/fNBin;
         yBest = ((Double_t)(jUp+1))/fNBin;
         if(jLow == 0 )       xBest = yBest;
         if(jUp  == fNBin-1) yBest = xBest;
         // division ratio in units of 1/fNBin, testing
         jDivi = jLow;
         if(jLow == 0 )     jDivi=jUp+1;
      }
      //======  extra histograms for debug purposes
      //cout<<"kProj= "<<kProj<<" jLow= "<<jLow<<" jUp= "<<jUp<<endl;
      for(iBin=0;    iBin<fNBin;  iBin++)
         ((TH1D *)(*fHistDbg)[kProj])->SetBinContent(iBin+1,binMax);
      for(iBin=jLow; iBin<jUp+1;   iBin++)
         ((TH1D *)(*fHistDbg)[kProj])->SetBinContent(iBin+1,yLevel);
   }//kProj
   if( (kBest >= fDim) || (kBest<0) ) Error("Carver", "Something wrong with kBest \n" );
   delete [] bins;
}          //TFoam::Carver

//______________________________________________________________________________________________
void TMVA::TFoam::MakeAlpha()
{
// Internal subrogram used by Initialize.
// Provides random vector Alpha  0< Alpha(i) < 1
   Int_t k;
   if(fDim<1) return;

   // simply generate and load kDim uniform random numbers
   fPseRan->RndmArray(fDim,fRvec);   // kDim random numbers needed
   for(k=0; k<fDim; k++) fAlpha[k] = fRvec[k];
} //MakeAlpha


//_____________________________________________________________________________________________
void TMVA::TFoam::Grow()
{
// Internal subrogram used by Initialize.
// It grow new cells by the binary division process.

   Long_t iCell;
   TFoamCell* newCell;

   while ( (fLastCe+2) < fNCells ) {  // this condition also checked inside Divide
      iCell   = PeekMax();            // peek up cell with maximum driver integral
      if( (iCell<0) || (iCell>fLastCe) ) Error("Grow", "Wrong iCell \n");
      newCell = fCells[iCell];

      OutputGrow();

      if( Divide( newCell )==0) break;  // and divide it into two
   }
   OutputGrow( true );
   CheckAll(0);   // set arg=1 for more info
}// Grow

//_____________________________________________________________________________________________
void TMVA::TFoam::OutputGrow( Bool_t finished ){
   if( finished ) {
      cout << endl << flush;
      return;
   }
   if(fLastCe!=0) {
      Int_t kEcho=10;
      if(fLastCe>=10000) kEcho=100;
      if( (fLastCe%kEcho)==0) {
	 if(fDim<10)
	    cout<<fDim<<flush;
	 else
	    cout<<"."<<flush;
	 if( (fLastCe%(100*kEcho))==0)  cout<<"|"<<fLastCe<<endl<<flush;
      }
   }
}

//_____________________________________________________________________________________________
Long_t  TMVA::TFoam::PeekMax()
{
// Internal subprogram used by Initialize.
// It finds cell with maximal driver integral for the purpose of the division.

   Long_t  i;
   Long_t iCell = -1;
   Double_t  drivMax, driv;

   drivMax = gVlow;
   for(i=0; i<=fLastCe; i++) {//without root
      if( fCells[i]->GetStat() == 1 ) {
         driv =  TMath::Abs( fCells[i]->GetDriv());
         //cout<<"PeekMax: Driv = "<<driv<<endl;
         if(driv > drivMax) {
            drivMax = driv;
            iCell = i;
         }
      }
   }
   //  cout << 'TFoam_PeekMax: iCell=' << iCell << endl;
   if (iCell == -1)
      cout << "STOP in TFoam::PeekMax: not found iCell=" <<  iCell << endl;
   return(iCell);
}                 // TFoam_PeekMax

//_____________________________________________________________________________________________
Int_t TMVA::TFoam::Divide(TFoamCell *cell)
{
// Internal subrogram used by Initialize.
// It divides cell iCell into two daughter cells.
// The iCell is retained and tagged as inactive, daughter cells are appended
// at the end of the buffer.
// New vertex is added to list of vertices.
// List of active cells is updated, iCell removed, two daughters added
// and their properties set with help of MC sampling (TFoam_Explore)
// Returns Code RC=-1 of buffer limit is reached,  fLastCe=fnBuf.

   Double_t xdiv;
   Int_t   kBest;

   if(fLastCe+1 >= fNCells) Error("Divide", "Buffer limit is reached, fLastCe=fnBuf \n");

   cell->SetStat(0); // reset cell as inactive
   fNoAct--;

   xdiv  = cell->GetXdiv();
   kBest = cell->GetBest();
   if( kBest<0 || kBest>=fDim ) Error("Divide", "Wrong kBest \n");

   //////////////////////////////////////////////////////////////////
   //           define two daughter cells (active)                 //
   //////////////////////////////////////////////////////////////////

   Int_t d1 = CellFill(1,   cell);
   Int_t d2 = CellFill(1,   cell);
   cell->SetDau0((fCells[d1]));
   cell->SetDau1((fCells[d2]));
   Explore( (fCells[d1]) );
   Explore( (fCells[d2]) );
   return 1;
} // TFoam_Divide


//_________________________________________________________________________________________
void TMVA::TFoam::MakeActiveList()
{
// Internal subrogram used by Initialize.
// It finds out number of active cells fNoAct,
// creates list of active cell fCellsAct and primary cumulative fPrimAcu.
// They are used during the MC generation to choose randomly an active cell.

   Long_t n, iCell;
   Double_t sum;
   // flush previous result
   if(fPrimAcu  != 0) delete [] fPrimAcu;
   if(fCellsAct != 0) delete fCellsAct;

   // Allocate tables of active cells
   fCellsAct = new TRefArray();

   // Count Active cells and find total Primary
   // Fill-in tables of active cells

   fPrime = 0.0; n = 0;
   for(iCell=0; iCell<=fLastCe; iCell++) { 
      if (fCells[iCell]->GetStat()==1) {
         fPrime += fCells[iCell]->GetPrim();
         fCellsAct->Add(fCells[iCell]);
         n++;
      }
   }

   if(fNoAct != n)  Error("MakeActiveList", "Wrong fNoAct               \n"  );
   if(fPrime == 0.) Error("MakeActiveList", "Integrand function is zero  \n"  );

   fPrimAcu  = new  Double_t[fNoAct]; // cumulative primary for MC generation
   if( fCellsAct==0 || fPrimAcu==0 ) Error("MakeActiveList", "Cant allocate fCellsAct or fPrimAcu \n");

   sum =0.0;
   for(iCell=0; iCell<fNoAct; iCell++) {
      sum = sum + ( (TFoamCell *) (fCellsAct->At(iCell)) )->GetPrim()/fPrime;
      fPrimAcu[iCell]=sum;
   }

} //MakeActiveList

//__________________________________________________________________________________________
void TMVA::TFoam::ResetPseRan(TRandom *PseRan)
{
// User may optionally reset random number generator using this method
// Usually it is done when FOAM object is restored from the disk.
// IMPORTANT: this method deletes existing  random number generator registered in the FOAM object.
// In particular such an object is created by the streamer during the disk-read operation.

   if(fPseRan) {
      Info("ResetPseRan", "Resetting random number generator  \n");
      delete fPseRan;
   }
   SetPseRan(PseRan);
}

//__________________________________________________________________________________________
void TMVA::TFoam::SetRho(TFoamIntegrand *fun)
{
// User may use this method to set (register) random number generator used by
// the given instance of the FOAM event generator. Note that single r.n. generator
// may serve several FOAM objects.

   if (fun)
      fRho=fun;
   else
      Error("SetRho", "Bad function \n" );
}

//__________________________________________________________________________________________
void TMVA::TFoam::ResetRho(TFoamIntegrand *fun)
{
// User may optionally reset the distribution using this method
// Usually it is done when FOAM object is restored from the disk.
// IMPORTANT: this method deletes existing  distribution object registered in the FOAM object.
// In particular such an object is created by the streamer diring the disk-read operation.
// This method is used only in very special cases, because the distribution in most cases
// should be "owned" by the FOAM object and should not be replaced by another one after initialization.

   if(fRho) {
      Info("ResetRho", "!!! Resetting distribution function  !!!\n");
      delete fRho;
   }
   SetRho(fun);
}

//__________________________________________________________________________________________
void TMVA::TFoam::SetRhoInt(void *fun)
{
// User may use this to set pointer to the global function (not descending
// from TFoamIntegrand) serving as a distribution for FOAM.
// It is useful for simple interactive applications.
// Note that persistency for FOAM object will not work in the case of such
// a distribution.

   const Char_t *namefcn = G__p2f2funcname(fun); //name of integrand function
   if(namefcn) {
      fMethodCall=new TMethodCall();
      fMethodCall->InitWithPrototype(namefcn, "Int_t, Double_t *");
   }
   fRho=0;
}

//__________________________________________________________________________________________
Double_t TMVA::TFoam::Eval(Double_t *xRand)
{
// Internal subprogram.
// Evaluates distribution to be generated.

   Double_t result;

   if(!fRho) {   //interactive mode
      Long_t paramArr[3];
      paramArr[0]=(Long_t)fDim;
      paramArr[1]=(Long_t)xRand;
      fMethodCall->SetParamPtrs(paramArr);
      fMethodCall->Execute(result);
   } else {       //compiled mode
      result=fRho->Density(fDim,xRand);
   }

   return result;
}

//___________________________________________________________________________________________
void TMVA::TFoam::GenerCel2(TFoamCell *&pCell)
{
// Internal subprogram.
// Return randomly chosen active cell with probability equal to its
// contribution into total driver integral using interpolation search.

   Long_t  lo, hi, hit;
   Double_t fhit, flo, fhi;
   Double_t random;

   random=fPseRan->Rndm();
   lo  = 0;              hi =fNoAct-1;
   flo = fPrimAcu[lo];  fhi=fPrimAcu[hi];
   while(lo+1<hi) {
      hit = lo + (Int_t)( (hi-lo)*(random-flo)/(fhi-flo)+0.5);
      if (hit<=lo)
         hit = lo+1;
      else if(hit>=hi)
         hit = hi-1;
      fhit=fPrimAcu[hit];
      if (fhit>random) {
         hi = hit;
         fhi = fhit;
      } else {
         lo = hit;
         flo = fhit;
      }
   }
   if (fPrimAcu[lo]>random)
      pCell = (TFoamCell *) fCellsAct->At(lo);
   else
      pCell = (TFoamCell *) fCellsAct->At(hi);
}       // TFoam::GenerCel2


//___________________________________________________________________________________________
void TMVA::TFoam::MakeEvent(void)
{
// User subprogram.
// It generates randomly point/vector according to user-defined distribution.
// Prior initialization with help of Initialize() is mandatory.
// Generated MC point/vector is available using GetMCvect and the MC weight with GetMCwt.
// MC point is generated with wt=1 or with variable weight, see OptRej switch.

   Int_t      j;
   Double_t   wt,dx,mcwt;
   TFoamCell *rCell;
   //
   //********************** MC LOOP STARS HERE **********************
ee0:
   GenerCel2(rCell);   // choose randomly one cell

   MakeAlpha();

   TFoamVect  cellPosi(fDim); TFoamVect  cellSize(fDim);
   rCell->GetHcub(cellPosi,cellSize);
   for(j=0; j<fDim; j++)
      fMCvect[j]= cellPosi[j] +fAlpha[j]*cellSize[j];
   dx = rCell->GetVolume();      // Cartesian volume of the Cell
   //  weight average normalized to PRIMARY integral over the cell

   wt=dx*Eval(fMCvect);

   mcwt = wt / rCell->GetPrim();  // PRIMARY controls normalization
   fNCalls++;
   fMCwt   =  mcwt;
   // accumulation of statistics for the main MC weight
   fSumWt  += mcwt;           // sum of Wt
   fSumWt2 += mcwt*mcwt;      // sum of Wt**2
   fNevGen++;                 // sum of 1d0
   fWtMax  =  TMath::Max(fWtMax, mcwt);   // maximum wt
   fWtMin  =  TMath::Min(fWtMin, mcwt);   // minimum wt
   fMCMonit->Fill(mcwt);
   fHistWt->Fill(mcwt,1.0);          // histogram
   //*******  Optional rejection ******
   if(fOptRej == 1) {
      Double_t random;
      random=fPseRan->Rndm();
      if( fMaxWtRej*random > fMCwt) goto ee0;  // Wt=1 events, internal rejection
      if( fMCwt<fMaxWtRej ) {
         fMCwt = 1.0;                  // normal Wt=1 event
      } else {
         fMCwt = fMCwt/fMaxWtRej;    // weight for overweighted events! kept for debug
         fSumOve += fMCwt-fMaxWtRej; // contribution of overweighted
      }
   }
   //********************** MC LOOP ENDS HERE **********************
} // MakeEvent

//_________________________________________________________________________________
void TMVA::TFoam::GetMCvect(Double_t *MCvect)
{
// User may get generated MC point/vector with help of this method

   for ( Int_t k=0 ; k<fDim ; k++) *(MCvect +k) = fMCvect[k];
}//GetMCvect

//___________________________________________________________________________________
Double_t TMVA::TFoam::GetMCwt(void)
{
// User may get weight MC weight using this method

   return(fMCwt);
}
//___________________________________________________________________________________
void TMVA::TFoam::GetMCwt(Double_t &mcwt)
{
// User may get weight MC weight using this method

   mcwt=fMCwt;
}

//___________________________________________________________________________________
Double_t TMVA::TFoam::MCgenerate(Double_t *MCvect)
{
// User subprogram which generates MC event and returns MC weight

   MakeEvent();
   GetMCvect(MCvect);
   return(fMCwt);
}//MCgenerate

//___________________________________________________________________________________
void TMVA::TFoam::GetIntegMC(Double_t &mcResult, Double_t &mcError)
{
// User subprogram.
// It provides the value of the integral calculated from the averages of the MC run
// May be called after (or during) the MC run.

   Double_t mCerelat;
   mcResult = 0.0;
   mCerelat = 1.0;
   if (fNevGen>0) {
      mcResult = fPrime*fSumWt/fNevGen;
      mCerelat = sqrt( fSumWt2/(fSumWt*fSumWt) - 1/fNevGen);
   }
   mcError = mcResult *mCerelat;
}//GetIntegMC

//____________________________________________________________________________________
void  TMVA::TFoam::GetIntNorm(Double_t& IntNorm, Double_t& Errel )
{
// User subprogram.
// It returns NORMALIZATION integral to be combined with the average weights
// and content of the histograms in order to get proper absolute normalization
// of the integrand and distributions.
// It can be called after initialization, before or during the MC run.

   if(fOptRej == 1) {    // Wt=1 events, internal rejection
      Double_t intMC,errMC;
      GetIntegMC(intMC,errMC);
      IntNorm = intMC;
      Errel   = errMC;
   } else {                // Wted events, NO internal rejection
      IntNorm = fPrime;
      Errel   = 0;
   }
}//GetIntNorm

//______________________________________________________________________________________
void  TMVA::TFoam::GetWtParams(Double_t eps, Double_t &aveWt, Double_t &wtMax, Double_t &sigma)
{
// May be called optionally after the MC run.
// Returns various parameters of the MC weight for efficiency evaluation

   Double_t mCeff, wtLim;
   fMCMonit->GetMCeff(eps, mCeff, wtLim);
   wtMax = wtLim;
   aveWt = fSumWt/fNevGen;
   sigma = sqrt( fSumWt2/fNevGen -aveWt*aveWt );
}//GetmCeff

//_______________________________________________________________________________________
void TMVA::TFoam::Finalize(Double_t& IntNorm, Double_t& Errel)
{
// May be called optionally by the user after the MC run.
// It provides normalization and also prints some information/statistics on the MC run.

   GetIntNorm(IntNorm,Errel);
   Double_t mcResult,mcError;
   GetIntegMC(mcResult,mcError);
   Double_t mCerelat= mcError/mcResult;
   //
   if(fChat>0) {
      Double_t eps = 0.0005;
      Double_t mCeff, mcEf2, wtMax, aveWt, sigma;
      GetWtParams(eps, aveWt, wtMax, sigma);
      mCeff=0;
      if(wtMax>0.0) mCeff=aveWt/wtMax;
      mcEf2 = sigma/aveWt;
      Double_t driver = fCells[0]->GetDriv();
      //
      BXOPE;
      BXTXT("****************************************");
      BXTXT("******  TMVA::TFoam::Finalize    ******");
      BXTXT("****************************************");
      BX1I("    NevGen",fNevGen, "Number of generated events in the MC generation   ");
      BX1I("    nCalls",fNCalls, "Total number of function calls                    ");
      BXTXT("----------------------------------------");
      BX1F("     AveWt",aveWt,    "Average MC weight                      ");
      BX1F("     WtMin",fWtMin,  "Minimum MC weight (absolute)           ");
      BX1F("     WtMax",fWtMax,  "Maximum MC weight (absolute)           ");
      BXTXT("----------------------------------------");
      BX1F("    XPrime",fPrime,  "Primary total integral, R_prime        ");
      BX1F("    XDiver",driver,   "Driver  total integral, R_loss         ");
      BXTXT("----------------------------------------");
      BX2F("    IntMC", mcResult,  mcError,      "Result of the MC Integral");
      BX1F(" mCerelat", mCerelat,  "Relative error of the MC integral      ");
      BX1F(" <w>/WtMax",mCeff,     "MC efficiency, acceptance rate");
      BX1F(" Sigma/<w>",mcEf2,     "MC efficiency, variance/ave_wt");
      BX1F("     WtMax",wtMax,     "WtMax(esp= 0.0005)            ");
      BX1F("     Sigma",sigma,     "variance of MC weight         ");
      if(fOptRej==1) {
         Double_t avOve=fSumOve/fSumWt;
         BX1F("<OveW>/<W>",avOve,     "Contrib. of events wt>MaxWtRej");
      }
      BXCLO;
   }
}  // Finalize

//_____________________________________________________________________________________
void  TMVA::TFoam::SetInhiDiv(Int_t iDim, Int_t InhiDiv)
{
// This can be called before Initialize, after setting kDim
// It defines which variables are excluded in the process of the cell division.
// For example 'FoamX->SetInhiDiv(1, 1);' inhibits division of y-variable.
// The resulting map of cells in 2-dim. case will look as follows:
//BEGIN_HTML <!--
/* -->
<img src="gif/foam_Map2.gif">
<!--*/
// -->END_HTML

   if(fDim==0) Error("TFoam","SetInhiDiv: fDim=0 \n");
   if(fInhiDiv == 0) {
      fInhiDiv = new Int_t[ fDim ];
      for(Int_t i=0; i<fDim; i++) fInhiDiv[i]=0;
   }
   //
   if( ( 0<=iDim) && (iDim<fDim)) {
      fInhiDiv[iDim] = InhiDiv;
   } else
      Error("SetInhiDiv:","Wrong iDim \n");
}//SetInhiDiv

//______________________________________________________________________________________
void  TMVA::TFoam::SetXdivPRD(Int_t iDim, Int_t len, Double_t xDiv[])
{
// This should be called before Initialize, after setting  kDim
// It predefines values of the cell division for certain variable iDim.
// For example setting 3 predefined division lines using:
//     xDiv[0]=0.30; xDiv[1]=0.40; xDiv[2]=0.65;
//     FoamX->SetXdivPRD(0,3,xDiv);
// results in the following 2-dim. pattern of the cells:
//BEGIN_HTML <!--
/* -->
<img src="gif/foam_Map3.gif">
<!--*/
// -->END_HTML

   Int_t i;

   if(fDim<=0)  Error("SetXdivPRD", "fDim=0 \n");
   if(   len<1 )  Error("SetXdivPRD", "len<1 \n");
   // allocate list of pointers, if it was not done before
   if(fXdivPRD == 0) {
      fXdivPRD = new TFoamVect*[fDim];
      for(i=0; i<fDim; i++)  fXdivPRD[i]=0;
   }
  // set division list for direction iDim in H-cubic space!!!
   if( ( 0<=iDim) && (iDim<fDim)) {
      fOptPRD =1;      // !!!!
      if( fXdivPRD[iDim] != 0)
         Error("SetXdivPRD", "Second allocation of XdivPRD not allowed \n");
      fXdivPRD[iDim] = new TFoamVect(len); // allocate list of division points
      for(i=0; i<len; i++) {
         (*fXdivPRD[iDim])[i]=xDiv[i]; // set list of division points
      }
   } else {
      Error("SetXdivPRD", "Wrong iDim  \n");
   }
   // Priting predefined division points
   cout<<" SetXdivPRD, idim= "<<iDim<<"  len= "<<len<<"   "<<endl;
   for(i=0; i<len; i++) {
      cout<< (*fXdivPRD[iDim])[i] <<"  ";
   }
   cout<<endl;
   for(i=0; i<len; i++)  cout<< xDiv[i] <<"   ";
   cout<<endl;
   //
}//SetXdivPRD

//_______________________________________________________________________________________
void TMVA::TFoam::CheckAll(Int_t level)
{
//  User utility, miscellaneous and debug.
//  Checks all pointers in the tree of cells. This is useful autodiagnostic.
//  level=0, no printout, failures causes STOP
//  level=1, printout, failures lead to WARNINGS only

   Int_t errors, warnings;
   TFoamCell *cell;
   Long_t iCell;

   errors = 0; warnings = 0;
   if (level==1) cout << "///////////////////////////// FOAM_Checks /////////////////////////////////" << endl;
   for(iCell=1; iCell<=fLastCe; iCell++) {
      cell = fCells[iCell];
      //  checking general rules
      if( ((cell->GetDau0()==0) && (cell->GetDau1()!=0) ) ||
         ((cell->GetDau1()==0) && (cell->GetDau0()!=0) ) ) {
         errors++;
         if (level==1) Error("CheckAll","ERROR: Cell's no %d has only one daughter \n",iCell);
      }
      if( (cell->GetDau0()==0) && (cell->GetDau1()==0) && (cell->GetStat()==0) ) {
         errors++;
         if (level==1) Error("CheckAll","ERROR: Cell's no %d  has no daughter and is inactive \n",iCell);
      }
      if( (cell->GetDau0()!=0) && (cell->GetDau1()!=0) && (cell->GetStat()==1) ) {
         errors++;
         if (level==1) Error("CheckAll","ERROR: Cell's no %d has two daughters and is active \n",iCell);
      }

      // checking parents
      if( (cell->GetPare())!=fCells[0] ) { // not child of the root
         if ( (cell != cell->GetPare()->GetDau0()) && (cell != cell->GetPare()->GetDau1()) ) {
            errors++;
            if (level==1) Error("CheckAll","ERROR: Cell's no %d parent not pointing to this cell\n ",iCell);
         }
      }

      // checking daughters
      if(cell->GetDau0()!=0) {
         if(cell != (cell->GetDau0())->GetPare()) {
            errors++;
            if (level==1)  Error("CheckAll","ERROR: Cell's no %d daughter 0 not pointing to this cell \n",iCell);
         }
      }
      if(cell->GetDau1()!=0) {
         if(cell != (cell->GetDau1())->GetPare()) {
            errors++;
            if (level==1) Error("CheckAll","ERROR: Cell's no %d daughter 1 not pointing to this cell \n",iCell);
         }
      }
   }// loop after cells;

   // Check for empty cells
   for(iCell=0; iCell<=fLastCe; iCell++) {
      cell = fCells[iCell];
      if( (cell->GetStat()==1) && (cell->GetDriv()==0) ) {
         warnings++;
         if(level==1) Warning("CheckAll", "Warning: Cell no. %d is active but empty \n", iCell);
      }
   }
   // summary
   if(level==1){
      Info("CheckAll","Check has found %d errors and %d warnings \n",errors, warnings);
   }
   if(errors>0){
      Info("CheckAll","Check - found total %d  errors \n",errors);
   }
} // Check

//________________________________________________________________________________________
void TMVA::TFoam::PrintCells(void)
{
// Prints geometry of ALL cells of the FOAM

   Long_t iCell;

   for(iCell=0; iCell<=fLastCe; iCell++) {
      cout<<"Cell["<<iCell<<"]={ ";
      //cout<<"  "<< fCells[iCell]<<"  ";  // extra DEBUG
      cout<<endl;
      fCells[iCell]->Print("1");
      cout<<"}"<<endl;
   }
}

//_________________________________________________________________________________________
void TMVA::TFoam::RootPlot2dim(Char_t *filename)
{
// Debugging tool which plots 2-dimensional cells as rectangles
// in C++ format readable for root

   ofstream outfile(filename, ios::out);
   Double_t   x1,y1,x2,y2,x,y;
   Long_t    iCell;
   Double_t offs =0.1;
   Double_t lpag   =1-2*offs;
   outfile<<"{" << endl;
   outfile<<"cMap = new TCanvas(\"Map1\",\" Cell Map \",600,600);"<<endl;
   //
   outfile<<"TBox*a=new TBox();"<<endl;
   outfile<<"a->SetFillStyle(0);"<<endl;  // big frame
   outfile<<"a->SetLineWidth(4);"<<endl;
   outfile<<"a->SetLineColor(2);"<<endl;
   outfile<<"a->DrawBox("<<offs<<","<<offs<<","<<(offs+lpag)<<","<<(offs+lpag)<<");"<<endl;
   //
   outfile<<"TText*t=new TText();"<<endl;  // text for numbering
   outfile<<"t->SetTextColor(4);"<<endl;
   if(fLastCe<51)
      outfile<<"t->SetTextSize(0.025);"<<endl;  // text for numbering
   else if(fLastCe<251)
      outfile<<"t->SetTextSize(0.015);"<<endl;
   else
      outfile<<"t->SetTextSize(0.008);"<<endl;
   //
   outfile<<"TBox*b=new TBox();"<<endl;  // single cell
   outfile <<"b->SetFillStyle(0);"<<endl;
   //
   if(fDim==2 && fLastCe<=2000) {
      TFoamVect  cellPosi(fDim); TFoamVect  cellSize(fDim);
      outfile << "// =========== Rectangular cells  ==========="<< endl;
      for(iCell=1; iCell<=fLastCe; iCell++) {
         if( fCells[iCell]->GetStat() == 1) {
            fCells[iCell]->GetHcub(cellPosi,cellSize);
            x1 = offs+lpag*(        cellPosi[0]); y1 = offs+lpag*(        cellPosi[1]);
            x2 = offs+lpag*(cellPosi[0]+cellSize[0]); y2 = offs+lpag*(cellPosi[1]+cellSize[1]);
            //     cell rectangle
            if(fLastCe<=2000)
            outfile<<"b->DrawBox("<<x1<<","<<y1<<","<<x2<<","<<y2<<");"<<endl;
            //     cell number
            if(fLastCe<=250) {
               x = offs+lpag*(cellPosi[0]+0.5*cellSize[0]); y = offs+lpag*(cellPosi[1]+0.5*cellSize[1]);
               outfile<<"t->DrawText("<<x<<","<<y<<","<<"\""<<iCell<<"\""<<");"<<endl;
            }
         }
      }
      outfile<<"// ============== End Rectangles ==========="<< endl;
   }//kDim=2
   //
   //
   outfile << "}" << endl;
   outfile.close();
}

void TMVA::TFoam::LinkCells()
{
// Void function for backward compatibility

   Info("LinkCells", "VOID function for backward compatibility \n");
   return;
}

////////////////////////////////////////////////////////////////////////////////
//       End of Class TFoam                                                   //
////////////////////////////////////////////////////////////////////////////////

// @(#)root/foam:$Id: TFoamWrapper.cxx,v 1.3 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

// #include "TFoamIntegrand.h"

ClassImp(TMVA::TFoamIntegrand);

//_________________________________________
// Class TFoamIntegrand
// =====================
// Abstract class representing n-dimensional real positive integrand function
// @(#)root/foam:$Id: TFoamWrapper.cxx,v 1.3 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

TMVA::TFoamIntegrand::TFoamIntegrand(){};

//____________________________________________________________________________
//
// Class  TFoamMaxwt
// =================
// Small auxiliary class for controlling MC weight.
// It provides certain measure of the "maximum weight"
// depending on small user-parameter "epsilon".
// It creates and uses 2 histograms of the TH1D class.
// User defines no. of bins nBin,  nBin=1000 is  recommended
// wmax defines weight range (1,wmax), it is adjusted "manually"
//
//____________________________________________________________________________


// #include "Riostream.h"
// #include "TH1.h"
// #include "TFoamMaxwt.h"

ClassImp(TMVA::TFoamMaxwt);

//____________________________________________________________________________
TMVA::TFoamMaxwt::TFoamMaxwt()
{
// Constructor for streamer
   fNent = 0;
   fnBin = 0;
   fWtHst1 = 0;
   fWtHst2 = 0;
}

//____________________________________________________________________________
TMVA::TFoamMaxwt::TFoamMaxwt(Double_t wmax, Int_t nBin)
{
// Principal user constructor
   fNent = 0;
   fnBin = nBin;
   fwmax = wmax;
   fWtHst1 = new TH1D("TFoamMaxwt_hst_Wt1","Histo of weight   ",nBin,0.0,wmax);
   fWtHst2 = new TH1D("TFoamMaxwt_hst_Wt2","Histo of weight**2",nBin,0.0,wmax);
   fWtHst1->SetDirectory(0);// exclude from diskfile
   fWtHst2->SetDirectory(0);// and enable deleting
}

//______________________________________________________________________________
TMVA::TFoamMaxwt::TFoamMaxwt(TFoamMaxwt &From): TObject(From)
{
// Explicit COPY CONSTRUCTOR (unused, so far)
   fnBin   = From.fnBin;
   fwmax   = From.fwmax;
   fWtHst1 = From.fWtHst1;
   fWtHst2 = From.fWtHst2;
   Error("TFoamMaxwt","COPY CONSTRUCTOR NOT TESTED!");
}

//_______________________________________________________________________________
TMVA::TFoamMaxwt::~TFoamMaxwt()
{
// Destructor
   delete fWtHst1; // For this SetDirectory(0) is needed!
   delete fWtHst2; //
   fWtHst1=0;
   fWtHst2=0;
}
//_______________________________________________________________________________
void TMVA::TFoamMaxwt::Reset()
{
// Reseting weight analysis
   fNent = 0;
   fWtHst1->Reset();
   fWtHst2->Reset();
}

//_______________________________________________________________________________
TMVA::TFoamMaxwt& TMVA::TFoamMaxwt::operator=(const TFoamMaxwt &From)
{
// substitution =
   if (&From == this) return *this;
   fnBin = From.fnBin;
   fwmax = From.fwmax;
   fWtHst1 = From.fWtHst1;
   fWtHst2 = From.fWtHst2;
   return *this;
}

//________________________________________________________________________________
void TMVA::TFoamMaxwt::Fill(Double_t wt)
{
// Filling analyzed weight
   fNent =  fNent+1.0;
   fWtHst1->Fill(wt,1.0);
   fWtHst2->Fill(wt,wt);
}

//________________________________________________________________________________
void TMVA::TFoamMaxwt::Make(Double_t eps, Double_t &MCeff)
{
// Calculates Efficiency= aveWt/wtLim for a given tolerance level epsilon<<1
// To be called at the end of the MC run.

   Double_t wtLim,aveWt;
   GetMCeff(eps, MCeff, wtLim);
   aveWt = MCeff*wtLim;
   cout<< "00000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
   cout<< "00 -->wtLim: No_evt ="<<fNent<<"   <Wt> = "<<aveWt<<"  wtLim=  "<<wtLim<<endl;
   cout<< "00 -->wtLim: For eps = "<<eps  <<"    EFFICIENCY <Wt>/wtLim= "<<MCeff<<endl;
   cout<< "00000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
}

//_________________________________________________________________________________
void TMVA::TFoamMaxwt::GetMCeff(Double_t eps, Double_t &MCeff, Double_t &wtLim)
{
// Calculates Efficiency= aveWt/wtLim for a given tolerance level epsilon<<1
// using information stored in two histograms.
// To be called at the end of the MC run.

   Int_t ib,ibX;
   Double_t lowEdge,bin,bin1;
   Double_t aveWt, aveWt1;

   fWtHst1->Print();
   fWtHst2->Print();

// Convention on bin-numbering: nb=1 for 1-st bin, underflow nb=0, overflow nb=Nb+1
   Double_t sum   = 0.0;
   Double_t sumWt = 0.0;
   for(ib=0;ib<=fnBin+1;ib++) {
      sum   += fWtHst1->GetBinContent(ib);
      sumWt += fWtHst2->GetBinContent(ib);
   }
   if( (sum == 0.0) || (sumWt == 0.0) ) {
      cout<<"TFoamMaxwt::Make: zero content of histogram !!!,sum,sumWt ="<<sum<<sumWt<<endl;
   }
   aveWt = sumWt/sum;
   //--------------------------------------
   for( ibX=fnBin+1; ibX>0; ibX--) {
      lowEdge = (ibX-1.0)*fwmax/fnBin;
      sum   = 0.0;
      sumWt = 0.0;
      for( ib=0; ib<=fnBin+1; ib++) {
         bin  = fWtHst1->GetBinContent(ib);
         bin1 = fWtHst2->GetBinContent(ib);
         if(ib >= ibX) bin1=lowEdge*bin;
         sum   += bin;
         sumWt += bin1;
      }
      aveWt1 = sumWt/sum;
      if( TMath::Abs(1.0-aveWt1/aveWt) > eps ) break;
   }
   //---------------------------
   if(ibX == (fnBin+1) ) {
      wtLim = 1.0e200;
      MCeff   = 0.0;
      cout<< "+++++ wtLim undefined. Higher uper limit in histogram"<<endl;
   } else if( ibX == 1) {
      wtLim = 0.0;
      MCeff   =-1.0;
      cout<< "+++++ wtLim undefined. Lower uper limit or more bins "<<endl;
   } else {
      wtLim= (ibX)*fwmax/fnBin; // We over-estimate wtLim, under-estimate MCeff
      MCeff  = aveWt/wtLim;
   }
}
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//      End of    Class  TFoamMaxwt                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
// @(#)root/foam:$Id: TFoamWrapper.cxx,v 1.3 2008/08/07 17:39:02 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

//_____________________________________________________________________________
//                                                                            //
// Auxiliary class TFoamVect of n-dimensional vector, with dynamic allocation //
// used for the cartesian geometry of the TFoam  cells                        //
//                                                                            //
//_____________________________________________________________________________

// #include "Riostream.h"
// #include "TSystem.h"
// #include "TFoamVect.h"

#define SW2 setprecision(7) << setw(12)

ClassImp(TMVA::TFoamVect);

//_____________________________________________________________________________
TMVA::TFoamVect::TFoamVect()
{
// Default constructor for streamer

   fDim    =0;
   fCoords =0;
   fNext   =0;
   fPrev   =0;
}

//______________________________________________________________________________
TMVA::TFoamVect::TFoamVect(Int_t n)
{
// User constructor creating n-dimensional vector
// and allocating dynamically array of components

   Int_t i;
   fNext=0;
   fPrev=0;
   fDim=n;
   fCoords = 0;
   if (n>0) {
      fCoords = new Double_t[fDim];
      if(gDebug) {
         if(fCoords == 0)
            Error("TFoamVect", "Constructor failed to allocate\n");
      }
      for (i=0; i<n; i++) *(fCoords+i)=0.0;
   }
   if(gDebug) Info("TFoamVect", "USER CONSTRUCTOR TFoamVect(const Int_t)\n ");
}

//___________________________________________________________________________
TMVA::TFoamVect::TFoamVect(const TFoamVect &Vect): TObject(Vect)
{
// Copy constructor

   fNext=0;
   fPrev=0;
   fDim=Vect.fDim;
   fCoords = 0;
   if(fDim>0)  fCoords = new Double_t[fDim];
   if(gDebug) {
      if(fCoords == 0) {
         Error("TFoamVect", "Constructor failed to allocate fCoords\n");
      }
   }
   for(Int_t i=0; i<fDim; i++)
      fCoords[i] = Vect.fCoords[i];
   Error("TFoamVect","+++++ NEVER USE Copy constructor !!!!!\n ");
}

//___________________________________________________________________________
TMVA::TFoamVect::~TFoamVect()
{
// Destructor
   if(gDebug) Info("TFoamVect"," DESTRUCTOR TFoamVect~ \n");
   delete [] fCoords; //  free(fCoords)
   fCoords=0;
}


//////////////////////////////////////////////////////////////////////////////
//                     Overloading operators                                //
//////////////////////////////////////////////////////////////////////////////

//____________________________________________________________________________
TMVA::TFoamVect& TMVA::TFoamVect::operator =(const TFoamVect& Vect)
{
// substitution operator

   Int_t i;
   if (&Vect == this) return *this;
   if( fDim != Vect.fDim )
      Error("TFoamVect","operator=Dims. are different: %d and %d \n ",fDim,Vect.fDim);
   if( fDim != Vect.fDim ) {  // cleanup
      delete [] fCoords;
      fCoords = new Double_t[fDim];
   }
   fDim=Vect.fDim;
   for(i=0; i<fDim; i++)
      fCoords[i] = Vect.fCoords[i];
   fNext=Vect.fNext;
   fPrev=Vect.fPrev;
   if(gDebug)  Info("TFoamVect", "SUBSITUTE operator =\n ");
   return *this;
}

//______________________________________________________________________
Double_t &TMVA::TFoamVect::operator[](Int_t n)
{
// [] is for access to elements as in ordinary matrix like a[j]=b[j]
// (Perhaps against some strict rules but rather practical.)
// Range protection is built in, consequently for substitution
// one should use rather use a=b than explicit loop!

   if ((n<0) || (n>=fDim)) {
      Error( "TFoamVect","operator[], out of range \n");
   }
   return fCoords[n];
}

//______________________________________________________________________
TMVA::TFoamVect& TMVA::TFoamVect::operator*=(const Double_t &x)
{
// unary multiplication operator *=

   for(Int_t i=0;i<fDim;i++)
      fCoords[i] = fCoords[i]*x;
   return *this;
}

//_______________________________________________________________________
TMVA::TFoamVect& TMVA::TFoamVect::operator+=(const TFoamVect& Shift)
{
// unary addition operator +=; adding vector c*=x,
   if( fDim != Shift.fDim){
      Error( "TFoamVect","operator+, different dimensions= %d %d \n",fDim,Shift.fDim);
   }
   for(Int_t i=0;i<fDim;i++)
      fCoords[i] = fCoords[i]+Shift.fCoords[i];
   return *this;
}

//________________________________________________________________________
TMVA::TFoamVect& TMVA::TFoamVect::operator-=(const TFoamVect& Shift)
{
// unary subtraction operator -=
   if( fDim != Shift.fDim) {
      Error( "TFoamVect","operator+, different dimensions= %d %d \n",fDim,Shift.fDim);
   }
   for(Int_t i=0;i<fDim;i++)
      fCoords[i] = fCoords[i]-Shift.fCoords[i];
   return *this;
}

//_________________________________________________________________________
TMVA::TFoamVect TMVA::TFoamVect::operator+(const TFoamVect &p2)
{
// addition operator +; sum of 2 vectors: c=a+b, a=a+b,
// NEVER USE IT, VERY SLOW!!!
   TFoamVect temp(fDim);
   temp  = (*this);
   temp += p2;
   return temp;
}

//__________________________________________________________________________
TMVA::TFoamVect TMVA::TFoamVect::operator-(const TFoamVect &p2)
{
// subtraction operator -; difference of 2 vectors; c=a-b, a=a-b,
// NEVER USE IT, VERY SLOW!!!
   TFoamVect temp(fDim);
   temp  = (*this);
   temp -= p2;
   return temp;
}

//___________________________________________________________________________
TMVA::TFoamVect& TMVA::TFoamVect::operator =(Double_t Vect[])
{
// Loading in ordinary double prec. vector, sometimes can be useful
   Int_t i;
   for(i=0; i<fDim; i++)
      fCoords[i] = Vect[i];
   return *this;
}

//____________________________________________________________________________
TMVA::TFoamVect& TMVA::TFoamVect::operator =(Double_t x)
{
// Loading in double prec. number, sometimes can be useful
   if(fCoords != 0) {
      for(Int_t i=0; i<fDim; i++)
         fCoords[i] = x;
   }
   return *this;
}
//////////////////////////////////////////////////////////////////////////////
//                          OTHER METHODS                                   //
//////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
void TMVA::TFoamVect::Print(Option_t *option) const
{
// Printout of all vector components on "cout"
   if(!option) Error("Print ", "No option set \n");
   Int_t i;
   cout << "(";
   for(i=0; i<fDim-1; i++) cout  << SW2 << *(fCoords+i) << ",";
   cout  << SW2 << *(fCoords+fDim-1);
   cout << ")";
}
//______________________________________________________________________________
void TMVA::TFoamVect::PrintList(void)
{
// Printout of all member vectors in the list starting from "this"
   Long_t i=0;
   if(this == 0) return;
   TFoamVect *current=this;
   while(current != 0) {
      cout<<"vec["<<i<<"]=";
      current->Print("1");
      cout<<endl;
      current = current->fNext;
      i++;
   }
}

///////////////////////////////////////////////////////////////////////////////
//                End of Class TFoamVect                                        //
///////////////////////////////////////////////////////////////////////////////

#endif
