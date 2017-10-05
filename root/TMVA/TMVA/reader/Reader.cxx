// @(#)root/tmva $Id: Reader.cxx,v 1.12 2008/07/21 12:07:33 speckmayer Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Reader                                                                *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Reader class to be used in the user application to interpret the trained  *
 *      MVAs in an analysis context                                               *
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
 * (http://ttmva.sourceforge.net/LICENSE)                                         *
 **********************************************************************************/

//_______________________________________________________________________
//
//  The Reader class serves to use the MVAs in a specific analysis context.
//  Within an event loop, a vector is filled that corresponds to the variables
//  that were used to train the MVA(s) during the training stage. This vector
//  is transfered to the Reader, who takes care of interpreting the weight 
//  file of the MVA of choice, and to return the MVA's output. This is then 
//  used by the user for further analysis.
//
//  ---------------------------------------------------------------------
//  Usage:
//           
//    // ------ before starting the event loop (eg, in the initialisation step)
//
//    //
//    // create TMVA::Reader object
//    //
//    TMVA::Reader *reader = new TMVA::Reader();    
//
//    // create a set of variables and declare them to the reader
//    // - the variable names must corresponds in name and type to 
//    // those given in the weight file(s) that you use
//    Float_t var1, var2, var3, var4;
//    reader->AddVariable( "var1", &var1 );
//    reader->AddVariable( "var2", &var2 );
//    reader->AddVariable( "var3", &var3 );
//    reader->AddVariable( "var4", &var4 );
//      
//    // book the MVA of your choice (prior training of these methods, ie, 
//    // existence of the weight files is required)
//    reader->BookMVA( "Fisher method",  "weights/Fisher.weights.txt"   );
//    reader->BookMVA( "MLP method",     "weights/MLP.weights.txt" );
//    // ... etc
//    
//    // ------- start your event loop
//
//    for (Long64_t ievt=0; ievt<myTree->GetEntries();ievt++) {
//
//      // fill vector with values of variables computed from those in the tree
//      var1 = myvar1;
//      var2 = myvar2;
//      var3 = myvar3;
//      var4 = myvar4;
//            
//      // retrieve the corresponding MVA output
//      double mvaFi = reader->EvaluateMVA( "Fisher method" );
//      double mvaNN = reader->EvaluateMVA( "MLP method"    );
//
//      // do something with these ...., e.g., fill them into your ntuple
//
//    } // end of event loop
//
//    delete reader;
//  ---------------------------------------------------------------------
//
//  An example application of the Reader can be found in TMVA/macros/TMVApplication.C.
//_______________________________________________________________________

#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TClass.h"
#include "TH1D.h"
#include "TKey.h"
#include "TVector.h"
#include <stdlib.h>

#include <string>
#include <vector>
#include <fstream>

#include <iostream>
#include "TMVA/Reader.h"
#include "TMVA/Config.h"
#include "TMVA/ClassifierFactory.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodCuts.h"

#define TMVA_Reader_TestIO__
#undef  TMVA_Reader_TestIO__

ClassImp(TMVA::Reader)

//_______________________________________________________________________
TMVA::Reader::Reader( const TString& theOption, Bool_t verbose )
   : Configurable( theOption ),
     fDataSetInfo(),
     fVerbose( verbose ),
     fColor( kFALSE ),
     fLogger ( this )
{
   // constructor

   DataSetManager::createInstance(fDataInputHandler);
   DataSetManager::Instance().AddDataSetInfo(fDataSetInfo);

   DeclareOptions();
   ParseOptions();

   Init();
}

//_______________________________________________________________________
TMVA::Reader::Reader( std::vector<TString>& inputVars, const TString& theOption, Bool_t verbose )
   : Configurable( theOption ),
     fDataSetInfo(),
     fVerbose( verbose ),
     fColor( kFALSE ),
     fLogger ( this )
{
   // constructor
   DeclareOptions();
   ParseOptions();

   // arguments: names of input variables (vector)
   //            verbose flag
   for (std::vector<TString>::iterator ivar = inputVars.begin(); ivar != inputVars.end(); ivar++) 
      DataInfo().AddVariable( *ivar );
      
   Init();
}

//_______________________________________________________________________
TMVA::Reader::Reader( std::vector<std::string>& inputVars, const TString& theOption, Bool_t verbose )
   : Configurable( theOption ),
     fDataSetInfo(),
     fVerbose( verbose ),
     fColor( kFALSE ),
     fLogger ( this )
{
   // constructor
   DeclareOptions();
   ParseOptions();

   // arguments: names of input variables (vector)
   //            verbose flag
   for (std::vector<std::string>::iterator ivar = inputVars.begin(); ivar != inputVars.end(); ivar++) 
      DataInfo().AddVariable( ivar->c_str() );

   Init();
}

//_______________________________________________________________________
TMVA::Reader::Reader( const std::string varNames, const TString& theOption, Bool_t verbose )
   : Configurable( theOption ),
     fDataSetInfo(),
     fVerbose( verbose ),
     fColor( kFALSE ),
     fLogger ( this )
{
   // constructor
   DeclareOptions();
   ParseOptions();

   // arguments: names of input variables given in form: "name1:name2:name3"
   //            verbose flag
   this->DecodeVarNames(varNames);
   Init();
}

//_______________________________________________________________________
TMVA::Reader::Reader( const TString& varNames, const TString& theOption, Bool_t verbose )
   : Configurable( theOption ),
     fDataSetInfo(),
     fVerbose( verbose ),
     fColor( kFALSE ),
     fLogger ( this )
{
   // constructor
   DeclareOptions();
   ParseOptions();

   // arguments: names of input variables given in form: "name1:name2:name3"
   //            verbose flag
   this->DecodeVarNames(varNames);
   Init();
}

//_______________________________________________________________________
void TMVA::Reader::DeclareOptions() 
{
   // declaration of configuration options
   DeclareOptionRef( fVerbose, "V", "verbose flag" );
   DeclareOptionRef( fColor=kTRUE, "Color", "color flag (default on)" );
   
   ParseOptions(kFALSE);
   
   fLogger.SetMinType( Verbose() ? kVERBOSE : kINFO );
   
   gConfig().SetUseColor(fColor);

   //   if (fDataSetInfo!=0) fDataSetInfo->SetVerbose(Verbose());
}

//_______________________________________________________________________
TMVA::Reader::~Reader( void )
{
   // destructor
}  

//_______________________________________________________________________
void TMVA::Reader::Init( void )
{
   // default initialisation (no member variables)
}

//_______________________________________________________________________
void TMVA::Reader::AddVariable( const TString& expression, Float_t* datalink ) 
{
   // Add a float variable or expression to the reader
  DataInfo().AddVariable(expression, 'F', kFALSE ,(void*)datalink); // <============== should this be kFALSE or rather kTRUE?
}

//_______________________________________________________________________
void TMVA::Reader::AddVariable( const TString& expression, Int_t* datalink ) 
{
   // Add a integer variable or expression to the reader
   DataInfo().AddVariable(expression, 'I', kFALSE, (void*)datalink); // <============== should this be kFALSE or rather kTRUE?
}

//_______________________________________________________________________
TMVA::IMethod* TMVA::Reader::BookMVA( const TString& methodTag, const TString& weightfile )
{
   // read method name from weight file

   std::cout << "Reader::BookMVA 1" << std::endl;

   std::map<TString, IMethod*>::iterator it = fMethodMap.find( methodTag );
   if (it != fMethodMap.end()) {
      fLogger << kFATAL << "<BookMVA> method tag \"" << methodTag << "\" already exists!" << Endl;
   }

   fLogger << kINFO << "Booking \"" << methodTag << "\"" << Endl;
   ifstream fin( weightfile );
   if (!fin.good()) { // file not found --> Error
      fLogger << kFATAL << "<BookMVA> fatal error: "
              << "unable to open input weight file: " << weightfile << Endl;
   }

   char buf[512];

   // read the method name
   fin.getline(buf,512);
   while (!TString(buf).BeginsWith("Method")) fin.getline(buf,512);
   TString ls(buf);
   Int_t idx1 = ls.First(':')+2; Int_t idx2 = ls.Index(' ',idx1)-idx1; if (idx2<0) idx2=ls.Length();
   fin.close();  

   TString fullname = ls(idx1,idx2);
   idx1 = fullname.First(':');
   Int_t idxtit = (idx1<0 ? fullname.Length() : idx1);
   TString methodRealName  = fullname(0, idxtit);
   TString methodTitle;
   Bool_t notit;
   if (idx1<0) {
      methodTitle=methodRealName;
      notit=kTRUE;
   } 
   else {
      methodTitle=fullname(idxtit+2,fullname.Length()-1);
      notit=kFALSE;
   }


   fLogger << kINFO << "Method tag : \"" << methodTag << "\"" << Endl;

   MethodBase* method = dynamic_cast<MethodBase*>(this->BookMVA( Types::Instance().GetMethodType(methodRealName), 
                                                                 weightfile ) );
   std::cout << "OOOOO " << methodTag << std::endl;

   method->SetMethodTitle(methodTitle);
   std::cout << "Reader::BookMVA 2" << std::endl;

   fLogger << kINFO << "Method tag : \"" << methodTag << "\"" << Endl;

   fLogger << kINFO << "Read method name  : \"" << method->GetMethodName()  << "\"" << Endl;
   fLogger << kINFO << "   - method title : \"" << method->GetMethodTitle() << "\"" << Endl;

   std::cout << "Reader::BookMVA 3" << std::endl;

   fLogger << kINFO << "Method tag : \"" << methodTag << "\"" << Endl;
   std::cout << "Reader::BookMVA 4" << std::endl;

   return fMethodMap[methodTag] = method;

   std::cout << "Reader::BookMVA 5" << std::endl;

}

//_______________________________________________________________________
TMVA::IMethod* TMVA::Reader::BookMVA( TMVA::Types::EMVA methodType, const TString& weightfile )
{
   // books MVA method from weightfile
   IMethod* method = ClassifierFactory::Instance().Create(std::string(Types::Instance().GetMethodName( methodType )), DataInfo(), weightfile );


   // read weight file
   (dynamic_cast<MethodBase*>(method))->ReadStateFromFile();

   fLogger << kINFO << "Booked classifier " << dynamic_cast<MethodBase*>(method)->GetMethodName()
           << " with title: \"" << dynamic_cast<MethodBase*>(method)->GetMethodTitle() << "\"" << Endl;

#ifdef TMVA_Reader_TestIO__
   // testing the read/write procedure
   std::ofstream tfile( weightfile+".control" );
   dynamic_cast<MethodBase*>(method)->WriteStateToStream(tfile);
   tfile.close();
#endif

   return method;
}

//_______________________________________________________________________
Double_t TMVA::Reader::EvaluateMVA( const std::vector<Float_t>& /*inputVec*/, const TString& methodTag, Double_t aux )
{
   // Evaluate a vector<float> of input data for a given method
   // The parameter aux is obligatory for the cuts method where it represents the efficiency cutoff

   //   for (UInt_t ivar=0; ivar<inputVec.size(); ivar++) DataInfo().GetDataSet().SetGetEvent().SetVal( ivar, inputVec[ivar] );
 
   return EvaluateMVA( methodTag, aux );
}

//_______________________________________________________________________
Double_t TMVA::Reader::EvaluateMVA( const std::vector<Double_t>& /*inputVec*/, const TString& methodTag, Double_t aux )
{
   // Evaluate a vector<double> of input data for a given method

   // The parameter aux is obligatory for the cuts method where it represents the efficiency cutoff

   //   for (UInt_t ivar=0; ivar<inputVec.size(); ivar++) DataInfo().GetEvent().SetVal( ivar, (Float_t)inputVec[ivar] );

   return EvaluateMVA( methodTag, aux );
}

//_______________________________________________________________________
Double_t TMVA::Reader::EvaluateMVA( const TString& methodTag, Double_t aux )
{
   // evaluates MVA for given set of input variables
   IMethod* method = 0;

   std::map<TString, IMethod*>::iterator it = fMethodMap.find( methodTag );
   if (it == fMethodMap.end()) {
      fLogger << kINFO << "<EvaluateMVA> unknown classifier in map; "
              << "you looked for \"" << methodTag << "\" within available methods: " << Endl;
      for (it = fMethodMap.begin(); it!=fMethodMap.end(); it++) fLogger << " --> " << it->first << Endl;
      fLogger << "Check calling string" << kFATAL << Endl;
   }

   else method = it->second;

   return this->EvaluateMVA( dynamic_cast<TMVA::MethodBase*>(method), aux );
}  

//_______________________________________________________________________
Double_t TMVA::Reader::EvaluateMVA( MethodBase* method, Double_t aux )
{
   // evaluates the MVA

   // the aux value is only needed for MethodCuts: it sets the required signal efficiency 
   if (method->GetMethodType() == TMVA::Types::kCuts) 
      dynamic_cast<TMVA::MethodCuts*>(method)->SetTestSignalEfficiency( aux );

   return method->GetMvaValue();
}


//_______________________________________________________________________
TMVA::IMethod* TMVA::Reader::FindMVA( const TString& methodTag )
{
   // return pointer to method with tag "methodTag"
   std::map<TString, IMethod*>::iterator it = fMethodMap.find( methodTag );
   if (it != fMethodMap.end()) return it->second;
   fLogger << kERROR << "Method " << methodTag << " not found!" << Endl;
   return 0;
}



//_______________________________________________________________________
Double_t TMVA::Reader::GetProba( const TString& methodTag,  Double_t ap_sig, Double_t mvaVal )
{
  // evaluates probability of MVA for given set of input variables
   IMethod* method = 0;
   std::map<TString, IMethod*>::iterator it = fMethodMap.find( methodTag );
   if (it == fMethodMap.end()) {
      for (it = fMethodMap.begin(); it!=fMethodMap.end(); it++) fLogger << "M" << it->first << Endl;
      fLogger << kFATAL << "<EvaluateMVA> unknown classifier in map: " << method << "; "
              << "you looked for " << methodTag<< " while the available methods are : " << Endl;
   }
   else method = it->second;

   MethodBase* kl = dynamic_cast<MethodBase*>(method);
   if (mvaVal == -9999999) mvaVal = kl->GetMvaValue();

   return kl->GetProba( mvaVal, ap_sig );
}

//_______________________________________________________________________
Double_t TMVA::Reader::GetRarity( const TString& methodTag, Double_t mvaVal )
{
  // evaluates the MVA's rarity
   IMethod* method = 0;
   std::map<TString, IMethod*>::iterator it = fMethodMap.find( methodTag );
   if (it == fMethodMap.end()) {
      for (it = fMethodMap.begin(); it!=fMethodMap.end(); it++) fLogger << "M" << it->first << Endl;
      fLogger << kFATAL << "<EvaluateMVA> unknown classifier in map: \"" << method << "\"; "
              << "you looked for \"" << methodTag<< "\" while the available methods are : " << Endl;
   }
   else method = it->second;

   MethodBase* kl = dynamic_cast<MethodBase*>(method);
   if (mvaVal == -9999999) mvaVal = kl->GetMvaValue();

   return kl->GetRarity( mvaVal );
}

// ---------------------------------------------------------------------------------------
// ----- methods related to the decoding of the input variable names ---------------------
// ---------------------------------------------------------------------------------------

//_______________________________________________________________________
void TMVA::Reader::DecodeVarNames( const std::string& varNames ) 
{
   // decodes "name1:name2:..." form
   size_t ipos = 0, f = 0;
   while (f != varNames.length()) {
      f = varNames.find( ':', ipos );
      if (f > varNames.length()) f = varNames.length();
      std::string subs = varNames.substr( ipos, f-ipos ); ipos = f+1;    
      DataInfo().AddVariable( subs.c_str() );
   }  
}

//_______________________________________________________________________
void TMVA::Reader::DecodeVarNames( const TString& varNames )
{
   // decodes "name1:name2:..." form

   TString format;  
   Int_t   n = varNames.Length();
   TString format_obj;

   for (int i=0; i< n+1 ; i++) {
      format.Append(varNames(i));
      if ( (varNames(i)==':') || (i==n)) {
         format.Chop();
         format_obj = format;
         format_obj.ReplaceAll("@","");
         DataInfo().AddVariable( format_obj );
         format.Resize(0); 
      }
   }
} 
