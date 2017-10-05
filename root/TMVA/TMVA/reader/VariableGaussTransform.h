// @(#)root/tmva $Id: VariableGaussTransform.h,v 1.1 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableGaussTransform                                               *
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

#ifndef ROOT_TMVA_VariableGaussTransform
#define ROOT_TMVA_VariableGaussTransform

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VariableGaussTransform                                                  //
//                                                                      //
// Gaussian Decorrelation transformation of input variables             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TMatrixD.h"

#ifndef ROOT_TMVA_VariableTransformBase
#include "TMVA/VariableTransformBase.h"
#endif

namespace TMVA {

   class VariableGaussTransform : public VariableTransformBase {

   public:
  
      VariableGaussTransform(const std::vector<TMVA::VariableInfo>&);
      virtual ~VariableGaussTransform( void );

      Bool_t PrepareTransformation( const std::vector<Event*>& );

      virtual const Event* Transform(const Event* const, Types::ESBType type = Types::kMaxSBType) const;

      void WriteTransformationToStream ( std::ostream& ) const;
      void ReadTransformationFromStream( std::istream& );

      virtual void PrintTransformation( ostream & o );

      // provides string vector describing explicit transformation
      std::vector<TString>* GetTransformationStrings( Types::ESBType type ) const;

      // writer of function code
      virtual void MakeFunction( std::ostream& fout, const TString& fncName, Int_t part, UInt_t trCounter);

   private:
      Bool_t  fApplyGaussTransform;
      Bool_t  fApplyDecorrTransform;
      Bool_t  fFlatNotGaussD;

      mutable Event*     fTransformedEvent;

      std::vector< std::vector< TH1F* > >  fCumulativeDist; //! The Cummulative distributions 
      
      TMatrixD* fDecorrMatrix[2];     //! Decorrelation matrix [signal/background]

      void GetCumulativeDist  ( const std::vector<Event*>& );
      void GetSQRMats         ( const std::vector<Event*>& );
      void GetCovarianceMatrix( const std::vector<Event*>&, Bool_t isSignal, TMatrixDBase* mat );

      ClassDef(VariableGaussTransform,0) // Variable transformation: decorrelation
   };

} // namespace TMVA

#endif 


