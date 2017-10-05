// @(#)root/tmva $Id: DataInputHandler.h,v 1.4 2008/07/14 13:09:03 speckmayer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataInputHandler                                                               *
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

#ifndef ROOT_TMVA_DataInputHandler
#define ROOT_TMVA_DataInputHandler

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// DataInputHandler                                                              //
//                                                                      //
// Class that contains all the data information                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <vector>
#include <fstream>

#include "TTree.h"

#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {
   
   class TreeInfo {

   public:

      TreeInfo( TTree* tr, Double_t weight=1.0, Types::ETreeType tt = Types::kMaxTreeType, Bool_t own=kFALSE ) 
         : fTree(tr), fWeight(weight), fTreeType(tt), fOwner(own) {}
      ~TreeInfo() {
         if(fOwner) { delete fTree; }
      }

      TTree*           GetTree()     const { return fTree; }
      Double_t         GetWeight()   const { return fWeight; }
      UInt_t           GetEntries()  const { return fTree->GetEntries(); }
      Types::ETreeType GettreeType() const { return fTreeType; }

   private:

      TTree*           fTree;     //! pointer to the tree
      Double_t         fWeight;   //! weight for the tree
      Types::ETreeType fTreeType; //! tree is for training/testing/both
      Bool_t           fOwner;    //! true if created from file
   };

   class DataInputHandler {

   public:

      DataInputHandler();
      ~DataInputHandler();

      // setters
      void     AddSignalTree    ( TTree* tr, Double_t weight=1.0, Types::ETreeType tt = Types::kMaxTreeType );
      void     AddBackgroundTree( TTree* tr, Double_t weight=1.0, Types::ETreeType tt = Types::kMaxTreeType );
      void     AddSignalTree    ( const TString& tr, Double_t weight=1.0, Types::ETreeType tt = Types::kMaxTreeType );
      void     AddBackgroundTree( const TString& tr, Double_t weight=1.0, Types::ETreeType tt = Types::kMaxTreeType );
      void     AddInputTrees    (TTree* inputTree, const TCut& SigCut, const TCut& BgCut);

      // accessors
      UInt_t           GetNSignalTrees()           const { return fInputTrees[Types::kSignal].size(); }
      UInt_t           GetNBackgroundTrees()       const { return fInputTrees[Types::kBackground].size(); }
      UInt_t           GetSignalEntries()          const { return GetEntries(fInputTrees[Types::kSignal]); }
      UInt_t           GetBackgroundEntries()      const { return GetEntries(fInputTrees[Types::kBackground]); }
      UInt_t           GetEntries()                const { return GetSignalEntries()+GetBackgroundEntries(); }
      const TreeInfo&  SignalTreeInfo(Int_t i)     const { return fInputTrees[Types::kSignal][i]; }
      const TreeInfo&  BackgroundTreeInfo(Int_t i) const { return fInputTrees[Types::kBackground][i]; }

      std::vector<TreeInfo>::const_iterator begin(Int_t t) const { return fInputTrees[t].begin(); }
      std::vector<TreeInfo>::const_iterator end(Int_t t)   const { return fInputTrees[t].end(); }
      std::vector<TreeInfo>::const_iterator Sbegin() const { return begin(Types::kSignal); }
      std::vector<TreeInfo>::const_iterator Send()   const { return end  (Types::kSignal); }
      std::vector<TreeInfo>::const_iterator Bbegin() const { return begin(Types::kBackground); }
      std::vector<TreeInfo>::const_iterator Bend()   const { return end  (Types::kBackground); }

      // reset the list of trees
      void     ClearSignalTreeList()     { fInputTrees[Types::kSignal].clear(); }
      void     ClearBackgroundTreeList() { fInputTrees[Types::kBackground].clear(); }

   private:

      UInt_t GetEntries(const std::vector<TreeInfo>& tiV) const;

      TTree * ReadInputTree( const TString& dataFile );
      
      std::vector<TreeInfo>          fInputTrees[2];    //! list of input trees
      Bool_t                         fExplicitTrainTest[2]; //! if set to true the user has specified training and testing data explicitly
      mutable MsgLogger              fLogger;           //! message logger
   };
}

#endif
