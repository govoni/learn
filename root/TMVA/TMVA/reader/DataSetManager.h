// @(#)root/tmva $Id: DataSetManager.h,v 1.3 2007/07/27 14:55:14 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSetManager                                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Contains all the data information                                         *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      U. of Victoria, Canada                                                    *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#ifndef ROOT_TMVA_DataSetManager
#define ROOT_TMVA_DataSetManager

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// DataSetManager                                                              //
//                                                                      //
// Class that contains all the data information                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TList.h"
#include "TString.h"

#ifndef ROOT_TMVA_DataSetManager
#include "TMVA/DataSetManager.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {

   class DataSet;
   class DataSetInfo;
   class DataInputHandler;

   class DataSetManager {
   public:

      static DataSetManager& Instance() { return *fgDSManager; }

      static void createInstance(DataInputHandler& dataInput) { fgDSManager = new DataSetManager(dataInput); }

      ~DataSetManager();

      // ownership stays with this handler
      DataSet* CreateDataSet(const TString& dsiName);
      DataSetInfo* GetDataSetInfo(const TString& dsiName);

      // makes a local copy of the dataset info object
      DataSetInfo& AddDataSetInfo(DataSetInfo& dsi);



   private:

      static DataSetManager * fgDSManager;

      // private default constructor
      DataSetManager();
      DataSetManager(DataInputHandler& dataInput);

      // access to input data
      DataInputHandler& DataInput() { return fDataInput; }

      DataInputHandler&          fDataInput;             //! source of input data
      TList                      fDataSetInfoCollection; //! all registered dataset definitions
      mutable MsgLogger          fLogger;                //! message logger

   };
}

#endif
