// @(#)Root/tmva $Id: ClassifierFactory.cxx,v 1.7 2008/07/20 20:44:28 stelzer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Factory                                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Joerg Stelzer   <stelzer@cern.ch>        - DESY, Germany                  *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      DESY, Germany                                                             * 
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_______________________________________________________________________
//                                                                      
// This is the MVA factory
//_______________________________________________________________________


#include "TMVA/ClassifierFactory.h"
#include <assert.h>
#include <iostream>

///
/// Initialize static singleton pointer
///
TMVA::ClassifierFactory* TMVA::ClassifierFactory::fgInstance = 0;

//_______________________________________________________________________
TMVA::ClassifierFactory& TMVA::ClassifierFactory::Instance()
{
   if(!fgInstance)
   {
      fgInstance = new TMVA::ClassifierFactory();
   }

  return *fgInstance;
}

//_______________________________________________________________________
bool TMVA::ClassifierFactory::Register(const std::string &name, Creator creator) 
{ 
   if(fCalls.find(name) != fCalls.end())
   {
      std::cerr << "ClassifierFactory<>::Register - " << name << " already exists" << std::endl;
      return false;
   }

   return fCalls.insert(CallMap::value_type(name, creator)).second;
}

//_______________________________________________________________________
bool TMVA::ClassifierFactory::Unregister(const std::string &name) 
{ 
    return fCalls.erase(name) == 1; 
}

//_______________________________________________________________________
TMVA::IMethod* TMVA::ClassifierFactory::Create(const std::string &name,
                                               const TString& job,
                                               const TString& title,
                                               DataSetInfo& dsi,
                                               const TString& option) 
{
   CallMap::const_iterator it = fCalls.find(name);
   
   // handle unknown algorithm request
   if(it == fCalls.end()) {
      std::cerr << "ClassifierFactory<>::Create - don't know anything about " << name << std::endl;
      assert(0);
   }
   
   return (it->second)(job, title, dsi, option);
}

//_______________________________________________________________________
TMVA::IMethod* TMVA::ClassifierFactory::Create(const std::string &name,
                                               DataSetInfo& dsi,
                                               const TString& weightfile)
{
   CallMap::const_iterator it = fCalls.find(name);
   
   // handle unknown algorithm request
   if(it == fCalls.end()) {
      std::cerr << "ClassifierFactory<>::Create - don't know anything about " << name << std::endl;
      assert(0);
   }
   
   return (it->second)("", "", dsi, weightfile);
}

//_______________________________________________________________________
const std::vector<std::string> TMVA::ClassifierFactory::List() const
{
   std::vector<std::string> svec;

   CallMap::const_iterator it = fCalls.begin();
   for(; it != fCalls.end(); ++it)
   {
      svec.push_back(it -> first);
   }

   return svec;
}

//_______________________________________________________________________
void TMVA::ClassifierFactory::Print() const
{
   std::cout << "Print: ClassifierFactory<> knows about " << fCalls.size() << " objects" << std::endl;  

   CallMap::const_iterator it = fCalls.begin();
   for(; it != fCalls.end(); ++it)
   {
      std::cout << "Registerted object name " << it -> first << std::endl;
   }
}
