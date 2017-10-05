// @(#)root/tmva $Id: Factory.h,v 1.10 2008/07/20 20:44:28 stelzer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Factory                                                               *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      This is the main MVA steering class: it creates (books) all MVA methods,  *
 *      and guides them through the training, testing and evaluation phases.      *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Xavier Prudent  <prudent@lapp.in2p3.fr>  - LAPP, France                   *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *      Kai Voss        <Kai.Voss@cern.ch>       - U. of Victoria, Canada         *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      U. of Victoria, Canada                                                    * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#ifndef ROOT_TMVA_Factory
#define ROOT_TMVA_Factory

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Factory                                                              //
//                                                                      //
// This is the main MVA steering class: it creates all MVA methods,     //
// and guides them through the training, testing and evaluation         //
// phases                                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <map>
#include "TCut.h"

#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif
#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

class TFile;
class TTree;
class TTreeFormula;
class TDirectory;

namespace TMVA {

   class IMethod;
   class MethodBase;

   class DataInputHandler;
   class DataSetManager;
   class DataSetInfo;
   class VariableTransformBase;

   class Factory : public Configurable {
   public:

      typedef std::vector<IMethod*> MVector;

      // no default  constructor
      Factory( TString theJobName, TFile* theTargetFile, TString theOption = "" );

      // default destructor
      virtual ~Factory();

      virtual const char*  GetName() const { return "Factory"; }

      // add events to training and testing trees
      void AddSignalTrainingEvent    ( std::vector<Double_t>& /*event*/, Double_t /*weight = 1.0*/ ) {};
      void AddBackgroundTrainingEvent( std::vector<Double_t>& /*event*/, Double_t /*weight = 1.0*/ ) {};
      void AddSignalTestEvent        ( std::vector<Double_t>& /*event*/, Double_t /*weight = 1.0*/ ) {};
      void AddBackgroundTestEvent    ( std::vector<Double_t>& /*event*/, Double_t /*weight = 1.0*/ ) {};

      DataSetInfo& AddDataSet( DataSetInfo& );
      DataSetInfo& AddDataSet( const TString&  );

      // Data input related
      void SetInputTrees(const TString& signalFileName, const TString& backgroundFileName, 
                         Double_t signalWeight=1.0, Double_t backgroundWeight=1.0 );
      void SetInputTrees(TTree* inputTree, const TCut& SigCut, const TCut& BgCut);
      // Set input trees at once
      void SetInputTrees(TTree* signal, TTree* background, 
                         Double_t signalWeight=1.0, Double_t backgroundWeight=1.0);
      // set signal tree
      void AddSignalTree( TTree* signal, Double_t weight=1.0, Types::ETreeType treetype = Types::kMaxTreeType );
      void AddSignalTree( TTree* signal, Double_t weight, const TString& treetype );      
      // ... depreciated, kept for backwards compatibility
      void SetSignalTree(TTree* signal, Double_t weight=1.0);

      // set background tree
      void AddBackgroundTree( TTree* background, Double_t weight=1.0, Types::ETreeType treetype = Types::kMaxTreeType );
      void AddBackgroundTree( TTree* background, Double_t weight, const TString & treetype );
      // ... depreciated, kept for backwards compatibility
      void SetBackgroundTree( TTree* background, Double_t weight=1.0 );


      // set input variable
      void SetInputVariables( std::vector<TString>* theVariables );
      void AddVariable( const TString& expression, char type='F',
                        Double_t min = 0, Double_t max = 0 );
      void SetWeightExpression( const TString& variable )  { 
         SetSignalWeightExpression    ( variable );
         SetBackgroundWeightExpression( variable );
      }
      void SetSignalWeightExpression( const TString& variable );
      void SetBackgroundWeightExpression( const TString& variable );

      // set cuts
      void SetCut( const TString& cut, Types::ESBType tt = Types::kSBBoth );
      void SetCut( const TCut& cut, Types::ESBType tt = Types::kSBBoth );


      // depreciated: prepare input tree for training
      void PrepareTrainingAndTestTree( const TCut& cut, 
                                       Int_t Ntrain, Int_t Ntest = -1 );

      void PrepareTrainingAndTestTree( const TCut& cut,
                                       Int_t NsigTrain, Int_t NbkgTrain, Int_t NsigTest, Int_t NbkgTest, 
                                       const TString& otherOpt="SplitMode=Random:!V" );

      void PrepareTrainingAndTestTree( const TCut& cut, 
                                       const TString& splitOpt="NsigTrain=3000:NbkgTrain=3000:SplitMode=Random" );
      void PrepareTrainingAndTestTree( TCut sigcut, TCut bkgcut, 
                                       const TString& splitOpt="NsigTrain=3000:NbkgTrain=3000:SplitMode=Random" );

      Bool_t BookMethod( TString theMethodName, TString methodTitle, TString theOption = "" );
      Bool_t BookMethod( Types::EMVA theMethod,  TString methodTitle, TString theOption = "" );
      Bool_t BookMethod( TMVA::Types::EMVA /*theMethod*/, TString /*methodTitle*/, TString /*methodOption*/, TMVA::Types::EMVA /*theCommittee*/, TString /*committeeOption = ""*/ ) { return 0; }; 

      // training for all booked methods
      void TrainAllMethods( void );

      // testing
      void TestAllMethods( void );

      // performance evaluation
      void EvaluateAllMethods( void );
      void EvaluateAllVariables( TString options = "" ); 
  
      // delete all methods and reset the method vector
      void DeleteAllMethods( void );

      // accessors
      IMethod* GetMethod( const TString& title ) const;

      Bool_t Verbose( void ) const { return fVerbose; }
      void SetVerbose( Bool_t v=kTRUE );

      // make ROOT-independent C++ class for classifier response 
      // (classifier-specific implementation)
      // If no classifier name is given, help messages for all booked 
      // classifiers are printed
      virtual void MakeClass( const TString& methodTitle = "" ) const;

      // prints classifier-specific hepl messages, dedicated to 
      // help with the optimisation and configuration options tuning.
      // If no classifier name is given, help messages for all booked 
      // classifiers are printed
      void PrintHelpMessage( const TString& methodTitle = "" ) const;

      static TDirectory* RootBaseDir() { return (TDirectory*)fgTargetFile; }

   private:

      // the beautiful greeting message
      void Greetings();

      void WriteDataInformation();

      DataInputHandler&        DataInput() { return *fDataInputHandler; }
      DataSetInfo&             DefaultDataSetInfo();
      void                     SetInputTreesFromEventAssignTrees() {}

      static TFile*              fgTargetFile;         // ROOT output file

      DataInputHandler*          fDataInputHandler;

      std::vector<TMVA::VariableTransformBase*> fDefaultTrfs; // list of transformations on default DataSet

      // cd to local directory
      TString                    fOptions;            // option string given by construction (presently only "V")
      TString                    fTransformations;    // List of transformations to test
      Bool_t                     fVerbose;            // verbose mode
      Bool_t                     fColor;              // color mode

      std::vector<TTreeFormula*> fInputVarFormulas; // local forulas of the same
      MVector                    fMethods;          // all MVA methods
      TString                    fJobName;          // jobname, used as extension in weight file names

      // flag determining the way training and test data are assigned to Factory
      enum DataAssignType { kUndefined = 0, kAssignTrees, kAssignEvents };
      DataAssignType             fDataAssignType;         // flags for data assigning

   protected:

      ClassDef(Factory,0)  // The factory creates all MVA methods, and performs their training and testing
   };

} // namespace TMVA

#endif

