// @(#)root/tmva $Id: Types.h,v 1.8 2008/07/30 15:18:22 alexvoigt Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Types                                                                 *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      GLobal types (singleton class)                                            *
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

#ifndef ROOT_TMVA_Types
#define ROOT_TMVA_Types

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Types                                                                //
//                                                                      //
// Singleton class for Global types used by TMVA                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TString.h"

#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {

   class Types {
      
   public:
         
      // available MVA methods in TMVA
      enum EMVA {
         kVariable    = 0,
         kCuts           ,     
         kSeedDistance   ,     
         kLikelihood     ,
         kPDERS          ,
         kHMatrix        ,
         kFisher         ,
         kKNN            ,
         kCFMlpANN       ,
         kTMlpANN        , 
         kBDT            ,
         kDT             ,
         kRuleFit        ,
         kSVM            ,
         kMLP            ,
         kBayesClassifier,
         kFDA            ,
         kCommittee      ,
         kMaxMethod      ,
         kBoost          ,
         kPDEFoam
      };

      enum EVariableTransform {
         kNone = 0,
         kDecorrelated,
         kPCA,
         kGaussDecorr,
         kMaxVariableTransform
      };

      enum ESBType { 
         kSignal = 0,  // Never change this number - it is elsewhere assumed to be zero !
         kBackground, 
         kSBBoth, 
         kMaxSBType,
         kTrueType
      };

      enum ETreeType { kTraining = 0, kTesting, kMaxTreeType, kValidation, kTrainingOriginal };

      enum EBoostStage { kBoostProcBegin=0, kBeforeTraining, kBeforeBoosting, kAfterBoosting, kBoostValidation, kBoostProcEnd };

   public:

      static Types& Instance() { return fgTypesPtr ? *fgTypesPtr : *(fgTypesPtr = new Types()); }
      ~Types() {}

      Types::EMVA GetMethodType( const TString& method ) const;
      const TString GetMethodName( Types::EMVA method ) const;

      bool AddTypeMapping(Types::EMVA method, const TString& methodname);

   private:

      Types();      
      static Types* fgTypesPtr;
                  
   private:
         
      std::map<TString, TMVA::Types::EMVA> fStr2type; // types-to-text map
      mutable MsgLogger       fLogger;   // message logger
         
   };
}

#endif
