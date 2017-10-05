// @(#)root/tmva $Id: MethodBayesClassifier.cxx,v 1.6 2008/06/22 22:00:38 stelzer Exp $    
// Author: Marcin .... 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodBayesClassifier                                                 *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header file for description)                          *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Abhishek Narain, <narainabhi@gmail.com> - University of Houston           *
 *                                                                                *
 * Copyright (c) 2005-2006:                                                       *
 *      University of Houston,                                                    *
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
// ... description of bayesian classifiers ...
//_______________________________________________________________________

#include "TMVA/ClassifierFactory.h"
#include "TMVA/MethodBayesClassifier.h"
#include "TMVA/Tools.h"
#include "Riostream.h"

REGISTER_METHOD(BayesClassifier)

ClassImp(TMVA::MethodBayesClassifier)

//_______________________________________________________________________
TMVA::MethodBayesClassifier::MethodBayesClassifier( TString jobName, TString methodTitle, DataSetInfo& theData, 
                                                    TString theOption, TDirectory* theTargetDir )
   : TMVA::MethodBase( jobName, methodTitle, theData, theOption, theTargetDir )
{
   // standard constructor
   InitBayesClassifier();
}

//_______________________________________________________________________
TMVA::MethodBayesClassifier::MethodBayesClassifier( DataSetInfo& theData, 
                                                    TString theWeightFile,  
                                                    TDirectory* theTargetDir )
   : TMVA::MethodBase( theData, theWeightFile, theTargetDir ) 
{
   // constructor from weight file
   InitBayesClassifier();
}

//_______________________________________________________________________
void TMVA::MethodBayesClassifier::InitBayesClassifier( void )
{
   // default initialisation
   SetMethodName( "BayesClassifier" );
   SetMethodType( TMVA::Types::kBayesClassifier );
   SetTestvarName();
}

//_______________________________________________________________________
void TMVA::MethodBayesClassifier::DeclareOptions() 
{
   // define the options (their key words) that can be set in the option string 
}

//_______________________________________________________________________
void TMVA::MethodBayesClassifier::ProcessOptions() 
{
   // the option string is decoded, for availabel options see "DeclareOptions"
   MethodBase::ProcessOptions();
}

//_______________________________________________________________________
TMVA::MethodBayesClassifier::~MethodBayesClassifier( void )
{
   // destructor
}

//_______________________________________________________________________
void TMVA::MethodBayesClassifier::Train( void )
{
   // some training 
}

//_______________________________________________________________________
void  TMVA::MethodBayesClassifier::WriteWeightsToStream( ostream & o ) const
{  
   // write the weight from the training to a file (stream)
   o << "whatever" << endl;
}
  
//_______________________________________________________________________
void  TMVA::MethodBayesClassifier::ReadWeightsFromStream( istream & )
{
   // read back the training results from a file (stream)
}

//_______________________________________________________________________
Double_t TMVA::MethodBayesClassifier::GetMvaValue()
{
   // returns MVA value for given event
   Double_t myMVA = 0;

   return myMVA;
}

//_______________________________________________________________________
void TMVA::MethodBayesClassifier::MakeClassSpecific( std::ostream& fout, const TString& className ) const
{
   // write specific classifier response
   fout << "   // not implemented for class: \"" << className << "\"" << endl;
   fout << "};" << endl;
}

//_______________________________________________________________________
void TMVA::MethodBayesClassifier::GetHelpMessage() const
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
