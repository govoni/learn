// @(#)root/tmva $Id: TransformationHandler.cxx,v 1.6 2008/07/20 20:44:28 stelzer Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TransformationHandler                                                               *
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
#include <iomanip>

#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#ifndef ROOT_TMVA_Config
#include "TMVA/Config.h"
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
#ifndef ROOT_TMVA_Ranking
#include "TMVA/Ranking.h"
#endif
#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif
#ifndef ROOT_TMVA_TransformationHandler
#include "TMVA/TransformationHandler.h"
#endif
#ifndef ROOT_TMVA_VariableTransformBase
#include "TMVA/VariableTransformBase.h"
#endif

//_______________________________________________________________________
TMVA::TransformationHandler::TransformationHandler(DataSetInfo& dsi) 
   : fDataSetInfo(dsi),
     fRootBaseDir(0),
     fLogger( "TransformationHandler", kINFO )
{
   // constructor
   fVariableStats[0] = std::vector<TMVA::TransformationHandler::VariableStat>(dsi.GetNVariables());
   fVariableStats[1] = std::vector<TMVA::TransformationHandler::VariableStat>(dsi.GetNVariables());
   fVariableStats[2] = std::vector<TMVA::TransformationHandler::VariableStat>(dsi.GetNVariables());
}

//_______________________________________________________________________
TMVA::TransformationHandler::~TransformationHandler() 
{
   // destructor
}

//_______________________________________________________________________
void TMVA::TransformationHandler::AddTransformation(VariableTransformBase *trf) 
{
   fTransformations.Add(trf);
}

//_______________________________________________________________________
void TMVA::TransformationHandler::AddStats(UInt_t k, UInt_t ivar, Float_t mean, Float_t rms, Float_t min, Float_t max) 
{

   if (rms <= 0) {
      fLogger << kWARNING << "Variable \"" << Variable(ivar).GetExpression() 
              << "\" has zero or negative RMS^2 " 
              << "==> set to zero. Please check the variable content" << Endl;
      rms = 0;
   }

   VariableStat stat; stat.fMean = mean; stat.fRMS = rms; stat.fMin = min; stat.fMax = max;
   fVariableStats[k][ivar] = stat;
}

//_______________________________________________________________________
const TMVA::Event* TMVA::TransformationHandler::Transform(const Event* ev, Types::ESBType type) const 
{
   TListIter trIt(&fTransformations);
   const Event* trEv = ev;
   while( VariableTransformBase *trf = (VariableTransformBase*) trIt() ) {
      if(trf->IsCreated())
         trEv = trf->Transform(trEv, type);
      else break;
   }
   return trEv;
}

//_______________________________________________________________________
void TMVA::TransformationHandler::CalcTransformations(const std::vector<Event*>& events) 
{
   const std::vector<Event*>* tmpEvents = &events;

   Bool_t replaceColl = kFALSE;

   TListIter trIt(&fTransformations);
   while( VariableTransformBase *trf = (VariableTransformBase*) trIt() ) {
      trf->PrepareTransformation(*tmpEvents);
      tmpEvents = TransformCollection(trf, tmpEvents, replaceColl);
      replaceColl = kTRUE;
   }

   CalcStats(*tmpEvents);

   // plot the variables once in this transformation
   PlotVariables(*tmpEvents);

   if(replaceColl) {// then we have created a new vector
      for( UInt_t ievt = 0; ievt<tmpEvents->size(); ievt++)
         delete (*tmpEvents)[ievt];
      delete tmpEvents;
   }
   

//    // print ranking
//    trbase->PrintVariableRanking();

}

//_______________________________________________________________________
const std::vector<TMVA::Event*>*
TMVA::TransformationHandler::TransformCollection( VariableTransformBase* trf,
                                                  const std::vector<TMVA::Event*>* events,
                                                  Bool_t replace ) const 
{
   std::vector<TMVA::Event*>* tmpEvents = 0;
   if(replace) {
      tmpEvents = const_cast<std::vector<TMVA::Event*>*>(events);
   } else {
      tmpEvents = new std::vector<TMVA::Event*>(events->size());
   }
   for( UInt_t ievt = 0; ievt<events->size(); ievt++) {
      if(replace) {
         *(*tmpEvents)[ievt] = *trf->Transform((*events)[ievt]);
      } else {
         (*tmpEvents)[ievt] = new Event(*trf->Transform((*events)[ievt]));
      }
   }
   return tmpEvents;
}

//_______________________________________________________________________
void TMVA::TransformationHandler::CalcStats(const std::vector<Event*>& events)
{
   // method to calculate minimum, maximum, mean, and RMS for all
   // variables used in the MVA

   UInt_t nevts = events.size();

   if(nevts==0)
      fLogger << kFATAL << "No events available to find min, max, mean and rms" << Endl;

   // if transformation has not been succeeded, the tree may be empty
   const UInt_t nvar = events[0]->GetNVariables();

   Double_t sumOfWeights[2];
   Double_t *x2[2];
   Double_t *x0[2];
   Float_t  *varMin[2];
   Float_t  *varMax[2];
   
   for(UInt_t k=0; k<2; k++) {
      sumOfWeights[k]=0;
      x2[k]     = new Double_t[nvar];
      x0[k]     = new Double_t[nvar];
      varMin[k] = new Float_t[nvar];
      varMax[k] = new Float_t[nvar];
      for (UInt_t ivar=0; ivar<nvar; ivar++) {
         x0[k][ivar] = x2[k][ivar] = 0;
         varMin[k][ivar] = +1e15;
         varMax[k][ivar] = -1e15;
      }
   }

   for (UInt_t ievt=0; ievt<nevts; ievt++) {
      Event * ev = events[ievt];
      UInt_t k=ev->IsSignal()?0:1;

      Double_t weight = ev->GetWeight();
      sumOfWeights[k] += weight;
      for (UInt_t ivar=0; ivar<nvar; ivar++) {
         Double_t x = ev->GetVal(ivar);

         if (x < varMin[k][ivar]) varMin[k][ivar]= x;
         if (x > varMax[k][ivar]) varMax[k][ivar]= x;

         x0[k][ivar] += x*weight;
         x2[k][ivar] += x*x*weight;
      }
   }

   // set Mean and RMS
   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      UInt_t k=0;
      Double_t mean = x0[k][ivar]/sumOfWeights[k];
      Double_t rms = TMath::Sqrt( x2[k][ivar]/sumOfWeights[k] - mean*mean); 
      AddStats(k, ivar, mean, rms, varMin[k][ivar], varMax[k][ivar]);

      k=1;
      mean = x0[k][ivar]/sumOfWeights[k];
      rms = TMath::Sqrt( x2[k][ivar]/sumOfWeights[k] - mean*mean); 
      AddStats(k, ivar, mean, rms, varMin[k][ivar], varMax[k][ivar]);

      mean = (x0[0][ivar]+x0[1][ivar])/(sumOfWeights[0]+sumOfWeights[1]);
      rms = TMath::Sqrt( (x2[0][ivar]+x2[1][ivar])/(sumOfWeights[0]+sumOfWeights[1]) - mean*mean); 
      AddStats(2, ivar, mean, rms,
               TMath::Min(varMin[0][ivar],varMin[1][ivar]), 
               TMath::Max(varMax[0][ivar],varMax[1][ivar]));
   }



   fLogger << kVERBOSE << "Set mean,rms,min,max for variables to: " << Endl;
   fLogger << std::setprecision(3);
   for (UInt_t ivar=0; ivar<nvar; ivar++)
      fLogger << "    " << GetMean(ivar) << ", " << GetRMS(ivar)
              << "\t: [" << GetMin(ivar) << "\t, " << GetMax(ivar) << "\t] " << Endl;
   fLogger << std::setprecision(5); // reset to better value       
}

//_______________________________________________________________________
void TMVA::TransformationHandler::MakeFunction(std::ostream& fout, const TString& fncName, Int_t part) const 
{
   // create transformation function
   TListIter trIt(&fTransformations);
   UInt_t trCounter=1;
   while( VariableTransformBase *trf = (VariableTransformBase*) trIt() ) {
      trf->MakeFunction(fout, fncName, part, trCounter++);
   }
   if(part==1) {
      for(Int_t i=0; i<fTransformations.GetSize(); i++) {
         fout << "   void InitTransform_"<<i+1<<"();" << std::endl;
         fout << "   void Transform_"<<i+1<<"( std::vector<double> & iv, int sigOrBgd ) const;" << std::endl;
      }
   }
   if(part==2) {
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fncName << "::InitTransform()" << std::endl;
      fout << "{" << std::endl;
      for(Int_t i=0; i<fTransformations.GetSize(); i++)
         fout << "   InitTransform_"<<i+1<<"();" << std::endl;
      fout << "}" << std::endl;
      fout << std::endl;
      fout << "//_______________________________________________________________________" << std::endl;
      fout << "inline void " << fncName << "::Transform( std::vector<double>& iv, int sigOrBgd ) const" << std::endl;
      fout << "{" << std::endl;
      for(Int_t i=0; i<fTransformations.GetSize(); i++)
         fout << "   Transform_"<<i+1<<"( iv, sigOrBgd );" << std::endl;

      fout << "}" << std::endl;
   }
}

TString
TMVA::TransformationHandler::GetName() {
   TString name("Empty");
   TListIter trIt(&fTransformations);
   VariableTransformBase *trf;
   if( (trf = (VariableTransformBase*) trIt()) ) {
      name = TString(trf->GetShortName());
      while( (trf = (VariableTransformBase*) trIt()) )
         name += "_" + TString(trf->GetShortName());
   }
   return name;
}


//_______________________________________________________________________
void TMVA::TransformationHandler::PlotVariables( const std::vector<Event*>& events )
{
   // create histograms from the input variables
   // - histograms for all input variables
   // - scatter plots for all pairs of input variables

   if(fRootBaseDir==0) return;

   // extension for transformation type
   TString transfType = "_";
   transfType += GetName();

   const UInt_t nvar = fDataSetInfo.GetNVariables();
   
   // Create all histograms
   // do both, scatter and profile plots
   std::vector<TH1F*> vS ( nvar );
   std::vector<TH1F*> vB ( nvar );
   std::vector<std::vector<TH2F*> >     mycorrS( nvar );
   std::vector<std::vector<TH2F*> >     mycorrB( nvar );
   std::vector<std::vector<TProfile*> > myprofS( nvar );
   std::vector<std::vector<TProfile*> > myprofB( nvar );
   for (UInt_t ivar=0; ivar < nvar; ivar++) {
      mycorrS[ivar].resize(nvar);
      mycorrB[ivar].resize(nvar);
      myprofS[ivar].resize(nvar);
      myprofB[ivar].resize(nvar);
   }

   // if there are too many input variables, the creation of correlations plots blows up
   // memory and basically kills the TMVA execution
   // --> avoid above critical number (which can be user defined)
   if (nvar > (UInt_t)gConfig().GetVariablePlotting().fMaxNumOfAllowedVariablesForScatterPlots) {
      Int_t nhists = nvar*(nvar - 1)/2;
      fLogger << kINFO << gTools().Color("dgreen") << Endl;
      fLogger << kINFO << "<PlotVariables> Will not produce scatter plots ==> " << Endl;
      fLogger << kINFO
              << "|  The number of " << nvar << " input variables would require " << nhists << " two-dimensional" << Endl;
      fLogger << kINFO
              << "|  histograms, which would occupy the computer's memory. Note that this" << Endl;
      fLogger << kINFO
              << "|  suppression does not have any consequences for your analysis, other" << Endl;
      fLogger << kINFO
              << "|  than not disposing of these scatter plots. You can modify the maximum" << Endl;
      fLogger << kINFO
              << "|  number of input variables allowed to generate scatter plots in your" << Endl; 
      fLogger << "|  script via the command line:" << Endl;
      fLogger << kINFO
              << "|  \"(TMVA::gConfig().GetVariablePlotting()).fMaxNumOfAllowedVariablesForScatterPlots = <some int>;\""
              << gTools().Color("reset") << Endl;
      fLogger << Endl;
      fLogger << kINFO << "Some more output" << Endl;
   }

   Float_t timesRMS  = gConfig().GetVariablePlotting().fTimesRMS;
   UInt_t  nbins1D   = gConfig().GetVariablePlotting().fNbins1D;
   UInt_t  nbins2D   = gConfig().GetVariablePlotting().fNbins2D;
   for (UInt_t ivar=0; ivar<nvar; ivar++) {
      TString myVari = Variable(ivar).GetInternalVarName();  
      Float_t meanS = fVariableStats[0][ivar].fMean;
      Float_t meanB = fVariableStats[1][ivar].fMean;
      Float_t rmsS  = fVariableStats[0][ivar].fRMS;
      Float_t rmsB  = fVariableStats[1][ivar].fRMS;

      // choose reasonable histogram ranges, by removing outliers
      if (Variable(ivar).GetVarType() == 'I') {
         // special treatment for integer variables
         Int_t xmin = TMath::Nint( GetMin(ivar) );
         Int_t xmax = TMath::Nint( GetMax(ivar) + 1 );
         Int_t nbins = xmax - xmin;

         vS[ivar] = new TH1F( Form("%s__S%s", myVari.Data(), transfType.Data()), Variable(ivar).GetExpression(), nbins, xmin, xmax );
         vB[ivar] = new TH1F( Form("%s__B%s", myVari.Data(), transfType.Data()), Variable(ivar).GetExpression(), nbins, xmin, xmax );
      }
      else {
         Double_t xmin = TMath::Max( GetMin(ivar), TMath::Min( meanS - timesRMS*rmsS, meanB - timesRMS*rmsB ) );
         Double_t xmax = TMath::Min( GetMax(ivar), TMath::Max( meanS + timesRMS*rmsS, meanB + timesRMS*rmsB ) );
      
         // protection
         if (xmin >= xmax) xmax = xmin*1.1; // try first...
         if (xmin >= xmax) xmax = xmin + 1; // this if xmin == xmax == 0

         vS[ivar] = new TH1F( Form("%s__S%s", myVari.Data(), transfType.Data()), Variable(ivar).GetExpression(), nbins1D, xmin, xmax );
         vB[ivar] = new TH1F( Form("%s__B%s", myVari.Data(), transfType.Data()), Variable(ivar).GetExpression(), nbins1D, xmin, xmax );         
      }

      vS[ivar]->SetXTitle(Variable(ivar).GetExpression());
      vB[ivar]->SetXTitle(Variable(ivar).GetExpression());
      vS[ivar]->SetLineColor(4);
      vB[ivar]->SetLineColor(2);
   
      // profile and scatter plots
      if (nvar <= (UInt_t)gConfig().GetVariablePlotting().fMaxNumOfAllowedVariablesForScatterPlots) {

         for (UInt_t j=ivar+1; j<nvar; j++) {
            TString myVarj = Variable(j).GetInternalVarName();  
            
            mycorrS[ivar][j] = new TH2F( Form( "scat_%s_vs_%s_sig%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                         Form( "%s versus %s (signal)%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                         nbins2D, Variable(ivar).GetMin(), Variable(ivar).GetMax(), 
                                         nbins2D, Variable(j).GetMin(), Variable(j).GetMax() );
            mycorrS[ivar][j]->SetXTitle(Variable(ivar).GetExpression());
            mycorrS[ivar][j]->SetYTitle(Variable(j).GetExpression());
            mycorrB[ivar][j] = new TH2F( Form( "scat_%s_vs_%s_bgd%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                         Form( "%s versus %s (background)%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                         nbins2D, Variable(ivar).GetMin(), Variable(ivar).GetMax(), 
                                         nbins2D, Variable(j).GetMin(), Variable(j).GetMax() );
            mycorrB[ivar][j]->SetXTitle(Variable(ivar).GetExpression());
            mycorrB[ivar][j]->SetYTitle(Variable(j).GetExpression());
         
            myprofS[ivar][j] = new TProfile( Form( "prof_%s_vs_%s_sig%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                             Form( "profile %s versus %s (signal)%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                             nbins1D, Variable(ivar).GetMin(), Variable(ivar).GetMax() );
            myprofB[ivar][j] = new TProfile( Form( "prof_%s_vs_%s_bgd%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                             Form( "profile %s versus %s (background)%s", myVarj.Data(), myVari.Data(), transfType.Data() ), 
                                             nbins1D, Variable(ivar).GetMin(), Variable(ivar).GetMax() );
         }
      }   
   }

   UInt_t nevts = events.size();

   // fill the histograms (this approach should be faster than individual projection
   for (UInt_t ievt=0; ievt<nevts; ievt++) {

      const Event* ev = events[ievt];

      Float_t weight = ev->GetWeight();

      for (UInt_t ivar=0; ivar<nvar; ivar++) {
         Float_t vali = ev->GetVal(ivar);

         // variable histos
         if (ev->IsSignal()) vS[ivar]->Fill( vali, weight );
         else                vB[ivar]->Fill( vali, weight );
         
         // correlation histos
         if (nvar <= (UInt_t)gConfig().GetVariablePlotting().fMaxNumOfAllowedVariablesForScatterPlots) {

            for (UInt_t j=ivar+1; j<nvar; j++) {
               Float_t valj = ev->GetVal(j);
               if (ev->IsSignal()) {
                  mycorrS[ivar][j]->Fill( vali, valj, weight );
                  myprofS[ivar][j]->Fill( vali, valj, weight );
               }
               else {
                  mycorrB[ivar][j]->Fill( vali, valj, weight );
                  myprofB[ivar][j]->Fill( vali, valj, weight );
               }
            }
         }
      }
   }
      
   //    // computes ranking of input variables
   //    fRanking = new Ranking( GetName(), "Separation" );
   //    for (UInt_t i=0; i<nvar; i++) {   
   //       Double_t sep = gTools().GetSeparation( vS[i], vB[i] );
   //       fRanking->AddRank( *new Rank( vS[i]->GetTitle(), sep ) );
   //    }

   // write histograms

   // create directory in root dir
   fRootBaseDir->cd();
   TString outputDir = TString("InputVariables");

   TListIter trIt(&fTransformations);
   while( VariableTransformBase *trf = (VariableTransformBase*) trIt() )
      outputDir += "_" + TString(trf->GetShortName());

   TObject* o = fRootBaseDir->FindObject(outputDir);
   if (o != 0) {
      fLogger << kFATAL << "A " << o->ClassName() << " with name " << o->GetName() << " already exists in " 
              << fRootBaseDir->GetPath() << "("<<outputDir<<")" << Endl;
   }
   TDirectory* localDir = fRootBaseDir->mkdir( outputDir );
   localDir->cd();

   fLogger << kVERBOSE << "Create and switch to directory " << localDir->GetPath() << Endl;
   for (UInt_t i=0; i<nvar; i++) {
      vS[i]->Write();
      vB[i]->Write();
      vS[i]->SetDirectory(0);
      vB[i]->SetDirectory(0);
   }

   // correlation plots have dedicated directory
   if (nvar <= (UInt_t)gConfig().GetVariablePlotting().fMaxNumOfAllowedVariablesForScatterPlots) {

      localDir = localDir->mkdir( "CorrelationPlots" );
      localDir ->cd();
      fLogger << kINFO << "Create scatter and profile plots in target-file directory: " << Endl;
      fLogger << kINFO << localDir->GetPath() << Endl;
   
      for (UInt_t i=0; i<nvar; i++) {
         for (UInt_t j=i+1; j<nvar; j++) {
            mycorrS[i][j]->Write();
            mycorrB[i][j]->Write();
            myprofS[i][j]->Write();
            myprofB[i][j]->Write();
            mycorrS[i][j]->SetDirectory(0);
            mycorrB[i][j]->SetDirectory(0);
            myprofS[i][j]->SetDirectory(0);
            myprofB[i][j]->SetDirectory(0);
         }
      }         
   }

   fRootBaseDir->cd();
}








//_______________________________________________________________________
void TMVA::TransformationHandler::WriteToStream( std::ostream& o ) const 
{
   // write transformatino to stream
   TListIter trIt(&fTransformations);
   o << "NTransformtations " << fTransformations.GetSize() << std::endl << std::endl;
   UInt_t i = 1;
   while( VariableTransformBase *trf = (VariableTransformBase*) trIt() ) {
      o << "#TR -*-*-*-*-*-*-* transformation " << i++ << ": " << trf->GetName() << " -*-*-*-*-*-*-*-" << std::endl;
      trf->WriteTransformationToStream(o);
   }   
}

//_______________________________________________________________________
void TMVA::TransformationHandler::ReadFromStream( std::istream& ) 
{
   // TODO
}


//_______________________________________________________________________
void TMVA::TransformationHandler::PrintVariableRanking() const
{
   // prints ranking of input variables
   fLogger << kINFO << "Ranking input variables..." << Endl;
   fRanking->Print();
}
