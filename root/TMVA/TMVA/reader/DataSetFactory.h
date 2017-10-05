// @(#)root/tmva $Id: DataSetFactory.h,v 1.9 2008/04/05 10:35:39 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSetFactory                                                               *
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

#ifndef ROOT_TMVA_DataSetFactory
#define ROOT_TMVA_DataSetFactory

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// DataSetFactory                                                       //
//                                                                      //
// Class that contains all the data information                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "TTree.h"
#include "TCut.h"
#include "TTreeFormula.h"
#include "TMatrixD.h"
#include "TPrincipal.h"

#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_VariableInfo
#include "TMVA/VariableInfo.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {
   
   class DataSet;
   class DataSetInfo;
   class DataInputHandler;

   class DataSetFactory {

   public:

      DataSetFactory();
      ~DataSetFactory();

      static DataSetFactory& Instance() { if(!fgInstance) fgInstance=new DataSetFactory(); return *fgInstance; } 

      DataSet * createDataSet(DataSetInfo &, DataInputHandler&);

   private:
      
      static DataSetFactory *fgInstance;

      DataSet*                   buildInitialDataSet( DataSetInfo&, TMVA::DataInputHandler& );

      // auxiliary functions to compute correlations
      TMatrixD*                  CalcCorrelationMatrix( DataSet*, Bool_t isSignal );
      TMatrixD*                  CalcCovarianceMatrix ( DataSet*, Bool_t isSignal );
      void                       CalcMinMax( DataSet* ds, TMVA::DataSetInfo& dsi );

      // resets branch addresses to current event
      void ResetBranchAndEventAddresses( TTree* );
      void ResetCurrentTree() { fCurrentTree = 0; }

      void ChangeToNewTree( TTree*, const DataSetInfo & );


      // verbosity
      Bool_t Verbose() { return fVerbose; }

      // data members

      // verbosity
      Bool_t                     fVerbose;           //! Verbosity

      // the event 
      mutable TTree*             fCurrentTree;       //! the tree, events are currently read from
      mutable UInt_t             fCurrentEvtIdx;     //! the current event (to avoid reading of the same event)

      // the formulas for reading the original tree
      std::vector<TTreeFormula*> fInputVarFormulas;  //! local formulas of the same
      TTreeFormula*              fWeightFormula[2];  //! local weight formula
      TTreeFormula*              fCutFormula[2];  //! the pretraining cut as formula
      
      mutable MsgLogger          fLogger;           //! message logger

   };
}

#endif
