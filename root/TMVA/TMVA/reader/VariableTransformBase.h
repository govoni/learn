// @(#)root/tmva $Id: VariableTransformBase.h,v 1.6 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableTransformBase                                                 *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Pre-transformation of input variables (base class)                        *
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
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#ifndef ROOT_TMVA_VariableTransformBase
#define ROOT_TMVA_VariableTransformBase

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VariableTransformBase                                                //
//                                                                      //
// Linear interpolation class                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TH1.h"
#include "TDirectory.h"

#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_VariableInfo
#include "TMVA/VariableInfo.h"
#endif
#ifndef ROOT_TMVA_DataSetInfo
#include "TMVA/DataSetInfo.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {

   class VariableTransformBase : public TObject {

   public:
  
      VariableTransformBase(const std::vector<TMVA::VariableInfo>&, Types::EVariableTransform tf, const TString& trfName );
      virtual ~VariableTransformBase( void );

      virtual Bool_t PrepareTransformation( const std::vector<Event*>&  ) = 0;

      virtual const Event* Transform(const Event* const, Types::ESBType type = Types::kMaxSBType) const = 0;


      //      void PlotVariables( TTree* theTree );

      // accessors
      void   SetEnabled  ( Bool_t e ) { fEnabled = e; }
      void   SetNormalise( Bool_t n ) { fNormalise = n; }
      Bool_t IsEnabled()    const { return fEnabled; }
      Bool_t IsCreated()    const { return fCreated; }
      Bool_t IsNormalised() const { return fNormalise; }

      void SetUseSignalTransform( Bool_t e=kTRUE) { fUseSignalTransform = e; }
      Bool_t UseSignalTransform() const { return fUseSignalTransform; }

      virtual const char* GetName() const { return fTransformName.Data(); }
      TString GetShortName() const { TString a(fTransformName); a.ReplaceAll("Transform",""); return a; }

      virtual void WriteTransformationToStream ( std::ostream& o ) const = 0;
      virtual void ReadTransformationFromStream( std::istream& istr ) = 0;


      Types::EVariableTransform GetVariableTransform() const { return fVariableTransform; }

      virtual void PrintTransformation(ostream &) {};

      // writer of function code
      virtual void MakeFunction(std::ostream& fout, const TString& fncName, Int_t part, UInt_t trCounter) = 0;

      const std::vector<TMVA::VariableInfo>& Variables() const { return fVariables; }

   protected:

      void CalcNorm( const std::vector<Event*>& );

      void SetCreated( Bool_t c = kTRUE ) { fCreated = c; }
      void SetNVariables( UInt_t i )      { fNVars = i; }
      void SetName( const TString& c )    { fTransformName = c; }

      UInt_t GetNVariables() const { return fVariables.size(); }

      std::vector<TMVA::VariableInfo>& Variables() { return fVariables; }
      
   private:

      Types::EVariableTransform fVariableTransform;  // Decorrelation, PCA, etc.

      void           UpdateNorm( Int_t ivar, Double_t x );

      Bool_t                           fUseSignalTransform; // true if transformation bases on signal data
      Bool_t                           fEnabled;            // has been enabled
      Bool_t                           fCreated;            // has been created
      Bool_t                           fNormalise;          // normalise input variables
      UInt_t                           fNVars;              // number of variables
      TString                          fTransformName;      // name of transformation
      std::vector<TMVA::VariableInfo>  fVariables; // event variables [saved to weight file]

   protected:

      mutable MsgLogger  fLogger;         // message logger

      ClassDef(VariableTransformBase,0)   //  Base class for variable transformations
   };

} // namespace TMVA

#endif 


