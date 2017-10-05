// @(#)root/tmva $Id: RuleFit.cxx,v 1.59 2008/06/25 13:15:15 helgevoss Exp $
// Author: Andreas Hoecker, Joerg Stelzer, Fredrik Tegenfeldt, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Rule                                                                  *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      A class describung a 'rule'                                               *
 *      Each internal node of a tree defines a rule from all the parental nodes.  *
 *      A rule with 0 or 1 nodes in the list is a root rule -> corresponds to a0. *
 *      Input: a decision tree (in the constructor)                               *
 *             its coefficient                                                    *
 *                                                                                *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Fredrik Tegenfeldt <Fredrik.Tegenfeldt@cern.ch> - Iowa State U., USA      *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         *
 *      Iowa State U.                                                             *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//______________________________________________________________________
/*
  Implementation of various fits of rule ensembles
*/
//______________________________________________________________________


#include <iostream>
#include <algorithm>

#include "TKey.h"
#include "TRandom.h"

#include "TMVA/RuleFit.h"
#include "TMVA/MethodRuleFit.h"
#include "TMVA/Timer.h"

ClassImp(TMVA::RuleFit)

//_______________________________________________________________________
TMVA::RuleFit::RuleFit( const MethodBase *rfbase )
   : fVisHistsUseImp( kTRUE ),
     fLogger( "RuleFit" )
{
   // constructor
   Initialize( rfbase );
}

//_______________________________________________________________________
TMVA::RuleFit::RuleFit()
   : fVisHistsUseImp( kTRUE ),
     fLogger( "RuleFit" )
{
   // default constructor
}

//_______________________________________________________________________
TMVA::RuleFit::~RuleFit()
{
   // destructor

   std::vector< const TMVA::DecisionTree *>::iterator dtIt = fForest.begin();
   for(;dtIt != fForest.begin(); dtIt++) delete *dtIt;
}

//_______________________________________________________________________
void TMVA::RuleFit::InitNEveEff()
{
   // init effective number of events (using event weights)
   UInt_t neve = fTrainingEvents.size();
   if (neve==0) return;
   //
   fNEveEffTrain = CalcWeightSum( &fTrainingEvents );
   //
}

//_______________________________________________________________________
void TMVA::RuleFit::InitPtrs(  const MethodBase *rfbase )
{
   // initialize pointers
   this->SetMethodBase(rfbase);
   fRuleEnsemble.Initialize( this );
   fRuleFitParams.SetRuleFit( this );
}

//_______________________________________________________________________
void TMVA::RuleFit::Initialize(  const MethodBase *rfbase )
{
   // initialize the parameters of the RuleFit method and make rules
   InitPtrs(rfbase);

   if (fMethodRuleFit) 
      SetTrainingEvents( fMethodRuleFit->GetTrainingEvents() );

   InitNEveEff();

   MakeForest();

   // Make the model - Rule + Linear (if fDoLinear is true)
   fRuleEnsemble.MakeModel();

   // init rulefit params
   fRuleFitParams.Init();

}

//_______________________________________________________________________
void TMVA::RuleFit::SetMethodBase( const MethodBase *rfbase )
{
   // set MethodBase
   fMethodBase = rfbase;
   fMethodRuleFit = dynamic_cast<const MethodRuleFit *>(rfbase);
}

//_______________________________________________________________________
// void TMVA::RuleFit::Copy( const RuleFit& other )
// {
//    // copy method
//    if(this != &other) {
//       fMethodRuleFit   = other.GetMethodRuleFit();
//       fMethodBase      = other.GetMethodBase();
//       fTrainingEvents  = other.GetTrainingEvents();
//       //      fSubsampleEvents = other.GetSubsampleEvents();
   
//       fForest       = other.GetForest();
//       fRuleEnsemble = other.GetRuleEnsemble();
//    }
// }

//_______________________________________________________________________
Double_t TMVA::RuleFit::CalcWeightSum( const std::vector<Event *> *events, UInt_t neve )
{
   // calculate the sum of weights
   if (events==0) return 0.0;
   if (neve==0) neve=events->size();
   //
   Double_t sumw=0;
   for (UInt_t ie=0; ie<neve; ie++) {
      sumw += ((*events)[ie])->GetWeight();
   }
   return sumw;
}

//_______________________________________________________________________
void TMVA::RuleFit::SetMsgType( EMsgType t )
{
   // set the current message type to that of mlog for this class and all other subtools
   fLogger.SetMinType(t);
   fRuleEnsemble.SetMsgType(t);
   fRuleFitParams.SetMsgType(t);
}

//_______________________________________________________________________
void TMVA::RuleFit::BuildTree( DecisionTree *dt )
{
   // build the decision tree using fNTreeSample events from fTrainingEventsRndm
   if (dt==0) return;
   if (fMethodRuleFit==0) {
      fLogger << kFATAL << "RuleFit::BuildTree() - Attempting to build a tree NOT from a MethodRuleFit" << Endl;
   }
   std::vector<Event *> evevec;
   for (UInt_t ie=0; ie<fNTreeSample; ie++) {
      evevec.push_back(fTrainingEventsRndm[ie]);
   }
   dt->BuildTree(evevec);
   if (fMethodRuleFit->GetPruneMethod() != DecisionTree::kNoPruning) {
      dt->SetPruneMethod(fMethodRuleFit->GetPruneMethod());
      dt->SetPruneStrength(fMethodRuleFit->GetPruneStrength());
      dt->PruneTree();
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::MakeForest()
{
   // make a forest of decisiontrees
   if (fMethodRuleFit==0) {
      fLogger << kFATAL << "RuleFit::BuildTree() - Attempting to build a tree NOT from a MethodRuleFit" << Endl;
   }
   fLogger << kDEBUG << "Creating a forest with " << fMethodRuleFit->GetNTrees() << " decision trees" << Endl;
   fLogger << kDEBUG << "Each tree is built using a random subsample with " << fNTreeSample << " events" << Endl;
   //
   Timer timer( fMethodRuleFit->GetNTrees(), "RuleFit" );

   Double_t fsig;
   Int_t nsig,nbkg;
   //
   TRandom rndGen;
   //
   Int_t nminRnd;
   //
   // First save all event weights.
   // Weights are modifed by the boosting.
   // Those weights we do not want for the later fitting.
   //
   Bool_t useBoost = fMethodRuleFit->UseBoost();

   if (useBoost) SaveEventWeights();

   for (Int_t i=0; i<fMethodRuleFit->GetNTrees(); i++) {
      //      timer.DrawProgressBar(i);
      if (!useBoost) ReshuffleEvents();
      nsig=0;
      nbkg=0;
      for (UInt_t ie = 0; ie<fNTreeSample; ie++) {
         if (fTrainingEventsRndm[ie]->IsSignal()) nsig++; // ignore weights here
         else nbkg++;
      }
      fsig = Double_t(nsig)/Double_t(nsig+nbkg);
//       if ((nbkg==0) || (nsig==0)) {
//          fLogger << kFATAL << "BUG TRAP: only signal or bkg (not both) in sample for making forest, nsig,nbkg = "
//                  << nsig << " , " << nbkg << Endl;
//       }
//       if ((fsig>0.7) || (fsig<0.3)) {
//          fLogger << kFATAL << "BUG TRAP: number of signal or bkg not roughly equal in sample for making forest, nsig,nbkg = "
//                  << nsig << " , " << nbkg << Endl;
//       }
      SeparationBase *qualitySepType = new GiniIndex();
      // generate random number of events
      // do not implement the above in this release...just set it to default
      //      nminRnd = fNodeMinEvents;
      DecisionTree *dt(0);
      Bool_t tryAgain=kTRUE;
      Int_t ntries=0;
      const Int_t ntriesMax=10;
      while (tryAgain) {
         Double_t frnd = rndGen.Uniform( fMethodRuleFit->GetMinFracNEve(), fMethodRuleFit->GetMaxFracNEve() );
         nminRnd = Int_t(frnd*static_cast<Double_t>(fNTreeSample));
         dt = new DecisionTree( fMethodRuleFit->GetSeparationBase(), nminRnd, fMethodRuleFit->GetNCuts(), qualitySepType );
         BuildTree(dt); // reads fNTreeSample events from fTrainingEventsRndm
         if (dt->GetNNodes()<3) {
            delete dt;
            dt=0;
         }
         ntries++;
         tryAgain = ((dt==0) && (ntries<ntriesMax));
      }
      if (dt) {
         fForest.push_back(dt);
         if (useBoost) Boost(dt);
      } else {
         fLogger << kWARNING << "------------------------------------------------------------------" << Endl;
         fLogger << kWARNING << " Failed growing a tree even after " << ntriesMax << " trials" << Endl;
         fLogger << kWARNING << " Possible solutions: " << Endl;
         fLogger << kWARNING << "   1. increase the number of training events" << Endl;
         fLogger << kWARNING << "   2. set a lower min fraction cut (fEventsMin)" << Endl;
         fLogger << kWARNING << "   3. maybe also decrease the max fraction cut (fEventsMax)" << Endl;
         fLogger << kWARNING << " If the above warning occurs rarely only, it can be ignored" << Endl;
         fLogger << kWARNING << "------------------------------------------------------------------" << Endl;
      }

      fLogger << kDEBUG << "Built tree with minimum cut at N = " << nminRnd
              << " => N(nodes) = " << fForest.back()->GetNNodes()
              << " ; n(tries) = " << ntries
              << Endl;
   }

   // Now restore event weights
   if (useBoost) RestoreEventWeights();

   // print statistics on the forest created
   ForestStatistics();
}

//_______________________________________________________________________
void TMVA::RuleFit::SaveEventWeights()
{
   // save event weights - must be done before making the forest
   fEventWeights.clear();
   for (std::vector<Event*>::iterator e=fTrainingEvents.begin(); e!=fTrainingEvents.end(); e++) {
      Double_t w = (*e)->GetWeight();
      fEventWeights.push_back(w);
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::RestoreEventWeights()
{
   // save event weights - must be done before making the forest
   UInt_t ie=0;
   if (fEventWeights.size() != fTrainingEvents.size()) {
      fLogger << kERROR << "RuleFit::RestoreEventWeights() called without having called SaveEventWeights() before!" << Endl;
      return;
   }
   for (std::vector<Event*>::iterator e=fTrainingEvents.begin(); e!=fTrainingEvents.end(); e++) {
      (*e)->SetWeight(fEventWeights[ie]);
      ie++;
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::Boost( DecisionTree *dt )
{
   // Boost the events. The algorithm below is the called AdaBoost.
   // See MethodBDT for details.
   // Actually, this is a more or less copy of MethodBDT::AdaBoost().
   Double_t sumw=0;      // sum of initial weights - all events
   Double_t sumwfalse=0; // idem, only missclassified events
   //
   std::vector<Bool_t> correctSelected;
   //
   for (std::vector<Event*>::iterator e=fTrainingEvents.begin(); e!=fTrainingEvents.end(); e++) {
      Bool_t isSignalType = (dt->CheckEvent(*(*e),kTRUE) > 0.5 );
      Double_t w = (*e)->GetWeight();
      sumw += w;
      // 
      if (isSignalType == (*e)->IsSignal()) { // correctly classified
         correctSelected.push_back(kTRUE);
      } else {                                // missclassified
         sumwfalse+= w;
         correctSelected.push_back(kFALSE);
      }    
   }
   // missclassification error
   Double_t err = sumwfalse/sumw;
   // calculate boost weight for missclassified events
   // use for now the exponent = 1.0
   // one could have w = ((1-err)/err)^beta
   Double_t boostWeight = (err>0 ? (1.0-err)/err : 1000.0);
   Double_t newSumw=0.0;
   UInt_t ie=0;
   // set new weight to missclassified events
   for (std::vector<Event*>::iterator e=fTrainingEvents.begin(); e!=fTrainingEvents.end(); e++) {
      if (!correctSelected[ie])
         (*e)->SetWeight( (*e)->GetWeight() * boostWeight);
      newSumw+=(*e)->GetWeight();    
      ie++;
   }
   // reweight all events
   Double_t scale = sumw/newSumw;
   for (std::vector<Event*>::iterator e=fTrainingEvents.begin(); e!=fTrainingEvents.end(); e++) {
      (*e)->SetWeight( (*e)->GetWeight() * scale);
   }
   fLogger << kDEBUG << "boostWeight = " << boostWeight << "    scale = " << scale << Endl;
}

//_______________________________________________________________________
void TMVA::RuleFit::ForestStatistics()
{
   // summary of statistics of all trees
   // * end-nodes: average and spread
   UInt_t ntrees = fForest.size();
   if (ntrees==0) return;
   const DecisionTree *tree;
   Double_t sumn2 = 0;
   Double_t sumn  = 0;
   Double_t nd;
   for (UInt_t i=0; i<ntrees; i++) {
      tree = fForest[i];
      nd = Double_t(tree->GetNNodes());
      sumn  += nd;
      sumn2 += nd*nd;
   }
   Double_t sig = TMath::Sqrt( gTools().ComputeVariance( sumn2, sumn, ntrees ));
   fLogger << kVERBOSE << "Nodes in trees: average & std dev = " << sumn/ntrees << " , " << sig << Endl;
}

//_______________________________________________________________________
void TMVA::RuleFit::FitCoefficients()
{
   //
   // Fit the coefficients for the rule ensemble
   //
   fLogger << kVERBOSE << "Fitting rule/linear terms" << Endl;
   fRuleFitParams.MakeGDPath();
}

//_______________________________________________________________________
void TMVA::RuleFit::CalcImportance()
{
   // calculates the importance of each rule

   fLogger << kVERBOSE << "Calculating importance" << Endl;
   fRuleEnsemble.CalcImportance();
   fRuleEnsemble.CleanupRules();
   fRuleEnsemble.CleanupLinear();
   fRuleEnsemble.CalcVarImportance();
   fLogger << kVERBOSE << "Filling rule statistics" << Endl;
   fRuleEnsemble.RuleResponseStats();
}

//_______________________________________________________________________
Double_t TMVA::RuleFit::EvalEvent( const Event& e )
{
   // evaluate single event

   return fRuleEnsemble.EvalEvent( e );
}

//_______________________________________________________________________
void TMVA::RuleFit::SetTrainingEvents( const std::vector<Event *>& el )
{
   // set the training events randomly
   if (fMethodRuleFit==0) fLogger << kFATAL << "RuleFit::SetTrainingEvents - MethodRuleFit not initialized" << Endl;
   UInt_t neve = el.size();
   if (neve==0) fLogger << kWARNING << "An empty sample of training events was given" << Endl;

   // copy vector
   fTrainingEvents.clear();
   fTrainingEventsRndm.clear();
   for (UInt_t i=0; i<neve; i++) {
      fTrainingEvents.push_back(static_cast< Event *>(el[i]));
      fTrainingEventsRndm.push_back(static_cast< Event *>(el[i]));
   }

   // Re-shuffle the vector, ie, recreate it in a random order
   std::random_shuffle( fTrainingEventsRndm.begin(), fTrainingEventsRndm.end() );

   // fraction events per tree
   fNTreeSample = static_cast<UInt_t>(neve*fMethodRuleFit->GetTreeEveFrac());
   fLogger << kDEBUG << "Number of events per tree : " << fNTreeSample
           << " ( N(events) = " << neve << " )"
           << " randomly drawn without replacement" << Endl;
}

//_______________________________________________________________________
void TMVA::RuleFit::GetRndmSampleEvents(std::vector< const Event * > & evevec, UInt_t nevents)
{
   // draw a random subsample of the training events without replacement
   ReshuffleEvents();
   if ((nevents<fTrainingEventsRndm.size()) && (nevents>0)) {
      evevec.resize(nevents);
      for (UInt_t ie=0; ie<nevents; ie++) {
         evevec[ie] = fTrainingEventsRndm[ie];
      }
   } else {
      fLogger << kWARNING << "GetRndmSampleEvents() : requested sub sample size larger than total size (BUG!).";
   }
}
//_______________________________________________________________________
void TMVA::RuleFit::NormVisHists(std::vector<TH2F *> & hlist)
{
   // normalize rule importance hists
   //
   // if all weights are positive, the scale will be 1/maxweight
   // if minimum weight < 0, then the scale will be 1/max(maxweight,abs(minweight))
   //
   if (hlist.size()==0) return;
   //
   Double_t wmin=0;
   Double_t wmax=0;
   Double_t w,wm;
   Double_t awmin;
   Double_t scale;
   for (UInt_t i=0; i<hlist.size(); i++) {
      TH2F *hs = hlist[i];
      w  = hs->GetMaximum();
      wm = hs->GetMinimum();
      if (i==0) {
         wmin=wm;
         wmax=w;
      } else {
         if (w>wmax)  wmax=w;
         if (wm<wmin) wmin=wm;
      }
   }
   awmin = TMath::Abs(wmin);
   Double_t usemin,usemax;
   if (awmin>wmax) {
      scale = 1.0/awmin;
      usemin = -1.0;
      usemax = scale*wmax;
   } else {
      scale = 1.0/wmax;
      usemin = scale*wmin;
      usemax = 1.0;
   }
   
   //
   for (UInt_t i=0; i<hlist.size(); i++) {
      TH2F *hs = hlist[i];
      hs->Scale(scale);
      hs->SetMinimum(usemin);
      hs->SetMaximum(usemax);
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::FillCut(TH2F* h2, const Rule *rule, Int_t vind)
{
   // Fill cut

   if (rule==0) return;
   if (h2==0) return;
   //
   Double_t rmin,  rmax;
   Bool_t   dormin,dormax;
   Bool_t ruleHasVar = rule->GetRuleCut()->GetCutRange(vind,rmin,rmax,dormin,dormax);
   if (!ruleHasVar) return;
   //
   Int_t firstbin = h2->GetBin(1,1,1);
   Int_t lastbin = h2->GetBin(h2->GetNbinsX(),1,1);
   Int_t binmin=(dormin ? h2->FindBin(rmin,0.5):firstbin);
   Int_t binmax=(dormax ? h2->FindBin(rmax,0.5):lastbin);
   Int_t fbin;
   Double_t xbinw = h2->GetBinWidth(firstbin);
   Double_t fbmin = h2->GetBinLowEdge(binmin-firstbin+1);
   Double_t lbmax = h2->GetBinLowEdge(binmax-firstbin+1)+xbinw;
   Double_t fbfrac = (dormin ? ((fbmin+xbinw-rmin)/xbinw):1.0);
   Double_t lbfrac = (dormax ? ((rmax-lbmax+xbinw)/xbinw):1.0);
   Double_t f;
   Double_t xc;
   Double_t val;

   for (Int_t bin = binmin; bin<binmax+1; bin++) {
      fbin = bin-firstbin+1;
      if (bin==binmin) {
         f = fbfrac;
      } else if (bin==binmax) {
         f = lbfrac;
      } else {
         f = 1.0;
      }
      xc = h2->GetBinCenter(fbin);
      //
      if (fVisHistsUseImp) {
         val = rule->GetImportance();
      } else {
         val = rule->GetCoefficient()*rule->GetSupport();
      }
      h2->Fill(xc,0.5,val*f);
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::FillLin(TH2F* h2,Int_t vind)
{
   // fill lin

   if (h2==0) return;
   if (!fRuleEnsemble.DoLinear()) return;
   //
   Int_t firstbin = 1;
   Int_t lastbin = h2->GetNbinsX();
   Double_t xc;
   Double_t val;
   if (fVisHistsUseImp) {
      val = fRuleEnsemble.GetLinImportance(vind);
   } else {
      val = fRuleEnsemble.GetLinCoefficients(vind);
   }
   for (Int_t bin = firstbin; bin<lastbin+1; bin++) {
      xc = h2->GetBinCenter(bin);
      h2->Fill(xc,0.5,val);
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::FillCorr(TH2F* h2,const Rule *rule,Int_t vx, Int_t vy)
{
   // fill rule correlation between vx and vy, weighted with either the importance or the coefficient
   if (rule==0) return;
   if (h2==0) return;
   Double_t val;
   if (fVisHistsUseImp) {
      val = rule->GetImportance();
   } else {
      val = rule->GetCoefficient()*rule->GetSupport();
   }
   //
   Double_t rxmin,   rxmax,   rymin,   rymax;
   Bool_t   dorxmin, dorxmax, dorymin, dorymax;
   //
   // Get range in rule for X and Y
   //
   Bool_t ruleHasVarX = rule->GetRuleCut()->GetCutRange(vx,rxmin,rxmax,dorxmin,dorxmax);
   Bool_t ruleHasVarY = rule->GetRuleCut()->GetCutRange(vy,rymin,rymax,dorymin,dorymax);
   if (!(ruleHasVarX || ruleHasVarY)) return;
   // min max of varX and varY in hist
   Double_t vxmin = (dorxmin ? rxmin:h2->GetXaxis()->GetXmin());
   Double_t vxmax = (dorxmax ? rxmax:h2->GetXaxis()->GetXmax());
   Double_t vymin = (dorymin ? rymin:h2->GetYaxis()->GetXmin());
   Double_t vymax = (dorymax ? rymax:h2->GetYaxis()->GetXmax());
   // min max bin in X and Y
   Int_t binxmin= h2->GetXaxis()->FindBin(vxmin);
   Int_t binxmax= h2->GetXaxis()->FindBin(vxmax);
   Int_t binymin= h2->GetYaxis()->FindBin(vymin);
   Int_t binymax= h2->GetYaxis()->FindBin(vymax);
   // bin widths
   Double_t xbinw = h2->GetXaxis()->GetBinWidth(binxmin);
   Double_t ybinw = h2->GetYaxis()->GetBinWidth(binxmin);
   Double_t xbinmin = h2->GetXaxis()->GetBinLowEdge(binxmin);
   Double_t xbinmax = h2->GetXaxis()->GetBinLowEdge(binxmax)+xbinw;
   Double_t ybinmin = h2->GetYaxis()->GetBinLowEdge(binymin);
   Double_t ybinmax = h2->GetYaxis()->GetBinLowEdge(binymax)+ybinw;
   // fraction of edges
   Double_t fxbinmin = (dorxmin ? ((xbinmin+xbinw-vxmin)/xbinw):1.0);
   Double_t fxbinmax = (dorxmax ? ((vxmax-xbinmax+xbinw)/xbinw):1.0);
   Double_t fybinmin = (dorymin ? ((ybinmin+ybinw-vymin)/ybinw):1.0);
   Double_t fybinmax = (dorymax ? ((vymax-ybinmax+ybinw)/ybinw):1.0);
   //
   Double_t fx,fy;
   Double_t xc,yc;
   // fill histo
   for (Int_t binx = binxmin; binx<binxmax+1; binx++) {
      if (binx==binxmin) {
         fx = fxbinmin;
      } else if (binx==binxmax) {
         fx = fxbinmax;
      } else {
         fx = 1.0;
      }
      xc = h2->GetXaxis()->GetBinCenter(binx);
      for (Int_t biny = binymin; biny<binymax+1; biny++) {
         if (biny==binymin) {
            fy = fybinmin;
         } else if (biny==binymax) {
            fy = fybinmax;
         } else {
            fy = 1.0;
         }
         yc = h2->GetYaxis()->GetBinCenter(biny);
         h2->Fill(xc,yc,val*fx*fy);
      }
   }
}

//_______________________________________________________________________
void TMVA::RuleFit::FillVisHistCut(const Rule * rule, std::vector<TH2F *> & hlist)
{
   // help routine to MakeVisHists() - fills for all variables
   //   if (rule==0) return;
   Int_t nhists = hlist.size();
   Int_t nvar   = fMethodBase->GetNvar();
   if (nhists!=nvar) fLogger << kFATAL << "BUG TRAP: number of hists is not equal the number of variables!" << Endl;
   //
   std::vector<Int_t> vindex;
   TString hstr;
   // not a nice way to do a check...
   for (Int_t ih=0; ih<nhists; ih++) {
      hstr = hlist[ih]->GetTitle();
      for (Int_t iv=0; iv<nvar; iv++) {
         if (fMethodBase->GetInputExp(iv) == hstr)
            vindex.push_back(iv);
      }
   }
   //
   for (Int_t iv=0; iv<nvar; iv++) {
      if (rule) {
         if (rule->ContainsVariable(vindex[iv])) {
            FillCut(hlist[iv],rule,vindex[iv]);
         }
      } else {
         FillLin(hlist[iv],vindex[iv]);
      }
   }
}
//_______________________________________________________________________
void TMVA::RuleFit::FillVisHistCorr(const Rule * rule, std::vector<TH2F *> & hlist)
{
   // help routine to MakeVisHists() - fills for all correlation plots
   if (rule==0) return;
   Double_t ruleimp  = rule->GetImportance();
   if (!(ruleimp>0)) return;
   if (ruleimp<fRuleEnsemble.GetImportanceCut()) return;
   //
   Int_t nhists = hlist.size();
   Int_t nvar   = fMethodBase->GetNvar();
   Int_t ncorr  = (nvar*(nvar+1)/2)-nvar;
   if (nhists!=ncorr) fLogger << kERROR << "BUG TRAP: number of corr hists is not correct! ncorr = "
                              << ncorr << " nvar = " << nvar << Endl;
   //
   std::vector< std::pair<Int_t,Int_t> > vindex;
   TString hstr, var1, var2;
   Int_t iv1=0,iv2=0;
   // not a nice way to do a check...
   for (Int_t ih=0; ih<nhists; ih++) {
      hstr = hlist[ih]->GetName();
      if (GetCorrVars( hstr, var1, var2 )) {
         iv1 = fMethodBase->Data().FindVar( var1 );
         iv2 = fMethodBase->Data().FindVar( var2 );
         vindex.push_back( std::pair<Int_t,Int_t>(iv2,iv1) ); // pair X, Y
      } else {
         fLogger << kERROR << "BUG TRAP: should not be here - failed getting var1 and var2" << Endl;
      }
   }
   //
   for (Int_t ih=0; ih<nhists; ih++) {
      if ( (rule->ContainsVariable(vindex[ih].first)) ||
           (rule->ContainsVariable(vindex[ih].second)) ) {
         FillCorr(hlist[ih],rule,vindex[ih].first,vindex[ih].second);
      }
   }
}
//_______________________________________________________________________
Bool_t TMVA::RuleFit::GetCorrVars(TString & title, TString & var1, TString & var2)
{
   // get first and second variables from title
   var1="";
   var2="";
   if(!title.BeginsWith("scat_")) return kFALSE;

   TString titleCopy = title(5,title.Length());
   if(titleCopy.Index("_RF2D")>=0) titleCopy.Remove(titleCopy.Index("_RF2D"));

   Int_t splitPos = titleCopy.Index("_vs_");
   if(splitPos>=0) { // there is a _vs_ in the string
      var1 = titleCopy(0,splitPos);
      var2 = titleCopy(splitPos+4, titleCopy.Length());
      return kTRUE;
   } else {
      var1 = titleCopy;
      return kFALSE;
   }
}
//_______________________________________________________________________
void TMVA::RuleFit::MakeVisHists()
{
   // this will create histograms visualizing the rule ensemble

   const TString directories[4] = { "InputVariables_NoTransform",
                                    "InputVariables_DecorrTransform",
                                    "InputVariables_PCATransform", 
                                    "InputVariables_GaussDecorr" 
  };

   const TString corrDirName = "CorrelationPlots";   
   //   const TString outfname[TMVAGlob::kNumOfMethods] = { "variables",
   //                                                       "variables_decorr",
   //                                                       "variables_pca",
   //                                                       "variables_gaussd"
   //   };
   
   TDirectory* localDir = fMethodBase->Data().BaseRootDir();
   //   TDirectory* methodDir = fMethodBase->GetMethodBaseDir();
   TDirectory* methodDir = fMethodBase->BaseDir();
   TDirectory* varDir = 0;
   TDirectory* corrDir=0;
   TString varDirName;
   //
   Bool_t done=(localDir==0);
   Int_t type=0;
   if (done) fLogger << kWARNING << "No basedir - BUG??" << Endl;
   while (!done) {
      varDir = (TDirectory*)localDir->Get( directories[type] );
      type++;
      done = ((varDir!=0) || (type>2));
   }
   if (varDir==0) {
      fLogger << kWARNING << "No input variable directory found - bug?" << Endl;
      return;
   }
   if (methodDir==0) {
      fLogger << kWARNING << "No rulefit method directory found - bug?" << Endl;
      return;
   }
   varDirName = varDir->GetName();
   varDir->cd();
   //
   // get correlation plot directory
   corrDir = (TDirectory *)varDir->Get(corrDirName);
   if (corrDir==0) fLogger << kWARNING << "No correlation directory found : " << corrDirName << Endl;
   // how many plots are in the var directory?
   Int_t noPlots = ((varDir->GetListOfKeys())->GetSize()) / 2;
   fLogger << kDEBUG << "Got number of plots = " << noPlots << Endl;
 
   // loop over all objects in directory
   std::vector<TH2F *> h1Vector;
   std::vector<TH2F *> h2CorrVector;
   TIter next(varDir->GetListOfKeys());
   TKey *key;
   while ((key = (TKey*)next())) {
      // make sure, that we only look at histograms
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1F")) continue;
      TH1F *sig = (TH1F*)key->ReadObj();
      TString hname= sig->GetName();
      fLogger << kDEBUG << "Got histogram : " << hname << Endl;

      // check for all signal histograms
      if (hname.Contains("__S")){ // found a new signal plot
         TString htitle = sig->GetTitle();
         htitle.ReplaceAll("signal","");
         TString newname = hname;
         newname.ReplaceAll("__S","__RF");
         //         TString newtitle = "RuleFit: " + htitle;
         methodDir->cd();
         TH2F *newhist = new TH2F(newname,htitle,sig->GetNbinsX(),sig->GetXaxis()->GetXmin(),sig->GetXaxis()->GetXmax(),
                                  1,sig->GetYaxis()->GetXmin(),sig->GetYaxis()->GetXmax());
         newhist->Write();
         varDir->cd();
         h1Vector.push_back( newhist );
      }
   }
   //
   corrDir->cd();
   TString var1,var2;
   TIter nextCorr(corrDir->GetListOfKeys());
   while ((key = (TKey*)nextCorr())) {
      // make sure, that we only look at histograms
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH2F")) continue;
      TH2F *sig = (TH2F*)key->ReadObj();
      TString hname= sig->GetName();

      // check for all signal histograms
      if ((hname.Contains("scat_")) && (hname.Contains("_sig_"))) {
         fLogger << kDEBUG << "Got histogram : " << hname << Endl;
         TString htitle = sig->GetTitle();
         htitle.ReplaceAll("(signal)_","");
         TString newname = hname;
         newname.ReplaceAll("_sig_","_RF2D_");
         //         TString newtitle = "RuleFit: " + htitle;
         methodDir->cd();
         const Int_t rebin=2;
         TH2F *newhist = new TH2F(newname,htitle,
                                  sig->GetNbinsX()/rebin,sig->GetXaxis()->GetXmin(),sig->GetXaxis()->GetXmax(),
                                  sig->GetNbinsY()/rebin,sig->GetYaxis()->GetXmin(),sig->GetYaxis()->GetXmax());
         newhist->Write();
         if (GetCorrVars( newname, var1, var2 )) {
            Int_t iv1 = fMethodBase->Data().FindVar(var1);
            Int_t iv2 = fMethodBase->Data().FindVar(var2);
            if (iv1<0) {
               sig->GetYaxis()->SetTitle(var1);
            } else {
               sig->GetYaxis()->SetTitle(fMethodBase->GetInputExp(iv1));
            }
            if (iv2<0) {
               sig->GetXaxis()->SetTitle(var2);
            } else {
               sig->GetXaxis()->SetTitle(fMethodBase->GetInputExp(iv2));
            }
         }
         corrDir->cd();
         h2CorrVector.push_back( newhist );
      }
   }

   varDir->cd();
   // fill rules
   UInt_t nrules = fRuleEnsemble.GetNRules();
   const Rule *rule;
   for (UInt_t i=0; i<nrules; i++) {
      rule = fRuleEnsemble.GetRulesConst(i);
      FillVisHistCut(rule, h1Vector);
   }
   // fill linear terms and normalise hists
   FillVisHistCut(0, h1Vector);
   NormVisHists(h1Vector);
   //
   corrDir->cd();
   // fill rules
   for (UInt_t i=0; i<nrules; i++) {
      rule = fRuleEnsemble.GetRulesConst(i);
      FillVisHistCorr(rule, h2CorrVector);
   }
   NormVisHists(h2CorrVector);
}

//_______________________________________________________________________
void TMVA::RuleFit::MakeDebugHists()
{
   // this will create a histograms intended rather for debugging or for the curious user

   TDirectory* methodDir = fMethodBase->BaseDir();
   if (methodDir==0) {
      fLogger << kWARNING << "<MakeDebugHists> No rulefit method directory found - bug?" << Endl;
      return;
   }
   //
   methodDir->cd();
   std::vector<Double_t> distances;
   std::vector<Double_t> fncuts;
   std::vector<Double_t> fnvars;
   const Rule *ruleA;
   const Rule *ruleB;
   Double_t dABmin=1000000.0;
   Double_t dABmax=-1.0;
   UInt_t nrules = fRuleEnsemble.GetNRules();
   for (UInt_t i=0; i<nrules; i++) {
      ruleA = fRuleEnsemble.GetRulesConst(i);
      for (UInt_t j=i+1; j<nrules; j++) {
         ruleB = fRuleEnsemble.GetRulesConst(j);
         Double_t dAB = ruleA->RuleDist( *ruleB, kTRUE );
         if (dAB>-0.5) {
            UInt_t nc = ruleA->GetNcuts();
            UInt_t nv = ruleA->GetNumVarsUsed();
            distances.push_back(dAB);
            fncuts.push_back(static_cast<Double_t>(nc));
            fnvars.push_back(static_cast<Double_t>(nv));
            if (dAB<dABmin) dABmin=dAB;
            if (dAB>dABmax) dABmax=dAB;
         }
      }
   }
   //
   TH1F *histDist = new TH1F("RuleDist","Rule distances",100,dABmin,dABmax);
   TTree *distNtuple = new TTree("RuleDistNtuple","RuleDist ntuple");
   Double_t ntDist;
   Double_t ntNcuts;
   Double_t ntNvars;
   distNtuple->Branch("dist", &ntDist,  "dist/D");
   distNtuple->Branch("ncuts",&ntNcuts, "ncuts/D");
   distNtuple->Branch("nvars",&ntNvars, "nvars/D");
   //
   for (UInt_t i=0; i<distances.size(); i++) {
      histDist->Fill(distances[i]);
      ntDist  = distances[i];
      ntNcuts = fncuts[i];
      ntNvars = fnvars[i];
      distNtuple->Fill();
   }
   distNtuple->Write();
}
