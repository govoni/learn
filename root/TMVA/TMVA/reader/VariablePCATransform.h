// @(#)root/tmva $Id: VariablePCATransform.h,v 1.3 2008/07/06 21:29:33 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariablePCATransform                                                  *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Principal value composition of input variables                            *
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

#ifndef ROOT_TMVA_VariablePCATransform
#define ROOT_TMVA_VariablePCATransform

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VariablePCATransform                                                 //
//                                                                      //
// Linear interpolation class                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPrincipal.h"

#ifndef ROOT_TMVA_VariableTransformBase
#include "TMVA/VariableTransformBase.h"
#endif

namespace TMVA {

   class VariablePCATransform : public VariableTransformBase {

   public:
  
      VariablePCATransform(const std::vector<TMVA::VariableInfo>&);
      virtual ~VariablePCATransform( void );

      Bool_t PrepareTransformation( const std::vector<Event*>& );

      virtual const Event* Transform(const Event* const, Types::ESBType type = Types::kMaxSBType) const;

      void WriteTransformationToStream ( std::ostream& ) const;
      void ReadTransformationFromStream( std::istream& );

      // writer of function code
      virtual void MakeFunction( std::ostream& fout, const TString& fncName, Int_t part, UInt_t trCounter);

   private:

      void CalculatePrincipalComponents( const std::vector<Event*>& );
      std::vector<Double_t> X2P( const std::vector<Float_t>&, Int_t ) const;

      mutable Event*     fTransformedEvent;

      TPrincipal* fPCA[2];        //! PCA [signal/background]
      
      // store relevant parts of PCA locally
      TVectorD* fMeanValues[2];   // mean values
      TMatrixD* fEigenVectors[2]; // eigenvectors

      ClassDef(VariablePCATransform,0) // Variable transformation: Principal Value Composition
   };

} // namespace TMVA

#endif 


