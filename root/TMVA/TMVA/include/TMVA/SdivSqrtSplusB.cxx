// @(#)root/tmva $Id: SdivSqrtSplusB.cxx,v 1.16 2008/03/30 21:08:46 helgevoss Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : SdivSqrtSplusB                                                        *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description: Implementation of the SdivSqrtSplusB as separation criterion      *
 *              s / sqrt( s+b )                                                   * 
 *                                                                                *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Xavier Prudent  <prudent@lapp.in2p3.fr>  - LAPP, France                   *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *      Kai Voss        <Kai.Voss@cern.ch>       - U. of Victoria, Canada         *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      U. of Victoria, Canada                                                    * 
 *      Heidelberg U., Germany                                                    * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//______________________________________________________________________
//
// Implementation of the statistical significance as separation criterion
// Begin_Latex
// Index = #frac{S}{#sqrt{S+B}}
// End_Latex
//______________________________________________________________________


#include "TMath.h"
#include "TMVA/SdivSqrtSplusB.h"

ClassImp(TMVA::SdivSqrtSplusB)

//_______________________________________________________________________
Double_t  TMVA::SdivSqrtSplusB::GetSeparationIndex( const Double_t &s, const Double_t &b )
{
   // Index = S/sqrt(S+B)  (statistical significance)                 
   if (s+b > 0 && s>=0) return s / TMath::Sqrt(s+b);
   else return 0.5;
}

