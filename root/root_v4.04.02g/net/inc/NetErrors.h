// @(#)root/net:$Name: v4-04-02g $:$Id: NetErrors.h,v 1.6 2004/11/05 13:55:13 rdm Exp $
// Author: Fons Rademakers   11/08/97

/*************************************************************************
 * Copyright (C) 1995-2003, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_NetErrors
#define ROOT_NetErrors


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// NetErrors                                                            //
//                                                                      //
// This header file defines error codes generated by rootd/proofd.      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

enum ERootdErrors {
   kErrUndef,
   kErrNoFile,
   kErrBadFile,
   kErrFileExists,
   kErrNoAccess,
   kErrFileOpen,
   kErrFileWriteOpen,
   kErrFileReadOpen,
   kErrNoSpace,
   kErrBadOp,
   kErrBadMess,
   kErrFilePut,
   kErrFileGet,
   kErrNoUser,
   kErrNoAnon,
   kErrBadUser,
   kErrNoHome,
   kErrNoPasswd,
   kErrBadPasswd,
   kErrNoSRP,
   kErrFatal,
   kErrRestartSeek,
   kErrNotAllowed,
   kErrConnectionRefused,
   kErrAuthNotOK,
   kErrWrongUser,
   kErrNoPipeInfo,
   kErrNoChangePermission,
   kErrBadRtag,
   kErrNoPassHEquNoFiles,
   kErrNoPassHEquBadFiles,
   kErrNoPassHEquFailed,
   kErrBadPassHEquNoFiles,
   kErrBadPassHEquBadFiles,
   kErrBadPassHEquFailed,
   kErrBadRtagHEquNoFiles,
   kErrBadRtagHEquBadFiles,
   kErrBadRtagHEquFailed,
   kErrBadPwdFile,
   kErrBadPwdFileHEquNoFiles,
   kErrBadPwdFileHEquBadFiles,
   kErrBadPwdFileHEquFailed
};

R__EXTERN const char *gRootdErrStr[];

#endif
