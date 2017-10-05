// @(#)root/tmva $Id: CrossEntropy.cxx,v 1.15 2008/03/30 21:08:46 helgevoss Exp $       
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TMVA::CrossEntropy                                                    *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description: Implementation of the CrossEntropy as separation criterion        *
 *              -p log (p) - (1-p)log(1-p);     p=purity                          * 
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

//________________________________________________________________________
//
// Implementation of the CrossEntropy as separation criterion           
//
// Begin_Latex                                                             
// -p log(p) - (1-p) log(1-p)   p=purity                        
// End_Latex
//________________________________________________________________________


#include <math.h>
#include "TMVA/CrossEntropy.h"

ClassImp(TMVA::CrossEntropy)
   
//_______________________________________________________________________
Double_t  TMVA::CrossEntropy::GetSeparationIndex( const Double_t &s, const Double_t &b )
{
   //  Cross Entropy defined as
   //  -p log (p) - (1-p)log(1-p);     p=purity = s/(s+b)                       
   if (s+b <= 0 || s < 0) return 0.5;
   Double_t p = s/(s+b);
   if (p<=0 || p >=1) return 0;
   return - ( p * log (p) + (1-p)*log(1-p) );
}
