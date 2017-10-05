// @(#)root/tmva $Id: VariableDecorrTransform.h,v 1.3 2008/07/06 21:29:33 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableDecorrTransform                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Decorrelation of input variables                                          *
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

#ifndef ROOT_TMVA_VariableDecorrTransform
#define ROOT_TMVA_VariableDecorrTransform

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VariableDecorrTransform                                              //
//                                                                      //
// Linear interpolation class                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TMatrixD.h"

#ifndef ROOT_TMVA_VariableTransformBase
#include "TMVA/VariableTransformBase.h"
#endif

namespace TMVA {

   class VariableDecorrTransform : public VariableTransformBase {

   public:
  
      VariableDecorrTransform(const std::vector<TMVA::VariableInfo>&);
      virtual ~VariableDecorrTransform( void ) {}

      Bool_t PrepareTransformation( const std::vector<Event*>& );

      virtual const Event* Transform(const Event* const, Types::ESBType type = Types::kMaxSBType) const;

      void WriteTransformationToStream ( std::ostream& ) const;
      void ReadTransformationFromStream( std::istream& );

      virtual void PrintTransformation( ostream & o );

      // writer of function code
      virtual void MakeFunction( std::ostream& fout, const TString& fncName, Int_t part, UInt_t trCounter);

   private:

      mutable Event*     fTransformedEvent;

      TMatrixD*          fDecorrMatrix[2];     //! Decorrelation matrix [signal/background]

      void CalcSQRMats( const std::vector<Event*>& );
      TMatrixDBase* CalcCovarianceMatrix( const std::vector<Event*>&, Bool_t isSignal );

      ClassDef(VariableDecorrTransform,0) // Variable transformation: decorrelation
   };

} // namespace TMVA

#endif 


