// @(#)root/tmva $Id: Types.cxx,v 1.6 2007/07/29 20:52:20 stelzer Exp $   
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
#include <iostream>

#include "TMVA/Types.h"

TMVA::Types* TMVA::Types::fgTypesPtr = 0;

//_______________________________________________________________________
TMVA::Types::Types()
   : fLogger( "Types" )
{
   // constructor
}

//_______________________________________________________________________
bool TMVA::Types::AddTypeMapping( Types::EMVA method, const TString& methodname ) {
   std::map<TString, EMVA>::const_iterator it = fStr2type.find( methodname );
   if (it != fStr2type.end())
      fLogger << kFATAL << "can not add method " << methodname << " to the name->type map because it exists already" << Endl;
   fStr2type[methodname] = method;
   return kTRUE;
}

//_______________________________________________________________________
TMVA::Types::EMVA TMVA::Types::GetMethodType( const TString& method ) const 
{ 
   // returns the method type (enum) for a given method (string)
   std::map<TString, EMVA>::const_iterator it = fStr2type.find( method );
   if (it == fStr2type.end()) {
      fLogger << kFATAL << "unknown method in map: " << method << Endl;
      return kVariable; // Inserted to get rid of GCC warning...
   }
   else return it->second;
}


//_______________________________________________________________________
const TString TMVA::Types::GetMethodName( TMVA::Types::EMVA method ) const {
   std::map<TString, EMVA>::const_iterator it = fStr2type.begin();
   for(;it!=fStr2type.end(); it++) {
      if(it->second == method) return it->first;
   }
   fLogger << kFATAL << "unknown method index in map: " << method << Endl;
   return "";
}
