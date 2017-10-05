// @(#)root/tmva $Id: DataSet.cxx,v 1.7 2008/07/19 19:43:32 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSet                                                               *
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

#ifndef ROOT_TMVA_DataSetInfo
#include "TMVA/DataSetInfo.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif
#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif

//_______________________________________________________________________
TMVA::DataSet::DataSet(const DataSetInfo& dsi) 
   : fdsi(dsi),
     fCurrentTreeIdx(0),
     fCurrentEventIdx(0),
     fHasNegativeEventWeights(kFALSE),
     fLogger( "DataSet", kINFO )
{
   // constructor
   fEventCollection[0] = fEventCollection[1] = fEventCollection[2] = fEventCollection[3] = 0;
   fSignalEvents[0] = fSignalEvents[1] = fSignalEvents[2] = fSignalEvents[3] = -1;
   fBlockBelongToTraining.reserve(10);
   fBlockBelongToTraining.push_back(kTRUE);
}

//_______________________________________________________________________
TMVA::DataSet::~DataSet() 
{
   // destructor
   DestroyCollection(Types::kTraining);
   DestroyCollection(Types::kTesting);
   fBlockBelongToTraining.clear();
}


//_______________________________________________________________________
void
TMVA::DataSet::DestroyCollection(Types::ETreeType type) {
   // destroys the event collection (events + vector)
   Int_t i = TreeIndex(type);
   if(fEventCollection[i]!=0) {
      for(std::vector<Event*>::iterator it = fEventCollection[i]->begin();
          it != fEventCollection[i]->end(); it++) delete *it;
      delete fEventCollection[i];
      fEventCollection[i]=0;
   }
}

//_______________________________________________________________________
UInt_t
TMVA::DataSet::GetNVariables() const {
   // access the number of variables through the datasetinfo
   return fdsi.GetNVariables();
}


//_______________________________________________________________________
void
TMVA::DataSet::AddEvent(Event * ev, Types::ETreeType type) {
   // add event to event list
   // after which the event is owned by the dataset
   fEventCollection[Int_t(type)]->push_back(ev);
   if(ev->GetWeight()<0)
      fHasNegativeEventWeights = kTRUE;
}


//_______________________________________________________________________
void TMVA::DataSet::SetEventCollection(std::vector<TMVA::Event*>* events, Types::ETreeType type) {
   // Sets the event collection (by DataSetFactory)
   DestroyCollection(type);

   const Int_t t = TreeIndex(type);
   fSignalEvents[t] = -1;
   fEventCollection[t] = events;
   for(UInt_t i=0; i<events->size(); i++)
      if( (*events)[i]->IsSignal() ) fSignalEvents[t]++;
}

//_______________________________________________________________________
std::vector<Float_t>* TMVA::DataSet::CreateClassifierOutputVector(const TString &clName, Types::ETreeType type, Bool_t mva) {
   std::vector<Float_t>* clOV = GetClassifierOutputVector(clName, type, mva);
   if(clOV!=0)
      fLogger << kFATAL << "Classifier " << clName << " has already a result vector stored in the dataset" << Endl;
   return &(fClassifierResult[TreeIndex(type)][mva==0?0:1][clName]);
}

//_______________________________________________________________________
std::vector<Float_t>* TMVA::DataSet::GetClassifierOutputVector(const TString & clName, Types::ETreeType type, Bool_t mva) {
   const Int_t t = TreeIndex(type);
   std::map<TString, std::vector<Float_t> >::iterator it = fClassifierResult[t][mva==0?0:1].find(clName);
   if(it != fClassifierResult[t][mva==0?0:1].end() )
      return &(it->second);
   return 0;
}

//_______________________________________________________________________
void TMVA::DataSet::Count(Types::ETreeType type) {
   const Int_t t = TreeIndex(type);
   fSignalEvents[t] = 0;
   for(UInt_t i=0; i<fEventCollection[t]->size(); i++)
      if( (*fEventCollection[t])[i]->IsSignal() ) fSignalEvents[t]++;
}



//_______________________________________________________________________
Long64_t TMVA::DataSet::GetNEvtSignal(Types::ETreeType type) {
   const Int_t t = TreeIndex(type);
   if(fSignalEvents[t]<0) Count(type); 
   return fSignalEvents[t];
}



//_______________________________________________________________________
Long64_t TMVA::DataSet::GetNEvtBackground(Types::ETreeType type) {
   return GetNEvents(type)-GetNEvtSignal(type);
}

//_______________________________________________________________________
void TMVA::DataSet::DivideTrainingSet(UInt_t blockNum)
{
   Int_t tOrg = TreeIndex(Types::kTrainingOriginal),tTrn = TreeIndex(Types::kTraining);
   //not changing anything ??
   if (fBlockBelongToTraining.size() == blockNum) return;
   //storing the original training vector
   if (fBlockBelongToTraining.size() == 1)
   {
      if (fEventCollection[tOrg] == 0)
         fEventCollection[tOrg]=new std::vector<TMVA::Event*>(fEventCollection[tTrn]->size());
      fEventCollection[tOrg]->clear();
      for(UInt_t i=0; i<fEventCollection[tTrn]->size(); i++)
         fEventCollection[tOrg]->push_back((*fEventCollection[tTrn])[i]);
      fSignalEvents[tOrg] = fSignalEvents[tTrn];
   }
   //reseting the event division vector
   fBlockBelongToTraining.clear();
   for (UInt_t i=0 ; i < blockNum ; i++)
      fBlockBelongToTraining.push_back(kTRUE);

   ApplyTrainingSetDivision();
}

//_______________________________________________________________________
void TMVA::DataSet::ApplyTrainingSetDivision()
{
   Int_t tOrg = TreeIndex(Types::kTrainingOriginal), tTrn = TreeIndex(Types::kTraining), tVld = TreeIndex(Types::kValidation);
   fEventCollection[tTrn]->clear();
   if (fEventCollection[tVld]==0)
      fEventCollection[tVld] = new std::vector<TMVA::Event*>(fEventCollection[tOrg]->size());
   fEventCollection[tVld]->clear();

   //creating the new events collections, notice that the events that can't be evenly divided belong to the last event
   for(UInt_t i=0; i<fEventCollection[tOrg]->size(); i++) {
      if (fBlockBelongToTraining[i % fBlockBelongToTraining.size()])
         fEventCollection[tTrn]->push_back((*fEventCollection[tOrg])[i]);
      else
         fEventCollection[tVld]->push_back((*fEventCollection[tOrg])[i]);
   }
   Count(Types::kTraining);
   Count(Types::kValidation);
}


//_______________________________________________________________________
void TMVA::DataSet::MoveTrainingBlock(Int_t blockInd,Types::ETreeType dest, Bool_t applyChanges)
{
   if (dest == Types::kValidation)
      fBlockBelongToTraining[blockInd]=kFALSE;
   else
      fBlockBelongToTraining[blockInd]=kTRUE;
   if (applyChanges) ApplyTrainingSetDivision();
}
