// @(#)root/tmva $Id: DataInputHandler.cxx,v 1.4 2008/04/05 10:35:39 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataInputHandler                                                               *
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

#include "TMVA/DataInputHandler.h"
#include "TMVA/MsgLogger.h"
#include "TEventList.h"
#include "TCut.h"
#include "TFile.h"
#include "TROOT.h"

#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif

//_______________________________________________________________________
TMVA::DataInputHandler::DataInputHandler() 
   : fLogger( "DataInputHandler", kINFO )
{
   // constructor
   fExplicitTrainTest[Types::kSignal] = fExplicitTrainTest[Types::kBackground] = kFALSE;
}

//_______________________________________________________________________
TMVA::DataInputHandler::~DataInputHandler() 
{
   // destructor
}


//_______________________________________________________________________
void
TMVA::DataInputHandler::AddSignalTree( TTree* tr, Double_t weight, Types::ETreeType tt ) 
{
   // add a signal tree to be used as input
   if (!tr) fLogger << kFATAL << "Zero pointer for signal tree " << Endl;
   if(fInputTrees[Types::kSignal].size()==0) {
      // on the first tree check if explicit treetype is given
      fExplicitTrainTest[Types::kSignal] = (tt!=Types::kMaxTreeType);
   } else {
      // if the first tree has a specific type, all later tree's must also have one
      if(fExplicitTrainTest[Types::kSignal] != (tt!=Types::kMaxTreeType)) {
         if(tt==Types::kMaxTreeType)
            fLogger << kFATAL << "For the signal tree " << tr->GetName()
                    << " you did "<< (tt==Types::kMaxTreeType?"not ":"") << "specify a type,"
                    << " while you did "<< (tt==Types::kMaxTreeType?"":"not ") << "for the first signal tree "
                    << fInputTrees[Types::kSignal][0].GetTree()->GetName()
                    << Endl;
      }
   }
   fInputTrees[Types::kSignal].push_back(TreeInfo(tr,weight,tt));
}

//_______________________________________________________________________
void
TMVA::DataInputHandler::AddBackgroundTree( TTree* tr, Double_t weight, Types::ETreeType tt )
{
   // add a background tree to be used as input
   if (!tr) fLogger << kFATAL << "Zero pointer for background tree " << Endl;
   if(fInputTrees[Types::kBackground].size()==0) {
      // on the first tree check if explicit treetype is given
      fExplicitTrainTest[Types::kBackground] = (tt!=Types::kMaxTreeType);
   } else {
      // if the first tree has a specific type, all later tree's must also have one
      if(fExplicitTrainTest[Types::kBackground] != (tt!=Types::kMaxTreeType)) {
         if(tt==Types::kMaxTreeType)
            fLogger << kFATAL << "For the background tree " << tr->GetName()
                    << " you did "<< (tt==Types::kMaxTreeType?"not ":"") << "specify a type,"
                    << " while you did "<< (tt==Types::kMaxTreeType?"":"not ") << "for the first background tree "
                    << fInputTrees[Types::kBackground][0].GetTree()->GetName()
                    << Endl;
      }
   }
   fInputTrees[Types::kBackground].push_back(TreeInfo(tr,weight,tt));
}


//_______________________________________________________________________
void
TMVA::DataInputHandler::AddSignalTree( const TString& fn, Double_t weight, Types::ETreeType tt ) 
{
   // add a signal tree to the dataset to be used as input
   TTree * tr = ReadInputTree(fn);
   tr->SetName("TreeS");
   fInputTrees[Types::kSignal].push_back(TreeInfo(tr, weight, tt, kTRUE));
}

//_______________________________________________________________________
void
TMVA::DataInputHandler::AddBackgroundTree( const TString& fn, Double_t weight, Types::ETreeType tt )
{
   // add a background tree to the dataset to be used as input
   TTree * tr = ReadInputTree(fn);
   tr->SetName("TreeB");
   fInputTrees[Types::kBackground].push_back(TreeInfo(tr, weight, tt, kTRUE));
}

//_______________________________________________________________________
TTree *
TMVA::DataInputHandler::ReadInputTree( const TString& dataFile )
{
   // create trees from these ascii files
   TTree* tr = new TTree( "tmp", dataFile );
  
   ifstream in(dataFile);
   if (!in.good())
      fLogger << kFATAL << "Could not open file: " << dataFile << Endl;
   in.close();

   tr->ReadFile( dataFile );
  
   return tr;
}

//_______________________________________________________________________
void
TMVA::DataInputHandler::AddInputTrees(TTree* inputTree, const TCut& SigCut, const TCut& BgCut)
{
   // define the input trees for signal and background from single input tree,
   // containing both signal and background events distinguished by the type 
   // identifiers: SigCut and BgCut
   if (!inputTree)
      fLogger << kFATAL << "Zero pointer for input tree: " << inputTree << Endl;

   AddSignalTree    (inputTree->CopyTree(SigCut.GetTitle()), 1.0);
   AddBackgroundTree(inputTree->CopyTree(BgCut.GetTitle()) , 1.0);
}



//_______________________________________________________________________
UInt_t
TMVA::DataInputHandler::GetEntries(const std::vector<TreeInfo>& tiV) const {
   UInt_t entries = 0;
   std::vector<TreeInfo>::const_iterator tiIt = tiV.begin();
   for(;tiIt != tiV.end(); tiIt++)
      entries += tiIt->GetEntries();
   return entries;
}

