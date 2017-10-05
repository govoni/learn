// @(#)root/tmva $Id: VariableDecorrTransform.cxx,v 1.6 2008/08/01 12:19:17 speckmayer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableDecorrTransform                                               *
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

#include "Riostream.h"
#include "TVectorF.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDBase.h"

#ifndef ROOT_TMVA_VariableDecorrTransform
#include "TMVA/VariableDecorrTransform.h"
#endif
#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif

ClassImp(TMVA::VariableDecorrTransform)

//_______________________________________________________________________
TMVA::VariableDecorrTransform::VariableDecorrTransform(const std::vector<TMVA::VariableInfo>& varinfo)
   : VariableTransformBase( varinfo, Types::kDecorrelated, "Deco" ),
   fTransformedEvent(0)
{ 
   // constructor
   fDecorrMatrix[0] = fDecorrMatrix[1] = 0;
}


//_______________________________________________________________________
Bool_t TMVA::VariableDecorrTransform::PrepareTransformation( const std::vector<Event*>& events )
{
   // calculate the decorrelation matrix and the normalization
   if (!IsEnabled() || IsCreated()) return kTRUE;

   SetNVariables(events[0]->GetNVariables());

   if (GetNVariables() > 200) { 
      fLogger << kINFO << "----------------------------------------------------------------------------" 
              << Endl;
      fLogger << kINFO 
              << ": More than 200 variables, will not calculate decorrelation matrix "
              << "!" << Endl;
      fLogger << kINFO << "----------------------------------------------------------------------------" 
              << Endl;
      return kFALSE;
   }   

   CalcSQRMats( events );

   SetCreated( kTRUE );

   return kTRUE;
}

//_______________________________________________________________________
const TMVA::Event* TMVA::VariableDecorrTransform::Transform( const TMVA::Event* const ev, Types::ESBType type ) const
{
   // apply the decorrelation transformation
   if (!IsCreated())
      fLogger << kFATAL << "Transformation matrix not yet created" 
              << Endl;

   TMatrixD* m = type==Types::kSignal ? fDecorrMatrix[Types::kSignal] : fDecorrMatrix[Types::kBackground];
   if (m == 0)
      fLogger << kFATAL << "Transformation matrix for " << (Types::kSignal?"signal":"background") << " is not defined" 
              << Endl;
   
   // transformation to decorrelate the variables
   const Int_t nvar = GetNVariables();
   TVectorD vec( nvar );
   for (Int_t ivar=0; ivar<nvar; ivar++) vec(ivar) = ev->GetVal(ivar);

   // diagonalise variable vectors
   vec *= *m;

   if(fTransformedEvent==0 || fTransformedEvent->GetNVariables()!=ev->GetNVariables()) {
      if(fTransformedEvent!=0) { delete fTransformedEvent; fTransformedEvent = 0; }
      fTransformedEvent = new Event();
   }
   for (Int_t ivar=0; ivar<nvar; ivar++) fTransformedEvent->SetVal(ivar,vec(ivar));
   fTransformedEvent->SetType       ( ev->Type() );
   fTransformedEvent->SetWeight     ( ev->GetWeight() );
   fTransformedEvent->SetBoostWeight( ev->GetBoostWeight() );

   return fTransformedEvent;
}

//_______________________________________________________________________
void TMVA::VariableDecorrTransform::CalcSQRMats( const std::vector<Event*>& events )
{
   // compute square-root matrices for signal and background
   for (UInt_t i=0; i<2; i++) {
      if (0 != fDecorrMatrix[i] ) { delete fDecorrMatrix[i]; }
      
      TMatrixDSym* covMat = (TMatrixDSym*)CalcCovarianceMatrix( events, (i==0) );

      fDecorrMatrix[i] = gTools().GetSQRootMatrix( covMat );
      if (fDecorrMatrix[i] == 0) 
         fLogger << kFATAL << "<GetSQRMats> Zero pointer returned for SQR matrix" << Endl;
   }
}

//_______________________________________________________________________
TMatrixDBase* TMVA::VariableDecorrTransform::CalcCovarianceMatrix( const std::vector<Event*>& events, Bool_t isSignal)
{
   // compute covariance matrix

   UInt_t nvar = GetNVariables(), ivar = 0, jvar = 0;

   // init matrices
   TVectorD vec(nvar);
   TMatrixD mat2(nvar, nvar);      
   for (ivar=0; ivar<nvar; ivar++) {
      vec(ivar) = 0;
      for (jvar=0; jvar<nvar; jvar++) {
         mat2(ivar, jvar) = 0;
      }
   }

   // perform event loop
   Int_t ic = 0;
   for (UInt_t i=0; i<events.size(); i++) {

      // fill the event
      Event * ev = events[i];

      if (ev->IsSignal() == isSignal) {
         ic++; // count used events
         for (ivar=0; ivar<nvar; ivar++) {

            Double_t xi = ev->GetVal(ivar);
            vec(ivar) += xi;
            mat2(ivar, ivar) += (xi*xi);

            for (jvar=ivar+1; jvar<nvar; jvar++) {
               Double_t xj = ev->GetVal(jvar);
               mat2(ivar, jvar) += (xi*xj);
               mat2(jvar, ivar) = mat2(ivar, jvar); // symmetric matrix
            }
         }
      }
   }

   // variance-covariance
   TMatrixDSym* mat = new TMatrixDSym(nvar);
   Double_t n = (Double_t)ic;
   for (ivar=0; ivar<nvar; ivar++) {
      for (jvar=0; jvar<nvar; jvar++) {
         (*mat)(ivar, jvar) = mat2(ivar, jvar)/n - vec(ivar)*vec(jvar)/(n*n);
      }
   }

   return mat;
}

//_______________________________________________________________________
void TMVA::VariableDecorrTransform::WriteTransformationToStream( std::ostream& o ) const
{
   // write the decorrelation matrix to the stream
   for (Int_t matType=0; matType<2; matType++) {
      o << "# correlation matrix " << endl;
      TMatrixD* mat = fDecorrMatrix[matType];
      o << (matType==0?"signal":"background") << " " << mat->GetNrows() << " x " << mat->GetNcols() << endl;
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
void TMVA::VariableDecorrTransform::ReadTransformationFromStream( std::istream& istr )
{
   // Read the decorellation matrix from an input stream

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
         sstr >> nrows >> dummy >> ncols;
         Int_t matType = (strvar=="signal"?0:1);
         if (fDecorrMatrix[matType] != 0) delete fDecorrMatrix[matType];
         TMatrixD* mat = fDecorrMatrix[matType] = new TMatrixD(nrows,ncols);
         // now read all matrix parameters
         for (Int_t row = 0; row<mat->GetNrows(); row++) {
            for (Int_t col = 0; col<mat->GetNcols(); col++) {
               istr >> (*mat)[row][col];
            }
         }
      } // done reading a matrix
      istr.getline(buf,512); // reading the next line
   }
   SetCreated();
}

//_______________________________________________________________________
void TMVA::VariableDecorrTransform::PrintTransformation( ostream& ) 
{
   // prints the transformation matrix
   fLogger << kINFO << "Transformation matrix signal:" << Endl;
   fDecorrMatrix[0]->Print();
   fLogger << kINFO << "Transformation matrix background:" << Endl;
   fDecorrMatrix[1]->Print();
}

//_______________________________________________________________________
void TMVA::VariableDecorrTransform::MakeFunction( std::ostream& fout, const TString& fcncName, Int_t part, UInt_t trCounter) 
{
   // creates a decorrelation function
   TMatrixD* mat = fDecorrMatrix[0];
   if (part==1) {
      fout << std::endl;
      fout << "   double fSigDecTF_"<<trCounter<<"["<<mat->GetNrows()<<"]["<<mat->GetNcols()<<"];" << std::endl;
      fout << "   double fBgdDecTF_"<<trCounter<<"["<<mat->GetNrows()<<"]["<<mat->GetNcols()<<"];" << std::endl;
   }

   if (part==2) {
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::InitTransform_"<<trCounter<<"()" << std::endl;
      fout << "{" << std::endl;
      TMatrixD* mat = fDecorrMatrix[0];
      for (int i=0; i<mat->GetNrows(); i++) {
         for (int j=0; j<mat->GetNcols(); j++) {
            fout << "   fSigDecTF_"<<trCounter<<"["<<i<<"]["<<j<<"] = " << std::setprecision(12) << (*mat)[i][j] << ";" << std::endl;
            fout << "   fBgdDecTF_"<<trCounter<<"["<<i<<"]["<<j<<"] = " << std::setprecision(12) << (*fDecorrMatrix[1])[i][j] << ";" << std::endl;
         }
      }
      fout << "}" << std::endl;
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::Transform_"<<trCounter<<"( std::vector<double>& iv, int sigOrBgd ) const" << std::endl;
      fout << "{" << std::endl;
      fout << "   std::vector<double> tv;" << std::endl;
      fout << "   for (int i=0; i<"<<mat->GetNrows()<<";i++) {" << std::endl;
      fout << "      double v = 0;" << std::endl;
      fout << "      for (int j=0; j<"<<mat->GetNcols()<<"; j++)" << std::endl;
      fout << "         v += iv[j] * (sigOrBgd==0 ? fSigDecTF_"<<trCounter<<"[i][j] : fBgdDecTF_"<<trCounter<<"[i][j]);" << std::endl;
      fout << "      tv.push_back(v);" << std::endl;
      fout << "   }" << std::endl;
      fout << "   for (int i=0; i<"<<mat->GetNrows()<<";i++) iv[i] = tv[i];" << std::endl;
      fout << "}" << std::endl;
   }
}
