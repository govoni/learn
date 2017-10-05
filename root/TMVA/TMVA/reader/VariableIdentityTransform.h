// @(#)root/tmva $Id: VariableIdentityTransform.h,v 1.3 2008/07/06 21:29:33 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : VariableIdentityTransform                                             *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Identity transform                                                        *
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

#ifndef ROOT_TMVA_VariableIdentityTransform
#define ROOT_TMVA_VariableIdentityTransform

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VariableIdentityTransform                                            //
//                                                                      //
// Linear interpolation class                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TMVA_VariableTransformBase
#include "TMVA/VariableTransformBase.h"
#endif

namespace TMVA {

   class VariableIdentityTransform : public VariableTransformBase {

   public:
  
      VariableIdentityTransform(const std::vector<TMVA::VariableInfo>&);
      virtual ~VariableIdentityTransform( void ) {}

      Bool_t PrepareTransformation( const std::vector<Event*>& );

      void WriteTransformationToStream ( std::ostream& ) const {}
      void ReadTransformationFromStream( std::istream& ) { SetCreated(); }

      virtual const Event* Transform(const Event* const, Types::ESBType type = Types::kMaxSBType) const;

      // writer of function code
      virtual void MakeFunction(std::ostream& fout, const TString& fncName, Int_t part, UInt_t trCounter);

      ClassDef(VariableIdentityTransform,0) // Variable transformation: identity
   };

} // namespace TMVA

#endif 


