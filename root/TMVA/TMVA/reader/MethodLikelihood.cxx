// @(#)root/tmva $Id: MethodLikelihood.cxx,v 1.11 2008/06/26 23:01:52 stelzer Exp $ 
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate Data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TMVA::MethodLikelihood                                                *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Xavier Prudent  <prudent@lapp.in2p3.fr>  - LAPP, France                   *
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

//_______________________________________________________________________
/* Begin_Html

  Likelihood analysis ("non-parametric approach")                      
  
  <p>
  Also implemented is a "diagonalized likelihood approach",            
  which improves over the uncorrelated likelihood ansatz by            
  transforming linearly the input variables into a diagonal space,     
  using the square-root of the covariance matrix                       

  <p>                                                                  
  The method of maximum likelihood is the most straightforward, and 
  certainly among the most elegant multivariate analyser approaches.
  We define the likelihood ratio, <i>R<sub>L</sub></i>, for event 
  <i>i</i>, by:<br>
  <center>
  <img vspace=6 src="gif/tmva_likratio.gif" align="bottom" > 
  </center>
  Here the signal and background likelihoods, <i>L<sub>S</sub></i>, 
  <i>L<sub>B</sub></i>, are products of the corresponding probability 
  densities, <i>p<sub>S</sub></i>, <i>p<sub>B</sub></i>, of the 
  <i>N</i><sub>var</sub> discriminating variables used in the MVA: <br>
  <center>
  <img vspace=6 src="gif/tmva_lik.gif" align="bottom" > 
  </center>
  and accordingly for L<sub>B</sub>.
  In practise, TMVA uses polynomial splines to estimate the probability 
  density functions (PDF) obtained from the distributions of the 
  training variables.<br>

  <p>
  Note that in TMVA the output of the likelihood ratio is transformed
  by<br> 
  <center>
  <img vspace=6 src="gif/tmva_likratio_trans.gif" align="bottom"/> 
  </center>
  to avoid the occurrence of heavy peaks at <i>R<sub>L</sub></i>=0,1.   

  <b>Decorrelated (or "diagonalized") Likelihood</b>

  <p>
  The biggest drawback of the Likelihood approach is that it assumes
  that the discriminant variables are uncorrelated. If it were the case,
  it can be proven that the discrimination obtained by the above likelihood
  ratio is optimal, ie, no other method can beat it. However, in most 
  practical applications of MVAs correlations are present. <br><p></p>

  <p>
  Linear correlations, measured from the training sample, can be taken 
  into account in a straightforward manner through the square-root
  of the covariance matrix. The square-root of a matrix
  <i>C</i> is the matrix <i>C&prime;</i> that multiplied with itself
  yields <i>C</i>: <i>C</i>=<i>C&prime;C&prime;</i>. We compute the 
  square-root matrix (SQM) by means of diagonalising (<i>D</i>) the 
  covariance matrix: <br>
  <center>
  <img vspace=6 src="gif/tmva_sqm.gif" align="bottom" > 
  </center>
  and the linear transformation of the linearly correlated into the 
  uncorrelated variables space is then given by multiplying the measured
  variable tuple by the inverse of the SQM. Note that these transformations
  are performed for both signal and background separately, since the
  correlation pattern is not the same in the two samples.

  <p>
  The above diagonalisation is complete for linearly correlated,
  Gaussian distributed variables only. In real-world examples this 
  is not often the case, so that only little additional information
  may be recovered by the diagonalisation procedure. In these cases,
  non-linear methods must be applied.

End_Html */
//_______________________________________________________________________

#include <iomanip>
#include <vector>

#include "TMatrixD.h"
#include "TVector.h"
#include "TMath.h"
#include "TObjString.h"
#include "TFile.h"
#include "TKey.h"
#include "TH1.h"
#include "TClass.h"

#include "TMVA/ClassifierFactory.h"
#include "TMVA/MethodLikelihood.h"
#include "TMVA/Tools.h"
#include "TMVA/Ranking.h"

REGISTER_METHOD(Likelihood)

ClassImp(TMVA::MethodLikelihood)

using std::endl;

//_______________________________________________________________________
TMVA::MethodLikelihood::MethodLikelihood( const TString& jobName, const TString& methodTitle, DataSetInfo& theData, 
                                          const TString& theOption, TDirectory* theTargetDir )
      : TMVA::MethodBase( jobName, methodTitle, theData, theOption, theTargetDir )
{
   // standard constructor
   //
   // MethodLikelihood options:
   // format and syntax of option string: "Spline2:0:25:D"
   //
   // where:
   //  SplineI [I=0,12,3,5] - which spline is used for smoothing the pdfs
   //                    0  - how often the input histos are smoothed
   //                    25 - average num of events per PDF bin 
   //                    D  - use square-root-matrix to decorrelate variable space 
   // 
   InitLik();

   // interpretation of configuration option string
   SetConfigName( TString("Method") + GetMethodName() );
   DeclareOptions();
   ParseOptions();
   ProcessOptions();

   fTransformLikelihoodOutput = kTRUE;
}

//_______________________________________________________________________
TMVA::MethodLikelihood::MethodLikelihood( DataSetInfo& theData, 
                                          const TString& theWeightFile,  
                                          TDirectory* theTargetDir )
   : TMVA::MethodBase( theData, theWeightFile, theTargetDir ) 
{  
   // construct likelihood references from file
   InitLik();

   DeclareOptions();
}

//_______________________________________________________________________
TMVA::MethodLikelihood::~MethodLikelihood( void )
{
   // destructor  
   if (NULL != fHistSig)        delete fHistSig;
   if (NULL != fHistBgd)        delete fHistBgd;
   if (NULL != fHistSig_smooth) delete fHistSig_smooth;
   if (NULL != fHistBgd_smooth) delete fHistBgd_smooth;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++){
      if ((*fPDFSig)[ivar] !=0) delete (*fPDFSig)[ivar];
      if ((*fPDFBgd)[ivar] !=0) delete (*fPDFBgd)[ivar];
   }
   if (NULL != fPDFSig)         delete fPDFSig;
   if (NULL != fPDFBgd)         delete fPDFBgd;

   delete[] fNsmoothVarS;
   delete[] fNsmoothVarB;

   delete[] fAverageEvtPerBinVarS;
   delete[] fAverageEvtPerBinVarB;

   delete[] fInterpolateString;
   delete[] fInterpolateMethod;
}

//_______________________________________________________________________
void TMVA::MethodLikelihood::InitLik( void )
{
   // default initialisation called by all constructors
   fHistBgd        = NULL; 
   fHistSig_smooth = NULL; 
   fHistBgd_smooth = NULL;
   fPDFSig         = NULL;
   fPDFBgd         = NULL;
  
   // no ranking test
   fDropVariable   = -1;

   SetMethodName( "Likelihood" );
   SetMethodType( TMVA::Types::kLikelihood );
   SetTestvarName();

   fEpsilon        = 1e-8;

   fHistSig        = new std::vector<TH1*>      ( GetNvar(), (TH1*)0 ); 
   fHistBgd        = new std::vector<TH1*>      ( GetNvar(), (TH1*)0 ); 
   fHistSig_smooth = new std::vector<TH1*>      ( GetNvar(), (TH1*)0 ); 
   fHistBgd_smooth = new std::vector<TH1*>      ( GetNvar(), (TH1*)0 );
   fPDFSig         = new std::vector<TMVA::PDF*>( GetNvar(), (TMVA::PDF*)0 );
   fPDFBgd         = new std::vector<TMVA::PDF*>( GetNvar(), (TMVA::PDF*)0 );
   fSpline         = -1;
}

//_______________________________________________________________________
void TMVA::MethodLikelihood::DeclareOptions() 
{
   // define the options (their key words) that can be set in the option string 
   // know options:
   // PDFInterpol[ivar] <string>   Spline0, Spline1, Spline2 <default>, Spline3, Spline5, KDE  used to interpolate reference histograms
   //             if no variable index is given, it is valid for ALL the variables
   //
   // NSmooth           <int>    how often the input histos are smoothed
   // NAvEvtPerBin      <int>    minimum average number of events per PDF bin 
   // TransformOutput   <bool>   transform (often strongly peaked) likelihood output through sigmoid inversion
   // fKDEtype          <KernelType>   type of the Kernel to use (1 is Gaussian)
   // fKDEiter          <KerneIter>    number of iterations (1 --> "static KDE", 2 --> "adaptive KDE")
   // fBorderMethod     <KernelBorder> the method to take care about "border" effects (1=no treatment , 2=kernel renormalization, 3=sample mirroring)
   

   // initialize 
   MethodBase::DeclareOptions();
   DeclareOptionRef( fNsmooth = 1, "NSmooth",
                     "Number of smoothing iterations for the input histograms");

   fNsmoothVarS = new Int_t[GetNvar()];
   fNsmoothVarB = new Int_t[GetNvar()];
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++)
      fNsmoothVarS[ivar] = fNsmoothVarB[ivar] = -1;
   DeclareOptionRef(fNsmoothVarS, GetNvar(), "NSmoothSig",
                    "Number of smoothing iterations for the input histograms");
   DeclareOptionRef(fNsmoothVarB, GetNvar(), "NSmoothBkg",
                    "Number of smoothing iterations for the input histograms");

   DeclareOptionRef( fAverageEvtPerBin = 50, "NAvEvtPerBin",
                     "Average number of events per PDF bin");

   fAverageEvtPerBinVarS = new Int_t[GetNvar()];
   fAverageEvtPerBinVarB = new Int_t[GetNvar()];
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++)
      fAverageEvtPerBinVarS[ivar] = fAverageEvtPerBinVarB[ivar] = -1;
   DeclareOptionRef( fAverageEvtPerBinVarS, GetNvar(), "NAvEvtPerBinSig",
                     "Average num of events per PDF bin and variable (signal)");   
   DeclareOptionRef( fAverageEvtPerBinVarB, GetNvar(), "NAvEvtPerBinBkg",
                     "Average num of events per PDF bin and variable (background)");



   //reading every PDF's definition and passing the option string to the next one to be read and marked
   fDefaultPDF = new PDF(GetOptions());
   for (UInt_t ivar = 0; ivar< DataInfo().GetNVariables(); ivar++)
   {
      (*fPDFBgd)[ivar] = 
         new PDF((ivar==0)?fDefaultPDF->GetOptions():(*fPDFSig)[ivar-1]->GetOptions(),
         Form("Bkg[%d]",ivar),fDefaultPDF);
      (*fPDFSig)[ivar] = 
         new PDF((*fPDFBgd)[ivar]->GetOptions(),
         Form("Sig[%d]",ivar),fDefaultPDF);
   }
   //the final marked option string is written back to the original likelihood
   SetOptions((fPDFSig->back())->GetOptions());




   DeclareOptionRef( fTransformLikelihoodOutput = kFALSE, "TransformOutput", 	 
                     "Transform likelihood output by inverse sigmoid function" );

   fInterpolateString = new TString[GetNvar()];
   fInterpolateMethod = new TMVA::PDF::EInterpolateMethod[GetNvar()];

   for (UInt_t i=0; i<GetNvar(); i++) fInterpolateString[i] = "Spline2";

   DeclareOptionRef(fInterpolateString, GetNvar(), "PDFInterpol", "Method of interpolating reference histograms (e.g. Spline2 or KDE)");
   AddPreDefVal(TString("Spline0")); // take histogram                    
   AddPreDefVal(TString("Spline1")); // linear interpolation between bins 
   AddPreDefVal(TString("Spline2")); // quadratic interpolation           
   AddPreDefVal(TString("Spline3")); // cubic interpolation               
   AddPreDefVal(TString("Spline5")); // fifth order polynome interpolation
   AddPreDefVal(TString("KDE"));     // use kernel density estimator

   DeclareOptionRef( fKDEtypeString = "Gauss",       "KDEtype", "KDE kernel type (1=Gauss)" );
   AddPreDefVal(TString("Gauss"));

   DeclareOptionRef( fKDEiterString = "Nonadaptive", "KDEiter", "Number of iterations (1=non-adaptive, 2=adaptive)" );
   AddPreDefVal(TString("Nonadaptive"));
   AddPreDefVal(TString("Adaptive"));

   DeclareOptionRef( fKDEfineFactor =1. , "KDEFineFactor", 
                     "Fine tuning factor for Adaptive KDE: Factor to multyply the width of the kernel");

   DeclareOptionRef( fBorderMethodString = "None", "KDEborder", 
                     "Border effects treatment (1=no treatment , 2=kernel renormalization, 3=sample mirroring)" );
   AddPreDefVal(TString("None"));
   AddPreDefVal(TString("Renorm"));
   AddPreDefVal(TString("Mirror"));
}

//_______________________________________________________________________
void TMVA::MethodLikelihood::ProcessOptions() 
{
   // process user options
   MethodBase::ProcessOptions();
  
   // test if to use kernel density estimation, if not fall back to splines
   // individual options

   // reference cut value to distingiush signal-like from background-like events   
   SetSignalReferenceCut( TransformLikelihoodOutput( 0.5, 0.5 ) );
}

//_______________________________________________________________________
void TMVA::MethodLikelihood::Train( void )
{
   // create reference distributions (PDFs) from signal and background events:
   // fill histograms and smooth them; if decorrelation is required, compute 
   // corresponding square-root matrices

   // create reference histograms

   // fine binned histos needed for the KDE smoothing
   std::vector<TH1*>* sigFineBinKDE = new std::vector<TH1*>( GetNvar() );
   std::vector<TH1*>* bgdFineBinKDE = new std::vector<TH1*>( GetNvar() );   
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) 
   {
      TString var = (*fInputVars)[ivar];

      Double_t xmin(0),    xmax(1);
      Int_t    nbinsS(10), nbinsB(10);
      // special treatment for discrete variables
      if (DataInfo().GetVarType(ivar) == 'I') {
         // special treatment for integer variables
         Int_t xmin = TMath::Nint( GetXmin(ivar) );
         Int_t xmax = TMath::Nint( GetXmax(ivar) + 1 );
         Int_t nbins = xmax - xmin;

         (*fHistSig)[ivar] = new TH1F( var + "_sig", var + " signal training",     nbins, xmin, xmax );
         (*fHistBgd)[ivar] = new TH1F( var + "_bgd", var + " background training", nbins, xmin, xmax );
      }
      else {
         UInt_t minNEvt = TMath::Min(Data()->GetNEvtSigTrain(),Data()->GetNEvtBkgdTrain());
         nbinsS = (*fPDFSig)[ivar]->GetHistNBins( minNEvt );
         nbinsB = (*fPDFBgd)[ivar]->GetHistNBins( minNEvt );
         xmin = GetXmin(ivar);
         xmax = GetXmax(ivar);
      }
      (*fHistSig)[ivar] = new TH1F( Form("%s_sig",var.Data()),
                                    Form("%s signal training",var.Data()),
                                    nbinsS, xmin, xmax );
      (*fHistBgd)[ivar] = new TH1F( Form("%s_bgd",var.Data()),
                                    Form("%s background training",var.Data()),
                                    nbinsB, xmin, xmax );
   }

   // ----- fill the reference histograms
   fLogger << kINFO << "Filling reference histograms" << Endl;
   // event loop
   for (Int_t ievt=0; ievt<Data()->GetNEvents(); ievt++) {

      // use the true-event-type's transformation
      const Event * ev = GetTransformationHandler().Transform(Data()->GetEvent(ievt), Types::kTrueType);

      // the event weight
      Float_t weight = ev->GetWeight();

      // fill variable vector
      for (UInt_t ivar=0; ivar<GetNvar(); ivar++) 
      {
         Float_t value  = ev->GetVal(ivar);
         if (ev->IsSignal()) (*fHistSig)[ivar]->Fill( value, weight );
         else (*fHistBgd)[ivar]->Fill( value, weight );
      }
   }
   //building the pdfs
   TH1*  htmp;
   fLogger << kINFO << "Building PDF out of reference histograms" << Endl;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++)
   {
      htmp = (TH1*)((*fHistSig)[ivar])->Clone( Form("%s_smooth",
              (*fHistSig)[ivar]->GetName()) );
      (*fPDFSig)[ivar]->BuildPDF    ( htmp );
      htmp = (TH1*)((*fHistBgd)[ivar])->Clone( Form("%s_smooth",
              (*fHistBgd)[ivar]->GetName()) );
      (*fPDFBgd)[ivar]->BuildPDF    ( htmp );
      (*fPDFSig)[ivar]->ValidatePDF ( (*fHistSig)[ivar] );
      (*fPDFBgd)[ivar]->ValidatePDF ( (*fHistBgd)[ivar] );
      //saving the smoothed histograms
      if ( (*fPDFSig)[ivar]->GetSmoothedHist() != 0)
         (*fHistSig_smooth)[ivar] = (*fPDFSig)[ivar]->GetSmoothedHist();
      if ( (*fPDFBgd)[ivar]->GetSmoothedHist() != 0)
         (*fHistBgd_smooth)[ivar] = (*fPDFBgd)[ivar]->GetSmoothedHist();
   }
   delete sigFineBinKDE;
   delete bgdFineBinKDE;
}

//_______________________________________________________________________
Double_t TMVA::MethodLikelihood::GetMvaValue()
{
   // returns the likelihood estimator for signal

   // fill a new Likelihood branch into the testTree
   UInt_t ivar;
    
   // retrieve variables, and transform, if required
   TVector vs( GetNvar() );
   TVector vb( GetNvar() );

   // need to distinguish signal and background in case of variable transformation
   // signal first

   const Event * ev = GetTransformationHandler().Transform(Data()->GetEvent(), Types::kSignal);
   for (ivar=0; ivar<GetNvar(); ivar++) vs(ivar) = ev->GetVal(ivar);

   ev = GetTransformationHandler().Transform(Data()->GetEvent(), Types::kBackground);
   for (ivar=0; ivar<GetNvar(); ivar++) vb(ivar) = ev->GetVal(ivar);
   
   // compute the likelihood (signal)
   Double_t ps(1), pb(1), p(0);
   for (ivar=0; ivar<GetNvar(); ivar++) {

      // drop one variable (this is ONLY used for internal variable ranking !)
      if (ivar == fDropVariable) continue;

      Double_t x[2] = { vs(ivar), vb(ivar) };
    
      for (UInt_t itype=0; itype < 2; itype++) {

         // verify limits
         if      (x[itype] > (*fPDFSig)[ivar]->GetXmax()) x[itype] = (*fPDFSig)[ivar]->GetXmax() - 1.0e-15;
         else if (x[itype] < (*fPDFSig)[ivar]->GetXmin()) x[itype] = (*fPDFSig)[ivar]->GetXmin();

         // find corresponding histogram from cached indices                 
         PDF* pdf = (itype == 0) ? (*fPDFSig)[ivar] : (*fPDFBgd)[ivar];
         if (pdf == 0) fLogger << kFATAL << "<GetMvaValue> Reference histograms don't exist" << Endl;
         TH1* hist = pdf->GetPDFHist();

         // interpolate linearly between adjacent bins
         // this is not useful for discrete variables
         Int_t bin = hist->FindBin(x[itype]);

         // **** POTENTIAL BUG: PREFORMANCE IS WORSE WHEN USING TRUE TYPE ***
         // ==> commented out at present
         if ((*fPDFSig)[ivar]->GetInterpolMethod() == TMVA::PDF::kSpline0 || DataInfo().GetVarType(ivar) == 'N') { 
            p = TMath::Max( hist->GetBinContent(bin), fEpsilon );
         }
         else { // splined PDF

            Int_t nextbin = bin;
            if ((x[itype] > hist->GetBinCenter(bin) && bin != hist->GetNbinsX()) || bin == 1) 
               nextbin++;
            else
               nextbin--;  

            
            Double_t dx   = hist->GetBinCenter(bin)  - hist->GetBinCenter(nextbin);
            Double_t dy   = hist->GetBinContent(bin) - hist->GetBinContent(nextbin);
            Double_t like = hist->GetBinContent(bin) + (x[itype] - hist->GetBinCenter(bin)) * dy/dx;

            p = TMath::Max( like, fEpsilon );
         }

         if (itype == 0) ps *= p;
         else            pb *= p;
      }            
   }     

   // the likelihood ratio (transform it ?)
   return TransformLikelihoodOutput( ps, pb );
}

//_______________________________________________________________________
Double_t TMVA::MethodLikelihood::TransformLikelihoodOutput( Double_t ps, Double_t pb ) const
{
   // returns transformed or non-transformed output
   if (ps + pb < fEpsilon) pb = fEpsilon;
   Double_t r = ps/(ps + pb);

   if (fTransformLikelihoodOutput) {
      // inverse Fermi function

      // sanity check
      if      (r <= 0.0) r = fEpsilon;
      else if (r >= 1.0) r = 1.0 - fEpsilon;

      Double_t tau = 15.0;
      r = - TMath::Log(1.0/r - 1.0)/tau;
   }

   return r;
}

//______________________________________________________________________
void TMVA::MethodLikelihood::WriteOptionsToStream( ostream& o, const TString& prefix ) const 
{
   Configurable::WriteOptionsToStream( o, prefix);
   //writing the options defined for the different pdfs
   if (fDefaultPDF != 0) 
   {
      o << prefix << endl << prefix << "#Default Likelihood PDF Options:" << endl << prefix << endl;
      fDefaultPDF->WriteOptionsToStream( o, prefix );
   }
   for (UInt_t ivar = 0; ivar < fPDFSig->size(); ivar++)
   {
      if ((*fPDFSig)[ivar] != 0) 
      {
         o << prefix << endl << prefix << Form("#Signal[%d] Likelihood PDF Options:",ivar) << endl << prefix << endl;
         (*fPDFSig)[ivar]->WriteOptionsToStream( o, prefix );
      }
      if ((*fPDFBgd)[ivar] != 0) 
      {
         o << prefix << endl << prefix << "#Background[%d] Likelihood PDF Options:" << endl << prefix << endl;
         (*fPDFBgd)[ivar]->WriteOptionsToStream( o, prefix );
      }
   }

}


//_______________________________________________________________________
const TMVA::Ranking* TMVA::MethodLikelihood::CreateRanking() 
{
   // computes ranking of input variables

   // create the ranking object
   if(fRanking) delete fRanking;
   fRanking = new Ranking( GetName(), "Delta Separation" );

   Double_t sepRef = -1, sep = -1;
   for (Int_t ivar=-1; ivar<(Int_t)GetNvar(); ivar++) {

      // this variable should not be used
      fDropVariable = ivar;
      
      TString nameS = Form( "rS_%i", ivar+1 );
      TString nameB = Form( "rB_%i", ivar+1 );
      TH1* rS = new TH1F( nameS, nameS, 80, 0, 1 );
      TH1* rB = new TH1F( nameB, nameB, 80, 0, 1 );

      // the event loop
      for (Int_t ievt=0; ievt<Data()->GetNTrainingEvents(); ievt++) {
         
         const Event * ev = GetTransformationHandler().Transform(Data()->GetEvent(ievt), Types::kTrueType);

         Double_t lk = this->GetMvaValue();
         if (ev->IsSignal()) rS->Fill( lk );
         else                rB->Fill( lk );
      }

      // compute separation
      sep = TMVA::gTools().GetSeparation( rS, rB );
      if (ivar == -1) sepRef = sep;
      sep = sepRef - sep;
      
      // don't need these histograms anymore
      delete rS;
      delete rB;

      if (ivar >= 0) fRanking->AddRank( *new Rank( DataInfo().GetInternalVarName(ivar), sep ) );
   }

   fDropVariable = -1;
   
   return fRanking;
}

//_______________________________________________________________________
void  TMVA::MethodLikelihood::WriteWeightsToStream( ostream& o ) const
{  
   // write weights to stream 

   if (TxtWeightsOnly()) {
      for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
         if ( (*fPDFSig)[ivar]==0 || (*fPDFBgd)[ivar]==0 )
            fLogger << kFATAL << "Reference histograms for variable " << ivar << " don't exist, can't write it to weight file" << Endl;
         o << *(*fPDFSig)[ivar];
         o << *(*fPDFBgd)[ivar];
      }
   } 
   else {
      TString rfname( GetWeightFileName() ); rfname.ReplaceAll( ".txt", ".root" );
      o << "# weights stored in root i/o file: " << rfname << endl;  
   }
}

//_______________________________________________________________________
void  TMVA::MethodLikelihood::WriteWeightsToStream( TFile& ) const
{
   // write reference PDFs to ROOT file
   TString pname = "PDF_";
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++){ 
      (*fPDFSig)[ivar]->Write( pname + GetInputVar( ivar ) + "_S" );
      (*fPDFBgd)[ivar]->Write( pname + GetInputVar( ivar ) + "_B" );
   }
}
  
//_______________________________________________________________________
void  TMVA::MethodLikelihood::ReadWeightsFromStream( istream & istr )
{
   // read weight info from file
   // nothing to do for this method
   TString pname = "PDF_";
   Bool_t addDirStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(0); // this avoids the binding of the hists in TMVA::PDF to the current ROOT file
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++){ 
      if ((*fPDFSig)[ivar] !=0) delete (*fPDFSig)[ivar];
      if ((*fPDFBgd)[ivar] !=0) delete (*fPDFBgd)[ivar];
      (*fPDFSig)[ivar] = new PDF();
      (*fPDFBgd)[ivar] = new PDF();
      (*fPDFSig)[ivar]->SetReadingVersion( GetTrainingTMVAVersionCode() );
      (*fPDFBgd)[ivar]->SetReadingVersion( GetTrainingTMVAVersionCode() );
      istr >> *(*fPDFSig)[ivar];
      istr >> *(*fPDFBgd)[ivar];
   }
   TH1::AddDirectory(addDirStatus);
}

//_______________________________________________________________________
void  TMVA::MethodLikelihood::ReadWeightsFromStream( TFile& rf )
{
   // read reference PDF from ROOT file
   TString pname = "PDF_";
   Bool_t addDirStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(0); // this avoids the binding of the hists in TMVA::PDF to the current ROOT file
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++){ 
      (*fPDFSig)[ivar] = (TMVA::PDF*)rf.Get( Form( "PDF_%s_S", GetInputVar( ivar ).Data() ) );
      (*fPDFBgd)[ivar] = (TMVA::PDF*)rf.Get( Form( "PDF_%s_B", GetInputVar( ivar ).Data() ) );
   }
   TH1::AddDirectory(addDirStatus);

}

//_______________________________________________________________________
void  TMVA::MethodLikelihood::WriteMonitoringHistosToFile( void ) const
{
   // write histograms and PDFs to file for monitoring purposes

   fLogger << kINFO << "write monitoring histograms to file: " << BaseDir()->GetPath() << Endl;
   BaseDir()->cd();
  
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) { 
      (*fHistSig)[ivar]->Write();
      (*fHistBgd)[ivar]->Write();
      if ((*fHistSig_smooth)[ivar] != 0)
         (*fHistSig_smooth)[ivar]->Write();
      if ((*fHistBgd_smooth)[ivar] != 0)
         (*fHistBgd_smooth)[ivar]->Write();
      (*fPDFSig)[ivar]->GetPDFHist()->Write();
      (*fPDFBgd)[ivar]->GetPDFHist()->Write();
      if ((*fPDFSig)[ivar]->GetNSmoothHist() != 0)
         (*fPDFSig)[ivar]->GetNSmoothHist()->Write();
      if ((*fPDFBgd)[ivar]->GetNSmoothHist() != 0)
         (*fPDFBgd)[ivar]->GetNSmoothHist()->Write();

      // add special plots to check the smoothing in the GetVal method
      Float_t xmin=((*fPDFSig)[ivar]->GetPDFHist()->GetXaxis())->GetXmin();
      Float_t xmax=((*fPDFSig)[ivar]->GetPDFHist()->GetXaxis())->GetXmax();
      TH1F* mm = new TH1F( (*fInputVars)[ivar]+"_additional_check",
                           (*fInputVars)[ivar]+"_additional_check", 15000, xmin, xmax );
      Double_t intBin = (xmax-xmin)/15000;
      for (Int_t bin=0; bin < 15000; bin++) {
         Double_t x = (bin + 0.5)*intBin + xmin;
         mm->SetBinContent(bin+1 ,(*fPDFSig)[ivar]->GetVal(x));
      }
      mm->Write();
   }
}

void TMVA::MethodLikelihood::MakeClassSpecificHeader( std::ostream& fout, const TString& ) const
{
   // write specific header of the classifier (mostly include files)
   fout << "#include <math.h>" << endl;
}

//_______________________________________________________________________
void TMVA::MethodLikelihood::MakeClassSpecific( std::ostream& fout, const TString& className ) const
{
   // write specific classifier response
   fout << "   double       fEpsilon;" << endl;

   Int_t * nbin = new Int_t[GetNvar()];

   Int_t nbinMax=-1;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      nbin[ivar]=(*fPDFSig)[ivar]->GetPDFHist()->GetNbinsX();
      if (nbin[ivar] > nbinMax) nbinMax=nbin[ivar];
   }
   
   fout << "   static float fRefS[][" << nbinMax << "]; " 
        << "// signal reference vector [nvars][max_nbins]" << endl;
   fout << "   static float fRefB[][" << nbinMax << "]; "
        << "// backgr reference vector [nvars][max_nbins]" << endl << endl;
   fout << "// if a variable has its PDF encoded as a spline0 --> treat it like an Integer valued one" <<endl;
   fout << "   bool    fHasDiscretPDF[" << GetNvar() <<"]; "<< endl;
   fout << "   int    fNbin[" << GetNvar() << "]; "
        << "// number of bins (discrete variables may have less bins)" << endl;
   fout << "   double TransformLikelihoodOutput( double, double ) const;" << endl;
   fout << "};" << endl;
   fout << "" << endl;
   fout << "inline void " << className << "::Initialize() " << endl;
   fout << "{" << endl;
   fout << "   fEpsilon = " << fEpsilon << ";" << endl;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      fout << "   fNbin[" << ivar << "] = " << (*fPDFSig)[ivar]->GetPDFHist()->GetNbinsX() << ";" << endl;
      // sanity check (for previous code lines)
      if ((((*fPDFSig)[ivar]->GetPDFHist()->GetNbinsX() != nbin[ivar] ||
            (*fPDFBgd)[ivar]->GetPDFHist()->GetNbinsX() != nbin[ivar]) 
           //           && Data().GetVarType(ivar) != 'I' 
          ) ||
          (*fPDFSig)[ivar]->GetPDFHist()->GetNbinsX() != (*fPDFBgd)[ivar]->GetPDFHist()->GetNbinsX()) {
         fLogger << kFATAL << "<MakeClassSpecific> Mismatch in binning of variable " 
                 << "\"" << GetOriginalVarName(ivar) << "\" of type: \'" << DataInfo().GetVarType(ivar)
                 << "\' : " 
                 << "nxS = " << (*fPDFSig)[ivar]->GetPDFHist()->GetNbinsX() << ", "
                 << "nxB = " << (*fPDFBgd)[ivar]->GetPDFHist()->GetNbinsX() 
                 << " while we expect " << nbin[ivar]
                 << Endl;
      }
   }
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++){ 
      if ((*fPDFSig)[ivar]->GetInterpolMethod() == TMVA::PDF::kSpline0) 
         fout << "   fHasDiscretPDF[" << ivar <<"] = true;  " << endl;
      else
         fout << "   fHasDiscretPDF[" << ivar <<"] = false; " << endl;
   }

   fout << "}" << endl << endl;

   fout << "inline double " << className 
        << "::GetMvaValue__( const std::vector<double>& inputValues ) const" << endl;
   fout << "{" << endl;
   fout << "   double ps(1), pb(1);" << endl;
   fout << "   std::vector<double> inputValuesSig = inputValues;" << endl;
   fout << "   std::vector<double> inputValuesBgd = inputValues;" << endl;
   if (GetTransformationHandler().GetTransformationList().GetSize() != 0) {
      fout << "   Transform(inputValuesSig,0);" << endl;
      fout << "   Transform(inputValuesBgd,1);" << endl;
   }
   fout << "   for (size_t ivar = 0; ivar < GetNvar(); ivar++) {" << endl;
   fout << endl;
   fout << "      // dummy at present... will be used for variable transforms" << endl;
   fout << "      double x[2] = { inputValuesSig[ivar], inputValuesBgd[ivar] };" << endl;
   fout << endl;    
   fout << "      for (int itype=0; itype < 2; itype++) {" << endl;
   fout << endl;
   fout << "         // interpolate linearly between adjacent bins" << endl;
   fout << "         // this is not useful for discrete variables (or forced Spline0)" << endl;
   fout << "         int bin = int((x[itype] - fVmin[ivar])/(fVmax[ivar] - fVmin[ivar])*fNbin[ivar]) + 0;" << endl;
   fout << endl;
   fout << "         // since the test data sample is in general different from the training sample" << endl;
   fout << "         // it can happen that the min/max of the training sample are trespassed --> correct this" << endl;
   fout << "         if      (bin < 0) {" << endl;
   fout << "            bin = 0;" << endl;
   fout << "            x[itype] = fVmin[ivar];" << endl;
   fout << "         }" << endl;
   fout << "         else if (bin >= fNbin[ivar]) {" << endl;
   fout << "            bin = fNbin[ivar]-1;" << endl;
   fout << "            x[itype] = fVmax[ivar];" << endl;
   fout << "         }" << endl;
   fout << endl;
   fout << "         // find corresponding histogram from cached indices" << endl;               
   fout << "         float ref = (itype == 0) ? fRefS[ivar][bin] : fRefB[ivar][bin];" << endl;
   fout << endl;
   fout << "         // sanity check" << endl;
   fout << "         if (ref < 0) {" << endl;
   fout << "            std::cout << \"Fatal error in " << className 
        << ": bin entry < 0 ==> abort\" << std::endl;" << endl;
   fout << "            exit(1);" << endl;
   fout << "         }" << endl;
   fout << endl;
   fout << "         double p = ref;" << endl;
   fout << endl;
   fout << "         if (GetType(ivar) != 'I' && !fHasDiscretPDF[ivar]) {" << endl;
   fout << "            float bincenter = (bin + 0.5)/fNbin[ivar]*(fVmax[ivar] - fVmin[ivar]) + fVmin[ivar];" << endl;
   fout << "            int nextbin = bin;" << endl;
   fout << "            if ((x[itype] > bincenter && bin != fNbin[ivar]-1) || bin == 0) " << endl;
   fout << "               nextbin++;" << endl;
   fout << "            else" << endl;
   fout << "               nextbin--;  " << endl;
   fout << endl;
   fout << "            double refnext      = (itype == 0) ? fRefS[ivar][nextbin] : fRefB[ivar][nextbin];" << endl;
   fout << "            float nextbincenter = (nextbin + 0.5)/fNbin[ivar]*(fVmax[ivar] - fVmin[ivar]) + fVmin[ivar];" << endl;
   fout << endl;
   fout << "            double dx = bincenter - nextbincenter;" << endl;
   fout << "            double dy = ref - refnext;" << endl;
   fout << "            p += (x[itype] - bincenter) * dy/dx;" << endl;
   fout << "         }" << endl;
   fout << endl;
   fout << "         if (p < fEpsilon) p = fEpsilon; // avoid zero response" << endl;
   fout << endl;
   fout << "         if (itype == 0) ps *= p;" << endl;
   fout << "         else            pb *= p;" << endl;
   fout << "      }            " << endl;
   fout << "   }     " << endl;
   fout << endl;
   fout << "   // the likelihood ratio (transform it ?)" << endl;
   fout << "   return TransformLikelihoodOutput( ps, pb );   " << endl;
   fout << "}" << endl << endl;

   fout << "inline double " << className << "::TransformLikelihoodOutput( double ps, double pb ) const" << endl;
   fout << "{" << endl;
   fout << "   // returns transformed or non-transformed output" << endl;
   fout << "   if (ps + pb < fEpsilon) pb = fEpsilon;" << endl;
   fout << "   double r = ps/(ps + pb);" << endl;
   fout << endl;
   fout << "   if (" << (fTransformLikelihoodOutput ? "true" : "false") << ") {" << endl;
   fout << "      // inverse Fermi function" << endl;
   fout << endl;
   fout << "      // sanity check" << endl;
   fout << "      if      (r <= 0.0) r = fEpsilon;" << endl;
   fout << "      else if (r >= 1.0) r = 1.0 - fEpsilon;" << endl;
   fout << endl;
   fout << "      double tau = 15.0;" << endl;
   fout << "      r = - log(1.0/r - 1.0)/tau;" << endl;
   fout << "   }" << endl;
   fout << endl;
   fout << "   return r;" << endl;
   fout << "}" << endl;
   fout << endl;

   fout << "// Clean up" << endl;
   fout << "inline void " << className << "::Clear() " << endl;
   fout << "{" << endl;
   fout << "   // nothing to clear" << endl;
   fout << "}" << endl << endl;

   fout << "// signal map" << endl;
   fout << "float " << className << "::fRefS[][" << nbinMax << "] = " << endl;
   fout << "{ " << endl;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      fout << "   { ";
      for (Int_t ibin=1; ibin<=nbinMax; ibin++) {
         if (ibin-1 < nbin[ivar])
            fout << (*fPDFSig)[ivar]->GetPDFHist()->GetBinContent(ibin);
         else
            fout << -1;

         if (ibin < nbinMax) fout << ", ";
      }
      fout << "   }, " << endl;
   }   
   fout << "}; " << endl;
   fout << endl;

   fout << "// background map" << endl;
   fout << "float " << className << "::fRefB[][" << nbinMax << "] = " << endl;
   fout << "{ " << endl;
   for (UInt_t ivar=0; ivar<GetNvar(); ivar++) {
      fout << "   { ";
      fout << std::setprecision(8);
      for (Int_t ibin=1; ibin<=nbinMax; ibin++) {
         if (ibin-1 < nbin[ivar])
            fout << (*fPDFBgd)[ivar]->GetPDFHist()->GetBinContent(ibin);
         else
            fout << -1;

         if (ibin < nbinMax) fout << ", ";
      }
      fout << "   }, " << endl;
   }   
   fout << "}; " << endl;
   fout << endl;

   delete[] nbin;
}

//_______________________________________________________________________
void TMVA::MethodLikelihood::GetHelpMessage() const
{
   // get help message text
   //
   // typical length of text line: 
   //         "|--------------------------------------------------------------|"
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Short description:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "The maximum-likelihood classifier models the data with probability " << Endl;
   fLogger << "density functions (PDF) reproducing the signal and background" << Endl;
   fLogger << "distributions of the input variables. Correlations among the " << Endl;
   fLogger << "variables are ignored." << Endl;
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Performance optimisation:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "Required for good performance are decorrelated input variables" << Endl;
   fLogger << "(PCA transformation via the option \"VarTransform=Decorrelate\"" << Endl;
   fLogger << "may be tried). Irreducible non-linear correlations may be reduced" << Endl;
   fLogger << "by precombining strongly correlated input variables, or by simply" << Endl;
   fLogger << "removing one of the variables." << Endl;
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Performance tuning via configuration options:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "High fidelity PDF estimates are mandatory, i.e., sufficient training " << Endl;
   fLogger << "statistics is required to populate the tails of the distributions" << Endl;
   fLogger << "It would be a surprise if the default Spline or KDE kernel parameters" << Endl;
   fLogger << "provide a satisfying fit to the data. The user is advised to properly" << Endl;
   fLogger << "tune the events per bin and smooth options in the spline cases" << Endl;
   fLogger << "individually per variable. If the KDE kernel is used, the adaptive" << Endl;
   fLogger << "Gaussian kernel may lead to artefacts, so please always also try" << Endl;
   fLogger << "the non-adaptive one." << Endl;
   fLogger << "" << Endl;
   fLogger << "All tuning parameters must be adjusted individually for each input" << Endl;
   fLogger << "variable!" << Endl;
}

