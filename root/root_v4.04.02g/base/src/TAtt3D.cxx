// @(#)root/base:$Name: v4-04-02g $:$Id: TAtt3D.cxx,v 1.1.1.1 2000/05/16 17:00:38 rdm Exp $
// Author: Fons Rademakers   08/09/99

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TAtt3D                                                               //
//                                                                      //
// Use this attribute class when an object should have 3D capabilities. //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TAtt3D.h"


ClassImp(TAtt3D)

//______________________________________________________________________________
void TAtt3D::Sizeof3D() const
{
   // Set total size of this 3D object (used by X3D interface).

   return;
}
