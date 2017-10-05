// @(#)root/tmva $Id: Event.h,v 1.12 2008/08/01 12:17:46 speckmayer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Event                                                                 *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Event container                                                           *
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

#ifndef ROOT_TMVA_Event
#define ROOT_TMVA_Event

#include <iosfwd>
#include <vector>

#include <iostream> // <==== put in only for debugging purposes

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif
#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif

namespace TMVA {

   class Event;

   std::ostream& operator<<( std::ostream& os, const Event& event );
   std::ostream& operator<<( std::ostream& os, const Event* event );

   class Event {

      friend std::ostream& operator<<( std::ostream& os, const Event& event );
      friend std::ostream& operator<<( std::ostream& os, const Event* event );

   public:

      Event();
      explicit Event( const std::vector<Float_t>&, Bool_t isSignal = kTRUE, Float_t weight = 1.0, Float_t boostweight = 1.0 );
      explicit Event( const std::vector<Float_t*>*& );
      explicit Event( UInt_t size, Bool_t isSignal = kTRUE, Float_t weight = 1.0, Float_t boostweight = 1.0 ); // should not be need later REMOVE
      Event( const Event& );
      ~Event();

      // accessors
      Bool_t  IsSignal()          const { return (fType==1); }
      Float_t GetWeight()         const { return fWeight*fBoostWeight; }
      Float_t GetOriginalWeight() const { return fWeight; }
      Float_t GetBoostWeight()    const { return fBoostWeight; }
      Int_t   GetType()           const { return fType; }  // better use IsSignal()
      Int_t   Type()              const { return fType; }  // backward compatible -> to be removed
      UInt_t  GetNVariables()     const { return fValues.size(); }
      UInt_t  GetNVars()          const { return fValues.size(); }  // backward compatible -> to be removed
      Float_t GetVal(UInt_t ivar) const { 
/*       std::cout << "TEST" << std::endl; */
/*       std::cout << "fValuesDynamic " << fValuesDynamic << std::endl; */
/*        if( fValuesDynamic ){  */
/*  	 std::cout << "size " << fValuesDynamic->size() << std::endl;  */
/*  	 std::cout << "ivar " << ivar << std::endl;  */
/*  	 std::cout << "add " << (*fValuesDynamic)[ivar] << std::endl;  */
/*  	 std::cout << "val " << (*((*fValuesDynamic)[ivar])) << std::endl;  */
/*        }  */
//	 return ( fValuesDynamic==0 || fValuesDynamic->size()==0 )?fValues[ivar]:(*(*fValuesDynamic)[ivar]); }
	 return ( fDynamic ?( *(*fValuesDynamic)[ivar] ) : fValues[ivar] ); }
      const std::vector<Float_t>& GetValues() const { return fValues; }


      void    ScaleWeight(Float_t s)    { fWeight*=s; }
      void    SetWeight(Float_t w)      { fWeight=w; }
      void    SetBoostWeight(Float_t w) { fBoostWeight=w; }
      void    ScaleBoostWeight(Float_t s) { fBoostWeight *= s; }
      void    SetType(Int_t t)          { fType=t; }
      void    SetType(Types::ESBType t) { fType=(t==Types::kSignal)?1:0; }
      void    SetVal(UInt_t ivar, Float_t val);
      void    SetValFloatNoCheck(UInt_t ivar, Float_t val) { fValues[ivar] = val; }

      static void ClearDynamicVariables() { 
	                                    if( fValuesDynamic != 0 ) {
//     	                                       std::cout << "CLEAR dynamic variables" << std::endl;
	                                       fValuesDynamic->clear();
					       delete fValuesDynamic;
                    	                       fValuesDynamic = 0;
	                                    }
                                          } 

      void    CopyVarValues( const Event& other );

      void    Print(std::ostream & o) const;

   private:

      std::vector<Float_t>   fValues;           // the event values
      static std::vector<Float_t*>* fValuesDynamic;           // the event values
      Int_t                  fType;            // signal or background type: signal=1, background=0
      Float_t                fWeight;          // event weight (product of global and individual weights)
      Float_t                fBoostWeight;     // internal weight to be set by boosting algorithm
      
      bool                   fDynamic;         // is set when the dynamic values are taken

      static Int_t           fgCount;       // count instances of Event

   };

}

#endif
