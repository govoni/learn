// @(#)root/tmva $Id: MethodBase.cxx,v 1.28 2008/07/30 15:18:21 alexvoigt Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodBase                                                            *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
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
 *                                                                                *
 **********************************************************************************/

//_______________________________________________________________________
/* Begin_Html
  Virtual base Class for all MVA method

  MethodBase hosts several specific evaluation methods.

  The kind of MVA that provides optimal performance in an analysis strongly
  depends on the particular application. The evaluation factory provides a
  number of numerical benchmark results to directly assess the performance
  of the MVA training on the independent test sample. These are:
  <ul>
  <li> The <i>signal efficiency</i> at three representative background efficiencies
  (which is 1 &minus; rejection).</li>
  <li> The <i>significance</I> of an MVA estimator, defined by the difference
  between the MVA mean values for signal and background, divided by the
  quadratic sum of their root mean squares.</li>
  <li> The <i>separation</i> of an MVA <i>x</i>, defined by the integral
  &frac12;&int;(S(x) &minus; B(x))<sup>2</sup>/(S(x) + B(x))dx, where
  S(x) and B(x) are the signal and background distributions, respectively.
  The separation is zero for identical signal and background MVA shapes,
  and it is one for disjunctive shapes.
  <li> <a name="mu_transform">
  The average, &int;x &mu;(S(x))dx, of the signal &mu;-transform.
  The &mu;-transform of an MVA denotes the transformation that yields
  a uniform background distribution. In this way, the signal distributions
  S(x) can be directly compared among the various MVAs. The stronger S(x)
  peaks towards one, the better is the discrimination of the MVA. The
  &mu;-transform is
  <a href=http://tel.ccsd.cnrs.fr/documents/archives0/00/00/29/91/index_fr.html>documented here</a>.
  </ul>
  The MVA standard output also prints the linear correlation coefficients between
  signal and background, which can be useful to eliminate variables that exhibit too
  strong correlations.

End_Html */
//_______________________________________________________________________

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "TROOT.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TQObject.h"
#include "TSpline.h"
#include "TMatrix.h"
#include "TMath.h"
#include "TFile.h"
#include "TKey.h"
#include "TGraph.h"

#include "TMVA/MethodBase.h"
#include "TMVA/MsgLogger.h"
#include "TMVA/Config.h"
#include "TMVA/Timer.h"
#include "TMVA/RootFinder.h"
#include "TMVA/PDF.h"
#include "TMVA/VariableIdentityTransform.h"
#include "TMVA/VariableDecorrTransform.h"
#include "TMVA/VariablePCATransform.h"
#include "TMVA/VariableGaussTransform.h"
#include "TMVA/VariableNormalizeTransform.h"
#include "TMVA/Version.h"
#include "TMVA/TSpline1.h"
#include "TMVA/Ranking.h"
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"

ClassImp(TMVA::MethodBase)

using std::endl;

const Int_t    MethodBase_MaxIterations_ = 200;
const Bool_t   Use_Splines_for_Eff_      = kTRUE;

const Int_t    NBIN_HIST_PLOT = 100;
const Int_t    NBIN_HIST_HIGH = 10000;

#ifdef _WIN32
/*Disable warning C4355: 'this' : used in base member initializer list*/
#pragma warning ( disable : 4355 )
#endif

//_______________________________________________________________________
TMVA::MethodBase::MethodBase( const TString& jobName,
                              const TString& methodTitle,
                              DataSetInfo& dsi,
                              const TString& theOption,
                              TDirectory* theBaseDir)
   : IMethod(),
     Configurable               ( theOption ),
     fDataSetInfo               ( dsi ),
     fSignalReferenceCut        ( 0.5 ),
     fVariableTransformType     ( Types::kSignal ),
     fJobName                   ( jobName ),
     fMethodName                ( "MethodBase"  ),
     fMethodType                ( Types::kMaxMethod ),
     fMethodTitle               ( methodTitle ),
     fTestvar                   ( "" ),
     fTestvarPrefix             ( "MVA_" ),
     fTMVATrainingVersion       ( TMVA_VERSION_CODE ),
     fROOTTrainingVersion       ( ROOT_VERSION_CODE ),
     fBaseDir                   ( 0 ),
     fMethodBaseDir             ( theBaseDir ),
     fWeightFile                ( "" ),
     fHistS_plotbin             ( 0 ),
     fHistB_plotbin             ( 0 ),
     fHistTrS_plotbin           ( 0 ),
     fHistTrB_plotbin           ( 0 ),
     fProbaS_plotbin            ( 0 ),
     fProbaB_plotbin            ( 0 ),
     fRarityS_plotbin           ( 0 ),
     fRarityB_plotbin           ( 0 ),
     fHistS_highbin             ( 0 ),
     fHistB_highbin             ( 0 ),
     fEffS                      ( 0 ),
     fEffB                      ( 0 ),
     fEffBvsS                   ( 0 ),
     fRejBvsS                   ( 0 ),
     finvBeffvsSeff             ( 0 ),
     fTrainEffS                 ( 0 ),
     fTrainEffB                 ( 0 ),
     fTrainEffBvsS              ( 0 ),
     fTrainRejBvsS              ( 0 ),
     fDefaultPDF                ( 0 ),
     fMVAPdfS                   ( 0 ),
     fMVAPdfB                   ( 0 ),
     fGraphS                    ( 0 ),
     fGraphB                    ( 0 ),
     fGrapheffBvsS              ( 0 ),
     fSplS                      ( 0 ),
     fSplB                      ( 0 ),
     fSpleffBvsS                ( 0 ),
     fGraphTrainS               ( 0 ),
     fGraphTrainB               ( 0 ),
     fGraphTrainEffBvsS         ( 0 ),
     fSplTrainS                 ( 0 ),
     fSplTrainB                 ( 0 ),
     fSplTrainEffBvsS           ( 0 ),
     fTransformation            ( dsi ),
     fSplRefS                   ( 0 ),
     fSplRefB                   ( 0 ),
     fSplTrainRefS              ( 0 ),
     fSplTrainRefB              ( 0 )   
{
   // standard constructur
   Init();

   // interpretation of configuration option string
   DeclareOptions();

   // default extension for weight files
   SetWeightFileDir( gConfig().GetIONames().fWeightFileDir );
   gSystem->MakeDirectory( GetWeightFileDir() );
}

//_______________________________________________________________________
TMVA::MethodBase::MethodBase( DataSetInfo& dsi,
                              const TString& weightFile,
                              TDirectory* theBaseDir )
   : IMethod(),
     Configurable(""),
     fDataSetInfo               ( dsi ),
     fSignalReferenceCut        ( 0.5 ),
     fVariableTransformType     ( Types::kSignal ),
     fJobName                   ( "" ),
     fMethodName                ( "MethodBase"  ),
     fMethodType                ( Types::kMaxMethod ),
     fMethodTitle               ( "" ),
     fTestvar                   ( "" ),
     fTestvarPrefix             ( "MVA_" ),
     fTMVATrainingVersion       ( 0 ),
     fROOTTrainingVersion       ( 0 ),
     fBaseDir                   ( theBaseDir ),
     fMethodBaseDir             ( 0 ),
     fWeightFile                ( weightFile ),
     fHistS_plotbin             ( 0 ),
     fHistB_plotbin             ( 0 ),
     fHistTrS_plotbin           ( 0 ),
     fHistTrB_plotbin           ( 0 ),
     fProbaS_plotbin            ( 0 ),
     fProbaB_plotbin            ( 0 ),
     fRarityS_plotbin           ( 0 ),
     fRarityB_plotbin           ( 0 ),
     fHistS_highbin             ( 0 ),
     fHistB_highbin             ( 0 ),
     fEffS                      ( 0 ),
     fEffB                      ( 0 ),
     fEffBvsS                   ( 0 ),
     fRejBvsS                   ( 0 ),
     finvBeffvsSeff             ( 0 ),
     fTrainEffS                 ( 0 ),
     fTrainEffB                 ( 0 ),
     fTrainEffBvsS              ( 0 ),
     fTrainRejBvsS              ( 0 ),
     fDefaultPDF                ( 0 ),
     fMVAPdfS                   ( 0 ),
     fMVAPdfB                   ( 0 ),
     fGraphS                    ( 0 ),
     fGraphB                    ( 0 ),
     fGrapheffBvsS              ( 0 ),
     fSplS                      ( 0 ),
     fSplB                      ( 0 ),
     fSpleffBvsS                ( 0 ),
     fGraphTrainS               ( 0 ),
     fGraphTrainB               ( 0 ),
     fGraphTrainEffBvsS         ( 0 ),
     fSplTrainS                 ( 0 ),
     fSplTrainB                 ( 0 ),
     fSplTrainEffBvsS           ( 0 ),
     fTransformation            ( dsi ),
     fSplRefS                   ( 0 ),
     fSplRefB                   ( 0 ),
     fSplTrainRefS              ( 0 ),
     fSplTrainRefB              ( 0 )
{
   // constructor used for Testing + Application of the MVA,
   // only (no training), using given WeightFiles

   Init();

   DeclareOptions();
}

//_______________________________________________________________________
TMVA::MethodBase::~MethodBase( void )
{
   // destructor
   if (fInputVars != 0) { fInputVars->clear(); delete fInputVars; }
   if (fRanking   != 0) delete fRanking;
   
   // PDFs
   if (fDefaultPDF!= 0) { delete fDefaultPDF; fDefaultPDF = 0; }
   if (fMVAPdfS   != 0) { delete fMVAPdfS; fMVAPdfS = 0; }
   if (fMVAPdfB   != 0) { delete fMVAPdfB; fMVAPdfB = 0; }
   
   // Splines
   if (fSplS)            { delete fSplS; fSplS = 0; }
   if (fSplB)            { delete fSplB; fSplB = 0; }
   if (fSpleffBvsS)      { delete fSpleffBvsS; fSpleffBvsS = 0; }
   if (fSplRefS)         { delete fSplRefS; fSplRefS = 0; }
   if (fSplRefB)         { delete fSplRefB; fSplRefB = 0; }

   // TGraphs
   // if (fGraphS)            { delete fGraphS; fGraphS = 0; }
   // if (fGraphB)            { delete fGraphB; fGraphB = 0; }
   // if (fGrapheffBvsS)      { delete fGrapheffBvsS; fGrapheffBvsS = 0; }
   // if (fGraphTrainEffBvsS) { delete fGraphTrainEffBvsS; fGraphTrainEffBvsS = 0; }
   if (fGraphTrainS)       { delete fGraphTrainS; fGraphTrainS = 0; }
   if (fGraphTrainB)       { delete fGraphTrainB; fGraphTrainB = 0; }

  // Histograms
  if (fHistS_plotbin)   delete fHistS_plotbin;
  if (fHistB_plotbin)   delete fHistB_plotbin;
//  if (fProbaS_plotbin)  { delete fProbaS_plotbin; fProbaS_plotbin = 0; }
//  if (fProbaB_plotbin)  { delete fProbaB_plotbin; fProbaB_plotbin = 0; }
//  if (fRarityS_plotbin) delete fRarityS_plotbin;
//  if (fRarityB_plotbin) delete fRarityB_plotbin;
  if (fHistS_highbin)   delete fHistS_highbin;
  if (fHistB_highbin)   delete fHistB_highbin;
  if (fHistTrS_plotbin) delete fHistTrS_plotbin;
  if (fHistTrB_plotbin) delete fHistTrB_plotbin;
  if (fTrainEffS)       delete fTrainEffS;
  if (fTrainEffB)       delete fTrainEffB;
  if (fTrainEffBvsS)    delete fTrainEffBvsS;
  if (fTrainRejBvsS)    delete fTrainRejBvsS;
  if (fEffS)            delete fEffS;
  if (fEffB)            delete fEffB;
  if (fEffBvsS)         delete fEffBvsS;
  if (fRejBvsS)         delete fRejBvsS;
  if (finvBeffvsSeff)   delete finvBeffvsSeff;
}

//_______________________________________________________________________
void TMVA::MethodBase::Init()
{
   // default initialization called by all constructors
   SetConfigDescription( "Configuration options for classifier architecture and tuning" );

   fNbins              = gConfig().fVariablePlotting.fNbinsXOfROCCurve;
   fNbinsH             = NBIN_HIST_HIGH;

   fTrainEffS          = 0; 
   fTrainEffB          = 0; 
   fTrainEffBvsS       = 0; 
   fTrainRejBvsS       = 0; 
   fGraphTrainS        = 0; 
   fGraphTrainB        = 0; 
   fGraphTrainEffBvsS  = 0; 
   fSplTrainS          = 0; 
   fSplTrainB          = 0; 
   fSplTrainEffBvsS    = 0; 
   fMeanS              = -1; 
   fMeanB              = -1; 
   fRmsS               = -1; 
   fRmsB               = -1; 
   fXmin               = 1e30; 
   fXmax               = -1e30; 
   fVerbose            = kFALSE; 
   fHelp               = kFALSE; 
   fHasMVAPdfs         = kFALSE; 
   fTxtWeightsOnly     = kTRUE; 
   fSplRefS            = 0; 
   fSplRefB            = 0; 

   fRanking            = 0;

   fGraphS             = 0;
   fGraphB             = 0;
   fGrapheffBvsS       = 0;
   fSplS               = 0;
   fSplB               = 0;

   fHistS_plotbin      = 0;
   fHistB_plotbin      = 0;
   fHistTrS_plotbin    = 0;
   fHistTrB_plotbin    = 0;
   fProbaS_plotbin     = 0;
   fProbaB_plotbin     = 0;
   fRarityS_plotbin    = 0;
   fRarityB_plotbin    = 0;
   fHistS_highbin      = 0;
   fHistB_highbin      = 0;
   fEffS               = 0;
   fEffB               = 0;
   fEffBvsS            = 0;
   fRejBvsS            = 0;
   finvBeffvsSeff      = 0;
   fMVAPdfS            = 0;
   fMVAPdfB            = 0;

   // temporary until the move to DataSet is complete
   fInputVars = new std::vector<TString>;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++)
      fInputVars->push_back(DataInfo().GetInternalVarName(ivar));

   // define "this" pointer
   ResetThisBase();
}

//_______________________________________________________________________
void TMVA::MethodBase::DeclareOptions()
{
   // define the options (their key words) that can be set in the option string
   // here the options valid for ALL MVA methods are declared.
   // know options: VariableTransform=None,Decorrelated,PCA  to use transformed variables
   //                                                        instead of the original ones
   //               VariableTransformType=Signal,Background  which decorrelation matrix to use
   //                                                        in the method. Only the Likelihood
   //                                                        Method can make proper use of independent
   //                                                        transformations of signal and background
   //               fNbinsMVAPdf   = 50 Number of bins used to create a PDF of MVA
   //               fNsmoothMVAPdf =  2 Number of times a histogram is smoothed before creating the PDF
   //               fHasMVAPdfs         create PDFs for the MVA outputs
   //               V                   for Verbose output (!V) for non verbos
   //               H                   for Help message

   DeclareOptionRef( fUseDecorr=kFALSE, "D", "Use-decorrelated-variables flag (depreciated)" );

   DeclareOptionRef( fNormalise=kFALSE, "Normalise", "Normalise input variables" ); // don't change the default !!!

   DeclareOptionRef( fVarTransformString="None", "VarTransform", "List of variable transformations before training e.g. 'D,P,G,N'" );
   //   AddPreDefVal( TString("None") );
   //    AddPreDefVal( TString("Decorrelate") );
   //    AddPreDefVal( TString("PCA") );
   //    AddPreDefVal( TString("GaussDecorr") );

   DeclareOptionRef( fVariableTransformTypeString="Signal", "VarTransformType",
                     "Use signal or background events to derive for variable transformation (the transformation is applied on both types of course)" );
   AddPreDefVal( TString("Signal") );
   AddPreDefVal( TString("Background") );

   DeclareOptionRef( fNbinsMVAPdf   = 60, "NbinsMVAPdf",   "Number of bins used for the PDFs of classifier outputs" );
   DeclareOptionRef( fNsmoothMVAPdf = 2,  "NsmoothMVAPdf", "Number of smoothing iterations for classifier PDFs" );

   DeclareOptionRef( fVerbose,   "V",       "Verbose mode" );

   DeclareOptionRef( fVerbosityLevelString="Default", "VerboseLevel", "Verbosity level" );
   AddPreDefVal( TString("Default") ); // uses default defined in MsgLogger header
   AddPreDefVal( TString("Debug")   );
   AddPreDefVal( TString("Verbose") );
   AddPreDefVal( TString("Info")    );
   AddPreDefVal( TString("Warning") );
   AddPreDefVal( TString("Error")   );
   AddPreDefVal( TString("Fatal")   );

   DeclareOptionRef( fHelp,             "H",             "Print classifier-specific help message" );
   DeclareOptionRef( fHasMVAPdfs=kFALSE, "CreateMVAPdfs", "Create PDFs for classifier outputs (signal and background)" );

   DeclareOptionRef( fTxtWeightsOnly=kTRUE, "TxtWeightFilesOnly", "If True: write all training results (weights) as text files (False: some are written in ROOT format)" );

   fDefaultPDF=fMVAPdfB=fMVAPdfS=0;
   if ( GetOptions().Contains("CreateMVAPdfs") )
   {
      //setting the default bin num.. maybe should be static ?

      PDF* tmpPDF = new PDF("NBins=50:NSmooth=2"); 
      //reading every PDF's definition and passing the option string to the next one to be read and marked
      fDefaultPDF = new PDF( GetOptions(),"MVAPdf",tmpPDF );
      fMVAPdfB = new PDF( fDefaultPDF->GetOptions(), "MVAPdfBkg", fDefaultPDF );
      fMVAPdfS = new PDF( fMVAPdfB->GetOptions(), "MVAPdfSig", fDefaultPDF);
      //the final marked option string is written back to the original methodbase
      SetOptions( fMVAPdfS->GetOptions() );
      delete tmpPDF;
   }
}

//_______________________________________________________________________
void TMVA::MethodBase::ProcessOptions()
{
   // the option string is decoded, for availabel options see "DeclareOptions"

   if(fVarTransformString != "None") {
      TList* trList = gTools().ParseFormatLine( fVarTransformString, "," );
      TListIter trIt(trList);
      while( TObjString* os = (TObjString*)trIt()) {
         const TString& s = os->GetString();
         if      ( s == "D" || s == "Decorrelate" ) GetTransformationHandler().AddTransformation( new VariableDecorrTransform(DataInfo().GetVariableInfos()) );
         else if ( s == "P" || s == "PCA" )         GetTransformationHandler().AddTransformation( new VariablePCATransform(DataInfo().GetVariableInfos()) );
         else if ( s == "G" || s == "Gauss" )       GetTransformationHandler().AddTransformation( new VariableGaussTransform(DataInfo().GetVariableInfos()) );
         else if ( s == "N" || s == "Norm" )        GetTransformationHandler().AddTransformation( new VariableNormalizeTransform(DataInfo().GetVariableInfos()) );
         else
            fLogger << kFATAL << "<ProcessOptions> Variable transform '"
                    << s << "' unknown." << Endl;
      }
   }

   // for backward compatibility
   if (fUseDecorr)
      fLogger << kFATAL << "<ProcessOptions> Option D depreciated. Use VarTransform=Decorrelate instead." << Endl;
   
   if      (fVariableTransformTypeString == "Signal")      fVariableTransformType = Types::kSignal;
   else if (fVariableTransformTypeString == "Background" ) fVariableTransformType = Types::kBackground;
   else {
      fLogger << kFATAL << "<ProcessOptions> Variable transformation type '"
              << fVariableTransformTypeString << "' unknown." << Endl;
   }

   if(!HasMVAPdfs()) {
      if (fDefaultPDF!= 0) { delete fDefaultPDF; fDefaultPDF = 0; }
      if (fMVAPdfS   != 0) { delete fMVAPdfS; fMVAPdfS = 0; }
      if (fMVAPdfB   != 0) { delete fMVAPdfB; fMVAPdfB = 0; }
   }

   if      (fVerbosityLevelString == "Debug"   ) fLogger.SetMinType( kDEBUG );
   else if (fVerbosityLevelString == "Verbose" ) fLogger.SetMinType( kVERBOSE );
   else if (fVerbosityLevelString == "Info"    ) fLogger.SetMinType( kINFO );
   else if (fVerbosityLevelString == "Warning" ) fLogger.SetMinType( kWARNING );
   else if (fVerbosityLevelString == "Error"   ) fLogger.SetMinType( kERROR );
   else if (fVerbosityLevelString == "Fatal"   ) fLogger.SetMinType( kFATAL );
   else if (fVerbosityLevelString != "Default" ) {
      fLogger << kFATAL << "<ProcessOptions> Verbosity level type '"
              << fVerbosityLevelString << "' unknown." << Endl;
   }

   if (Verbose()) fLogger.SetMinType( kVERBOSE );
}

//_______________________________________________________________________
void TMVA::MethodBase::TrainMethod()
{
   Data()->SetCurrentType(Types::kTraining);

   // train the classifier method
   if (Help()) PrintHelpMessage();

   // all histograms should be created in the method's subdirectory
   BaseDir()->cd();

   GetTransformationHandler().CalcTransformations(Data()->GetEventCollection());

   // call training of derived classifier
   Train();

   // create PDFs for the signal and background MVA distributions
   if (HasMVAPdfs()) {
      AddClassifierOutput(Types::kTraining);
      CreateMVAPdfs();
      AddClassifierOutputProb(Types::kTraining);
   }

   // write the current classifier state into stream
   // produced are one text file and one ROOT file
   WriteStateToFile();

   // produce standalone make class
   MakeClass();

   // write additional monitoring histograms to main target file (not the weight file)
   // again, make sure the histograms go into the method's subdirectory
   BaseDir()->cd();

   WriteMonitoringHistosToFile();
}

//_______________________________________________________________________
void TMVA::MethodBase::AddClassifierOutput(Types::ETreeType type)
{
   // prepare tree branch with the method's discriminating variable

   Data()->SetCurrentType(type);

   std::vector<Float_t>* mvaRes = Data()->CreateClassifierOutputVector(GetTestvarName(),type, kTRUE);

   // read the coefficients
   //   this->ReadStateFromFile();

   Long64_t nEvents = Data()->GetNEvents();

   // use timer
   Timer timer( nEvents, GetName(), kTRUE );

   fLogger << kINFO << "Evaluation of " << GetMethodTitle() << " on " << (type==Types::kTraining?"training":"testing") << " sample" << Endl;

   for (Int_t ievt=0; ievt<nEvents; ievt++) {
      Data()->SetCurrentEvent(ievt);
      Float_t mvaVal = GetMvaValue();
      mvaRes->push_back(mvaVal);

      // print progress
      Int_t modulo = Int_t(nEvents/100);
      if (ievt%modulo == 0) timer.DrawProgressBar( ievt );
   }

   fLogger << kINFO << "Elapsed time for evaluation of " << nEvents <<  " events: "
           << timer.GetElapsedTime() << "       " << Endl;
}


//_______________________________________________________________________
void TMVA::MethodBase::AddClassifierOutputProb(Types::ETreeType type)
{
   // prepare tree branch with the method's discriminating variable

   Data()->SetCurrentType(type);

   std::vector<Float_t>* mvaProb = Data()->CreateClassifierOutputVector(GetTestvarName(), type, kFALSE);

   // read the coefficients
   //   this->ReadStateFromFile();

   Long64_t nEvents = Data()->GetNEvents();

   // use timer
   Timer timer( nEvents, GetName(), kTRUE );

   fLogger << kINFO << "Evaluation of " << GetMethodTitle() << " on " << (type==Types::kTraining?"training":"testing") << " sample" << Endl;

   for (Int_t ievt=0; ievt<nEvents; ievt++) {
      Data()->SetCurrentEvent(ievt);
      Float_t mvaVal = GetMvaValue();
      mvaProb->push_back((Float_t)GetProba( mvaVal, 0.5 ));
      // print progress
      Int_t modulo = Int_t(nEvents/100);
      if (ievt%modulo == 0) timer.DrawProgressBar( ievt );
   }

   fLogger << kINFO << "Elapsed time for evaluation of " << nEvents <<  " events: "
           << timer.GetElapsedTime() << "       " << Endl;
}



//_______________________________________________________________________
void TMVA::MethodBase::Test()
{
   // test the method - not much is done here... mainly further initialization

   // initialization 
   Data()->SetCurrentType(Types::kTesting);

   std::vector<Float_t> * mvaRes = Data()->GetClassifierOutputVector(GetTestvarName(),Types::kTesting);   

   // sanity checks: tree must exist, and theVar must be in tree
   if ( 0==mvaRes && !(GetMethodName().Contains("Cuts"))) {
      fLogger << kFATAL << "<TestInit> Test variable " << GetTestvarName()
              << " not found in tree" << Endl;
   }

   // basic statistics operations are made in base class
   // note: cannot directly modify private class members
   Double_t meanS, meanB, rmsS, rmsB, xmin, xmax;

   gTools().ComputeStat( Data()->GetEventCollection(), mvaRes, meanS, meanB, rmsS, rmsB, xmin, xmax );

   // choose reasonable histogram ranges, by removing outliers
   Double_t nrms = 10;
   xmin = TMath::Max( TMath::Min( meanS - nrms*rmsS, meanB - nrms*rmsB ), xmin );
   xmax = TMath::Min( TMath::Max( meanS + nrms*rmsS, meanB + nrms*rmsB ), xmax );

   fMeanS = meanS; fMeanB = meanB;
   fRmsS  = rmsS;  fRmsB  = rmsB;
   fXmin  = xmin;  fXmax  = xmax;

   // determine cut orientation
   fCutOrientation = (fMeanS > fMeanB) ? kPositive : kNegative;

   // fill 2 types of histograms for the various analyses
   // this one is for actual plotting

   Double_t sxmax = fXmax+0.00001;

   // classifier response distributions for training sample
   if(fHistS_plotbin) { delete fHistS_plotbin; fHistS_plotbin = 0; }
   if(fHistB_plotbin) { delete fHistB_plotbin; fHistB_plotbin = 0; }
   fHistS_plotbin   = new TH1F( GetTestvarName() + "_S",GetTestvarName() + "_S", fNbins, fXmin, sxmax );
   fHistB_plotbin   = new TH1F( GetTestvarName() + "_B",GetTestvarName() + "_B", fNbins, fXmin, sxmax );

   

   if (HasMVAPdfs()) {
      if(fProbaS_plotbin) { delete fProbaS_plotbin; fProbaS_plotbin = 0; }
      if(fProbaB_plotbin) { delete fProbaB_plotbin; fProbaB_plotbin = 0; }
      fProbaS_plotbin = new TH1F( GetTestvarName() + "_Proba_S",GetTestvarName() + "_Proba_S", fNbins, 0.0, 1.0 );
      fProbaB_plotbin = new TH1F( GetTestvarName() + "_Proba_B",GetTestvarName() + "_Proba_B", fNbins, 0.0, 1.0 );

      if(fRarityS_plotbin) { delete fRarityS_plotbin; fRarityS_plotbin = 0; }
      if(fRarityB_plotbin) { delete fRarityB_plotbin; fRarityB_plotbin = 0; }
      fRarityS_plotbin = new TH1F( GetTestvarName() + "_Rarity_S",GetTestvarName() + "_Rarity_S", fNbins, 0.0, 1.0 );
      fRarityB_plotbin = new TH1F( GetTestvarName() + "_Rarity_B",GetTestvarName() + "_Rarity_B", fNbins, 0.0, 1.0 );
   }

   if(fHistS_highbin) { delete fHistS_highbin; fHistS_highbin = 0; }
   if(fHistB_highbin) { delete fHistB_highbin; fHistB_highbin = 0; }
   fHistS_highbin  = new TH1F( GetTestvarName() + "_S_high",GetTestvarName() + "_S_high", fNbinsH, fXmin, sxmax );
   fHistB_highbin  = new TH1F( GetTestvarName() + "_B_high",GetTestvarName() + "_B_high", fNbinsH, fXmin, sxmax );

   // enable quadratic errors
   fHistS_plotbin->Sumw2();
   fHistB_plotbin->Sumw2();

   if (HasMVAPdfs()) {
      fProbaS_plotbin->Sumw2();
      fProbaB_plotbin->Sumw2();

      fRarityS_plotbin->Sumw2();
      fRarityB_plotbin->Sumw2();
   }
   fHistS_highbin->Sumw2();
   fHistB_highbin->Sumw2();

   // fill the histograms
   std::vector<Float_t>* mvaProb = Data()->GetClassifierOutputVector(GetTestvarName(), Types::kTesting, kFALSE);
   fLogger << kINFO << "Loop over test events and fill histograms with classifier response ..." << Endl;
   if (mvaProb) fLogger << kINFO << "Also filling probability and rarity histograms (on request) ..." << Endl;
   for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++) {

      Event * ev = Data()->GetEvent(ievt);
      Float_t  v = (*mvaRes)[ievt];
      Float_t  w = ev->GetWeight();

      if (ev->IsSignal()) {
         fHistS_plotbin ->Fill( v, w );
         if (mvaProb) {
            //std::cout << "Filling " << fProbaS_plotbin->GetName() << std::endl;
            fProbaS_plotbin ->Fill( (*mvaProb)[ievt], w );
            fRarityS_plotbin->Fill( GetRarity( v ), w );
         }

         fHistS_highbin ->Fill( v, w );
      } else {
         fHistB_plotbin ->Fill( v, w );
         if (mvaProb) {
            fProbaB_plotbin->Fill( (*mvaProb)[ievt], w );
            fRarityB_plotbin->Fill( GetRarity( v ), w );
         }

         fHistB_highbin ->Fill( v, w );
      }
   }

   gTools().NormHist( fHistS_plotbin  );
   gTools().NormHist( fHistB_plotbin  );
   if (mvaProb) {
      gTools().NormHist( fProbaS_plotbin );
      gTools().NormHist( fProbaB_plotbin );

      gTools().NormHist( fRarityS_plotbin );
      gTools().NormHist( fRarityB_plotbin );
   }
   gTools().NormHist( fHistS_highbin  );
   gTools().NormHist( fHistB_highbin  );

   fHistS_plotbin ->SetDirectory(0);
   fHistB_plotbin ->SetDirectory(0);
   if (mvaProb) {
      fProbaS_plotbin->SetDirectory(0);
      fProbaB_plotbin->SetDirectory(0);

      fRarityS_plotbin->SetDirectory(0);
      fRarityB_plotbin->SetDirectory(0);
   }
   fHistS_highbin ->SetDirectory(0);
   fHistB_highbin ->SetDirectory(0);

   // create PDFs from histograms, using default splines, and no additional smoothing
   if(fSplS) { delete fSplS; fSplS = 0; }
   if(fSplB) { delete fSplB; fSplB = 0; }
   fSplS = new PDF( fHistS_plotbin, PDF::kSpline2 );
   fSplB = new PDF( fHistB_plotbin, PDF::kSpline2 );
}

//_______________________________________________________________________
void TMVA::MethodBase::WriteStateToStream( std::ostream& tf, Bool_t isClass ) const
{
   // general method used in writing the header of the weight files where
   // the used variables, variable transformation type etc. is specified

   TString prefix = "";
   UserGroup_t * userInfo = gSystem->GetUserInfo();

   tf << prefix << "#GEN -*-*-*-*-*-*-*-*-*-*-*- general info -*-*-*-*-*-*-*-*-*-*-*-" << endl << prefix << endl;
   tf << prefix << "Method         : " << GetMethodName() << "::" << GetMethodTitle() << endl;
   tf.setf(std::ios::left);
   tf << prefix << "TMVA Release   : " << std::setw(10) << GetTrainingTMVAVersionString() << "    [" << GetTrainingTMVAVersionCode() << "]" << endl;
   tf << prefix << "ROOT Release   : " << std::setw(10) << GetTrainingROOTVersionString() << "    [" << GetTrainingROOTVersionCode() << "]" << endl;
   tf << prefix << "Creator        : " << userInfo->fUser << endl;
   tf << prefix << "Date           : "; TDatime *d = new TDatime; tf << d->AsString() << endl; delete d;
   tf << prefix << "Host           : " << gSystem->GetBuildNode() << endl;
   tf << prefix << "Dir            : " << gSystem->WorkingDirectory() << endl;
   tf << prefix << "Training events: " << Data()->GetNTrainingEvents() << endl;
   tf << prefix << endl;

   // First write all options
   tf << prefix << endl << prefix << "#OPT -*-*-*-*-*-*-*-*-*-*-*-*- options -*-*-*-*-*-*-*-*-*-*-*-*-" << endl << prefix << endl;
   WriteOptionsToStream( tf, prefix );
   tf << prefix << endl;

   // Second write variable info
   tf << prefix << endl << prefix << "#VAR -*-*-*-*-*-*-*-*-*-*-*-* variables *-*-*-*-*-*-*-*-*-*-*-*-" << endl << prefix << endl;
   WriteVarsToStream( tf, prefix );
   tf << prefix << endl;

   // Third write transformation data if available
   // this is weight file specific information: not written if class is made
   if (!isClass) {
      tf << endl << "#MAT -*-*-*-*-*-*-*-*-* transformation data -*-*-*-*-*-*-*-*-*-" << endl;
      GetTransformationHandler().WriteToStream( tf );
      tf << endl;

      // Fourth write the MVA variable distributions
      if (HasMVAPdfs()) {
         tf << endl << "#MVAPDFS -*-*-*-*-*-*-*-*-*-*-* MVA PDFS -*-*-*-*-*-*-*-*-*-*-*-" << endl;
         tf << *fMVAPdfS << endl;
         tf << *fMVAPdfB << endl;
         tf << endl;
      }

      // Lst, write weights
      tf << endl << "#WGT -*-*-*-*-*-*-*-*-*-*-*-*- weights -*-*-*-*-*-*-*-*-*-*-*-*-" << endl << endl;

      // no weights when standalone class is produced, which would be duplication
      WriteWeightsToStream( tf ); 
   }
   delete userInfo;
}

//_______________________________________________________________________
void TMVA::MethodBase::WriteStateToStream( TFile& rf ) const
{
   // write reference MVA distributions (and other information)
   // to a ROOT type weight file

   rf.cd();
   if (fMVAPdfS && fMVAPdfB) {
      fMVAPdfS->Write("MVA_PDF_Signal");
      fMVAPdfB->Write("MVA_PDF_Background");
   }

   WriteWeightsToStream( rf );
}

//_______________________________________________________________________
void TMVA::MethodBase::ReadStateFromStream( TFile& rf )
{
   // write reference MVA distributions (and other information)
   // to a ROOT type weight file

   Bool_t addDirStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory( 0 ); // this avoids the binding of the hists in PDF to the current ROOT file
   fMVAPdfS = (PDF*)rf.Get( "MVA_PDF_Signal" );
   fMVAPdfB = (PDF*)rf.Get( "MVA_PDF_Background" );

   //   std::cout << "else readstream B " << fMVAPdfB << std::endl;
   //   std::cout << "else readstream S " << fMVAPdfS << std::endl;

   TH1::AddDirectory( addDirStatus );

   ReadWeightsFromStream( rf );
}

//_______________________________________________________________________
void TMVA::MethodBase::WriteStateToFile() const
{
   // write options and weights to file
   // note that each one text file for the main configuration information
   // and one ROOT file for ROOT objects are created

   // ---- create the text file
   TString tfname( GetWeightFileName() );
   fLogger << kINFO << "Creating weight file in text format: "
           << gTools().Color("lightblue") << tfname << gTools().Color("reset") << Endl;

   ofstream tfile( tfname );
   if (!tfile.good()) { // file could not be opened --> Error
      fLogger << kFATAL << "<WriteStateToFile> "
              << "Unable to open output weight file: " << tfname << Endl;
   }
   WriteStateToStream( tfile );

   tfile.close();

   if ( ! fTxtWeightsOnly ) {
      // ---- create the ROOT file
      TString rfname( tfname ); rfname.ReplaceAll( ".txt", ".root" );
      fLogger << kINFO << "Creating weight file in root format: "
              << gTools().Color("lightblue") << rfname << gTools().Color("reset") << Endl;
      TFile* rfile = TFile::Open( rfname, "RECREATE" );
      WriteStateToStream( *rfile );
      rfile->Close();
   }
}

//_______________________________________________________________________
void TMVA::MethodBase::ReadStateFromFile()
{
   // Function to write options and weights to file

   // get the filename
   TString tfname(GetWeightFileName());

   fLogger << kINFO << "Reading weight file: "
           << gTools().Color("lightblue") << tfname << gTools().Color("reset") << Endl;

   ifstream fin( tfname );
   if (!fin.good()) { // file not found --> Error
      fLogger << kFATAL << "<ReadStateFromFile> "
                 << "Unable to open input weight file: " << tfname << Endl;
   }

   ReadStateFromStream(fin);
   fin.close();

   if (!fTxtWeightsOnly) {
      // ---- read the ROOT file
      TString rfname( tfname ); rfname.ReplaceAll( ".txt", ".root" );
      fLogger << kINFO << "Reading root weight file: "
              << gTools().Color("lightblue") << rfname << gTools().Color("reset") << Endl;
      TFile* rfile = TFile::Open( rfname, "READ" );
      ReadStateFromStream( *rfile );
      rfile->Close();
   }
}

//_______________________________________________________________________
void TMVA::MethodBase::ReadStateFromStream( std::istream& fin )
{
   // read the header from the weight files of the different MVA methods
   char buf[512];

   // first read the method name
   GetLine(fin,buf);
   while (!TString(buf).BeginsWith("Method")) GetLine(fin,buf);
   TString lstr(buf);
   Int_t idx1 = lstr.First(':')+2; Int_t idx2 = lstr.Index(' ',idx1)-idx1; if (idx2<0) idx2=lstr.Length();
   TString fullname = lstr(idx1,idx2);
   idx1 = fullname.First(':');
   Int_t idxtit = (idx1<0 ? fullname.Length() : idx1);
   TString methodName  = fullname(0, idxtit);
   TString methodTitle;
   Bool_t notit;
   if (idx1<0) {
      methodTitle=methodName;
      notit=kTRUE;
   } 
   else {
      methodTitle=fullname(idxtit+2,fullname.Length()-1);
      notit=kFALSE;
   }
   fLogger << kINFO << "Read method with name <" << methodName << "> and title <" << methodTitle << ">" << Endl;
   this->SetMethodName( methodName );
   this->SetMethodTitle( methodTitle );

   // now the question is whether to read the variables first or the options (well, of course the order
   // of writing them needs to agree)
   //
   // the option "Decorrelation" is needed to decide if the variables we
   // read are decorrelated or not
   //
   // the variables are needed by some methods (TMLP) to build the NN
   // which is done in ProcessOptions so for the time being we first Read and Parse the options then
   // we read the variables, and then we process the options

   // now read all options
   GetLine(fin,buf);
   while (!TString(buf).BeginsWith("#OPT")) GetLine(fin,buf);

   ReadOptionsFromStream(fin);
   ParseOptions(Verbose());

   // Now read variable info
   fin.getline(buf,512);
   while (!TString(buf).BeginsWith("#VAR")) fin.getline(buf,512);
   ReadVarsFromStream(fin);

   fLogger << kINFO << "Create VariableTransformation \"" << fVarTransformString << "\"" << Endl;
   TList* trList = gTools().ParseFormatLine( fVarTransformString, "," );
   TListIter trIt(trList);
   while( TObjString* os = (TObjString*)trIt()) {
      const TString& s = os->GetString();
      if      (s == "Decorrelate" ) GetTransformationHandler().AddTransformation( new VariableDecorrTransform(DataInfo().GetVariableInfos()) );
      else if (s == "PCA" )         GetTransformationHandler().AddTransformation( new VariablePCATransform(DataInfo().GetVariableInfos()) );
      else if (s == "Gauss" )       GetTransformationHandler().AddTransformation( new VariableGaussTransform(DataInfo().GetVariableInfos()) );
      else {
         if(s != "None")
            fLogger << kFATAL << "<ProcessOptions> Variable transform '"
                    << s << "' unknown." << Endl;
      }
   }

   // now we process the options (of the derived class)
   ProcessOptions();

   // Now read decorrelation matrix if available
   if (GetTransformationHandler().GetTransformationList().GetSize() > 0) {
      fin.getline(buf,512);
      while (!TString(buf).BeginsWith("#MAT")) fin.getline(buf,512);
      GetTransformationHandler().ReadFromStream(fin);
   }

   if ( HasMVAPdfs()) {
      // Now read the MVA PDFs
      fin.getline(buf,512);
      while (!TString(buf).BeginsWith("#MVAPDFS")) fin.getline(buf,512);
      if (fMVAPdfS!=0) { delete fMVAPdfS; fMVAPdfS = 0; }
      if (fMVAPdfB!=0) { delete fMVAPdfB; fMVAPdfB = 0; }
      fMVAPdfS = new PDF();
      fMVAPdfB = new PDF();
      fMVAPdfS->SetReadingVersion( GetTrainingTMVAVersionCode() );
      fMVAPdfB->SetReadingVersion( GetTrainingTMVAVersionCode() );
      
      fin >> *fMVAPdfS;
      fin >> *fMVAPdfB;
   }

   // Now read weights
   fin.getline(buf,512);
   while (!TString(buf).BeginsWith("#WGT")) fin.getline(buf,512);
   fin.getline(buf,512);

   ReadWeightsFromStream( fin );
}


// TODO

//_______________________________________________________________________
void TMVA::MethodBase::WriteVarsToStream( std::ostream& o, const TString& prefix ) const 
{
   // write the list of variables (name, min, max) for a given data
   // transformation method to the stream
   o << prefix << "NVar " << DataInfo().GetNVariables() << endl;
   std::vector<VariableInfo>::const_iterator varIt = DataInfo().GetVariableInfos().begin();
   for (; varIt!=DataInfo().GetVariableInfos().end(); varIt++) { o << prefix; varIt->WriteToStream(o); }
}

//_______________________________________________________________________
void TMVA::MethodBase::ReadVarsFromStream( std::istream& istr ) 
{
   // Read the variables (name, min, max) for a given data
   // transformation method from the stream. In the stream we only
   // expect the limits which will be set

   TString dummy;
   UInt_t readNVar;
   istr >> dummy >> readNVar;

   if (readNVar!=DataInfo().GetNVariables()) {
      fLogger << kFATAL << "You declared "<< DataInfo().GetNVariables() << " variables in the Reader"
              << " while there are " << readNVar << " variables declared in the file"
              << Endl;
   }

   // we want to make sure all variables are read in the order they are defined
   VariableInfo varInfo;
   std::vector<VariableInfo>::iterator varIt = DataInfo().GetVariableInfos().begin();
   int varIdx = 0;
   for (; varIt!=DataInfo().GetVariableInfos().end(); varIt++, varIdx++) {
      varInfo.ReadFromStream(istr);
      if (varIt->GetExpression() == varInfo.GetExpression()) {
         varInfo.SetExternalLink((*varIt).GetExternalLink());
         (*varIt) = varInfo;
      } 
      else {
         fLogger << kINFO << "The definition (or the order) of the variables found in the input file is"  << Endl;
         fLogger << kINFO << "is not the same as the one declared in the Reader (which is necessary for" << Endl;
         fLogger << kINFO << "the correct working of the classifier):" << Endl;
         fLogger << kINFO << "   var #" << varIdx <<" declared in Reader: " << varIt->GetExpression() << Endl;
         fLogger << kINFO << "   var #" << varIdx <<" declared in file  : " << varInfo.GetExpression() << Endl;
         fLogger << kFATAL << "The expression declared to the Reader needs to be checked (name or order are wrong)" << Endl;
      }
   }
}

//_______________________________________________________________________
TDirectory* TMVA::MethodBase::BaseDir() const
{
   // returns the ROOT directory where info/histograms etc of the
   // corresponding MVA method instance are stored

   if (fBaseDir != 0) return fBaseDir;

   TDirectory* methodDir = MethodBaseDir();
   if (methodDir==0)
      fLogger << kFATAL << "MethodBase::BaseDir() - MethodBaseDir() return a NULL pointer!" << Endl;

   TDirectory* dir = 0;

   TString defaultDir = GetMethodTitle();

   TObject* o = methodDir->FindObject(defaultDir);
   if (o!=0 && o->InheritsFrom("TDirectory")) dir = (TDirectory*)o;

   if (dir != 0) return dir;

   TDirectory *sdir = methodDir->mkdir(defaultDir);

   // write weight file name into target file
   sdir->cd();
   TObjString wfilePath( gSystem->WorkingDirectory() );
   TObjString wfileName( GetWeightFileName() );
   wfilePath.Write( "TrainingPath" );
   wfileName.Write( "WeightFileName" );

   return sdir;
}

//_______________________________________________________________________
TDirectory* TMVA::MethodBase::MethodBaseDir() const
{
   // returns the ROOT directory where all instances of the
   // corresponding MVA method are stored

   if (fMethodBaseDir != 0) return fMethodBaseDir;

   const TString dirName(Form("Method_%s",GetName())); 

   TDirectory * dir = Factory::RootBaseDir()->GetDirectory(dirName);
   if (dir != 0) return dir;

   fMethodBaseDir = Factory::RootBaseDir()->mkdir(dirName,Form("Directory for all %s methods", GetName()));

   return fMethodBaseDir;
}

//_______________________________________________________________________
void TMVA::MethodBase::SetWeightFileDir( TString fileDir )
{
   // set directory of weight file

   fFileDir = fileDir;
   gSystem->MakeDirectory( fFileDir );
}

//_______________________________________________________________________
void TMVA::MethodBase::SetWeightFileName( TString theWeightFile)
{
   // set the weight file name (depreciated)
   fWeightFile = theWeightFile;
}

//_______________________________________________________________________
TString TMVA::MethodBase::GetWeightFileName() const
{
   // retrieve weight file name
   if (fWeightFile!="") return fWeightFile;

   // the default consists of
   // directory/jobname_methodname_suffix.extension.{root/txt}
   TString suffix = "";
   return ( GetWeightFileDir() + "/" + fJobName + "_" + GetMethodTitle() + 
            suffix + "." + gConfig().GetIONames().fWeightFileExtension + ".txt" );
}

//_______________________________________________________________________
void TMVA::MethodBase::WriteEvaluationHistosToFile()
{
   // writes all MVA evaluation histograms to file
   BaseDir()->cd();

   if (0 != fHistS_plotbin  ) fHistS_plotbin->Write();
   if (0 != fHistB_plotbin  ) fHistB_plotbin->Write();
   if (0 != fHistTrS_plotbin) fHistTrS_plotbin->Write();
   if (0 != fHistTrB_plotbin) fHistTrB_plotbin->Write();
   if (0 != fProbaS_plotbin ) fProbaS_plotbin->Write();
   if (0 != fProbaB_plotbin ) fProbaB_plotbin->Write();
   if (0 != fRarityS_plotbin) fRarityS_plotbin->Write();
   if (0 != fRarityB_plotbin) fRarityB_plotbin->Write();
   if (0 != fHistS_highbin  ) fHistS_highbin->Write();
   if (0 != fHistB_highbin  ) fHistB_highbin->Write();
   if (0 != fEffS           ) fEffS->Write();
   if (0 != fEffB           ) fEffB->Write();
   if (0 != fEffBvsS        ) fEffBvsS->Write();
   if (0 != fRejBvsS        ) fRejBvsS->Write();
   if (0 != finvBeffvsSeff  ) finvBeffvsSeff->Write();

   if (0 != fMVAPdfS) {
      fMVAPdfS->GetOriginalHist()->Write();
      fMVAPdfS->GetSmoothedHist()->Write();
      fMVAPdfS->GetPDFHist()->Write();
   }
   if (0 != fMVAPdfB) {
      fMVAPdfB->GetOriginalHist()->Write();
      fMVAPdfB->GetSmoothedHist()->Write();
      fMVAPdfB->GetPDFHist()->Write();
   }
}

//_______________________________________________________________________
void  TMVA::MethodBase::WriteMonitoringHistosToFile( void ) const
{
   // write special monitoring histograms to file - not implemented for this method
   fLogger << kINFO << "No monitoring histograms written" << Endl;
}

//_______________________________________________________________________
bool TMVA::MethodBase::GetLine(std::istream& fin, char* buf )
{
   // reads one line from the input stream
   // checks for certain keywords and interprets
   // the line if keywords are found
   fin.getline(buf,512);
   TString line(buf);
   if (line.BeginsWith("TMVA Release")) {
      Ssiz_t start = line.First('[')+1;
      Ssiz_t length = line.Index("]",start)-start;
      TString code = line(start,length);
      std::stringstream s(code.Data());
      s >> fTMVATrainingVersion;
      fLogger << kINFO << "Classifier was trained with TMVA Version: " << GetTrainingTMVAVersionString() << Endl;
   }
   if (line.BeginsWith("ROOT Release")) {
      Ssiz_t start = line.First('[')+1;
      Ssiz_t length = line.Index("]",start)-start;
      TString code = line(start,length);
      std::stringstream s(code.Data());
      s >> fROOTTrainingVersion;
      fLogger << kINFO << "Classifier was trained with ROOT Version: " << GetTrainingROOTVersionString() << Endl;
   }

   return true;
}

//_______________________________________________________________________
void TMVA::MethodBase::CreateMVAPdfs()
{
   // Create PDFs of the MVA output variables

   Data()->SetCurrentType(Types::kTraining);

   fLogger << kINFO << "<CreateMVAPdfs> Using " << fNbinsMVAPdf << " bins and smooth "
           << fNsmoothMVAPdf << " times" << Endl;

   std::vector<Float_t> * mvaRes = Data()->GetClassifierOutputVector(GetTestvarName());
   if(mvaRes==0 || mvaRes->size()==0)
      fLogger << kFATAL << "<CreateMVAPdfs> No result of classifier testing available" << Endl;

   Double_t minVal= *std::min_element(mvaRes->begin(),mvaRes->end());
   Double_t maxVal= *std::max_element(mvaRes->begin(),mvaRes->end());
   
   // create histograms that serve as basis to create the MVA Pdfs
   TH1* histMVAPdfS = new TH1F( GetMethodName() + "_tr_S", GetMethodName() + "_tr_S",
                                fMVAPdfS->GetHistNBins( mvaRes->size() ), minVal, maxVal );
   TH1* histMVAPdfB = new TH1F( GetMethodName() + "_tr_B", GetMethodName() + "_tr_B",
                                fMVAPdfB->GetHistNBins( mvaRes->size() ), minVal, maxVal );

   // compute sum of weights properly
   histMVAPdfS->Sumw2();
   histMVAPdfB->Sumw2();

   // fill histograms
   for (UInt_t ievt=0; ievt<mvaRes->size(); ievt++) {
      Double_t theVal = (*mvaRes)[ievt];
      if (Data()->GetEvent(ievt)->IsSignal())
         histMVAPdfS->Fill( theVal );
      else
         histMVAPdfB->Fill( theVal );
   }

   // normalisation
   gTools().NormHist( histMVAPdfS );
   gTools().NormHist( histMVAPdfB );

   // momentary hack for ROOT problem
   histMVAPdfS->Write();
   histMVAPdfB->Write();

   // create PDFs
   fMVAPdfS->BuildPDF( histMVAPdfS );
   fMVAPdfB->BuildPDF( histMVAPdfB );

   fMVAPdfS->ValidatePDF( histMVAPdfS );
   fMVAPdfB->ValidatePDF( histMVAPdfB );

   fLogger << kINFO
           << Form( "<CreateMVAPdfs> Separation from histogram (PDF): %1.3f (%1.3f)",
                    GetSeparation( histMVAPdfS, histMVAPdfB ), GetSeparation( fMVAPdfS, fMVAPdfB ) )
           << Endl;

   delete histMVAPdfS;
   delete histMVAPdfB;
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetProba( Double_t mvaVal, Double_t ap_sig )
{
   // compute likelihood ratio
   if (!fMVAPdfS || !fMVAPdfB) {
      fLogger << kWARNING << "<GetProba> MVA PDFs for Signal and Background don't exist" << Endl;
      return 0;
   }
//     std::cout << "methbase getproba " << std::endl; 
//     std::cout << "mg mvaVal " << mvaVal << std::endl;
//     std::cout << "fMVAPdfS " <<fMVAPdfS << std::endl;
//     std::cout << "fMVAPdfB " <<fMVAPdfB << std::endl;
   Double_t p_s = fMVAPdfS->GetVal( mvaVal );
//     std::cout << "sig done " << std::endl;
   Double_t p_b = fMVAPdfB->GetVal( mvaVal );
//     std::cout << "back done " << std::endl;

   Double_t denom = p_s*ap_sig + p_b*(1 - ap_sig);

   return (denom > 0) ? (p_s*ap_sig) / denom : -1;
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetRarity( Double_t mvaVal, Types::ESBType reftype ) const
{
   // compute rarity:
   // R(x) = Integrate_[-oo..x] { PDF(x') dx' }
   // where PDF(x) is the PDF of the classifier's signal or background distribution

   if ((reftype == Types::kSignal && !fMVAPdfS) || (reftype == Types::kBackground && !fMVAPdfB)) {
      fLogger << kWARNING << "<GetRarity> Required MVA PDF for Signal or Backgroud does not exist: "
              << "select option \"CreateMVAPdfs\"" << Endl;
      return 0.0;
   }

   PDF* thePdf = reftype == Types::kSignal ? fMVAPdfS : fMVAPdfB;

   return thePdf->GetIntegral( thePdf->GetXmin(), mvaVal );
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetEfficiency( const TString& theString, Types::ETreeType type,Double_t& effSerr )
{
   // fill background efficiency (resp. rejection) versus signal efficiency plots
   // returns signal efficiency at background efficiency indicated in theString

   Data()->SetCurrentType(type);
   std::vector<Float_t>* mvaRes = Data()->GetClassifierOutputVector(GetTestvarName());

   // parse input string for required background efficiency
   TList* list  = gTools().ParseFormatLine( theString );

   // sanity check
   Bool_t computeArea = kFALSE;
   if      (!list || list->GetSize() < 2) computeArea = kTRUE; // the area is computed
   else if (list->GetSize() > 2) {
      fLogger << kFATAL << "<GetEfficiency> Wrong number of arguments"
              << " in string: " << theString
              << " | required format, e.g., Efficiency:0.05, or empty string" << Endl;
      return -1;
   }

   // sanity check
   if (fHistS_highbin->GetNbinsX() != fHistB_highbin->GetNbinsX() ||
       fHistS_plotbin->GetNbinsX() != fHistB_plotbin->GetNbinsX()) {
      fLogger << kFATAL << "<GetEfficiency> Binning mismatch between signal and background histos" << Endl;
      return -1.0;
   }

   // create histograms

   // first, get efficiency histograms for signal and background
   Double_t xmin = fHistS_highbin->GetXaxis()->GetXmin();
   Double_t xmax = fHistS_highbin->GetXaxis()->GetXmax();

   // first round ? --> create histograms
   Bool_t firstPass = kFALSE;

   static Double_t nevtS;

   if (0 == fEffS && 0 == fEffB) firstPass = kTRUE;

   if (firstPass) {

      fEffS = new TH1F( GetTestvarName() + "_effS", GetTestvarName() + " (signal)",     fNbinsH, xmin, xmax );
      fEffB = new TH1F( GetTestvarName() + "_effB", GetTestvarName() + " (background)", fNbinsH, xmin, xmax );
      fEffS->SetDirectory(0);
      fEffB->SetDirectory(0);

      // sign if cut
      Int_t sign = (fCutOrientation == kPositive) ? +1 : -1;

      // this method is unbinned
      nevtS = 0;
      for (UInt_t ievt=0; ievt<Data()->GetNEvents(); ievt++) {

         // read the tree
         Int_t   theType   = Data()->GetEvent(ievt)->GetType();
         Float_t theWeight = Data()->GetEvent(ievt)->GetWeight();
         Float_t theVal    = (*mvaRes)[ievt];

         // select histogram depending on if sig or bgd
         TH1* theHist = (theType == 1) ? fEffS : fEffB;

         // count signal and background events in tree
         if (theType == 1) nevtS+=theWeight;

         TAxis* axis   = theHist->GetXaxis();
         Int_t  maxbin = Int_t((theVal - axis->GetXmin())/(axis->GetXmax() - axis->GetXmin())*fNbinsH) + 1;
         if (sign > 0 && maxbin > fNbinsH) continue; // can happen... event doesn't count
         if (sign < 0 && maxbin < 1      ) continue; // can happen... event doesn't count
         if (sign > 0 && maxbin < 1      ) maxbin = 1;
         if (sign < 0 && maxbin > fNbinsH) maxbin = fNbinsH;

         if (sign > 0)
            for (Int_t ibin=1; ibin<=maxbin; ibin++) theHist->AddBinContent( ibin , theWeight);
         else if (sign < 0)
            for (Int_t ibin=maxbin+1; ibin<=fNbinsH; ibin++) theHist->AddBinContent( ibin , theWeight );
         else
            fLogger << kFATAL << "<GetEfficiency> Mismatch in sign" << Endl;
      }

      // renormalise to maximum
      fEffS->Scale( 1.0/(fEffS->GetMaximum() > 0 ? fEffS->GetMaximum() : 1) );
      fEffB->Scale( 1.0/(fEffB->GetMaximum() > 0 ? fEffB->GetMaximum() : 1) );

      // now create efficiency curve: background versus signal
      fEffBvsS = new TH1F( GetTestvarName() + "_effBvsS", GetTestvarName() + "", fNbins, 0, 1 );
      fEffBvsS->SetDirectory(0);
      fEffBvsS->SetXTitle( "signal eff" );
      fEffBvsS->SetYTitle( "backgr eff" );
      fRejBvsS = new TH1F( GetTestvarName() + "_rejBvsS", GetTestvarName() + "", fNbins, 0, 1 );
      fRejBvsS->SetDirectory(0);
      fRejBvsS->SetXTitle( "signal eff" );
      fRejBvsS->SetYTitle( "backgr rejection (1-eff)" );
      finvBeffvsSeff = new TH1F( GetTestvarName() + "_invBeffvsSeff",
                                 GetTestvarName() + "", fNbins, 0, 1 );
      finvBeffvsSeff->SetDirectory(0);
      finvBeffvsSeff->SetXTitle( "signal eff" );
      finvBeffvsSeff->SetYTitle( "inverse backgr. eff (1/eff)" );

      // use root finder
      // spline background efficiency plot
      // note that there is a bin shift when going from a TH1F object to a TGraph :-(
      if (Use_Splines_for_Eff_) {
         if(fGraphS) { delete fGraphS; fGraphS = 0; }
         if(fGraphB) { delete fGraphB; fGraphB = 0; }
         fGraphS   = new TGraph( fEffS );
         fGraphB   = new TGraph( fEffB );
         fSplRefS  = new TSpline1( "spline2_signal",     fGraphS );
         fSplRefB  = new TSpline1( "spline2_background", fGraphB );

         // verify spline sanity
         gTools().CheckSplines( fEffS, fSplRefS );
         gTools().CheckSplines( fEffB, fSplRefB );
      }

      // make the background-vs-signal efficiency plot

      // create root finder
      // reset static "this" pointer before calling external function
      ResetThisBase();
      RootFinder rootFinder( &IGetEffForRoot, fXmin, fXmax );

      Double_t effB = 0;
      for (Int_t bini=1; bini<=fNbins; bini++) {

         // find cut value corresponding to a given signal efficiency
         Double_t effS = fEffBvsS->GetBinCenter( bini );
         Double_t cut  = rootFinder.Root( effS );

         // retrieve background efficiency for given cut
         if (Use_Splines_for_Eff_) effB = fSplRefB->Eval( cut );
         else                      effB = fEffB->GetBinContent( fEffB->FindBin( cut ) );

         // and fill histograms
         fEffBvsS->SetBinContent( bini, effB     );
         fRejBvsS->SetBinContent( bini, 1.0-effB );
         if (effB>std::numeric_limits<double>::epsilon())
            finvBeffvsSeff->SetBinContent( bini, 1.0/effB );
      }

      // create splines for histogram
      if(fGrapheffBvsS) { delete fGrapheffBvsS; fGrapheffBvsS = 0; }
      fGrapheffBvsS = new TGraph( fEffBvsS );
      fSpleffBvsS   = new TSpline1( "effBvsS", fGrapheffBvsS );

      // search for overlap point where, when cutting on it,
      // one would obtain: eff_S = rej_B = 1 - eff_B
      Double_t effS, rejB, effS_ = 0, rejB_ = 0;
      Int_t    nbins_ = 5000;
      for (Int_t bini=1; bini<=nbins_; bini++) {

         // get corresponding signal and background efficiencies
         effS = (bini - 0.5)/Float_t(nbins_);
         rejB = 1.0 - fSpleffBvsS->Eval( effS );

         // find signal efficiency that corresponds to required background efficiency
         if ((effS - rejB)*(effS_ - rejB_) < 0) break;
         effS_ = effS;
         rejB_ = rejB;
      }

      // find cut that corresponds to signal efficiency and update signal-like criterion
      Double_t cut = rootFinder.Root( 0.5*(effS + effS_) );
      SetSignalReferenceCut( cut );
   }

   // must exist...
   if (0 == fSpleffBvsS) return 0.0;

   // now find signal efficiency that corresponds to required background efficiency
   Double_t effS = 0, effB = 0, effS_ = 0, effB_ = 0;
   Int_t    nbins_ = 1000;

   if (computeArea) {

      // compute area of rej-vs-eff plot
      Double_t integral = 0;
      for (Int_t bini=1; bini<=nbins_; bini++) {

         // get corresponding signal and background efficiencies
         effS = (bini - 0.5)/Float_t(nbins_);
         effB = fSpleffBvsS->Eval( effS );
         integral += (1.0 - effB);
      }
      integral /= nbins_;

      return integral;
   }
   else {

      // that will be the value of the efficiency retured (does not affect
      // the efficiency-vs-bkg plot which is done anyway.
      Float_t effBref  = atof( ((TObjString*)list->At(1))->GetString() );

      // find precise efficiency value
      for (Int_t bini=1; bini<=nbins_; bini++) {

         // get corresponding signal and background efficiencies
         effS = (bini - 0.5)/Float_t(nbins_);
         effB = fSpleffBvsS->Eval( effS );

         // find signal efficiency that corresponds to required background efficiency
         if ((effB - effBref)*(effB_ - effBref) <= 0) break;
         effS_ = effS;
         effB_ = effB;
      }

      // take mean between bin above and bin below
      effS = 0.5*(effS + effS_);

      effSerr = 0;
      if (nevtS > 0) effSerr = TMath::Sqrt( effS*(1.0 - effS)/nevtS );

      return effS;
   }

   return -1;
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetTrainingEfficiency( TString theString)
{
   Data()->SetCurrentType(Types::kTraining);

   // fill background efficiency (resp. rejection) versus signal efficiency plots
   // returns signal efficiency at background efficiency indicated in theString

   // parse input string for required background efficiency
   TList*  list  = gTools().ParseFormatLine( theString );
   // sanity check

   if (list->GetSize() != 2) {
      fLogger << kFATAL << "<GetTrainingEfficiency> Wrong number of arguments"
              << " in string: " << theString
              << " | required format, e.g., Efficiency:0.05" << Endl;
      return -1;
   }
   // that will be the value of the efficiency retured (does not affect
   // the efficiency-vs-bkg plot which is done anyway.
   Float_t effBref  = atof( ((TObjString*)list->At(1))->GetString() );

   delete list;

   // sanity check
   if (fHistS_highbin->GetNbinsX() != fHistB_highbin->GetNbinsX() ||
       fHistS_plotbin->GetNbinsX() != fHistB_plotbin->GetNbinsX()) {
      fLogger << kFATAL << "<GetTrainingEfficiency> Binning mismatch between signal and background histos"
              << Endl;
      return -1.0;
   }

   // create histogram

   // first, get efficiency histograms for signal and background
   Double_t xmin = fHistS_highbin->GetXaxis()->GetXmin();
   Double_t xmax = fHistS_highbin->GetXaxis()->GetXmax();

   // first round ? --> create histograms
   Bool_t firstPass = kFALSE;
   if (0 == fTrainEffS && 0 == fTrainEffB) firstPass = kTRUE;

   if (firstPass) {

      // classifier response distributions for test sample
      Double_t sxmax = fXmax+0.00001;
      fHistTrS_plotbin = new TH1F( GetTestvarName() + "_Train_S",GetTestvarName() + "_Train_S", 
                                   fNbins, fXmin, sxmax );
      fHistTrB_plotbin = new TH1F( GetTestvarName() + "_Train_B",GetTestvarName() + "_Train_B", 
                                   fNbins, fXmin, sxmax );
      fHistTrS_plotbin->Sumw2();
      fHistTrB_plotbin->Sumw2();

      // efficiency plots
      fTrainEffS = new TH1F( GetTestvarName() + "_trainingEffS", GetTestvarName() + " (signal)",
                             fNbinsH, xmin, xmax );
      fTrainEffB = new TH1F( GetTestvarName() + "_trainingEffB", GetTestvarName() + " (background)",
                             fNbinsH, xmin, xmax );
      fTrainEffS->SetDirectory(0);
      fTrainEffB->SetDirectory(0);

      // sign if cut
      Int_t sign = (fCutOrientation == kPositive) ? +1 : -1;

      // this method is unbinned
      for (Int_t ievt=0; ievt<Data()->GetNEvents(); ievt++) {

         Data()->SetCurrentEvent(ievt);
         const Event * ev = GetEvent();

         Double_t theVal    = GetMvaValue();
         Double_t theWeight = ev->GetWeight();

         TH1* theEffHist = ev->IsSignal() ? fTrainEffS : fTrainEffB;
         TH1* theClsHist = ev->IsSignal() ? fHistTrS_plotbin : fHistTrB_plotbin;         

         theClsHist->Fill( theVal, theWeight );

         TAxis* axis   = theEffHist->GetXaxis();
         Int_t  maxbin = Int_t((theVal - axis->GetXmin())/(axis->GetXmax() - axis->GetXmin())*fNbinsH) + 1;
         if (sign > 0 && maxbin > fNbinsH) continue; // can happen... event doesn't count
         if (sign < 0 && maxbin < 1      ) continue; // can happen... event doesn't count
         if (sign > 0 && maxbin < 1      ) maxbin = 1;
         if (sign < 0 && maxbin > fNbinsH) maxbin = fNbinsH;

         if (sign > 0)
            for (Int_t ibin=1; ibin<=maxbin; ibin++) theEffHist->AddBinContent( ibin , theWeight );
         else if (sign < 0)
            for (Int_t ibin=maxbin+1; ibin<=fNbinsH; ibin++) theEffHist->AddBinContent( ibin , theWeight );
         else
            fLogger << kFATAL << "<GetEfficiency> Mismatch in sign" << Endl;
      }

      // normalise output distributions
      gTools().NormHist( fHistTrS_plotbin  );
      gTools().NormHist( fHistTrB_plotbin  );
      fHistTrS_plotbin->SetDirectory(0);
      fHistTrB_plotbin->SetDirectory(0);

      // renormalise to maximum
      fTrainEffS->Scale( 1.0/(fTrainEffS->GetMaximum() > 0 ? fTrainEffS->GetMaximum() : 1) );
      fTrainEffB->Scale( 1.0/(fTrainEffB->GetMaximum() > 0 ? fTrainEffB->GetMaximum() : 1) );

      // now create efficiency curve: background versus signal
      fTrainEffBvsS = new TH1F( GetTestvarName() + "_trainingEffBvsS", GetTestvarName() + "", fNbins, 0, 1 );
      fTrainRejBvsS = new TH1F( GetTestvarName() + "_trainingRejBvsS", GetTestvarName() + "", fNbins, 0, 1 );
      fTrainEffBvsS->SetDirectory(0);
      fTrainRejBvsS->SetDirectory(0);

      // use root finder
      // spline background efficiency plot
      // note that there is a bin shift when going from a TH1F object to a TGraph :-(
      if (Use_Splines_for_Eff_) {
         if(fGraphTrainS) { delete fGraphTrainS; fGraphTrainS = 0; }
         if(fGraphTrainB) { delete fGraphTrainB; fGraphTrainB = 0; }
         fGraphTrainS   = new TGraph( fTrainEffS );
         fGraphTrainB   = new TGraph( fTrainEffB );
         fSplTrainRefS  = new TSpline1( "spline2_signal",     fGraphTrainS );
         fSplTrainRefB  = new TSpline1( "spline2_background", fGraphTrainB );

         // verify spline sanity
         gTools().CheckSplines( fTrainEffS, fSplTrainRefS );
         gTools().CheckSplines( fTrainEffB, fSplTrainRefB );
      }

      // make the background-vs-signal efficiency plot

      // create root finder
      // reset static "this" pointer before calling external function
      ResetThisBase();
      RootFinder rootFinder(&IGetEffForRoot, fXmin, fXmax );

      Double_t effB = 0;
      for (Int_t bini=1; bini<=fNbins; bini++) {

         // find cut value corresponding to a given signal efficiency
         Double_t effS = fTrainEffBvsS->GetBinCenter( bini );

         Double_t cut  = rootFinder.Root( effS );

         // retrieve background efficiency for given cut
         if (Use_Splines_for_Eff_) effB = fSplTrainRefB->Eval( cut );
         else                      effB = fTrainEffB->GetBinContent( fTrainEffB->FindBin( cut ) );

         // and fill histograms
         fTrainEffBvsS->SetBinContent( bini, effB     );
         fTrainRejBvsS->SetBinContent( bini, 1.0-effB );
      }

      // create splines for histogram
      fGraphTrainEffBvsS = new TGraph( fTrainEffBvsS );
      fSplTrainEffBvsS   = new TSpline1( "effBvsS", fGraphTrainEffBvsS );
   }

   // must exist...
   if (0 == fSplTrainEffBvsS) return 0.0;

   // now find signal efficiency that corresponds to required background efficiency
   Double_t effS, effB, effS_ = 0, effB_ = 0;
   Int_t    nbins_ = 1000;
   for (Int_t bini=1; bini<=nbins_; bini++) {

      // get corresponding signal and background efficiencies
      effS = (bini - 0.5)/Float_t(nbins_);
      effB = fSplTrainEffBvsS->Eval( effS );

      // find signal efficiency that corresponds to required background efficiency
      if ((effB - effBref)*(effB_ - effBref) <= 0) break;
      effS_ = effS;
      effB_ = effB;
   }

   return 0.5*(effS + effS_); // the mean between bin above and bin below
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetSignificance( void ) const
{
   // compute significance of mean difference
   // significance = |<S> - <B>|/Sqrt(RMS_S2 + RMS_B2)
   Double_t rms = sqrt( fRmsS*fRmsS + fRmsB*fRmsB );

   return (rms > 0) ? TMath::Abs(fMeanS - fMeanB)/rms : 0;
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetSeparation( TH1* histoS, TH1* histoB ) const
{
   // compute "separation" defined as
   // <s2> = (1/2) Int_-oo..+oo { (S(x) - B(x))^2/(S(x) + B(x)) dx }
   return gTools().GetSeparation( histoS, histoB );
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetSeparation( PDF* pdfS, PDF* pdfB ) const
{
   // compute "separation" defined as
   // <s2> = (1/2) Int_-oo..+oo { (S(x)2 - B(x)2)/(S(x) + B(x)) dx }
   
   // note, if zero pointers given, use internal pdf
   // sanity check first
   if ((!pdfS && pdfB) || (pdfS && !pdfB))
      fLogger << kFATAL << "<GetSeparation> Mismatch in pdfs" << Endl;
   if (!pdfS) pdfS = fSplS;
   if (!pdfB) pdfB = fSplB;
   
   return gTools().GetSeparation( *pdfS, *pdfB );
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetMaximumSignificance( Double_t SignalEvents,
                                                   Double_t BackgroundEvents,
                                                   Double_t& max_significance_value ) const
{
   // plot significance, S/Sqrt(S^2 + B^2), curve for given number
   // of signal and background events; returns cut for maximum significance
   // also returned via reference is the maximum significance

   Double_t max_significance(0);
   Double_t effS(0),effB(0),significance(0);
   TH1F *temp_histogram = new TH1F("temp", "temp", fNbinsH, fXmin, fXmax );

   if (SignalEvents <= 0 || BackgroundEvents <= 0) {
      fLogger << kFATAL << "<GetMaximumSignificance> "
              << "Number of signal or background events is <= 0 ==> abort"
              << Endl;
   }

   fLogger << kINFO << "Using ratio SignalEvents/BackgroundEvents = "
           << SignalEvents/BackgroundEvents << Endl;

   if ((fEffS == 0) || (fEffB == 0)) {
      fLogger << kWARNING << "Efficiency histograms empty !" << Endl;
      fLogger << kWARNING << "no maximum cut found, return 0" << Endl;
      return 0;
   }

   for (Int_t bin=1; bin<=fNbinsH; bin++) {
      effS = fEffS->GetBinContent( bin );
      effB = fEffB->GetBinContent( bin );

      // put significance into a histogram
      significance = sqrt(SignalEvents)*( effS )/sqrt( effS + ( BackgroundEvents / SignalEvents) * effB  );

      temp_histogram->SetBinContent(bin,significance);
   }

   // find maximum in histogram
   max_significance = temp_histogram->GetBinCenter( temp_histogram->GetMaximumBin() );
   max_significance_value = temp_histogram->GetBinContent( temp_histogram->GetMaximumBin() );

   // delete
   delete temp_histogram;

   fLogger << kINFO << "Optimal cut at      : " << max_significance << Endl;
   fLogger << kINFO << "Maximum significance: " << max_significance_value << Endl;

   return max_significance;
}

//_______________________________________________________________________
void TMVA::MethodBase::Statistics( Types::ETreeType treeType, const TString& theVarName,
                                   Double_t& meanS, Double_t& meanB,
                                   Double_t& rmsS,  Double_t& rmsB,
                                   Double_t& xmin,  Double_t& xmax )
{
   // calculates rms,mean, xmin, xmax of the event variable
   // this can be either done for the variables as they are or for
   // normalised variables (in the range of 0-1) if "norm" is set to kTRUE

   Types::ETreeType previousTreeType = Data()->GetCurrentType();
   Data()->SetCurrentType(treeType);

   Long64_t entries = Data()->GetNEvents();

   // sanity check
   if (entries <=0)
      fLogger << kFATAL << "<CalculateEstimator> Wrong tree type: " << treeType << Endl;

   // index of the wanted variable
   UInt_t varIndex = DataInfo().FindVarIndex( theVarName );

   // first fill signal and background in arrays before analysis
   xmin               = +1e20;
   xmax               = -1e20;
   Long64_t nEventsS  = -1;
   Long64_t nEventsB  = -1;

   // take into account event weights
   meanS = 0;
   meanB = 0;
   rmsS  = 0;
   rmsB  = 0;
   Double_t sumwS = 0, sumwB = 0;

   // loop over all training events
   for (Int_t ievt = 0; ievt < entries; ievt++) {

      const Event * ev = GetEvent(ievt);

      Double_t theVar = ev->GetVal(varIndex);
      Double_t weight = ev->GetWeight();

      if (ev->IsSignal()) {
         sumwS               += weight;
         meanS               += weight*theVar;
         rmsS                += weight*theVar*theVar;
      } else {
         sumwB               += weight;
         meanB               += weight*theVar;
         rmsB                += weight*theVar*theVar;
      }
      xmin = TMath::Min( xmin, theVar );
      xmax = TMath::Max( xmax, theVar );
   }
   ++nEventsS;
   ++nEventsB;

   meanS = meanS/sumwS;
   meanB = meanB/sumwB;
   rmsS  = TMath::Sqrt( rmsS/sumwS - meanS*meanS );
   rmsB  = TMath::Sqrt( rmsB/sumwB - meanB*meanB );

   Data()->SetCurrentType(previousTreeType);
}

//_______________________________________________________________________
void TMVA::MethodBase::MakeClass( const TString& theClassFileName ) const
{
   // create reader class for classifier

   // the default consists of
   TString classFileName = "";
   if (theClassFileName == "")
      classFileName = GetWeightFileDir() + "/" + fJobName + "_" + GetMethodTitle() + ".class.C";
   else
      classFileName = theClassFileName;

   TString className = TString("Read") + GetMethodTitle();

   TString tfname( classFileName );
   fLogger << kINFO << "Creating standalone response class : "
           << gTools().Color("lightblue") << classFileName << gTools().Color("reset") << Endl;

   ofstream fout( classFileName );
   if (!fout.good()) { // file could not be opened --> Error
      fLogger << kFATAL << "<MakeClass> Unable to open file: " << classFileName << Endl;
   }

   // now create the class
   // preamble
   fout << "// Class: " << className << endl;
   fout << "// Automatically generated by MethodBase::MakeClass" << endl << "//" << endl;

   // print general information and configuration state
   fout << endl;
   fout << "/* configuration options =====================================================" << endl << endl;
   WriteStateToStream( fout, kTRUE );
   fout << endl;
   fout << "============================================================================ */" << endl;

   // generate the class
   fout << "" << endl;
   fout << "#include <vector>" << endl;
   fout << "#include <cmath>" << endl;
   fout << "#include <string>" << endl;
   fout << "#include <iostream>" << endl;
   fout << "" << endl;
   // now if the classifier needs to write some addicional classes for its response implementation
   // this code goes here: (at least the header declarations need to come before the main class
   this->MakeClassSpecificHeader( fout, className );

   fout << "#ifndef IClassifierReader__def" << endl;
   fout << "#define IClassifierReader__def" << endl;
   fout << endl;
   fout << "class IClassifierReader {" << endl;
   fout << endl;
   fout << " public:" << endl;
   fout << endl;
   fout << "   // constructor" << endl;
   fout << "   IClassifierReader() {}" << endl;
   fout << "   virtual ~IClassifierReader() {}" << endl;
   fout << endl;
   fout << "   // return classifier response" << endl;
   fout << "   virtual double GetMvaValue( const std::vector<double>& inputValues ) const = 0;" << endl;
   fout << "};" << endl;
   fout << endl;
   fout << "#endif" << endl;
   fout << endl;
   fout << "class " << className << " : public IClassifierReader {" << endl;
   fout << endl;
   fout << " public:" << endl;
   fout << endl;
   fout << "   // constructor" << endl;
   fout << "   " << className << "( std::vector<std::string>& theInputVars ) " << endl;
   fout << "      : IClassifierReader()," << endl;
   fout << "        fClassName( \"" << className << "\" )," << endl;
   fout << "        fStatusIsClean( true )," << endl;
   fout << "        fNvars( " << GetNvar() << " )," << endl;
   fout << "        fIsNormalised( " << (IsNormalised() ? "true" : "false") << " )" << endl;
   fout << "   {      " << endl;
   fout << "      // the training input variables" << endl;
   fout << "      const char* inputVars[] = { ";
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      fout << "\"" << GetOriginalVarName(ivar) << "\"";
      if (ivar<GetNvar()-1) fout << ", ";
   }
   fout << " };" << endl;
   fout << endl;
   fout << "      // sanity checks" << endl;
   fout << "      if (theInputVars.size() <= 0) {" << endl;
   fout << "         std::cout << \"Problem in class \\\"\" << fClassName << \"\\\": empty input vector\" << std::endl;" << endl;
   fout << "         fStatusIsClean = false;" << endl;
   fout << "      }" << endl;
   fout << endl;
   fout << "      if (theInputVars.size() != fNvars) {" << endl;
   fout << "         std::cout << \"Problem in class \\\"\" << fClassName << \"\\\": mismatch in number of input values: \"" << endl;
   fout << "                   << theInputVars.size() << \" != \" << fNvars << std::endl;" << endl;
   fout << "         fStatusIsClean = false;" << endl;
   fout << "      }" << endl;
   fout << endl;
   fout << "      // validate input variables" << endl;
   fout << "      for (size_t ivar = 0; ivar < theInputVars.size(); ivar++) {" << endl;
   fout << "         if (theInputVars[ivar] != inputVars[ivar]) {" << endl;
   fout << "            std::cout << \"Problem in class \\\"\" << fClassName << \"\\\": mismatch in input variable names\" << std::endl" << endl;
   fout << "                      << \" for variable [\" << ivar << \"]: \" << theInputVars[ivar].c_str() << \" != \" << inputVars[ivar] << std::endl;" << endl;
   fout << "            fStatusIsClean = false;" << endl;
   fout << "         }" << endl;
   fout << "      }" << endl;
   fout << endl;
   fout << "      // initialize min and max vectors (for normalisation)" << endl;
   for (UInt_t ivar = 0; ivar < GetNvar(); ivar++) {
      fout << "      fVmin[" << ivar << "] = " << std::setprecision(15) << GetXmin( ivar ) << ";" << endl;
      fout << "      fVmax[" << ivar << "] = " << std::setprecision(15) << GetXmax( ivar ) << ";" << endl;
   }
   fout << endl;
   fout << "      // initialize input variable types" << endl;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      fout << "      fType[" << ivar << "] = \'" << DataInfo().GetVarType(ivar) << "\';" << endl;
   }
   fout << endl;
   fout << "      // initialize constants" << endl;
   fout << "      Initialize();" << endl;
   fout << endl;
   if (GetTransformationHandler().GetTransformationList().GetSize() != 0) {
      fout << "      // initialize transformation" << endl;
      fout << "      InitTransform();" << endl;
   }
   fout << "   }" << endl;
   fout << endl;
   fout << "   // destructor" << endl;
   fout << "   virtual ~" << className << "() {" << endl;
   fout << "      Clear(); // method-specific" << endl;
   fout << "   }" << endl;
   fout << endl;
   fout << "   // the classifier response" << endl;
   fout << "   // \"inputValues\" is a vector of input values in the same order as the " << endl;
   fout << "   // variables given to the constructor" << endl;
   fout << "   double GetMvaValue( const std::vector<double>& inputValues ) const;" << endl;
   fout << endl;
   fout << " private:" << endl;
   fout << endl;
   fout << "   // method-specific destructor" << endl;
   fout << "   void Clear();" << endl;
   fout << endl;
   if (GetTransformationHandler().GetTransformationList().GetSize()!=0) {
      fout << "   // input variable transformation" << endl;
      GetTransformationHandler().MakeFunction(fout, className,1);
      fout << "   void InitTransform();" << endl;
      fout << "   void Transform( std::vector<double> & iv, int sigOrBgd ) const;" << endl;
      fout << endl;
   }
   fout << "   // common member variables" << endl;
   fout << "   const char* fClassName;" << endl;
   fout << "   bool fStatusIsClean;" << endl;
   fout << endl;
   fout << "   const size_t fNvars;" << endl;
   fout << "   size_t GetNvar()           const { return fNvars; }" << endl;
   fout << "   char   GetType( int ivar ) const { return fType[ivar]; }" << endl;
   fout << endl;
   fout << "   // normalisation of input variables" << endl;
   fout << "   const bool fIsNormalised;" << endl;
   fout << "   bool IsNormalised() const { return fIsNormalised; }" << endl;
   fout << "   double fVmin[" << GetNvar() << "];" << endl;
   fout << "   double fVmax[" << GetNvar() << "];" << endl;
   fout << "   double NormVariable( double x, double xmin, double xmax ) const {" << endl;
   fout << "      // normalise to output range: [-1, 1]" << endl;
   fout << "      return 2*(x - xmin)/(xmax - xmin) - 1.0;" << endl;
   fout << "   }" << endl;
   fout << endl;
   fout << "   // type of input variable: 'F' or 'I'" << endl;
   fout << "   char   fType[" << GetNvar() << "];" << endl;
   fout << endl;
   fout << "   // initialize internal variables" << endl;
   fout << "   void Initialize();" << endl;
   fout << "   double GetMvaValue__( const std::vector<double>& inputValues ) const;" << endl;
   fout << "" << endl;
   fout << "   // private members (method specific)" << endl;

   // call the classifier specific output (the classifier must close the class !)
   MakeClassSpecific( fout, className );

   fout << "   inline double " << className << "::GetMvaValue( const std::vector<double>& inputValues ) const" << endl;
   fout << "   {" << endl;
   fout << "      // classifier response value" << endl;
   fout << "      double retval = 0;" << endl;
   fout << endl;
   fout << "      // classifier response, sanity check first" << endl;
   fout << "      if (!fStatusIsClean) {" << endl;
   fout << "         std::cout << \"Problem in class \\\"\" << fClassName << \"\\\": cannot return classifier response\"" << endl;
   fout << "                   << \" because status is dirty\" << std::endl;" << endl;
   fout << "         retval = 0;" << endl;
   fout << "      }" << endl;
   fout << "      else {" << endl;
   fout << "         if (IsNormalised()) {" << endl;
   fout << "            // normalise variables" << endl;
   fout << "            std::vector<double> iV;" << endl;
   fout << "            int ivar = 0;" << endl;
   fout << "            for (std::vector<double>::const_iterator varIt = inputValues.begin();" << endl;
   fout << "                 varIt != inputValues.end(); varIt++, ivar++) {" << endl;
   fout << "               iV.push_back(NormVariable( *varIt, fVmin[ivar], fVmax[ivar] ));" << endl;
   fout << "            }" << endl;
   if (GetTransformationHandler().GetTransformationList().GetSize()!=0 && GetMethodType() != Types::kLikelihood )
      fout << "            Transform( iV, 0 );" << endl;
   fout << "            retval = GetMvaValue__( iV );" << endl;
   fout << "         }" << endl;
   fout << "         else {" << endl;
   if (GetTransformationHandler().GetTransformationList().GetSize()!=0 && GetMethodType() != Types::kLikelihood ){
      fout << "            std::vector<double> iV;" << endl;
      fout << "            int ivar = 0;" << endl;
      fout << "            for (std::vector<double>::const_iterator varIt = inputValues.begin();" << endl;
      fout << "                 varIt != inputValues.end(); varIt++, ivar++) {" << endl;
      fout << "               iV.push_back(*varIt);" << endl;
      fout << "            }" << endl;
      fout << "            Transform( iV, 0 );" << endl;
      fout << "            retval = GetMvaValue__( iV );" << endl;
   }else{
      fout << "            retval = GetMvaValue__( inputValues );" << endl;
   }
   fout << "         }" << endl;
   fout << "      }" << endl;
   fout << endl;
   fout << "      return retval;" << endl;
   fout << "   }" << endl;

   // create output for transformation - if any
   if (GetTransformationHandler().GetTransformationList().GetSize()!=0) 
      GetTransformationHandler().MakeFunction(fout, className,2);

   // close the file
   fout.close();
}

//_______________________________________________________________________
void TMVA::MethodBase::PrintHelpMessage() const
{
   // prints out classifier-specific help method

   // if options are written to reference file, also append help info 
   std::streambuf* cout_sbuf = std::cout.rdbuf(); // save original sbuf 
   std::ofstream* o = 0;
   if (gConfig().WriteOptionsReference()) {
      fLogger << kINFO << "Print Help message for class " << GetName() << " into file: " << GetReferenceFile() << Endl;
      o = new std::ofstream( GetReferenceFile(), std::ios::app );
      if (!o->good()) { // file could not be opened --> Error
         fLogger << kFATAL << "<PrintHelpMessage> Unable to append to output file: " << GetReferenceFile() << Endl;
      }
      std::cout.rdbuf( o->rdbuf() ); // redirect 'cout' to file      
   }

   //         "|--------------------------------------------------------------|"
   if (!o) {
      fLogger << kINFO << Endl;
      fLogger << gTools().Color("bold")
              << "================================================================" 
              << gTools().Color( "reset" )
              << Endl;      
      fLogger << gTools().Color("bold")
              << "H e l p   f o r   c l a s s i f i e r   [ " << GetName() << " ] :" 
              << gTools().Color( "reset" )
              << Endl;
   }
   else {
      fLogger << "Help for classifier [ " << GetName() << " ] :" << Endl;
   }

   // print method-specific help message
   GetHelpMessage(); 

   if (!o) {
      fLogger << Endl;
      fLogger << "<Suppress this message by specifying \"!H\" in the booking option>" << Endl;
      fLogger << gTools().Color("bold")
              << "================================================================" 
              << gTools().Color( "reset" )
              << Endl;
      fLogger << Endl;
   }
   else {
      // indicate END
      fLogger << "# End of Message___" << Endl;
   }

   std::cout.rdbuf( cout_sbuf ); // restore the original stream buffer 
   if (o) o->close();
}

// ----------------------- r o o t   f i n d i n g ----------------------------

TMVA::MethodBase* TMVA::MethodBase::fgThisBase = 0;

//_______________________________________________________________________
Double_t TMVA::MethodBase::IGetEffForRoot( Double_t theCut )
{
   // interface for RootFinder
   return MethodBase::GetThisBase()->GetEffForRoot( theCut );
}

//_______________________________________________________________________
Double_t TMVA::MethodBase::GetEffForRoot( Double_t theCut )
{
   // returns efficiency as function of cut
   Double_t retval=0;

   // retrieve the class object
   if (Use_Splines_for_Eff_) {
      retval = fSplRefS->Eval( theCut );
   }
   else
      retval = fEffS->GetBinContent( fEffS->FindBin( theCut ) );

   // caution: here we take some "forbidden" action to hide a problem:
   // in some cases, in particular for likelihood, the binned efficiency distributions
   // do not equal 1, at xmin, and 0 at xmax; of course, in principle we have the
   // unbinned information available in the trees, but the unbinned minimization is
   // too slow, and we don't need to do a precision measurement here. Hence, we force
   // this property.
   Double_t eps = 1.0e-5;
   if      (theCut-fXmin < eps) retval = (GetCutOrientation() == kPositive) ? 1.0 : 0.0;
   else if (fXmax-theCut < eps) retval = (GetCutOrientation() == kPositive) ? 0.0 : 1.0;

   return retval;
}


