// @(#)root/tmva $Id: DataSetManager.cxx,v 1.3 2007/07/27 14:55:14 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSetManager                                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#include <vector>
#include <iostream>
using std::endl;

#ifndef ROOT_TMVA_DataSetManager
#include "TMVA/DataSetManager.h"
#endif
#ifndef ROOT_TMVA_DataSetFactory
#include "TMVA/DataSetFactory.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_DataSetInfo
#include "TMVA/DataSetInfo.h"
#endif

TMVA::DataSetManager* TMVA::DataSetManager::fgDSManager = 0;


//_______________________________________________________________________
TMVA::DataSetManager::DataSetManager(DataInputHandler& dataInput) 
   : fDataInput(dataInput),
     fDataSetInfoCollection(),
     fLogger( "DataSetManager", kINFO )
{
   // constructor
}

//_______________________________________________________________________
TMVA::DataSetManager::~DataSetManager() 
{
   // destructor
}


//_______________________________________________________________________
TMVA::DataSet* TMVA::DataSetManager::CreateDataSet(const TString& dsiName) {
   DataSetInfo* dsi = GetDataSetInfo(dsiName);
   if(!dsi)
      fLogger << kFATAL << "DataSetInfo object '" << dsiName << "' not found" << endl;

   // factory to create dataset from datasetinfo and datainput
   return TMVA::DataSetFactory::Instance().createDataSet(*dsi, fDataInput);
}


//_______________________________________________________________________
TMVA::DataSetInfo*
TMVA::DataSetManager::GetDataSetInfo(const TString& dsiName) {
   // returns datasetinfo object for given name
   return (DataSetInfo*)fDataSetInfoCollection.FindObject(dsiName);
}


//_______________________________________________________________________
TMVA::DataSetInfo&
TMVA::DataSetManager::AddDataSetInfo(DataSetInfo& dsi) {
   // stores a copy of the dataset info object
   DataSetInfo * dsiInList = GetDataSetInfo(dsi.GetName());
   if(dsiInList!=0) return *dsiInList;
   fDataSetInfoCollection.Add(const_cast<DataSetInfo*>(&dsi) );
   return dsi;
}
