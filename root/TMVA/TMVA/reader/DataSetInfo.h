// // @(#)root/tmva $Id: DataSetInfo.h,v 1.8 2008/07/19 19:52:25 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSetInfo                                                           *
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

#ifndef ROOT_TMVA_DataSetInfo
#define ROOT_TMVA_DataSetInfo

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// DataSetInfo                                                          //
//                                                                      //
// Class that contains all the data information                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iosfwd>

#include "TObject.h"
#include "TString.h"
#include "TTree.h"
#include "TCut.h"
#include "TMatrixD.h"

#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_VariableInfo
#include "TMVA/VariableInfo.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

class TH2;

namespace TMVA {

   class DataSet;
   class VariableTransformBase;
   
   class DataSetInfo : public TObject {

   public:

      DataSetInfo(const TString& name = "Default");
      virtual ~DataSetInfo();

      virtual const char * GetName() const { return fName.Data(); }

      // the data set
      void        ClearDataSet() const;
      DataSet*    GetDataSet() const;

      // ---
      // the variable data
      // ---
      VariableInfo&     AddVariable( const TString& expression, char varType='F', Bool_t normalized = kTRUE, void* external = 0 );
      VariableInfo&     AddVariable( const TString& expression, Double_t min, Double_t max, char varType='F', Bool_t normalized = kTRUE, void* external = 0 );
      // accessors
      // general
      std::vector<VariableInfo>&       GetVariableInfos() { return fVariables; }
      const std::vector<VariableInfo>& GetVariableInfos() const { return fVariables; }
      UInt_t                           GetNVariables()    const { return fVariables.size(); }
      // by variable
      char              GetVarType(Int_t i)               const { return fVariables[i].GetVarType(); }
      const TString&    GetExpression(Int_t i)            const { return fVariables[i].GetExpression(); }
      const TString&    GetInternalVarName(Int_t i)       const { return fVariables[i].GetInternalVarName(); }
      Int_t             FindVarIndex(const TString&)      const;
      // weights
      void              SetWeightExpression          ( const TString& expr );
      void              SetSignalWeightExpression    ( const TString& expr ) { fWeightExp[Types::kSignal]     = expr; }
      void              SetBackgroundWeightExpression( const TString& expr ) { fWeightExp[Types::kBackground] = expr; }
      const TString&    GetWeightExpression(Int_t t)      const { return fWeightExp[t]; }
      const std::vector<TString> GetListOfVariables();


      // ---
      // the cut
      // ---
      void SetCut( const TString& cut, Types::ESBType tt = Types::kSBBoth ) { SetCut(TCut(cut),tt); fNeedsRebuilding = kTRUE; }
      void SetCut( const TCut& cut, Types::ESBType tt = Types::kSBBoth );
      // accessors
      const TCut& Cut(Types::ESBType tt)  const { return tt==Types::kSignal?fCut[0]:fCut[1]; }
      const char* CutS(Types::ESBType tt)  const { return (TString(Cut(tt).GetTitle())!=""?Cut(tt).GetTitle():"-"); }
      Bool_t HasCuts() { return TString(CutS(Types::kSignal))!="-" || TString(CutS(Types::kBackground))!="-"; }

      // ---
      // the transformation
      // ---
//       void SetTransformationType(Types::EVariableTransform type) { fTransformType=type; }
//       Types::EVariableTransform GetTransformationType() const { return fTransformType; }
//       void SetTransformation(VariableTransformBase* tr) { fTransform=tr; }
//       VariableTransformBase* GetTransformation() const { return fTransform; }


      // correlation matrix 
      const TMatrixD* CorrelationMatrix( Types::ESBType sigbgd ) const { return fDecorrMatrix[sigbgd]; }
      void SetCorrelationMatrix( TMatrixD* cm, Types::ESBType sigbgd ) { fDecorrMatrix[sigbgd] = cm; }
      void PrintCorrelationMatrix( Types::ESBType );
      TH2* createCorrelationMatrixHist( const TMatrixD* m,
                                        const TString& hName,
                                        const TString& hTitle ) const;


      // options
      void           SetSplitOptions(const TString& so) { fSplitOptions = so; fNeedsRebuilding = kTRUE; }
      const TString& GetSplitOptions() const { return fSplitOptions; }

      // root dir
      void           SetRootDir(TDirectory* d) { fOwnRootDir = d; }
      TDirectory*    GetRootDir() const { return fOwnRootDir; }

      void           Save();


   private:


      DataSetInfo(const DataSetInfo&) : TObject() {};

      void PrintCorrelationMatrix( TTree* theTree );

      TString                    fName;             //! name of the dataset info object

      mutable DataSet*           fDataSet;          //! dataset, owned by this datasetinfo object
      mutable Bool_t             fNeedsRebuilding;  //! flag if rebuilding of dataset is needed (after change of cuts, vars, etc.)

      // expressions/formulas
      std::vector<VariableInfo>  fVariables;        //! list of variable expressions/internal names

      // the weight
      TString                    fWeightExp[2];      //! the input formula string that is the weight for signal and background

      // the cut
      TCut                       fCut[2];              // the pretraining cuts for signal and background

      TMatrixD*                  fDecorrMatrix[2];  //! Decorrelation matrix [signal/background]

//       Types::EVariableTransform  fTransformType;    //! Type of the variable transformation
//       VariableTransformBase*     fTransform;        //! variable transformations

      TString                    fSplitOptions;

      // ROOT output dir
      TDirectory*                fOwnRootDir;
      // verbosity
      Bool_t                     fVerbose;           //! Verbosity
      
      mutable MsgLogger          fLogger;           //! message logger

   };
}

#endif
