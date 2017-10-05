// @(#)root/tmva $Id: MethodBase.h,v 1.13 2008/07/19 19:43:32 stelzer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodBase                                                            *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Virtual base class for all MVA method                                     *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *      Kai Voss        <Kai.Voss@cern.ch>       - U. of Victoria, Canada         *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      U. of Victoria, Canada                                                    * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#ifndef ROOT_TMVA_MethodBase
#define ROOT_TMVA_MethodBase

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MethodBase                                                           //
//                                                                      //
// Virtual base class for all TMVA method                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iosfwd>
#include <vector>

#ifndef ROOT_TMVA_IMethod
#include "TMVA/IMethod.h"
#endif
#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif
#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif
#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_TransformationHandler
#include "TMVA/TransformationHandler.h"
#endif

class TGraph;
class TTree;
class TDirectory;
class TSpline;

namespace TMVA {

   class Ranking;
   class MsgLogger;
   class PDF;
   class TSpline1;
   class MethodCuts;
   class MethodBoost;
   class DataSetInfo;

   class MethodBase : virtual public IMethod, public Configurable {
      
   public:

      enum EWeightFileType { kROOT=0, kTEXT };
      
      // default constructur
      MethodBase( const TString& jobName,
                  const TString& methodTitle, 
                  DataSetInfo& dsi,
                  const TString& theOption = "", 
                  TDirectory* theBaseDir = 0 );
      
      // constructor used for Testing + Application of the MVA, only (no training), 
      // using given weight file
      MethodBase( DataSetInfo& dsi,
                  const TString& weightFile, 
                  TDirectory* theBaseDir = 0 );

      // default destructur
      virtual ~MethodBase();

      // ---------- main training and testing methods ------------------------------

      // prepare tree branch with the method's discriminating variable
      virtual void AddClassifierOutput(Types::ETreeType type);

      void AddClassifierOutputProb(Types::ETreeType type);

      // performs classifier training
      // calls methods Train() implemented by derived classes
      void   TrainMethod();
      virtual void Train() = 0;

      // performs classifier testing
      virtual void Test();

      // classifier response
      virtual Double_t GetMvaValue() = 0;

      // probability of classifier response (mvaval) to be signal
      virtual Double_t GetProba( Double_t mvaVal, Double_t ap_sig );

      // Rarity of classifier response (signal or background (default) is uniform in [0,1])
      virtual Double_t GetRarity( Double_t mvaVal, Types::ESBType reftype = Types::kBackground ) const;
 
      // create ranking
      virtual const Ranking* CreateRanking() = 0;

      // perfrom extra actions during the boosting at different stages
      virtual Bool_t MonitorBoost(MethodBoost* /*booster*/) {return kFALSE;};

      // make ROOT-independent C++ class
      virtual void MakeClass( const TString& classFileName = "" ) const;

      // print help message
      void PrintHelpMessage() const;

      // streamer methods for training information (creates "weight" files) --------
      void WriteStateToFile   () const;
      void WriteStateToStream ( std::ostream& tf, Bool_t isClass = kFALSE ) const;
      void WriteStateToStream ( TFile&        rf ) const;
      void ReadStateFromFile  ();
      void ReadStateFromStream( std::istream& tf );
      void ReadStateFromStream( TFile&        rf );
      
      virtual void WriteWeightsToStream ( std::ostream& tf ) const = 0;
      virtual void WriteWeightsToStream ( TFile&      /*rf*/ ) const {}
      virtual void ReadWeightsFromStream( std::istream& tf ) = 0;
      virtual void ReadWeightsFromStream( TFile&      /*rf*/ ) {}
      void WriteVarsToStream ( std::ostream& o, const TString& prefix = "" ) const;
      void ReadVarsFromStream( std::istream& istr );




      // ---------------------------------------------------------------------------

      // write evaluation histograms into target file
      virtual void WriteEvaluationHistosToFile();

      // write classifier-specific monitoring information to target file
      virtual void WriteMonitoringHistosToFile() const;

      // ---------- public evaluation methods --------------------------------------

      // individual initialistion for testing of each method
      // overload this one for individual initialisation of the testing, 
      // it is then called automatically within the global "TestInit" 
      
      // variables (and private menber functions) for the Evaluation:
      // get the effiency. It fills a histogram for efficiency/vs/bkg
      // and returns the one value fo the efficiency demanded for 
      // in the TString argument. (Watch the string format)
      virtual Double_t  GetEfficiency( const TString&, Types::ETreeType, Double_t& err );
      virtual Double_t  GetTrainingEfficiency( TString );
      virtual Double_t  GetSignificance() const;
      virtual Double_t  GetMaximumSignificance( Double_t SignalEvents, Double_t BackgroundEvents, 
                                                Double_t& optimal_significance_value  ) const;
      virtual Double_t  GetSeparation( TH1*, TH1* ) const;
      virtual Double_t  GetSeparation( PDF* pdfS = 0, PDF* pdfB = 0 ) const;

      // ---------- public accessors -----------------------------------------------

      // classifier naming (a lot of names ... aren't they ;-)
      const TString& GetJobName    () const { return fJobName; }
      const TString& GetMethodName () const { return fMethodName; }
      const TString& GetMethodTitle() const { return fMethodTitle; }
      Types::EMVA    GetMethodType () const { return fMethodType; }
      const char*    GetName       () const { return GetMethodName().Data(); } // same as methodname (overwrites for TObject)
      const TString& GetTestvarName() const { return fTestvar; }
      const TString  GetProbaName  () const { return fTestvar + "_Proba"; }

      void SetMethodName ( TString methodName )     { fMethodName  = methodName; }
      void SetMethodTitle( TString methodTitle )    { fMethodTitle = methodTitle; }
      void SetMethodType ( Types::EMVA methodType ) { fMethodType  = methodType; }

      // build classifier name in Test tree
      // MVA prefix (e.g., "TMVA_")
      void SetTestvarPrefix( TString prefix )     { fTestvarPrefix = prefix; }
      void SetTestvarName( const TString & v="" ) { fTestvar = (v=="")?(fTestvarPrefix + GetMethodTitle()):v; }

      // number of input variable used by classifier
      UInt_t   GetNvar() const { return DataInfo().GetNVariables(); }

      // internal names and expressions of input variables
      const TString& GetInputVar( int i ) const { return DataInfo().GetInternalVarName(i); }
      const TString& GetInputExp( int i ) const { return DataInfo().GetExpression(i); }

      // normalisation and limit accessors
      Double_t GetMean( Int_t ivar )         const { return GetTransformationHandler().GetMean(ivar); }
      Double_t GetRMS( Int_t ivar )          const { return GetTransformationHandler().GetRMS(ivar); }
      Double_t GetXmin( Int_t ivar )         const { return GetTransformationHandler().GetMin(ivar); }
      Double_t GetXmax( Int_t ivar )         const { return GetTransformationHandler().GetMax(ivar); }

      // sets the minimum requirement on the MVA output to declare an event signal-like
      Double_t GetSignalReferenceCut() const { return fSignalReferenceCut; }

      // sets the minimum requirement on the MVA output to declare an event signal-like
      void     SetSignalReferenceCut( Double_t cut ) { fSignalReferenceCut = cut; }

      // pointers to ROOT directories
      TDirectory* BaseDir()       const;
      TDirectory* MethodBaseDir() const;
      void SetMethodDir ( TDirectory* methodDir ) { fBaseDir = fMethodBaseDir  = methodDir; }

      // the TMVA versions can be checked using
      // if(GetTrainingTMVAVersionCode()>TMVA_VERSION(3,7,2)) {...}
      // or
      // if (GetTrainingROOTVersionCode()>ROOT_VERSION(5,15,5)) {...}
      UInt_t  GetTrainingTMVAVersionCode()   const { return fTMVATrainingVersion; }
      UInt_t  GetTrainingROOTVersionCode()   const { return fROOTTrainingVersion; }
      TString GetTrainingTMVAVersionString() const;
      TString GetTrainingROOTVersionString() const;      

      // ---------- event accessors ------------------------------------------------

      // returns reference to data set
      DataSetInfo& DataInfo() const { return fDataSetInfo; }

      // event reference and update
      const Event* GetEvent(const TMVA::Event* ev) const;
      const Event* GetEvent() const;
      const Event* GetEvent(Long64_t ievt) const;
      const Event* GetEvent(Long64_t ievt, Types::ETreeType type) const;
      const Event* GetTrainingEvent(Long64_t ievt) const;
      const Event* GetTestingEvent(Long64_t ievt) const;

      // ---------- public auxiliary methods ---------------------------------------

      // this method is used to decide whether an event is signal- or background-like
      // the reference cut "xC" is taken to be where 
      // Int_[-oo,xC] { PDF_S(x) dx } = Int_[xC,+oo] { PDF_B(x) dx }
      virtual Bool_t IsSignalLike() { return GetMvaValue() > GetSignalReferenceCut() ? kTRUE : kFALSE; }     

      DataSet* Data() const { return DataInfo().GetDataSet(); }

      // set message type
      void SetMsgType( EMsgType t ) { fLogger.SetMinType(t); }

      Bool_t HasMVAPdfs() const { return fHasMVAPdfs; }

   protected:

      // ---------- protected acccessors -------------------------------------------

      //TDirectory*  LocalTDir() const { return Data().LocalRootDir(); }

      // weight file name and directory (given by global config variable)
      void    SetWeightFileName( TString );
      TString GetWeightFileName() const;

      TString GetWeightFileDir() const { return fFileDir; }
      void    SetWeightFileDir( TString fileDir );

      // are input variables normalised ?
      Bool_t   IsNormalised() const { return fNormalise; }
      void     SetNormalised( Bool_t norm ) { fNormalise = norm; }

      // set number of input variables (only used by MethodCuts, could perhaps be removed)
      //      void SetNvar( Int_t n ) { fNvar = n; }

      // verbose and help flags
      Bool_t Verbose() const { return fVerbose; }
      Bool_t Help   () const { return fHelp; }

      // ---------- protected event and tree accessors -----------------------------

      // names of input variables (if the original names are expressions, they are 
      // transformed into regexps)
      const TString& GetInternalVarName( Int_t ivar ) const { return (*fInputVars)[ivar]; }
      const TString& GetOriginalVarName( Int_t ivar ) const { return DataInfo().GetExpression(ivar); }

      Bool_t  HasTrainingTree() const { return Data()->GetNTrainingEvents() != 0; }

      // ---------- protected auxiliary methods ------------------------------------

      // declaration and processing of configuration options
      virtual void DeclareOptions();
      virtual void ProcessOptions();

      // make ROOT-independent C++ class for classifier response (classifier-specific implementation)
      virtual void MakeClassSpecific( std::ostream&, const TString& = "" ) const {}

      // header and auxiliary classes
      virtual void MakeClassSpecificHeader( std::ostream&, const TString& = "" ) const {}

      // static pointer to this object - required for ROOT finder (to be solved differently)
      static MethodBase* GetThisBase() { return fgThisBase; }        

      // some basic statistical analysis
      void Statistics( Types::ETreeType treeType, const TString& theVarName,
                       Double_t&, Double_t&, Double_t&, 
                       Double_t&, Double_t&, Double_t& );


      // if TRUE, write weights only to text files 
      Bool_t TxtWeightsOnly() const { return fTxtWeightsOnly; }

   private:

      // ---------- private definitions --------------------------------------------

      // used in efficiency computation
      enum ECutOrientation { kNegative = -1, kPositive = +1 };
      ECutOrientation GetCutOrientation() const { return fCutOrientation; }

      // ---------- private acccessors ---------------------------------------------

      // reset required for RootFinder
      void ResetThisBase() { fgThisBase = this; }

      // ---------- private auxiliary methods --------------------------------------

      // Initialisation
      void Init();

      // PDFs for classifier response (required to compute signal probability and Rarity)
      void   CreateMVAPdfs();

      // for root finder 
      static Double_t IGetEffForRoot( Double_t );  // interface
      Double_t        GetEffForRoot ( Double_t );  // implementation

      // used for file parsing
      Bool_t GetLine(std::istream& fin, char * buf );
      
      // ========== class members ==================================================

   protected:

      TransformationHandler&  GetTransformationHandler() { return fTransformation; }
      const TransformationHandler&  GetTransformationHandler() const { return fTransformation; }

      // direct accessors
      Ranking*         fRanking;             // pointer to ranking object (created by derived classifiers)
      std::vector<TString>* fInputVars;           // vector of input variables used in MVA

      // histogram binning
      Int_t            fNbins;               // number of bins in representative histograms
      Int_t            fNbinsH;              // number of bins in evaluation histograms

   private:

      // MethodCuts redefines some of the evaluation variables and histograms -> must access private members
      friend class MethodCuts; 

      // data sets
      DataSetInfo&     fDataSetInfo;         //! the data set information (sometimes needed)

      Double_t         fSignalReferenceCut;  // minimum requirement on the MVA output to declare an event signal-like
      Types::ESBType   fVariableTransformType;  // this is the event type (sig or bgd) assumed for variable transform

      // naming and versioning
      TString          fJobName;             // name of job -> user defined, appears in weight files
      TString          fMethodName;          // name of the method (set in derived class)
      Types::EMVA      fMethodType;          // type of method (set in derived class)      
      TString          fMethodTitle;         // user-defined title for method (used for weight-file names)
      TString          fTestvar;             // variable used in evaluation, etc (mostly the MVA)
      TString          fTestvarPrefix;       // 'MVA_' prefix of MVA variable
      UInt_t           fTMVATrainingVersion; // TMVA version used for training
      UInt_t           fROOTTrainingVersion; // ROOT version used for training
      Bool_t           fNormalise;           // normalise input variables
      
      // Directory structure: fMethodBaseDir/fBaseDir
      // where the first directory name is defined by the method type
      // and the second is user supplied (the title given in Factory::BookMethod())
      TDirectory*      fBaseDir;             // base directory for the instance, needed to know where to jump back from localDir
      mutable TDirectory* fMethodBaseDir;    // base directory for the method

      TString          fParentDir;           // method parent name, like booster name

      TString          fFileDir;             // unix sub-directory for weight files (default: "weights")
      TString          fWeightFile;          // weight file name

   private:

      TH1*             fHistS_plotbin;       // MVA plots used for graphics representation (signal)
      TH1*             fHistB_plotbin;       // MVA plots used for graphics representation (background)
      TH1*             fHistTrS_plotbin;     // same plots as above for training sample (check for overtraining)
      TH1*             fHistTrB_plotbin;     // same plots as above for training sample (check for overtraining)
      TH1*             fProbaS_plotbin;      // P(MVA) plots used for graphics representation (signal)
      TH1*             fProbaB_plotbin;      // P(MVA) plots used for graphics representation (background)
      TH1*             fRarityS_plotbin;     // R(MVA) plots used for graphics representation (signal)
      TH1*             fRarityB_plotbin;     // R(MVA) plots used for graphics representation (background)
      TH1*             fHistS_highbin;       // MVA plots used for efficiency calculations (signal)    
      TH1*             fHistB_highbin;       // MVA plots used for efficiency calculations (background)
      TH1*             fEffS;                // efficiency plot (signal)
      TH1*             fEffB;                // efficiency plot (background)
      TH1*             fEffBvsS;             // background efficiency versus signal efficiency
      TH1*             fRejBvsS;             // background rejection (=1-eff.) versus signal efficiency
      TH1*             finvBeffvsSeff;       // inverse background eff (1/eff.) versus signal efficiency

      TH1*             fTrainEffS;           // Training efficiency plot (signal)
      TH1*             fTrainEffB;           // Training efficiency plot (background)
      TH1*             fTrainEffBvsS;        // Training background efficiency versus signal efficiency
      TH1*             fTrainRejBvsS;        // Training background rejection (=1-eff.) versus signal efficiency

      Int_t            fNbinsMVAPdf;         // number of bins used in histogram that creates PDF
      Int_t            fNsmoothMVAPdf;       // number of times a histogram is smoothed before creating the PDF
      PDF*             fDefaultPDF;          // default PDF definitions
      PDF*             fMVAPdfS;             // signal MVA PDF
      PDF*             fMVAPdfB;             // background MVA PDF

      TGraph*          fGraphS;              // graphs used for splines for efficiency (signal)
      TGraph*          fGraphB;              // graphs used for splines for efficiency (background)
      TGraph*          fGrapheffBvsS;        // graphs used for splines for signal eff. versus background eff.
      PDF*             fSplS;                // PDFs of MVA distribution (signal)
      PDF*             fSplB;                // PDFs of MVA distribution (background)
      TSpline*         fSpleffBvsS;          // splines for signal eff. versus background eff.

      TGraph*          fGraphTrainS;         // graphs used for splines for training efficiency (signal)
      TGraph*          fGraphTrainB;         // graphs used for splines for training efficiency (background)
      TGraph*          fGraphTrainEffBvsS;   // graphs used for splines for training signal eff. versus background eff.
      PDF*             fSplTrainS;           // PDFs of training MVA distribution (signal)
      PDF*             fSplTrainB;           // PDFs of training MVA distribution (background)
      TSpline*         fSplTrainEffBvsS;     // splines for training signal eff. versus background eff.

   private:

      // basic statistics quantities of MVA
      Double_t         fMeanS;               // mean (signal)
      Double_t         fMeanB;               // mean (background)
      Double_t         fRmsS;                // RMS (signal)
      Double_t         fRmsB;                // RMS (background)
      Double_t         fXmin;                // minimum (signal and background)
      Double_t         fXmax;                // maximum (signal and background)

      // variable preprocessing
      Bool_t                    fUseDecorr;                   // kept for backward compatibility
      TString                   fVarTransformString;          // labels variable transform method
      TString                   fVariableTransformTypeString; // labels variable transform type

      TransformationHandler     fTransformation;              // the list of transformations


      // help and verbosity
      Bool_t           fVerbose;             // verbose flag
      TString          fVerbosityLevelString;  // verbosity level (user input string)
      EMsgType         fVerbosityLevel;      // verbosity level
      Bool_t           fHelp;                // help flag
      Bool_t           fHasMVAPdfs;          // MVA Pdfs are created for this classifier
      Bool_t           fTxtWeightsOnly;      // if TRUE, write weights only to text files 

   private:

      // orientation of cut: depends on signal and background mean values
      ECutOrientation  fCutOrientation;      // +1 if Sig>Bkg, -1 otherwise

      // for root finder
      TSpline1*        fSplRefS;             // helper splines for RootFinder (signal)
      TSpline1*        fSplRefB;             // helper splines for RootFinder (background)

      TSpline1*        fSplTrainRefS;        // helper splines for RootFinder (signal)
      TSpline1*        fSplTrainRefB;        // helper splines for RootFinder (background)

   private:

      // this carrier
      static MethodBase* fgThisBase;    // this pointer

   protected:

      ClassDef(MethodBase,0)  // Virtual base class for all TMVA method

   };
} // namespace TMVA


// ========== INLINE FUNCTIONS =========================================================


//_______________________________________________________________________
inline const TMVA::Event* TMVA::MethodBase::GetEvent(const TMVA::Event* ev) const {
   return GetTransformationHandler().Transform(ev);
}

inline const TMVA::Event* TMVA::MethodBase::GetEvent() const {
   return GetTransformationHandler().Transform(Data()->GetEvent());
}

inline const TMVA::Event* TMVA::MethodBase::GetEvent(Long64_t ievt) const {
   return GetTransformationHandler().Transform(Data()->GetEvent(ievt));
}

inline const TMVA::Event* TMVA::MethodBase::GetEvent(Long64_t ievt, Types::ETreeType type) const {
   return GetTransformationHandler().Transform(Data()->GetEvent(ievt, type));
}

inline const TMVA::Event* TMVA::MethodBase::GetTrainingEvent(Long64_t ievt) const {
   return GetEvent(ievt, Types::kTraining);
}

inline const TMVA::Event* TMVA::MethodBase::GetTestingEvent(Long64_t ievt) const {
   return GetEvent(ievt, Types::kTesting);
}

//_______________________________________________________________________
inline TString TMVA::MethodBase::GetTrainingTMVAVersionString() const
{
   // calculates the TMVA version string from the training version code on the fly
   UInt_t a = GetTrainingTMVAVersionCode() & 0xff0000; a>>=16;
   UInt_t b = GetTrainingTMVAVersionCode() & 0x00ff00; b>>=8;
   UInt_t c = GetTrainingTMVAVersionCode() & 0x0000ff;

   return TString(Form("%i.%i.%i",a,b,c));
}

//_______________________________________________________________________
inline TString TMVA::MethodBase::GetTrainingROOTVersionString() const
{
   // calculates the ROOT version string from the training version code on the fly
   UInt_t a = GetTrainingROOTVersionCode() & 0xff0000; a>>=16;
   UInt_t b = GetTrainingROOTVersionCode() & 0x00ff00; b>>=8;
   UInt_t c = GetTrainingROOTVersionCode() & 0x0000ff;

   return TString(Form("%i.%02i/%02i",a,b,c));
}

#endif

