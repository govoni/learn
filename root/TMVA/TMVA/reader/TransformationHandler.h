// @(#)root/tmva $Id: TransformationHandler.h,v 1.5 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TransformationHandler                                                               *
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

#ifndef ROOT_TMVA_TransformationHandler
#define ROOT_TMVA_TransformationHandler

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TransformationHandler                                                              //
//                                                                      //
// Class that contains all the data information                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TList.h"

#ifndef ROOT_TMVA_DataSetInfo
#include "TMVA/DataSetInfo.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {

   class Event;
   class DataSet;
   class Ranking;
   class VariableTransformBase;
   
   class TransformationHandler {
   public:

      struct VariableStat {
         Float_t fMean;
         Float_t fRMS;
         Float_t fMin;
         Float_t fMax;
      };

      TransformationHandler(DataSetInfo&);
      ~TransformationHandler();

      TString GetName();

      const Event* Transform(const Event*, Types::ESBType type = Types::kMaxSBType) const;

      void AddTransformation(VariableTransformBase*);
      const TList& GetTransformationList() const { return fTransformations; }
      void CalcTransformations(const std::vector<Event*>&);
      
      void CalcStats(const std::vector<Event*>& events);
      void AddStats(UInt_t k, UInt_t ivar, Float_t mean, Float_t rms, Float_t min, Float_t max);
      Float_t GetMean(UInt_t ivar, Types::ESBType t = Types::kSBBoth) const { return fVariableStats[idxOfType(t)][ivar].fMean; }
      Float_t GetRMS (UInt_t ivar, Types::ESBType t = Types::kSBBoth) const { return fVariableStats[idxOfType(t)][ivar].fRMS; }
      Float_t GetMin (UInt_t ivar, Types::ESBType t = Types::kSBBoth) const { return fVariableStats[idxOfType(t)][ivar].fMin; }
      Float_t GetMax (UInt_t ivar, Types::ESBType t = Types::kSBBoth) const { return fVariableStats[idxOfType(t)][ivar].fMax; }

      void WriteToStream ( std::ostream& o ) const;
      void ReadFromStream( std::istream& istr );

      // writer of function code
      void MakeFunction(std::ostream& fout, const TString& fncName, Int_t part) const;

      // variable ranking
      Ranking* GetVariableRanking()   const { return fRanking; }
      void     PrintVariableRanking() const;

      // setting file dir for histograms
      void SetRootDir(TDirectory *d) { fRootBaseDir = d; }

   private:

      const std::vector<TMVA::Event*>* TransformCollection(VariableTransformBase* trf,
                                                           const std::vector<TMVA::Event*>* events,
                                                           Bool_t replace) const;

      const TMVA::VariableInfo& Variable(UInt_t ivar) const { return fDataSetInfo.GetVariableInfos()[ivar]; }

      void PlotVariables( const std::vector<Event*>& events );

      DataSet* Data() { return fDataSetInfo.GetDataSet(); }

      UInt_t  idxOfType(Types::ESBType t) const { return t==Types::kSignal?0:t==Types::kBackground?1:2; }

      DataSetInfo&                                                    fDataSetInfo;
      TList                                                           fTransformations;    //! list of transformations
      std::vector<TMVA::TransformationHandler::VariableStat>          fVariableStats[3];
      Ranking*                                                        fRanking;            //! ranking object
      TDirectory*                                                     fRootBaseDir;        //! if set put input var hists
      mutable MsgLogger                                               fLogger;             //! message logger

   };
}

#endif
