// @(#)root/tmva $Id: Types.cxx,v 1.27 2008/01/09 23:52:32 stelzer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Types                                                                 *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation                                                            *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      U. of Victoria, Canada                                                    * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://mva.sourceforge.net/license.txt)                                       *
 **********************************************************************************/

#include <map>

#include "TMVA/Types.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Singleton class for TMVA typedefs and enums                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

TMVA::Types* TMVA::Types::fgTypesPtr = 0;

//_______________________________________________________________________
TMVA::Types::Types()
   : fLogger( "Types" )
{
   // constructor
   // fill map that links method names and enums

   fStr2type["Variable"]        = Types::kVariable;
   fStr2type["Cuts"]            = Types::kCuts;
   fStr2type["Likelihood"]      = Types::kLikelihood;
   fStr2type["PDERS"]           = Types::kPDERS;
   fStr2type["KNN"]             = Types::kKNN;
   fStr2type["HMatrix"]         = Types::kHMatrix;
   fStr2type["Fisher"]          = Types::kFisher;
   fStr2type["MLP"]             = Types::kMLP;
   fStr2type["CFMlpANN"]        = Types::kCFMlpANN;
   fStr2type["TMlpANN"]         = Types::kTMlpANN;
   fStr2type["BDT"]             = Types::kBDT;
   fStr2type["RuleFit"]         = Types::kRuleFit;
   fStr2type["SVM"]             = Types::kSVM;
   fStr2type["FDA"]             = Types::kFDA;
   fStr2type["BayesClassifier"] = Types::kBayesClassifier;
   fStr2type["Committee"]       = Types::kCommittee;
   fStr2type["SeedDistance"]    = Types::kSeedDistance;
   fStr2type["Plugins"]         = Types::kPlugins;
}

//_______________________________________________________________________
TMVA::Types::EMVA TMVA::Types::GetMethodType( const TString& method ) const 
{ 
   // returns the method type (enum) for a given method (string)
   std::map<TString, EMVA>::const_iterator it = fStr2type.find( method );
   if (it == fStr2type.end()) {
      fLogger << kINFO << "unknown method " << method << Endl;
      return kMaxMethod; // Inserted to get rid of GCC warning...
   }
   else return it->second;
}
