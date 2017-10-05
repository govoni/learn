// @(#)root/tmva $Id: VariableNormalizeTransform.cxx,v 1.1 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableNormalizeTransform                                               *
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

#ifndef ROOT_TMVA_VariableNormalizeTransform
#include "TMVA/VariableNormalizeTransform.h"
#endif
#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif

ClassImp(TMVA::VariableNormalizeTransform)

//_______________________________________________________________________
TMVA::VariableNormalizeTransform::VariableNormalizeTransform(const std::vector<TMVA::VariableInfo>& varinfo)
   : VariableTransformBase( varinfo, Types::kDecorrelated, "Norm" ),
   fTransformedEvent(0)
{ 
   // constructor

   UInt_t nvar = varinfo.size();
   for(UInt_t i=0; i<3; i++) {
      fMin[i] = new Float_t[nvar];
      fMax[i]  = new Float_t[nvar];
   }
}

//_______________________________________________________________________
Bool_t TMVA::VariableNormalizeTransform::PrepareTransformation( const std::vector<Event*>& events )
{
   if (!IsEnabled() || IsCreated()) return kTRUE;

   CalcNormalizationParams( events );

   SetCreated( kTRUE );

   return kTRUE;
}

//_______________________________________________________________________
const TMVA::Event* TMVA::VariableNormalizeTransform::Transform( const TMVA::Event* const ev, Types::ESBType type ) const
{
   // apply the decorrelation transformation
   if (!IsCreated())
      fLogger << kFATAL << "Transformation not yet created" 
              << Endl;

   if (!GetNVariables() != ev->GetNVariables() )
      fLogger << kFATAL << "Transformation defined for a different number of variables" 
              << Endl;


   UInt_t k = 2;
   if(type==Types::kSignal) k = 0;
   if(type==Types::kBackground) k = 1;

   const Int_t nvar = GetNVariables();

   if(fTransformedEvent==0) 
      fTransformedEvent = new Event(nvar);

   Float_t min,max;
   for (Int_t ivar=0; ivar<nvar; ivar++) {
      if(k<2) {
         min = fMin[k][ivar]; max = fMax[k][ivar];
      } else {
         min = TMath::Min(fMin[0][ivar],fMin[1][ivar]); max = TMath::Max(fMax[0][ivar],fMax[1][ivar]);
      }
      Float_t offset = fMin[k][ivar];
      Float_t scale  = 1/(fMax[k][ivar]-fMin[k][ivar]);

      Float_t valnorm = (ev->GetVal(ivar)-offset)*scale * 2 - 1;
      fTransformedEvent->SetVal(ivar,valnorm);
   }
   fTransformedEvent->SetType       ( ev->Type() );
   fTransformedEvent->SetWeight     ( ev->GetWeight() );
   fTransformedEvent->SetBoostWeight( ev->GetBoostWeight() );

   return fTransformedEvent;
}


//_______________________________________________________________________
void 
TMVA::VariableNormalizeTransform::CalcNormalizationParams( const std::vector<Event*>& events)
{
   // compute offset and scale from min and max
   if (events.size() <= 1) 
      fLogger << kFATAL << "Not enough events (found " << events.size() << ") to calculate the normalization" << Endl;
   

   UInt_t nvar = GetNVariables();

   for(UInt_t ivar=0; ivar<nvar; ivar++) {
      fMin[0][nvar] = fMin[1][nvar] =  1e15;
      fMax[0][nvar] = fMax[1][nvar] = -1e15;
   }

   std::vector<Event*>::const_iterator evIt = events.begin();
   for(;evIt!=events.end();evIt++) {
      for(UInt_t ivar=0; ivar<nvar; ivar++) {
         Float_t val = (*evIt)->GetVal(ivar);
         UInt_t k = (*evIt)->IsSignal()?0:1;
         if(fMin[k][nvar] > val) fMin[k][ivar] = val;
         if(fMax[k][nvar] < val) fMax[k][ivar] = val;
      }
   }

   return;
}

//_______________________________________________________________________
void TMVA::VariableNormalizeTransform::WriteTransformationToStream( std::ostream& o ) const
{
   // write the decorrelation matrix to the stream
   o << "# min max for all variables, first for signal, then for background" << std::endl;
   UInt_t nvar = GetNVariables();
   for (UInt_t k=0; k<2; k++) {
      o << (k==0?"signal":"background") << std::endl;
      for(UInt_t ivar=0; ivar<nvar; ivar++)
         o << setprecision(12) << setw(20) << fMin[k][ivar] << " " << setw(20) << fMax[k][ivar] << std::endl;
   }
   o << "##" << endl;
}

//_______________________________________________________________________
void TMVA::VariableNormalizeTransform::ReadTransformationFromStream( std::istream& istr )
{
   // Read the variable ranges from an input stream

   UInt_t nvar = GetNVariables();

   char buf[512];
   istr.getline(buf,512);
   TString strvar, dummy;
   while (!(buf[0]=='#'&& buf[1]=='#')) { // if line starts with ## return
      char* p = buf;
      while (*p==' ' || *p=='\t') p++; // 'remove' leading whitespace
      if (*p=='#' || *p=='\0') {
         istr.getline(buf,512);
         continue; // if comment or empty line, read the next line
      }
      std::stringstream sstr(buf);
      sstr >> strvar;
      UInt_t k = (strvar=="signal"?0:1);
      for(UInt_t ivar=0;ivar<nvar;ivar++) {
         sstr >> fMin[k][ivar] >> fMax[k][ivar];
      }
      istr.getline(buf,512); // reading the next line
   }
   SetCreated();
}

//_______________________________________________________________________
void TMVA::VariableNormalizeTransform::PrintTransformation( ostream& o ) 
{
   // prints the transformation ranges

   UInt_t nvar = GetNVariables();

   fLogger << kINFO << "Transformation for signal based on these ranges:" << Endl;
   for(UInt_t ivar=0;ivar<nvar;ivar++) {
      o << setw(20) << fMin[0][ivar] << setw(20) << fMax[0][ivar];
   }
   fLogger << kINFO << "Transformation for background based on these ranges:" << Endl;
   for(UInt_t ivar=0;ivar<nvar;ivar++) {
      o << setw(20) << fMin[1][ivar] << setw(20) << fMax[1][ivar];
   }
}

//_______________________________________________________________________
void TMVA::VariableNormalizeTransform::MakeFunction( std::ostream& fout, const TString& fcncName, Int_t part, UInt_t trCounter) 
{
   // creates a decorrelation function
   
   if (part==1) {
      fout << std::endl;
      fout << "   double fMin_"<<trCounter<<"["<<GetNVariables()<<"];" << std::endl;
      fout << "   double fMax_"<<trCounter<<"["<<GetNVariables()<<"];" << std::endl;
   }

   if (part==2) {
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::InitTransform_"<<trCounter<<"()" << std::endl;
      fout << "{" << std::endl;
      for (UInt_t ivar=0; ivar<GetNVariables(); ivar++) {
         fout << "   fMin_"<<trCounter<<"["<<ivar<<"] = " << std::setprecision(12)
              << TMath::Min(fMin[0][ivar],fMin[1][ivar]) << ";" << std::endl;
         fout << "   fMax_"<<trCounter<<"["<<ivar<<"] = " << std::setprecision(12)
              << TMath::Max(fMax[0][ivar],fMax[1][ivar]) << ";" << std::endl;
      }
      fout << "}" << std::endl;
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fcncName << "::Transform_"<<trCounter<<"( std::vector<double>& iv, int ) const" << std::endl;
      fout << "{" << std::endl;
      fout << "   for(int ivar=0;ivar<"<<GetNVariables()<<";ivar++) {" << std::endl;
      fout << "      double offset = fMin[ivar];" << std::endl;
      fout << "      double scale  = 1/(fMax[ivar]-fMin[ivar]);" << std::endl;
      fout << "      iv[ivar] = (iv[ivar]-offset)*scale * 2 - 1;" << std::endl;
      fout << "   }" << std::endl;
      fout << "}" << std::endl;
   }
}


