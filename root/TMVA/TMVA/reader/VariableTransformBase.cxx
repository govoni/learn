// @(#)root/tmva $Id: VariableTransformBase.cxx,v 1.6 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableTransformBase                                                 *
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
#include "TMath.h"
#include "TVectorD.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "TMVA/VariableTransformBase.h"
#include "TMVA/Ranking.h"
#include "TMVA/Config.h"
#include "TMVA/Tools.h"

ClassImp(TMVA::VariableTransformBase)

//_______________________________________________________________________
TMVA::VariableTransformBase::VariableTransformBase(const std::vector<TMVA::VariableInfo>& varinfo,
                                                   Types::EVariableTransform tf,
                                                   const TString& trfName)
   : TObject(),
     fVariableTransform(tf),
     fEnabled( kTRUE ),
     fCreated( kFALSE ),
     fNormalise( kFALSE ),
     fTransformName(trfName + "Transform"),
     fVariables(varinfo),
     fLogger( GetName(), kINFO )
{
   // standard constructor
}

//_______________________________________________________________________
TMVA::VariableTransformBase::~VariableTransformBase()
{
   // destructor
}

//_______________________________________________________________________
void TMVA::VariableTransformBase::CalcNorm( const std::vector<Event*>& events ) {
   // method to calculate minimum, maximum, mean, and RMS for all
   // variables used in the MVA

   if (!IsCreated()) return;

   const UInt_t nvar = GetNVariables();

   UInt_t nevts = events.size();

   TVectorD x2( nvar ); x2 *= 0;
   TVectorD x0( nvar ); x0 *= 0;   

   Double_t sumOfWeights = 0;
   for (UInt_t ievt=0; ievt<nevts; ievt++) {
      const Event* ev = events[ievt];

      Double_t weight = ev->GetWeight();
      sumOfWeights += weight;
      for (UInt_t ivar=0; ivar<nvar; ivar++) {
         Double_t x = ev->GetVal(ivar);
         if(ievt==0) {
            fVariables[ivar].SetMin(x);
            fVariables[ivar].SetMax(x);
         } else {
            UpdateNorm( ivar,  x );
         }
         x0(ivar) += x*weight;
         x2(ivar) += x*x*weight;
      }
   }

   // set Mean and RMS
   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      Double_t mean = x0(ivar)/sumOfWeights;
      fVariables[ivar].SetMean( mean ); 
      fVariables[ivar].SetRMS( TMath::Sqrt( x2(ivar)/sumOfWeights - mean*mean) );
   }

   fLogger << kVERBOSE << "Set minNorm/maxNorm for variables to: " << Endl;
   fLogger << setprecision(3);
   for (UInt_t ivar=0; ivar<GetNVariables(); ivar++)
      fLogger << "    " << fVariables[ivar].GetInternalVarName()
              << "\t: [" << fVariables[ivar].GetMin() << "\t, " << fVariables[ivar].GetMax() << "\t] " << Endl;
   fLogger << setprecision(5); // reset to better value       
}

//_______________________________________________________________________
void TMVA::VariableTransformBase::UpdateNorm ( Int_t ivar,  Double_t x ) 
{
   // update min and max of a given variable and a given transformation method
   if (x < fVariables[ivar].GetMin()) fVariables[ivar].SetMin(x);
   if (x > fVariables[ivar].GetMax()) fVariables[ivar].SetMax(x);
}



// TODO

// //_______________________________________________________________________
// void TMVA::VariableTransformBase::WriteVarsToStream( std::ostream& o, const TString& prefix ) const 
// {
//    // write the list of variables (name, min, max) for a given data
//    // transformation method to the stream
//    o << prefix << "NVar " << GetNVariables() << endl;
//    std::vector<VariableInfo>::const_iterator varIt = fVariables.begin();
//    for (; varIt!=fVariables.end(); varIt++) { o << prefix; varIt->WriteToStream(o); }
// }

// //_______________________________________________________________________
// void TMVA::VariableTransformBase::ReadVarsFromStream( std::istream& istr ) 
// {
//    // Read the variables (name, min, max) for a given data
//    // transformation method from the stream. In the stream we only
//    // expect the limits which will be set

//    TString dummy;
//    UInt_t readNVar;
//    istr >> dummy >> readNVar;

//    if (readNVar!=fVariables.size()) {
//       fLogger << kFATAL << "You declared "<< fVariables.size() << " variables in the Reader"
//               << " while there are " << readNVar << " variables declared in the file"
//               << Endl;
//    }

//    // we want to make sure all variables are read in the order they are defined
//    VariableInfo varInfo;
//    std::vector<VariableInfo>::iterator varIt = fVariables.begin();
//    int varIdx = 0;
//    for (; varIt!=fVariables.end(); varIt++, varIdx++) {
//       varInfo.ReadFromStream(istr);
//       if (varIt->GetExpression() == varInfo.GetExpression()) {
//          varInfo.SetExternalLink((*varIt).GetExternalLink());
//          (*varIt) = varInfo;
//       } 
//       else {
//          fLogger << kINFO << "The definition (or the order) of the variables found in the input file is"  << Endl;
//          fLogger << kINFO << "is not the same as the one declared in the Reader (which is necessary for" << Endl;
//          fLogger << kINFO << "the correct working of the classifier):" << Endl;
//          fLogger << kINFO << "   var #" << varIdx <<" declared in Reader: " << varIt->GetExpression() << Endl;
//          fLogger << kINFO << "   var #" << varIdx <<" declared in file  : " << varInfo.GetExpression() << Endl;
//          fLogger << kFATAL << "The expression declared to the Reader needs to be checked (name or order are wrong)" << Endl;
//       }
//    }
// }
