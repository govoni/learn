// @(#)root/tmva $Id: MethodBoost.cxx,v 1.7 2008/07/12 20:21:39 stelzer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss, Kai Voss,Or Cohen 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodCompositeBase                                                   *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Virtual base class for all MVA method                                     *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker <Andreas.Hocker@cern.ch> - CERN, Switzerland              *
 *      Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN, Switzerland              *
 *      Helge Voss      <Helge.Voss@cern.ch>     - MPI-K Heidelberg, Germany      *
 *      Kai Voss        <Kai.Voss@cern.ch>       - U. of Victoria, Canada         *
 *      Or Cohen        <orcohenor@gmail.com>    - Weizmann Inst., Israel         *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      U. of Victoria, Canada                    #include "TMVA/Timer.h"                                * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_______________________________________________________________________
//
// This class is meant to boost a single classifier. Boosting means    //
// training the classifier a few times. Everytime the wieghts of the   //
// events are modified according to how well the classifier performed  //
// on the test sample.                                                 //
//_______________________________________________________________________
#include <algorithm>
#include <iomanip>
#include <vector>

#include "Riostream.h"
#include "TRandom.h"
#include "TMath.h"
#include "TObjString.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TSpline.h"
#include "TDirectory.h"

#include "TMVA/MethodCompositeBase.h"
#include "TMVA/MethodBase.h"
#include "TMVA/MethodBoost.h"
#include "TMVA/Tools.h"
#include "TMVA/ClassifierFactory.h"
#include "TMVA/Timer.h"
#include "TMVA/Types.h"
#include "TMVA/PDF.h"
using std::vector;

REGISTER_METHOD(Boost)

ClassImp(TMVA::MethodBoost)

const Int_t  NBIN_HIST_PLOT   = 100;


//_______________________________________________________________________
TMVA::MethodBoost::MethodBoost( const TString& jobName,
                                const TString& methodTitle,
                                DataSetInfo& theData,
                                const TString& theOption,
                                TDirectory* theTargetDir )
   : TMVA::MethodCompositeBase( jobName, methodTitle, theData, theOption, theTargetDir )
{
   // common initialisation with defaults for the BDT-Method
   SetMethodName( "Boost" );
   SetMethodType( Types::kBoost );   DeclareOptionRef(fMethodWeightType, "MethodWeightType", "How to set the final weight of the boosted classifiers");
   SetTestvarName();

   DeclareOptions();
   ParseOptions();
   ProcessOptions();
   fBoostedMethodTitle = methodTitle;
   fBoostedMethodOptions = GetOptions();
   InitBoost();

}

//_______________________________________________________________________
TMVA::MethodBoost::MethodBoost( DataSetInfo& dsi,
                                const TString& theWeightFile,
                                TDirectory* theTargetDir )
   : TMVA::MethodCompositeBase( dsi, theWeightFile, theTargetDir )
{
   // common initialisation with defaults for the BDT-Method

   // needs to be fixed (MethodBoost from Reader) // Joerg 12.7.2008

//    SetMethodName( "Boost" );
//    SetMethodType( Types::kBoost );
//    DeclareOptionRef(fMethodWeightType, "MethodWeightType", "How to set the final weight of the boosted classifiers");
//    SetTestvarName();

//    DeclareOptions();
//    ParseOptions();
//    ProcessOptions();
//    fBoostedMethodTitle = methodTitle;
//    fBoostedMethodOptions = GetOptions();
//    InitBoost();

}

//_______________________________________________________________________
TMVA::MethodBoost::~MethodBoost( void )
{
   // destructor
   fMethodWeight.clear();
   //the histogram themselves are deleted when the file is closed
   if (fMonitorHist != 0) delete fMonitorHist;
   fTrainSigMVAHist.clear();
   fTrainBgdMVAHist.clear();
   fBTrainSigMVAHist.clear();
   fBTrainBgdMVAHist.clear();
   fTestSigMVAHist.clear();
   fTestBgdMVAHist.clear();
}

//_______________________________________________________________________
void TMVA::MethodBoost::DeclareOptions()
{
   fADABoostBeta = 1.0;
   fBoostType  = "AdaBoost";
   fMethodWeightType = "ByError";
   DeclareOptionRef( fBoostNum = 1, "BoostNum",
                     "Number of times the classifier is boosted");
   
   DeclareOptionRef( fMonitorBoostedMethod = kTRUE, "MonitorBoostedMethod",
                     "Whether to write monitoring histogram for each boosted classifier");
   DeclareOptionRef(fBoostType, "BoostType", "Boosting type for the classifiers");
   AddPreDefVal(TString("AdaBoost"));
   AddPreDefVal(TString("Bagging"));
   DeclareOptionRef(fMethodWeightType, "MethodWeightType", "How to set the final weight of the boosted classifiers");
   DeclareOptionRef(fRecalculateMVACut = kTRUE, "RecalculateMVACut", "Whether to recalculate the classifier MVA Signallike cut at every boost iteration");
   AddPreDefVal(TString("ByError"));
   AddPreDefVal(TString("Average"));
   AddPreDefVal(TString("LastMethod"));
   DeclareOptionRef(fADABoostBeta = 1.0, "AdaBoostBeta", "The ADA boost parameter that sets the effect of every boost step on the events' weights");
   

   TMVA::MethodCompositeBase::fMethods.reserve(fBoostNum);
}
//_______________________________________________________________________
Bool_t TMVA::MethodBoost::BookMethod( Types::EMVA theMethod, TString methodTitle, TString theOption ) 
{
   //just registering the string from which the boosted classifier will be created
   fBoostedMethodName = Types::Instance().GetMethodName( theMethod );
   fBoostedMethodTitle = methodTitle;
   fBoostedMethodOptions = theOption;
   return kTRUE;
}

//_______________________________________________________________________
void TMVA::MethodBoost::InitBoost()
{
   fMonitorHist=new vector<TH1*>();
   fMonitorHist->push_back(new TH1F("MethodWeight","Normalized Classifier Weight",fBoostNum,0,fBoostNum));
   fMonitorHist->push_back(new TH1F("BoostWeight","Boost Weight",fBoostNum,0,fBoostNum));
   fMonitorHist->push_back(new TH1F("ErrFraction","Error Fraction (by boosted event weights)",fBoostNum,0,fBoostNum));
   fMonitorHist->push_back(new TH1F("OrigErrFraction","Error Fraction (by original event weights)",fBoostNum,0,fBoostNum));
   fDefaultHistNum = fMonitorHist->size();
   (*fMonitorHist)[0]->GetXaxis()->SetTitle("Index of boosted classifier");
   (*fMonitorHist)[0]->GetYaxis()->SetTitle("Classifier Weight");
   (*fMonitorHist)[1]->GetXaxis()->SetTitle("Index of boosted classifier");
   (*fMonitorHist)[1]->GetYaxis()->SetTitle("Boost Weight");
   (*fMonitorHist)[2]->GetXaxis()->SetTitle("Index of boosted classifier");
   (*fMonitorHist)[2]->GetYaxis()->SetTitle("Error Fraction");
   (*fMonitorHist)[3]->GetXaxis()->SetTitle("Index of boosted classifier");
   (*fMonitorHist)[3]->GetYaxis()->SetTitle("Error Fraction");

   fMonitorTree= new TTree("MonitorBoost","Boost variables");
   fMonitorTree->Branch("iMethod",&fMethodIndex,"iMethod/I");
   fMonitorTree->Branch("boostWeight",&fBoostWeight,"boostWeight/D");
   fMonitorTree->Branch("errorFraction",&fMethodError,"errorFraction/D");
   fMonitorBoostedMethod = kTRUE;
}
//_______________________________________________________________________
void TMVA::MethodBoost::Train()
{
   Double_t AllMethodsWeight=0;
   TDirectory* methodDir=0; 
   TString dirName,dirTitle;
   Int_t StopCounter=0;

   if (Data()->GetNTrainingEvents()==0) fLogger << kFATAL << "<Train> Data() has zero events" << Endl;
   Data()->SetCurrentType(Types::kTraining);

   if (fMethods.size() > 0) fMethods.clear();

   fLogger << kINFO << "Training "<< fBoostNum << " " << fBoostedMethodName << " Classifiers ... patience please" << Endl;
   Timer timer( fBoostNum, GetName() );

   ResetBoostWeights();

   //training and boosting the classifiers
   for (fMethodIndex=0;fMethodIndex<fBoostNum;fMethodIndex++)
   {
      //the first classifier shows the option string output, the rest not
      if (fMethodIndex>0) TMVA::MsgLogger::SupressOutput();
      IMethod* method = 
         ClassifierFactory::Instance().Create(std::string(fBoostedMethodName),
                                              GetJobName(),
                                              Form("%s_B%04i",
                                                   fBoostedMethodName.Data(),fMethodIndex),
                                              DataInfo(),
                                              fBoostedMethodOptions);
      TMVA::MsgLogger::UnsupressOutput();
      //supressing the rest of the classifier output the right way
      dynamic_cast<MethodBase*>(method)->SetMsgType(kWARNING);

      //creating the directory of the classifier
      if (fMonitorBoostedMethod)
      {
         methodDir=MethodBaseDir()->GetDirectory(dirName=Form("%s_B%04i",fBoostedMethodName.Data(),fMethodIndex));
         if (methodDir==0)
            methodDir=BaseDir()->mkdir(dirName,dirTitle=Form("Directory Boosted %s #%04i", fBoostedMethodName.Data(),fMethodIndex));
         dynamic_cast<MethodBase*>(method)->SetMethodDir(methodDir);
         dynamic_cast<MethodBase*>(method)->BaseDir()->cd();
      }

      //training
      TMVA::MethodCompositeBase::fMethods.push_back(method);
      timer.DrawProgressBar( fMethodIndex );
      if (fMethodIndex==0) method->MonitorBoost(SetStage(Types::kBoostProcBegin));
      method->MonitorBoost(SetStage(Types::kBeforeTraining));
      TMVA::MsgLogger::SupressOutput(); //supressing Logger outside the method
      SingleTrain();
      TMVA::MsgLogger::UnsupressOutput();
      method->WriteMonitoringHistosToFile();
      if (fMethodIndex==0 && fMonitorBoostedMethod) CreateMVAHistorgrams();

      //boosting
      method->MonitorBoost(SetStage(Types::kBeforeBoosting));
      SingleBoost();
      method->MonitorBoost(SetStage(Types::kAfterBoosting));
      (*fMonitorHist)[1]->SetBinContent(fMethodIndex+1,fBoostWeight);
      (*fMonitorHist)[2]->SetBinContent(fMethodIndex+1,fMethodError);
      (*fMonitorHist)[3]->SetBinContent(fMethodIndex+1,fOrigMethodError);
      AllMethodsWeight += fMethodWeight.back();
      fMonitorTree->Fill();
      //stop boosting if needed when error has reached 0.5
      //thought of counting a few steps, but it doesn't seem to be necessary
      if (fMethodError > 0.499) StopCounter++; 
      if (StopCounter > 0 && fBoostType == "AdaBoost")
      {
         timer.DrawProgressBar( fBoostNum );
         fBoostNum = fMethodIndex+1; 
         fLogger << kINFO << "Error rate has reached 0.5, boosting process stopped at #" << fBoostNum << " classifier" << Endl;
         if (fBoostNum < 5)
            fLogger << kINFO << "The classifier might be too strong to boost with Beta = " << fADABoostBeta << ", try reducing it." <<Endl;
         for (Int_t i=0;i<fDefaultHistNum;i++)
            (*fMonitorHist)[i]->SetBins(fBoostNum,0,fBoostNum);
         break;
      }
   }
   if (fMethodWeightType == "LastMethod")
   {fMethodWeight.back() = AllMethodsWeight = 1.0;}

   fLogger << kINFO << "<Train> elapsed time: " << timer.GetElapsedTime()    
         << "                              " << Endl;

   ResetBoostWeights();
   Timer* timer1=new Timer();
   //normalizing the weights of the classifiers
   for (fMethodIndex=0;fMethodIndex<fBoostNum;fMethodIndex++)
   {
      //pefroming post-boosting actions
      if ( fMethods[fMethodIndex]->MonitorBoost(SetStage(Types::kBoostValidation)))
      {
         if (fMethodIndex==0) timer1=new Timer( fBoostNum, GetName() );
         timer1->DrawProgressBar( fMethodIndex );
         if (fMethodIndex==fBoostNum)
            fLogger << kINFO << "Elapsed time: " << timer1->GetElapsedTime() 
                  << "                              " << Endl;
      }
      fMethodWeight[fMethodIndex] = fMethodWeight[fMethodIndex] / AllMethodsWeight;
      (*fMonitorHist)[0]->SetBinContent(fMethodIndex+1,fMethodWeight[fMethodIndex]);
   }
   fMethods.back()->MonitorBoost(SetStage(Types::kBoostProcEnd));

}

//_______________________________________________________________________
void TMVA::MethodBoost::CreateMVAHistorgrams()
{
   //calculating histograms boundries and creating histograms..
   // nrms = number of rms around the average to use for outline (of the 0 classifier)
   Double_t meanS, meanB, rmsS, rmsB, xmin, xmax, nrms = 10;
   std::vector <Float_t>* mvaRes = new std::vector <Float_t>(Data()->GetNEvents());
   for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
   {
      Data()->GetEvent(ievt);
      (*mvaRes)[ievt] = fMethods[0]->GetMvaValue();
   }
   gTools().ComputeStat( Data()->GetEventCollection(), mvaRes, meanS, meanB, rmsS, rmsB, xmin, xmax );
   Int_t fNbins = NBIN_HIST_PLOT;
   xmin = TMath::Max( TMath::Min(meanS - nrms*rmsS, meanB - nrms*rmsB ), xmin );
   xmax = TMath::Min( TMath::Max(meanS + nrms*rmsS, meanB + nrms*rmsB ), xmax ) + 0.0001;
   //creating all the historgrams
   for (Int_t imtd=0; imtd<fBoostNum; imtd++)
   {
      fTrainSigMVAHist.push_back(new TH1F( Form("MVA_Train_S_%04i",imtd), "MVA_Train_S", fNbins, xmin, xmax ));
      fTrainBgdMVAHist.push_back(new TH1F( Form("MVA_Train_B%04i",imtd), "MVA_Train_B", fNbins, xmin, xmax ));
      fBTrainSigMVAHist.push_back(new TH1F( Form("MVA_BTrain_S%04i",imtd), "MVA_BoostedTrain_S", fNbins, xmin, xmax ));
      fBTrainBgdMVAHist.push_back(new TH1F( Form("MVA_BTrain_B%04i",imtd), "MVA_BoostedTrain_B", fNbins, xmin, xmax ));
      fTestSigMVAHist.push_back(new TH1F( Form("MVA_Test_S%04i",imtd), "MVA_Test_S", fNbins, xmin, xmax ));
      fTestBgdMVAHist.push_back(new TH1F( Form("MVA_Test_B%04i",imtd), "MVA_Test_B", fNbins, xmin, xmax ));
   }
   mvaRes->clear();
}

//_______________________________________________________________________
void TMVA::MethodBoost::ResetBoostWeights()
{
   //resetting back the boosted weights of the events to 1
   for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
   {
      Event *ev = Data()->GetEvent(ievt);
      ev->SetBoostWeight( 1.0 );
   }
}
//_______________________________________________________________________
void TMVA::MethodBoost::WriteMonitoringHistosToFile( void ) const
{
   TDirectory* dir=0;
   if (fMonitorBoostedMethod)
   {
      for (Int_t imtd=0;imtd<fBoostNum;imtd++)
      {
         //writing the histograms in the specific classifier's directory
         dir = dynamic_cast<MethodBase*>(fMethods[imtd])->BaseDir();
         dir->cd();
         fTrainSigMVAHist[imtd]->SetDirectory(dir);
         fTrainSigMVAHist[imtd]->Write();
         fTrainBgdMVAHist[imtd]->SetDirectory(dir);
         fTrainBgdMVAHist[imtd]->Write();
         fBTrainSigMVAHist[imtd]->SetDirectory(dir);
         fBTrainSigMVAHist[imtd]->Write();
         fBTrainBgdMVAHist[imtd]->SetDirectory(dir);
         fBTrainBgdMVAHist[imtd]->Write();
      }
   }
   //going back to the original folder
   BaseDir()->cd();
   for (UInt_t i=0;i<fMonitorHist->size();i++)
   {
      ((*fMonitorHist)[i])->SetName(Form("Booster_%s",((*fMonitorHist)[i])->GetName()));
      ((*fMonitorHist)[i])->Write();
   }
   fMonitorTree->Write();
}

//_______________________________________________________________________
void TMVA::MethodBoost::Test()
{
   MethodBase::Test();
   if (fMonitorBoostedMethod)
   {
      //running over all the events and populating the test MVA histograms
      Data()->SetCurrentType(Types::kTesting);
      for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
      {
         Event* ev = Data()->GetEvent(ievt);
         Float_t w = ev->GetWeight();
         if (ev->IsSignal())
            for (Int_t imtd=0; imtd<fBoostNum; imtd++)
               fTestSigMVAHist[imtd]->Fill(fMethods[imtd]->GetMvaValue(),w);
         else
            for (Int_t imtd=0; imtd<fBoostNum; imtd++)
               fTestBgdMVAHist[imtd]->Fill(fMethods[imtd]->GetMvaValue(),w);
      }
      Data()->SetCurrentType(Types::kTraining);
   }
}

//_______________________________________________________________________
void TMVA::MethodBoost::WriteEvaluationHistosToFile()
{
   MethodBase::WriteEvaluationHistosToFile();
   if (fMonitorBoostedMethod)
   {
      TDirectory* dir=0;
      for (Int_t imtd=0;imtd<fBoostNum;imtd++)
      {
         //writing the histograms in the specific classifier's directory
         dir = dynamic_cast<MethodBase*>(fMethods[imtd])->BaseDir();
         dir->cd();
         fTestSigMVAHist[imtd]->SetDirectory(dir);
         fTestSigMVAHist[imtd]->Write();
         fTestBgdMVAHist[imtd]->SetDirectory(dir);
         fTestBgdMVAHist[imtd]->Write();
      }
   }
}
//_______________________________________________________________________
void TMVA::MethodBoost::ProcessOptions() 
{
   // process user options
   MethodBase::ProcessOptions();
}

//_______________________________________________________________________
void TMVA::MethodBoost::SingleTrain()
{
   // initialization 
   Data()->SetCurrentType(Types::kTraining);
   IMethod* method =  GetLastMethod();
   method->Train();
}

//_______________________________________________________________________
void TMVA::MethodBoost::FindMVACut()
{
   MethodBase* method=dynamic_cast<MethodBase*>(fMethods.back());
   if (method->GetMethodType() == Types::kDT
       || method->GetMethodType() == Types::kLikelihood) return;
   if (!fRecalculateMVACut && fMethodIndex>0)
      method->SetSignalReferenceCut(dynamic_cast<MethodBase*>(fMethods[0])->GetSignalReferenceCut());
   else
   {
      //creating a fine histograms containing the MVA values of the events
      Double_t* SigVal=new Double_t[Data()->GetNEvtSigTrain()];
      Double_t* BgdVal=new Double_t[Data()->GetNEvtBkgdTrain()];
      Double_t* SigWgt=new Double_t[Data()->GetNEvtSigTrain()];
      Double_t* BgdWgt=new Double_t[Data()->GetNEvtBkgdTrain()];
      Double_t SigMin=0,SigMax=0,BgdMin=0,BgdMax=0,sumSig=0,sumBgd=0;
      Int_t cSig=0,cBgd=0;
      for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
      {
         if (Data()->GetEvent(ievt)->IsSignal())
         {
            SigVal[cSig]=method->GetMvaValue(); 
            SigWgt[cSig]=GetEvent(ievt)->GetWeight(); 
            if (cSig==0) SigMin=SigMax=SigVal[cSig];
            SigMin=TMath::Min(SigMin,SigVal[cSig]);
            SigMax=TMath::Max(SigMax,SigVal[cSig]);
            sumSig+=SigWgt[cSig];
            cSig++;
         }
         else
         {
            BgdVal[cBgd]=method->GetMvaValue(); 
            BgdWgt[cBgd]=GetEvent(ievt)->GetWeight();
            if (cBgd==0) BgdMin=BgdMax=BgdVal[cBgd];
            BgdMin=TMath::Min(BgdMin,BgdVal[cBgd]);
            BgdMax=TMath::Max(BgdMax,BgdVal[cBgd]);
            sumBgd+=BgdWgt[cBgd];
            cBgd++;
         }
      }
      Int_t nBins = Data()->GetNEvents() / 10;
      TH1F* pdfSig=new TH1F("SigMVAPDF","SigMVAPDF",nBins,TMath::Min(SigMin,BgdMin),TMath::Max(SigMax,BgdMax));
      TH1F* pdfBgd = new TH1F("BgdMVAPDF","BgdMVAPDF",nBins,TMath::Min(SigMin,BgdMin),TMath::Max(SigMax,BgdMax));
      pdfSig->FillN(cSig,SigVal,SigWgt);
      pdfBgd->FillN(cBgd,BgdVal,BgdWgt);

      //Summing over the two Histo's until the sum reaches 1.. the means signal eff. = background rej.
      Double_t sum=-1.0,tmp=0; Int_t i;
      for ( i=0 ; i < nBins && tmp*sum >= 0 ; i++)
      { tmp=sum;sum += pdfSig->GetBinContent(i+1)/sumSig+pdfBgd->GetBinContent(i+1)/sumBgd;}
      method->SetSignalReferenceCut(pdfSig->GetBinCenter(i+1)-pdfSig->GetBinWidth(i+1)/2);

      delete pdfSig; delete pdfBgd;
      delete SigVal; delete SigWgt;
      delete BgdVal; delete BgdWgt;

      //fLogger << kINFO << "<FindMVACut> MVA cut value calculated : " << method->GetSignalReferenceCut() << Endl;
   }
}

//_______________________________________________________________________
void TMVA::MethodBoost::SingleBoost()
{
   MethodBase* method =  dynamic_cast<MethodBase*>(fMethods.back());
   Event * ev; Float_t w,v,wo; Bool_t sig=kTRUE;
   Double_t sumAll=0, sumWrong=0, sumAllOrig=0, sumWrongOrig=0, sumAll1=0;
   Bool_t* WrongDetection=new Bool_t[Data()->GetNEvents()];
   //finding the MVA cut value for IsSignalLike, stored in the method
   FindMVACut();
   //finding the wrong events and calculating their total weights
   for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
   {
      ev = Data()->GetEvent(ievt);
      sig=ev->IsSignal();
      v = method->GetMvaValue();
      w = ev->GetWeight();
      wo = ev->GetOriginalWeight();
      if (sig && fMonitorBoostedMethod)
      {
         fBTrainSigMVAHist[fMethodIndex]->Fill(v,w);
         fTrainSigMVAHist[fMethodIndex]->Fill(v,ev->GetOriginalWeight());
      }
      else if (fMonitorBoostedMethod)
      {
         fBTrainBgdMVAHist[fMethodIndex]->Fill(v,w);
         fTrainBgdMVAHist[fMethodIndex]->Fill(v,ev->GetOriginalWeight());
      }
      sumAll += w;
      sumAllOrig += wo;
      if (sig != method->IsSignalLike())
      {WrongDetection[ievt]=kTRUE; sumWrong+=w; sumWrongOrig+=wo;}
      else WrongDetection[ievt]=kFALSE;
   }
   fMethodError=sumWrong/sumAll;
   fOrigMethodError = sumWrongOrig/sumAllOrig;
  //calculating the fMethodError and the fBoostWeight out of it uses the formula w = ((1-err)/err)^beta
   if (fMethodError>0 && fADABoostBeta == 1.0)
      fBoostWeight = (1.0-fMethodError)/fMethodError;
   else if (fMethodError>0 && fADABoostBeta != 1.0)
      fBoostWeight =  TMath::Power((1.0 - fMethodError)/fMethodError, fADABoostBeta);
   else
      fBoostWeight = 1000;

   if (fBoostType == "AdaBoost")
   {
      //ADA boosting, rescaling the weight of the wrong events according to the error level over the entire test sample
      //rescaling all the weights to have the same sum, but without touching the original weights (changing only the boosted weight of all the events)
      Double_t Factor=sumAll/(sumAll-sumWrong + sumWrong*fBoostWeight);
      for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
         Data()->GetEvent(ievt)->ScaleBoostWeight(Factor* ((WrongDetection[ievt])?fBoostWeight:1.0));
   }
   else if (fBoostType == "Bagging")
   {
      //Bagging or Bootstrap boosting, gives new random weight for every event
      TRandom *trandom   = new TRandom(fMethods.size()-1);
      for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
      {
         ev = Data()->GetEvent(ievt);
         ev->SetBoostWeight(trandom->Rndm());
         sumAll1+=ev->GetWeight();
      }
      //rescaling all the weights to have the same sum, but without touching the original weights (changing only the boosted weight of all the events)
      Double_t Factor=sumAll/sumAll1;
      for (Long64_t ievt=0; ievt<Data()->GetNEvents(); ievt++)
      {
         ev = Data()->GetEvent(ievt);
         ev->ScaleBoostWeight(Factor);
      }
   }

   if (fMethodWeightType == "ByError")
      fMethodWeight.push_back(TMath::Log(fBoostWeight));
   else if (fMethodWeightType == "Average")
      fMethodWeight.push_back(1.0);
   else
      fMethodWeight.push_back(0);

   delete WrongDetection;
}
//_______________________________________________________________________
void TMVA::MethodBoost::GetHelpMessage() const
{

}
const TMVA::Ranking* TMVA::MethodBoost::CreateRanking()
{ return 0;}



