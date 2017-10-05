// @(#)root/tmva $Id: MethodKNN.cxx,v 1.8 2008/07/09 10:47:04 speckmayer Exp $
// Author: Rustem Ospanov 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodKNN                                                             *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation                                                            *
 *                                                                                *
 * Author:                                                                        *
 *      Rustem Ospanov <rustem@fnal.gov> - U. of Texas at Austin, USA             *
 *                                                                                *
 * Copyright (c) 2007:                                                            *
 *      CERN, Switzerland                                                         * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      U. of Texas at Austin, USA                                                *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MethodKNN                                                            //
//                                                                      //
// Analysis of k-nearest neighbor                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// C++
#include <string>
#include <stdlib.h>

// ROOT
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

// TMVA
#include "TMVA/ClassifierFactory.h"
#include "TMVA/MethodKNN.h"
#include "TMVA/Ranking.h"
#include "TMVA/Tools.h"

REGISTER_METHOD(KNN)

ClassImp(TMVA::MethodKNN)

using std::endl;

//_______________________________________________________________________
TMVA::MethodKNN::MethodKNN(TString jobName,
                           TString methodTitle,
                           DataSetInfo& theData, 
                           TString theOption,
                           TDirectory* theTargetDir)
   :TMVA::MethodBase(jobName, methodTitle, theData, theOption, theTargetDir),
    fModule(0)
{
   // standard constructor
   InitKNN();

   // interpretation of configuration option string
   SetConfigName( TString("Method") + GetMethodName() );
   DeclareOptions();
   ParseOptions();
   ProcessOptions();
}

//_______________________________________________________________________
TMVA::MethodKNN::MethodKNN(DataSetInfo& theData, 
                           TString theWeightFile,  
                           TDirectory* theTargetDir)
   :TMVA::MethodBase(theData, theWeightFile, theTargetDir),
    fModule(0)
{
   // constructor from weight file
   InitKNN();

   // no options so far ... process base class options
   DeclareOptions();
}

//_______________________________________________________________________
TMVA::MethodKNN::~MethodKNN()
{
   // destructor
   if (fModule) delete fModule;
}

//_______________________________________________________________________
void TMVA::MethodKNN::DeclareOptions() 
{
   // MethodKNN options
 
   // fnkNN         = 40;     // number of k-nearest neighbors 
   // fBalanceDepth = 6;      // number of binary tree levels used for tree balancing
   // fScaleFrac    = 0.8;    // fraction of events used for scaling
   // fTrim         = false;  // use equal number of signal and background events
   // fUseKernel    = false;  // use polynomial kernel weight function
   // fUseWeight    = false;  // count events using weights
   
   // TreeOptDepth is now BalanceDepth, keep old option for backward compatability

   DeclareOptionRef(fnkNN         = 40,     "nkNN",         "Number of k-nearest neighbors");
   DeclareOptionRef(fBalanceDepth = 6,      "TreeOptDepth", "Binary tree optimiation depth");
   DeclareOptionRef(fBalanceDepth = 6,      "BalanceDepth", "Binary tree balance depth");
   DeclareOptionRef(fScaleFrac    = 0.80,   "ScaleFrac",    "Fraction of events used for scaling");
   DeclareOptionRef(fTrim         = kFALSE, "Trim",         "Use equal number of signal and background events");
   DeclareOptionRef(fUseKernel    = kFALSE, "UseKernel",    "Use polynomial kernel weight");
   DeclareOptionRef(fUseWeight    = kFALSE, "UseWeight",    "Use weight to count kNN events");
}

//_______________________________________________________________________
void TMVA::MethodKNN::ProcessOptions() 
{
   // process the options specified by the user
   MethodBase::ProcessOptions();

   if (!(fnkNN > 0)) {      
      fnkNN = 10;
      fLogger << kWARNING << "kNN must be a positive integer: set kNN = " << fnkNN << endl;
   }
   if (fScaleFrac < 0.0) {      
      fScaleFrac = 0.0;
      fLogger << kWARNING << "ScaleFrac can not be negative: set ScaleFrac = " << fScaleFrac << endl;
   }
   if (fScaleFrac > 1.0) {
      fScaleFrac = 1.0;
   }
   if (!(fBalanceDepth > 0)) {
      fBalanceDepth = 6;
      fLogger << kWARNING << "Optimize must be a positive integer: set Optimize = " << fBalanceDepth << endl;      
   }

   fLogger << kVERBOSE 
           << "kNN options: " << endl
           << "  kNN = " << fnkNN << endl
           << "  UseKernel = " << fUseKernel << endl
           << "  ScaleFrac = " << fScaleFrac << endl
           << "  Trim = " << fTrim << endl
           << "  Optimize = " << fBalanceDepth << Endl;
}

//_______________________________________________________________________
void TMVA::MethodKNN::InitKNN() 
{
   // Initialisation

   // fScaleFrac <= 0.0 then do not scale input variables
   // fScaleFrac >= 1.0 then use all event coordinates to scale input variables
   
   SetMethodName("KNN");
   SetMethodType(TMVA::Types::kKNN);
   SetTestvarName();

   fModule = new kNN::ModulekNN();
   fSumOfWeightsS = 0;
   fSumOfWeightsB = 0;
}

//_______________________________________________________________________
void TMVA::MethodKNN::MakeKNN() 
{
   // create kNN
   if (!fModule) {
      fLogger << kFATAL << "ModulekNN is not created" << Endl;
   }

   fModule->Clear();

   std::string option;
   if (fScaleFrac > 0.0) {
      option += "metric";
   }
   if (fTrim) {
      option += "trim";
   }

   fLogger << kINFO << "Creating kd-tree with " << fEvent.size() << " events" << Endl;

   for (kNN::EventVec::const_iterator event = fEvent.begin(); event != fEvent.end(); ++event) {
      fModule->Add(*event);
   }

   // create binary tree
   fModule->Fill(static_cast<UInt_t>(fBalanceDepth),
                 static_cast<UInt_t>(100.0*fScaleFrac),
                 option);
}

//_______________________________________________________________________
Double_t TMVA::MethodKNN::PolKernel(const Double_t value) const
{
   // polynomial kernel
   const Double_t avalue = TMath::Abs(value);

   if (!(avalue < 1.0)) {
      return 0.0;
   }

   const Double_t prod = 1.0 - avalue * avalue * avalue;

   return (prod * prod * prod);
}

//_______________________________________________________________________
void TMVA::MethodKNN::Train()
{
   // kNN training
   fLogger << kINFO << "<Train> start..." << Endl;

   // default sanity checks
//   if (!CheckSanity()) {
//      fLogger << kFATAL << "Sanity check failed" << Endl;
//   }
   
   if (IsNormalised()) {
      fLogger << kINFO << "Input events are normalized - setting ScaleFrac to 0" << Endl;
      fScaleFrac = 0.0;
   }
   
   if (!fEvent.empty()) {
      fLogger << kINFO << "Erasing " << fEvent.size() << " previously stored events" << Endl;
      fEvent.clear();
   }   

   const UInt_t nvar = DataInfo().GetNVariables();
   
   fLogger << kINFO << "Reading " << Data()->GetNEvents() << " events" << Endl;

   for (Int_t ievt = 0; ievt < Data()->GetNEvents(); ++ievt) {
      // read the training event
      const Event * ev = Data()->GetEvent(ievt);
            
      if (nvar < 1)
         fLogger << kFATAL << "MethodKNN::Train() - mismatched or wrong number of event variables" << Endl;
      
      const Double_t weight = ev->GetWeight();
      
      kNN::VarVec vvec(nvar, 0.0);
      
      for (UInt_t ivar = 0; ivar < nvar; ++ivar) {
         vvec[ivar] = ev->GetVal(ivar);
      }
      
      if (ev->IsSignal()) {
         fSumOfWeightsS += weight;
         fEvent.push_back(kNN::Event(vvec, weight, 1)); // signal type = 1
      }
      else {
         fSumOfWeightsB += weight;
         fEvent.push_back(kNN::Event(vvec, weight, 2)); // background type = 2
      }
   }
   
   fLogger << kINFO << "Number of signal events " << fSumOfWeightsS << Endl;
   fLogger << kINFO << "Number of background events " << fSumOfWeightsB << Endl;

   // create kd-tree (binary tree) structure
   MakeKNN();
}

//_______________________________________________________________________
Double_t TMVA::MethodKNN::GetMvaValue()
{
   // classifier response
   const Event * ev = Data()->GetEvent();

   const Int_t nvar = Data()->GetNVariables();
   const Double_t weight = ev->GetWeight();
   
   kNN::VarVec vvec(static_cast<UInt_t>(nvar), 0.0);
   
   for (Int_t ivar = 0; ivar < nvar; ++ivar) {
      vvec[ivar] = ev->GetVal(ivar);
   }   

   const UInt_t knn = static_cast<UInt_t>(fnkNN);

   // search for fnkNN+2 nearest neighbors, pad with two 
   // events to avoid Monte-Carlo events with zero distance
   // most of CPU time is spent in this recursive function
   
   
   if(fUseWeight)
   {
      fModule->Find(kNN::Event(vvec, weight, 3), knn + 2, "weight");
   }
   else
   {
      fModule->Find(kNN::Event(vvec, weight, 3), knn + 2);
   }

   const kNN::List &rlist = fModule->GetkNNList();
   if (rlist.size() != knn + 2) {
      fLogger << kFATAL << "kNN result list is empty" << Endl;
      return -100.0;  
   }

   Double_t maxradius = -1.0;
   if (fUseKernel) {
      Double_t kcount = 0;
      for (kNN::List::const_iterator lit = rlist.begin(); lit != rlist.end(); ++lit) {
         if (!(lit->second > 0.0)) {
            continue;
         }
         
	 if(fUseWeight)
	 {
	    kcount += lit -> first -> GetWeight();
	 }
	 else
	 {
	    ++kcount;
	 }
         
         if(maxradius < lit->second || maxradius < 0.0) {
            maxradius = lit->second;
         }
         
	 if (!(kcount < Double_t(knn))) {
	    break;
	 }
      }
      
      if (!(maxradius > 0.0)) {
         fLogger << kFATAL << "kNN radius is not positive" << Endl;
         return -100.0; 
      }
      
      maxradius = 1.0/TMath::Sqrt(maxradius);
   }
   
   Double_t count_all = 0, weight_all = 0, weight_sig = 0, weight_bac = 0;

   for (kNN::List::const_iterator lit = rlist.begin(); lit != rlist.end(); ++lit) {

      // get reference to current node to make code more readable
      const kNN::Node<kNN::Event> &node = *(lit->first);
      
      // ignore Monte-Carlo event with zero distance
      // this happens when this query event is also in learning sample
      if (!(lit->second > 0.0)) {
         continue;
      }
      
      Double_t weight = node.GetWeight();

      if (fUseWeight) {
	 count_all += weight;
      }
      else {
	 ++count_all;
      }

      if (fUseKernel) {
         weight *= PolKernel(TMath::Sqrt(lit->second)*maxradius);
      }
      
      weight_all += weight;
      
      if (node.GetEvent().GetType() == 1) { // signal type = 1	 
	 if (fUseWeight) {
	    weight_sig += weight;
	 }
	 else {
	    ++weight_sig;
	 }
      }
      else if (node.GetEvent().GetType() == 2) { // background type = 2
	 if (fUseWeight) {
	    weight_bac += weight;
	 }
	 else {
	    ++weight_bac;
	 }
      }
      else {
         fLogger << kFATAL << "Unknown type for training event" << Endl;
      }
      
      // use only fnkNN events
      if (!(count_all < Double_t(knn))) {
         break;
      }      
   }
   
   if (count_all < Double_t(knn)) {
      fLogger << kFATAL << "kNN result list is empty or has wrong size" << Endl;
      return -100.0;
   }
   if (!(weight_all > 0.0)) {
      fLogger << kFATAL << "kNN result total weight is not positive" << Endl;
      return -100.0;
   }
   
   return weight_sig/weight_all;
}

//_______________________________________________________________________
const TMVA::Ranking* TMVA::MethodKNN::CreateRanking() 
{
   // no ranking available
   return 0;
}

//_______________________________________________________________________
void TMVA::MethodKNN::WriteWeightsToStream(ostream& os) const
{  
   // save the weights   
   fLogger << kINFO << "Starting WriteWeightsToStream(ostream& os) function..." << Endl;
   
   if (fEvent.empty()) {
      fLogger << kWARNING << "MethodKNN contains no events " << Endl;
      return;
   }
 
   os << "# MethodKNN will write " << fEvent.size() << " events " << endl;
   os << "# event number, type, weight, variable values" << endl;

   const std::string delim = ", ";

   UInt_t ievent = 0;
   for (kNN::EventVec::const_iterator event = fEvent.begin(); event != fEvent.end(); ++event, ++ievent) {

      os << ievent << delim;
      os << event->GetType() << delim;
      os << event->GetWeight() << delim; 
      
      for (UInt_t ivar = 0; ivar < event->GetNVar(); ++ivar) {
         if (ivar + 1 < event->GetNVar()) {
            os << event->GetVar(ivar) << delim;
         }
         else {
            os << event->GetVar(ivar) << endl;
         }
      }
   }
}

//_______________________________________________________________________
void TMVA::MethodKNN::ReadWeightsFromStream(istream& is)
{
   // read the weights
   fLogger << kINFO << "Starting ReadWeightsFromStream(istream& is) function..." << Endl;

   if (!fEvent.empty()) {
      fLogger << kINFO << "Erasing " << fEvent.size() << " previously stored events" << Endl;
      fEvent.clear();
   }

   UInt_t nvar = 0;

   while (!is.eof()) {
      std::string line;
      std::getline(is, line);
      
      if (line.empty() || line.find("#") != std::string::npos) {
         continue;
      }
      
      UInt_t count = 0;
      std::string::size_type pos=0;
      while( (pos=line.find(',',pos)) != std::string::npos ) { count++; pos++; }

      if (nvar == 0) {
         nvar = count - 2;
      }
      if (count < 3 || nvar != count - 2) {
         fLogger << kFATAL << "Missing comma delimeter(s)" << Endl;
      }

      Int_t ievent = -1, type = -1;
      Double_t weight = -1.0;
      
      kNN::VarVec vvec(nvar, 0.0);
      
      UInt_t vcount = 0;
      std::string::size_type prev = 0;
      
      for (std::string::size_type ipos = 0; ipos < line.size(); ++ipos) {
         if (line[ipos] != ',' && ipos + 1 != line.size()) {
            continue;
         }
         
         if (!(ipos > prev)) {
            fLogger << kFATAL << "Wrog substring limits" << Endl;
         }
         
         std::string vstring = line.substr(prev, ipos - prev);
         if (ipos + 1 == line.size()) {
            vstring = line.substr(prev, ipos - prev + 1);
         }
         
         if (vstring.empty()) {	    
            fLogger << kFATAL << "Failed to parse string" << Endl;
         }
         
         if (vcount == 0) {
            ievent = std::atoi(vstring.c_str());
         }
         else if (vcount == 1) {
            type = std::atoi(vstring.c_str());
         }
         else if (vcount == 2) {
            weight = std::atof(vstring.c_str());
         }
         else if (vcount - 3 < vvec.size()) {	 
            vvec[vcount - 3] = std::atof(vstring.c_str());
         }
         else {
            fLogger << kFATAL << "Wrong variable count" << Endl;
         }
         
         prev = ipos + 1;
         ++vcount;
      }
      
      fEvent.push_back(kNN::Event(vvec, weight, type));
   }
   
   fLogger << kINFO << "Read " << fEvent.size() << " events from text file" << Endl;   

   // create kd-tree (binary tree) structure
   MakeKNN();
}

//-------------------------------------------------------------------------------------------
void TMVA::MethodKNN::WriteWeightsToStream(TFile &rf) const
{ 
   // save weights to ROOT file
   fLogger << kINFO << "Starting WriteWeightsToStream(TFile &rf) function..." << Endl;
   
   if (fEvent.empty()) {
      fLogger << kWARNING << "MethodKNN contains no events " << Endl;
      return;
   }

   kNN::Event *event = new kNN::Event();
   TTree *tree = new TTree("knn", "event tree");
   tree->SetDirectory(0);
   tree->Branch("event", "TMVA::kNN::Event", &event);

   Double_t size = 0.0;
   for (kNN::EventVec::const_iterator it = fEvent.begin(); it != fEvent.end(); ++it) {
      (*event) = (*it);
      size += tree->Fill();
   }

   // !!! hard coded tree name !!!
   rf.WriteTObject(tree, "knn", "Overwrite");

   // scale to MegaBytes
   size /= 1048576.0;

   fLogger << kINFO << "Wrote " << size << "MB and "  << fEvent.size() 
           << " events to ROOT file" << Endl;
   
   delete tree;
   delete event; 
}

//-------------------------------------------------------------------------------------------
void TMVA::MethodKNN::ReadWeightsFromStream(TFile &rf)
{ 
   // read weights from ROOT file
   fLogger << kINFO << "Starting ReadWeightsFromStream(TFile &rf) function..." << Endl;

   if (!fEvent.empty()) {
      fLogger << kINFO << "Erasing " << fEvent.size() << " previously stored events" << Endl;
      fEvent.clear();
   }

   // !!! hard coded tree name !!!
   TTree *tree = dynamic_cast<TTree *>(rf.Get("knn"));
   if (!tree) {
      fLogger << kFATAL << "Failed to find knn tree" << endl;
      return;
   }

   kNN::Event *event = new kNN::Event();
   tree->SetBranchAddress("event", &event);

   const Int_t nevent = tree->GetEntries();

   Double_t size = 0.0;
   for (Int_t i = 0; i < nevent; ++i) {
      size += tree->GetEntry(i);
      fEvent.push_back(*event);
   }

   // scale to MegaBytes
   size /= 1048576.0;

   fLogger << kINFO << "Read " << size << "MB and "  << fEvent.size() 
           << " events from ROOT file" << Endl;

   delete event;

   // create kd-tree (binary tree) structure
   MakeKNN();
}

//_______________________________________________________________________
void TMVA::MethodKNN::MakeClassSpecific(std::ostream& fout, const TString& className) const
{
   // write specific classifier response
   fout << "   // not implemented for class: \"" << className << "\"" << endl;
   fout << "};" << endl;
}

//_______________________________________________________________________
void TMVA::MethodKNN::GetHelpMessage() const
{
   // get help message text
   //
   // typical length of text line: 
   //         "|--------------------------------------------------------------|"
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Short description:" << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "k-nearest neighbor algorithm" << endl;
   fLogger << Endl;
   fLogger << gTools().Color("bold") << "--- Performance tuning via configuration options:" 
           << gTools().Color("reset") << Endl;
   fLogger << Endl;
   fLogger << "<None>" << Endl;
}

