// @(#)root/tmva $Id: Event.cxx,v 1.9 2008/08/01 12:17:46 speckmayer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Event                                                                 *
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
 *      U. of Victoria, Canada                                                    * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://mva.sourceforge.net/license.txt)                                       *
 **********************************************************************************/

#include "TMVA/Event.h"
#include "TMVA/Tools.h"
#include <iostream>
#include <iomanip>
 
Int_t TMVA::Event::fgCount = 0;
std::vector<Float_t*>* TMVA::Event::fValuesDynamic = 0;

//____________________________________________________________
TMVA::Event::Event() 
   : fValues(),
     fType(1),
     fWeight(1.0),
     fBoostWeight(1.0),
     fDynamic(false)
{
   // copy constructor
   fgCount++; 
}

TMVA::Event::Event( const std::vector<Float_t>& ev,
                    Bool_t isSignal,
                    Float_t weight,
                    Float_t boostweight )
   : fValues(ev),
     fType(isSignal?1:0),
     fWeight(weight),
     fBoostWeight(boostweight),
     fDynamic(false)
{
   // constructor
   fgCount++;
}

TMVA::Event::Event( const std::vector<Float_t*>*& evdyn )
//   : fValues(evdyn->size()),
   : fValues(0),
     fType(0),
     fWeight(0),
     fBoostWeight(0),
     fDynamic(true)
{

   fValuesDynamic = (std::vector<Float_t*>*) evdyn;
   // constructor for single events
   fgCount++;
}

TMVA::Event::Event( UInt_t size,
                    Bool_t isSignal,
                    Float_t weight,
                    Float_t boostweight )
   : fValues(size),
     fType(isSignal?1:0),
     fWeight(weight),
     fBoostWeight(boostweight),
     fDynamic(false)
{
   // constructor
   fgCount++;
}

TMVA::Event::Event( const Event& event ) 
   : fValues(event.fValues),
     fType(event.fType),
     fWeight(event.fWeight),
     fBoostWeight(event.fBoostWeight),
     fDynamic(event.fDynamic)
{
   // copy constructor
   fgCount++; 
}

//____________________________________________________________
TMVA::Event::~Event() 
{
   // Event destructor
   fgCount--;
   if( fDynamic )
      TMVA::Event::ClearDynamicVariables();
}
 


//____________________________________________________________
void TMVA::Event::CopyVarValues( const Event& other )
{
   // copies only the variable values
   fValues      = other.fValues;
   fType        = other.fType;
   fWeight      = other.fWeight;
   fBoostWeight = other.fBoostWeight;
}

//____________________________________________________________
void TMVA::Event::SetVal(UInt_t ivar, Float_t val) 
{
   // set variable ivar to val
   if(fValues.size()<=ivar)
      fValues.resize(ivar+1);

   fValues[ivar] = val;
}

//____________________________________________________________
void TMVA::Event::Print(std::ostream& o) const
{
   // print method
//   if( fValuesDynamic == 0 || fValuesDynamic->size()==0 ){
   if( !fDynamic ){
      o << fValues.size() << " variables: ";
      for (UInt_t ivar=0; ivar<fValues.size(); ivar++)
         o << " " << std::setw(10) << GetVal(ivar);
      o << ", weight = " << GetWeight();
      o << std::setw(10) << (IsSignal()?" signal":" background");
      o << std::endl;
   }else{
      o << "|dynamic variables|" << std::endl;
   }
}

//_______________________________________________________________________
ostream& TMVA::operator<<(ostream& os, const TMVA::Event& event)
{ 
   // Outputs the data of an event
   
   event.Print(os);
   return os;
}

//_______________________________________________________________________
ostream& TMVA::operator<<(ostream& os, const TMVA::Event* event)
{
   // Outputs the data of an event
   return os << *event;
}
