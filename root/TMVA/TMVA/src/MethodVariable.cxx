// @(#)root/tmva $Id: MethodVariable.cxx,v 1.36 2008/04/19 12:24:01 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate Data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodVariable                                                        *
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
//                                                                      
// Wrapper class for a single variable "MVA"; this is required for      
// the evaluation of the single variable discrimination performance     
//_______________________________________________________________________

#include "TMVA/MethodVariable.h"
#include <algorithm>

ClassImp(TMVA::MethodVariable)

using std::endl;
 
//_______________________________________________________________________
TMVA::MethodVariable::MethodVariable( const TString& jobName, const TString& methodTitle, DataSet& theData, 
                                      const TString& theOption, TDirectory* theTargetDir )
   : TMVA::MethodBase( jobName, methodTitle, theData, theOption, theTargetDir )
{
   // standard constructor
   // option string contains variable name - but not only ! 
   // there is a "Var_" prefix, which is useful in the context of later root plotting
   // so, remove this part

   SetMethodName( "Variable" );
   SetMethodType( TMVA::Types::kVariable );
   SetTestvarPrefix( "" );
   SetTestvarName();

   fLogger << kVERBOSE << "uses as discriminating variable just "
           << GetOptions() << " as specified in the option" << Endl;
  
   if (0 == Data().GetTrainingTree()->FindBranch(GetOptions())) {
      Data().GetTrainingTree()->Print();
      fLogger << kFATAL << "variable " << GetOptions() << " not found in tree" << Endl;
   }
   else {
      SetMethodName ( GetMethodName() + (TString)"_" + GetOptions() );
      SetTestvarName( GetOptions() );
      fLogger << kVERBOSE << "sucessfully initialized variable as " << GetMethodName() << Endl;
   }
}

//_______________________________________________________________________
TMVA::MethodVariable::~MethodVariable( void )
{
   // destructor
}

//_______________________________________________________________________
void TMVA::MethodVariable::Train( void )
{
   // no training required

   // default sanity checks
   if (!CheckSanity()) fLogger << kFATAL << "<Train> sanity check failed" << Endl;
}

//_______________________________________________________________________
Double_t TMVA::MethodVariable::GetMvaValue()
{
   // "MVA" value is variable value
   return GetEventVal(0);
}

//_______________________________________________________________________
void  TMVA::MethodVariable::WriteWeightsToStream( ostream & o ) const
{  
   // Write configuration to output stream
   o << "";
}
  
//_______________________________________________________________________
void  TMVA::MethodVariable::ReadWeightsFromStream( istream & /*istr*/ )
{
   // Read configuration from input stream

   // nothing to be done for this method
}

//_______________________________________________________________________
void TMVA::MethodVariable::MakeClassSpecific( std::ostream& fout, const TString& className ) const
{
   // write specific classifier response
   fout << "   // not implemented for class: \"" << className << "\"" << endl;
   fout << "};" << endl;
}

//_______________________________________________________________________
void TMVA::MethodVariable::GetHelpMessage() const
{
   // get help message text
   //
   // typical length of text line: 
   //         "|--------------------------------------------------------------|"
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Short description:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "<None>" << Endl;
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Performance optimisation:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "<None>" << Endl;
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Performance tuning via configuration options:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "<None>" << Endl;
}
