// @(#)root/tmva $Id: DataSetInfo.cxx,v 1.10 2008/08/01 12:17:46 speckmayer Exp $
// Author: Joerg Stelzer, Peter Speckmeier

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : DataSetInfo                                                           *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Peter Speckmayer <speckmay@mail.cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - DESY, Germany                  *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      DESY Hamburg, Germany                                                     *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#include <vector>
#include "Riostream.h"

#include "TMVA/Tools.h"
#include "TEventList.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TRandom.h"
#include "TMatrixF.h"
#include "TVectorF.h"
#include "TMath.h"
#include "TROOT.h"
#include "TObjString.h"

#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_DataSetInfo
#include "TMVA/DataSetInfo.h"
#endif
#ifndef ROOT_TMVA_DataSetManager
#include "TMVA/DataSetManager.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

//_______________________________________________________________________
TMVA::DataSetInfo::DataSetInfo(const TString& name) 
   : TObject(),
     fName(name),
     fDataSet( 0 ),
     fNeedsRebuilding( kTRUE ),
     fVariables(),
//      fTransformType(Types::kNone),
//      fTransform(0),
     fSplitOptions(""),
     fOwnRootDir(0),
     fVerbose( kFALSE ),
     fLogger( "DataSetInfo", kINFO )
{
   // constructor
   fDecorrMatrix[0] = fDecorrMatrix[1] = 0;
   fCut[0] = fCut[1] = "";
   fWeightExp[Types::kSignal] = fWeightExp[Types::kBackground] = "";
}

//_______________________________________________________________________
TMVA::DataSetInfo::~DataSetInfo() 
{
   // destructor
   ClearDataSet(); // <======== TEST
}

//_______________________________________________________________________
void TMVA::DataSetInfo::ClearDataSet() const 
{
   delete fDataSet; fDataSet=0;
}

//_______________________________________________________________________
TMVA::VariableInfo& TMVA::DataSetInfo::AddVariable( const TString& expression, char varType, Bool_t normalized, void* external ) 
{
   // add a variable (can be a complex expression) to the set of variables used in
   // the MV analysis   
   return this->AddVariable( expression, 0, 0, varType, normalized, external );
}

//_______________________________________________________________________
TMVA::VariableInfo& TMVA::DataSetInfo::AddVariable( const TString& expression, Double_t min, Double_t max, char varType,
                                                    Bool_t normalized, void* external )
{
   // add a variable (can be a complex expression) to the set of variables used in
   // the MV analysis
   TString regexpr = expression; // remove possible blanks
   regexpr.ReplaceAll(" ", "" );
   fVariables.push_back(VariableInfo( regexpr, fVariables.size()+1, varType, external, min, max, normalized ));
   fNeedsRebuilding = kTRUE;
   return fVariables.back();
}

//_______________________________________________________________________
Int_t TMVA::DataSetInfo::FindVarIndex(const TString& var) const
{
   // find variable by name
   for (UInt_t ivar=0; ivar<GetNVariables(); ivar++) 
      if (var == GetInternalVarName(ivar)) return ivar;
   
   for (UInt_t ivar=0; ivar<GetNVariables(); ivar++) 
      fLogger << kINFO  <<  GetInternalVarName(ivar) << Endl;
   
   fLogger << kFATAL << "<FindVarIndex> Variable \'" << var << "\' not found." << Endl;
 
   return -1;
}

//_______________________________________________________________________
void TMVA::DataSetInfo::SetWeightExpression( const TString& expr ) 
{
   // set the weight expressions for signal and background
   SetSignalWeightExpression    ( expr );
   SetBackgroundWeightExpression( expr );
}

//_______________________________________________________________________
void
TMVA::DataSetInfo::SetCut( const TCut& cut, Types::ESBType tt ) {
   if(tt==Types::kSignal || tt==Types::kSBBoth) fCut[0] = cut;
   if(tt==Types::kBackground || tt==Types::kSBBoth) fCut[1] = cut;
}

//_______________________________________________________________________
const std::vector<TString> TMVA::DataSetInfo::GetListOfVariables() 
{
   // returns list of variables
   std::vector<TString> vNames;
   std::vector<VariableInfo>::iterator viIt = fVariables.begin();
   for(;viIt != fVariables.end(); viIt++) vNames.push_back( (*viIt).GetExpression() );

   return vNames;
}

//_______________________________________________________________________
void TMVA::DataSetInfo::PrintCorrelationMatrix( Types::ESBType sigbg )
{ 
   // calculates the correlation matrices for signal and background, 
   // prints them to standard output, and fills 2D histograms
   fLogger << kINFO << "Correlation matrix (" << (sigbg==Types::kSignal?"signal":"background") << "):" << Endl;
   gTools().FormattedOutput( *CorrelationMatrix( sigbg ), GetListOfVariables(), fLogger );
   fLogger << Endl;
}

//_______________________________________________________________________
TH2* TMVA::DataSetInfo::createCorrelationMatrixHist( const TMatrixD* m,
                                                     const TString& hName,
                                                     const TString& hTitle ) const
{

   if (m==0) return 0;
   
   const UInt_t nvar = GetNVariables();

   // workaround till the TMatrix templates are comonly used
   // this keeps backward compatibility
   TMatrixF* tm = new TMatrixF( nvar, nvar );
   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      for (UInt_t jvar=0; jvar<nvar; jvar++) {
         (*tm)(ivar, jvar) = (*m)(ivar,jvar);
      }
   }  

   TH2F* h2 = new TH2F( *tm );
   h2->SetNameTitle( hName, hTitle );

   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      h2->GetXaxis()->SetBinLabel( ivar+1, GetExpression(ivar) );
      h2->GetYaxis()->SetBinLabel( ivar+1, GetExpression(ivar) );
   }
   
   // present in percent, and round off digits
   // also, use absolute value of correlation coefficient (ignore sign)
   h2->Scale( 100.0  ); 
   for (UInt_t ibin=1; ibin<=nvar; ibin++)
      for (UInt_t jbin=1; jbin<=nvar; jbin++)
         h2->SetBinContent( ibin, jbin, Int_t(h2->GetBinContent( ibin, jbin )) );
   
   // style settings
   const Float_t labelSize = 0.055;
   h2->SetStats( 0 );
   h2->GetXaxis()->SetLabelSize( labelSize );
   h2->GetYaxis()->SetLabelSize( labelSize );
   h2->SetMarkerSize( 1.5 );
   h2->SetMarkerColor( 0 );
   h2->LabelsOption( "d" ); // diagonal labels on x axis
   h2->SetLabelOffset( 0.011 );// label offset on x axis
   h2->SetMinimum( -100.0 );
   h2->SetMaximum( +100.0 );
   
   fLogger << kVERBOSE << "Created correlation matrix as 2D histogram: " << h2->GetName() << Endl;
   
   return h2;
}

//_______________________________________________________________________
TMVA::DataSet* TMVA::DataSetInfo::GetDataSet() const 
{
   // returns data set
   if (fDataSet==0 || fNeedsRebuilding) {
      if(fDataSet!=0) ClearDataSet();
      fDataSet = DataSetManager::Instance().CreateDataSet(GetName());
      fNeedsRebuilding = kFALSE;
   }
   return fDataSet;
}

//_______________________________________________________________________
void TMVA::DataSetInfo::Save()
{ 
   // histogramming
   UInt_t nvar = GetNVariables();
   
   // ====================
   // input expressions
   // ====================
   GetRootDir()->mkdir("input_expressions")->cd();
   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      TObjString o( fVariables[ivar].GetExpression().Data() );
      o.Write();
   }
   GetRootDir()->cd();
   
   // ====================
   // correlation matrices
   // ====================
   // workaround till the TMatrix templates are comonly used
   // this keeps backward compatibility
   TMatrixF mObj[2];
   mObj[0] = TMatrixF( nvar, nvar );
   mObj[1] = TMatrixF( nvar, nvar );
   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      for (UInt_t jvar=0; jvar<nvar; jvar++) {
         mObj[0](ivar, jvar) = (*CorrelationMatrix(Types::kSignal))(ivar,jvar);
         mObj[1](ivar, jvar) = (*CorrelationMatrix(Types::kBackground))(ivar,jvar);
      }
   }

   // settings
   const Float_t labelSize = 0.055;
   for (Int_t ic=0; ic<2; ic++) {
      
      TH2F* h2 = new TH2F( mObj[ic] );
      h2->SetNameTitle( Form("CorrelationMatrix%c", ic==0?'S':'B'),
                        Form("Correlation Matrix (%s)", ic==0?"signal":"background"));

      for (UInt_t ivar=0; ivar<nvar; ivar++) {
         h2->GetXaxis()->SetBinLabel( ivar+1, GetExpression(ivar) );
         h2->GetYaxis()->SetBinLabel( ivar+1, GetExpression(ivar) );
      }

      // present in percent, and round off digits
      // also, use absolute value of correlation coefficient (ignore sign)
      h2->Scale( 100.0  ); 
      for (UInt_t ibin=1; ibin<=nvar; ibin++)
         for (UInt_t jbin=1; jbin<=nvar; jbin++)
            h2->SetBinContent( ibin, jbin, Int_t(h2->GetBinContent( ibin, jbin )) );

      // style settings
      h2->SetStats( 0 );
      h2->GetXaxis()->SetLabelSize( labelSize );
      h2->GetYaxis()->SetLabelSize( labelSize );
      h2->SetMarkerSize( 1.5 );
      h2->SetMarkerColor( 0 );
      h2->LabelsOption( "d" ); // diagonal labels on x axis
      h2->SetLabelOffset( 0.011 );// label offset on x axis
      h2->SetMinimum( -100.0 );
      h2->SetMaximum( +100.0 );

      // -------------------------------------------------------------------------------------
      // just in case one wants to change the position of the color palette axis
      // -------------------------------------------------------------------------------------
      //     gROOT->SetStyle("Plain");
      //     TStyle* gStyle = gROOT->GetStyle( "Plain" );
      //     gStyle->SetPalette( 1, 0 );
      //     TPaletteAxis* paletteAxis 
      //                   = (TPaletteAxis*)h2->GetListOfFunctions()->FindObject( "palette" );
      // -------------------------------------------------------------------------------------

      // write to file
      h2->Write();
   
      delete h2;
   }
}
