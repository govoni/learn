// @(#)root/tmva $Id: VariableIdentityTransform.cxx,v 1.4 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableIdentityTransform                                             *
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


#include "TMVA/VariableIdentityTransform.h"
#include <iostream>

ClassImp(TMVA::VariableIdentityTransform)

//_______________________________________________________________________
TMVA::VariableIdentityTransform::VariableIdentityTransform(const std::vector<TMVA::VariableInfo>& varinfo)
   : VariableTransformBase( varinfo, Types::kNone, "Id" )
{
   // constructor
}

//_______________________________________________________________________
Bool_t TMVA::VariableIdentityTransform::PrepareTransformation( const std::vector<Event*>& events)
{
   // the identity does not need to be prepared, only calculate the normalization
   if (!IsEnabled() || IsCreated()) return kTRUE;

   SetNVariables(events[0]->GetNVariables());

   SetCreated( kTRUE );

   return kTRUE;
}

//_______________________________________________________________________
const TMVA::Event* TMVA::VariableIdentityTransform::Transform(const TMVA::Event* const ev, TMVA::Types::ESBType /*type*/) const {
   return ev;
}

//_______________________________________________________________________
void TMVA::VariableIdentityTransform::MakeFunction(std::ostream& fout, const TString& fncName, Int_t /*part*/, UInt_t trCounter) {
   fout << "inline void " << fncName << "::InitTransform_Identity_" << trCounter << "() {}" << std::endl;
   fout << std::endl;
   fout << "inline void " << fncName << "::Transform_Identity_" << trCounter << "(const std::vector<double> &, int) const {}" << std::endl;
}
