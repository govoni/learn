// @(#)root/tmva $Id: VariablePCATransform.cxx,v 1.4 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariablePCATransform                                                  *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#include "assert.h"

#include "Riostream.h"
#include "TVectorF.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDBase.h"

#include "TMVA/VariablePCATransform.h"
#include "TMVA/DataSet.h"
#include "TMVA/Tools.h"

ClassImp(TMVA::VariablePCATransform)

//_______________________________________________________________________
TMVA::VariablePCATransform::VariablePCATransform(const std::vector<TMVA::VariableInfo>& varinfo)
   : VariableTransformBase( varinfo, Types::kPCA, "PCA" ),
   fTransformedEvent(0)
{ 
   // constructor
   fPCA[0] = fPCA[1] = 0;

   for (Int_t i=0; i<2; i++) { fMeanValues[i] = 0; fEigenVectors[i] = 0; }
}

//_______________________________________________________________________
TMVA::VariablePCATransform::~VariablePCATransform() 
{
   // destructor
   for (Int_t i=0; i<2; i++) {
      if (fMeanValues[i]   != 0) delete fMeanValues[i];
      if (fEigenVectors[i] != 0) delete fEigenVectors[i];
   }
}

//_______________________________________________________________________
Bool_t TMVA::VariablePCATransform::PrepareTransformation( const std::vector<Event*>& events )
{
   // calculate the principal components using the ROOT class TPrincipal
   // and the normalization
   if (!IsEnabled() || IsCreated()) return kTRUE;

   SetNVariables(events[0]->GetNVariables());

   if (GetNVariables() > 200) { 
      fLogger << kINFO << "----------------------------------------------------------------------------" 
              << Endl;
      fLogger << kINFO 
              << ": More than 200 variables, will not calculate PCA!" << Endl;
      fLogger << kINFO << "----------------------------------------------------------------------------" 
              << Endl;
      return kFALSE;
   }   

   CalculatePrincipalComponents( events );

   SetCreated( kTRUE );

   return kTRUE;
}

//_______________________________________________________________________
const TMVA::Event* TMVA::VariablePCATransform::Transform(const Event* const ev, Types::ESBType type) const
{
   // apply the principal component analysis
   if (!IsCreated()) return 0;
   const Int_t nvar = ev->GetNVariables();

   // Perform PCA and put it into PCAed events tree
   std::vector<Double_t> rv = X2P( ev->GetValues(), type==Types::kSignal ? 0 : 1 );

   if(fTransformedEvent==0 || fTransformedEvent->GetNVariables()!=ev->GetNVariables()) {
      if(fTransformedEvent!=0) delete fTransformedEvent;
      fTransformedEvent = new Event();
   }
   for (Int_t ivar=0; ivar<nvar; ivar++) fTransformedEvent->SetVal(ivar, rv[ivar]);
   fTransformedEvent->SetType       ( ev->Type() );
   fTransformedEvent->SetWeight     ( ev->GetWeight() );
   fTransformedEvent->SetBoostWeight( ev->GetBoostWeight() );

   return fTransformedEvent;
}

//_______________________________________________________________________
void TMVA::VariablePCATransform::CalculatePrincipalComponents( const std::vector<Event*>& events )
{
   // calculate the principal components for the signal and the background data
   // it uses the MakePrincipal method of ROOT's TPrincipal class

   const Int_t nvar = GetNVariables();

   for (Int_t i=0; i<2; i++ ) {
      if (fPCA[i] != NULL) delete fPCA[i];
      fPCA[i] = new TPrincipal( nvar, "" ); // note: following code assumes that option "N" is NOT set !
   }
   // !! Not normalizing and not storing input data, for performance reasons. Should perhaps restore normalization.

   Long64_t ievt, entries = events.size();
   Double_t *dvec = new Double_t[nvar];

   for (ievt=0; ievt<entries; ievt++) {
      Event * ev = events[ievt];
      for (Int_t i = 0; i < nvar; i++) dvec[i] = (Double_t) ev->GetVal(i);
      fPCA[ev->IsSignal()?0:1]->AddRow( dvec );
   }

   for (Int_t i=0; i<2; i++ ) {
      fPCA[i]->MakePrincipals();

      // retrieve mean values, eigenvectors and sigmas
      fMeanValues[i]   = const_cast<TVectorD*>( fPCA[i]->GetMeanValues() );
      fEigenVectors[i] = const_cast<TMatrixD*>( fPCA[i]->GetEigenVectors() );
   }
   delete [] dvec;
}

//_______________________________________________________________________
std::vector<Double_t> TMVA::VariablePCATransform::X2P( const std::vector<Float_t>& x, Int_t index ) const
{
   // Calculate the principal components from the original data vector
   // x, and return it in p (function extracted from TPrincipal::X2P)
   // It's the users responsibility to make sure that both x and p are
   // of the right size (i.e., memory must be allocated for p).
   const Int_t nvar = x.size();
   std::vector<Double_t> p(nvar);
   // need this assert
   assert( index >= 0 && index < 2 );   

   for (Int_t i = 0; i < nvar; i++) {
      p[i] = 0;
      for (Int_t j = 0; j < nvar; j++) p[i] += (((Double_t)x[j]) - (*fMeanValues[index])(j)) * (*fEigenVectors[index])(j,i);
   }
   return p;
}

//_______________________________________________________________________
void TMVA::VariablePCATransform::WriteTransformationToStream( std::ostream& o ) const
{
   // write mean values to stream
   for (Int_t sbType=0; sbType<2; sbType++) {
      o << "# PCA mean values " << endl;
      const TVectorD* means = fMeanValues[sbType];
      o << (sbType==0 ? "signal" : "background") << " " << means->GetNrows() << endl;
      for (Int_t row = 0; row<means->GetNrows(); row++) {
         o << setprecision(12) << setw(20) << (*means)[row];
      }
      o << endl;
   }
   o << "##" << endl;

   // write eigenvectors to stream
   for (Int_t sbType=0; sbType<2; sbType++) {
      o << "# PCA eigenvectors " << endl;
      const TMatrixD* mat = fEigenVectors[sbType];
      o << (sbType==0 ? "signal" : "background") << " " << mat->GetNrows() << " x " << mat->GetNcols() << endl;
      for (Int_t row = 0; row<mat->GetNrows(); row++) {
         for (Int_t col = 0; col<mat->GetNcols(); col++) {
            o << setprecision(12) << setw(20) << (*mat)[row][col] << " ";
         }
         o << endl;
      }
   }
   o << "##" << endl;
}

//_______________________________________________________________________
void TMVA::VariablePCATransform::ReadTransformationFromStream( std::istream& istr )
{
   // Read mean values from input stream
   char buf[512];
   istr.getline(buf,512);
   TString strvar, dummy;
   Int_t nrows(0), ncols(0);

   while (!(buf[0]=='#'&& buf[1]=='#')) { // if line starts with ## return
      char* p = buf;
      while (*p==' ' || *p=='\t') p++; // 'remove' leading whitespace
      if (*p=='#' || *p=='\0') {
         istr.getline(buf,512);
         continue; // if comment or empty line, read the next line
      }
      std::stringstream sstr(buf);
      sstr >> strvar;
      if (strvar=="signal" || strvar=="background") {

         sstr >> nrows;
         Int_t sbType = (strvar=="signal" ? 0 : 1);
        
         if (fMeanValues[sbType] == 0) fMeanValues[sbType] = new TVectorD( nrows );
         else                          fMeanValues[sbType]->ResizeTo( nrows );

         // now read vector entries
         for (Int_t row = 0; row<nrows; row++) istr >> (*fMeanValues[sbType])(row);

      } // done reading vector

      istr.getline(buf,512); // reading the next line
   }

   // Read eigenvectors from input stream
   istr.getline(buf,512);
   while (!(buf[0]=='#'&& buf[1]=='#')) { // if line starts with ## return
      char* p = buf;
      while(*p==' ' || *p=='\t') p++; // 'remove' leading whitespace
      if (*p=='#' || *p=='\0') {
         istr.getline(buf,512);
         continue; // if comment or empty line, read the next line
      }
      std::stringstream sstr(buf);
      sstr >> strvar;
      if (strvar=="signal" || strvar=="background") {

         sstr >> nrows >> dummy >> ncols;
         Int_t sbType = (strvar=="signal" ? 0 : 1);

         if (fEigenVectors[sbType] == 0) fEigenVectors[sbType] = new TMatrixD( nrows, ncols );
         else                            fEigenVectors[sbType]->ResizeTo( nrows, ncols );

         // now read matrix entries
         for (Int_t row = 0; row<fEigenVectors[sbType]->GetNrows(); row++) {
            for (Int_t col = 0; col<fEigenVectors[sbType]->GetNcols(); col++) {
               istr >> (*fEigenVectors[sbType])[row][col];
            }
         }

      } // done reading matrix
      istr.getline(buf,512); // reading the next line
   }

   SetCreated();
}

//_______________________________________________________________________
void TMVA::VariablePCATransform::MakeFunction( std::ostream& fout, const TString& fcncName, Int_t part, UInt_t trCounter) 
{

   UInt_t nvar = fEigenVectors[0]->GetNrows();

   // creates a PCA transformation function
   if (part==1) {
      fout << std::endl;
      fout << "   void X2P_"<<trCounter<<"( const double*, double*, int ) const;" << std::endl;
      fout << "   double fMeanValues_"<<trCounter<<"[2]["
           << fMeanValues[0]->GetNrows()   << "];" << std::endl;   // mean values
      fout << "   double fEigenVectors_"<<trCounter<<"[2]["
           << fEigenVectors[0]->GetNrows() << "]["
           << fEigenVectors[0]->GetNcols() <<"];" << std::endl;   // eigenvectors
      fout << std::endl;
   }

   // sanity check
   if (fMeanValues[0]->GetNrows()   != fMeanValues[1]->GetNrows() ||
       fEigenVectors[0]->GetNrows() != fEigenVectors[1]->GetNrows() ||
       fEigenVectors[0]->GetNcols() != fEigenVectors[1]->GetNcols()) {
      fLogger << kFATAL << "<MakeFunction> Mismatch in vector/matrix dimensions" << Endl;
   }

   if (part==2) {

      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::X2P_"<<trCounter<<"( const double* x, double* p, int index ) const" << std::endl;
      fout << "{" << std::endl;
      fout << "   // Calculate the principal components from the original data vector" << std::endl;
      fout << "   // x, and return it in p (function extracted from TPrincipal::X2P)" << std::endl;
      fout << "   // It's the users responsibility to make sure that both x and p are" << std::endl;
      fout << "   // of the right size (i.e., memory must be allocated for p)." << std::endl;
      fout << "   const int nvar = " << nvar << ";" << std::endl;
      fout << std::endl;
      fout << "   for (int i = 0; i < nvar; i++) {" << std::endl;
      fout << "      p[i] = 0;" << std::endl;
      fout << "      for (int j = 0; j < nvar; j++) p[i] += (x[j] - fMeanValues_"<<trCounter<<"[index][j]) * fEigenVectors_"<<trCounter<<"[index][j][i];" << std::endl;
      fout << "   }" << std::endl;
      fout << "}" << std::endl;
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::InitTransform_"<<trCounter<<"()" << std::endl;
      fout << "{" << std::endl;

      // fill vector of mean values
      fout << "   // initialise vector of mean values" << std::endl;
      for (int index=0; index<2; index++) {
         for (int i=0; i<fMeanValues[index]->GetNrows(); i++) {
            fout << "   fMeanValues_"<<trCounter<<"["<<index<<"]["<<i<<"] = " << std::setprecision(12) 
                 << (*fMeanValues[index])(i) << ";" << std::endl;
         }
      }

      // fill matrix of eigenvectors
      fout << std::endl;
      fout << "   // initialise matrix of eigenvectors" << std::endl;
      for (int index=0; index<2; index++) {
         for (int i=0; i<fEigenVectors[index]->GetNrows(); i++) {
            for (int j=0; j<fEigenVectors[index]->GetNcols(); j++) {
               fout << "   fEigenVectors_"<<trCounter<<"["<<index<<"]["<<i<<"]["<<j<<"] = " << std::setprecision(12) 
                    << (*fEigenVectors[index])(i,j) << ";" << std::endl;
            }
         }
      }
      fout << "}" << std::endl;
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::Transform_"<<trCounter<<"( std::vector<double>& iv, int sigOrBgd ) const" << std::endl;
      fout << "{" << std::endl;      
      fout << "   const int nvar = " << nvar << ";" << std::endl;
      fout << "   double *dv = new double[nvar];" << std::endl;
      fout << "   double *rv = new double[nvar];" << std::endl;
      fout << "   for (int ivar=0; ivar<nvar; ivar++) dv[ivar] = iv[ivar];" << std::endl;
      fout << std::endl;
      fout << "   // Perform PCA and put it into PCAed events tree" << std::endl;
      fout << "   this->X2P( dv, rv, (sigOrBgd==0 ? 0 : 1 ) );" << std::endl;
      fout << "   for (int ivar=0; ivar<nvar; ivar++) iv[ivar] = rv[ivar];" << std::endl;
      fout << std::endl;
      fout << "   delete [] dv;" << std::endl;
      fout << "   delete [] rv;" << std::endl;
      fout << "}" << std::endl;
   }
}
