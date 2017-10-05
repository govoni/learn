// @(#)root/rpdutils:$Name: v4-04-02g $:$Id: rpderr.h,v 1.1 2003/08/29 10:38:19 rdm Exp $
// Author: Fons Rademakers   11/08/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_rpderr
#define ROOT_rpderr


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// rpderr                                                               //
//                                                                      //
// This header file defines error codes generated by rootd/proofd.      //
// NB: In case of change update also strings in net/inc/ErrStr.h        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_NetErrors
#include "NetErrors.h"
#endif

typedef void (*ErrorHandler_t)(int level, const char *msg);

#endif
