// @(#)root/tmva $Id: MethodCompositeBase.cxx,v 1.6 2008/07/12 20:21:39 stelzer Exp $   
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
 *      U. of Victoria, Canada                                                    * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      LAPP, Annecy, France                                                      *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_______________________________________________________________________
//
// This class is virtual class meant to combine more than one classifier//
// together. The training of the classifiers is done by classes that are//
// derived from this one, while the saving and loading of weights file  //
// and the evaluation is done here.                                     //
//_______________________________________________________________________

#include <algorithm>
#include <iomanip>
#include <vector>

#include "Riostream.h"
#include "TRandom.h"
#include "TMath.h"
#include "TObjString.h"

#include "TMVA/MethodCompositeBase.h"
#include "TMVA/MethodBoost.h"
#include "TMVA/MethodBase.h"
#include "TMVA/Tools.h"
#include "TMVA/Types.h"
#include "TMVA/Factory.h"
#include "TMVA/ClassifierFactory.h"

using std::vector;

ClassImp(TMVA::MethodCompositeBase)

//_______________________________________________________________________
TMVA::MethodCompositeBase::MethodCompositeBase( const TString& jobName, 
                                                const TString& methodTitle,
                                                DataSetInfo& theData,
                                                const TString& theOption,
                                                TDirectory* theTargetDir )
   : TMVA::MethodBase( jobName, methodTitle, theData, theOption, theTargetDir )
{}

//_______________________________________________________________________
TMVA::MethodCompositeBase::MethodCompositeBase( DataSetInfo& dsi,
                                                const TString& weightFile, 
                                                TDirectory* theTargetDir )
   : TMVA::MethodBase( dsi, weightFile, theTargetDir )
{}

//_______________________________________________________________________
TMVA::IMethod*
      TMVA::MethodCompositeBase::GetMethod( const TString &methodTitle ) const
{
   // returns pointer to MVA that corresponds to given method title
   vector<IMethod*>::const_iterator itrMethod    = fMethods.begin();
   vector<IMethod*>::const_iterator itrMethodEnd = fMethods.end();
   //
   for (; itrMethod != itrMethodEnd; itrMethod++) {
      MethodBase* mva = dynamic_cast<MethodBase*>(*itrMethod);    
      if ( (mva->GetMethodTitle())==methodTitle ) return mva;
   }
   return 0;
}

//_______________________________________________________________________
TMVA::IMethod*
      TMVA::MethodCompositeBase::GetMethod( const Int_t index ) const
{
   // returns pointer to MVA that corresponds to given method index
   vector<IMethod*>::const_iterator itrMethod = fMethods.begin()+index;
   if (itrMethod<fMethods.end())
      return *itrMethod;
   else return 0;
}

//_______________________________________________________________________
void  TMVA::MethodCompositeBase::WriteWeightsToStream( ostream& o ) const
{
   o << "NClassifier= " << fMethods.size() <<endl;
   for (UInt_t i=0; i< fMethods.size(); i++) 
   {
      MethodBase* method = dynamic_cast<MethodBase*>(fMethods[i]);
      o << "Classifier : " << method->GetMethodName()  << "; Index= " << i << " ; Weight= " << fMethodWeight[i] << endl;
      if (GetMethodType() != Types::kBoost || i==0) 
      {
         //for the booster - writing the values only for the first method
         o << "Job Name= " << method->GetJobName()  << endl;
         o << "Title= " << method->GetMethodTitle()  << endl;
         o << "Option= " << method->GetOptions()  << endl;
      }
      method->WriteWeightsToStream(o);
   }
}

//_______________________________________________________________________
TMVA::MethodCompositeBase::~MethodCompositeBase( void )
{
   // delete methods
   vector<IMethod*>::iterator itrMethod = fMethods.begin();
   for (; itrMethod != fMethods.end(); itrMethod++) {
      fLogger << kVERBOSE << "Delete method: " << (*itrMethod)->GetName() << Endl;    
      delete (*itrMethod);
   }
   fMethods.clear();
}

//_______________________________________________________________________
void  TMVA::MethodCompositeBase::ReadWeightsFromStream( istream& istr )
{
   TString var, dummy;
   TString methodName, methodTitle=GetMethodTitle(),
    jobName=GetJobName(),optionString=GetOptions();
   UInt_t methodNum; Double_t methodWeight;
   // and read the Weights (BDT coefficients)  
   istr >> dummy >> methodNum;
   fLogger << kINFO << "Read " << methodNum << " Classifiers" << Endl;
   for (UInt_t i=0;i<fMethods.size();i++) delete fMethods[i];
   fMethods.clear();
   fMethodWeight.clear();
   for (UInt_t i=0; i<methodNum; i++)
   {
      istr >> dummy >> methodName >>  dummy >> fMethodIndex >> dummy >> methodWeight;
      if ((UInt_t)fMethodIndex != i)
      {
         fLogger << kFATAL << "Error while reading weight file; mismatch MethodIndex=" 
               << fMethodIndex << " i=" << i 
               << " MethodName " << methodName
               << " dummy " << dummy
               << " MethodWeight= " << methodWeight 
               << Endl;
      }
      if (GetMethodType() != Types::kBoost || i==0)
      {
         istr >> dummy >> jobName;
         istr >> dummy >> methodTitle;
         istr >> dummy >> optionString;
         if (GetMethodType() == Types::kBoost)
            ((TMVA::MethodBoost*)this)->BookMethod( Types::Instance().GetMethodType( methodName), methodTitle,  optionString );
      }
      else methodTitle=Form("%s (%04i)",GetMethodTitle().Data(),fMethodIndex);
      fMethods.push_back(ClassifierFactory::Instance().Create(
         std::string(methodName),jobName, methodTitle,DataInfo(),optionString));
      fMethodWeight.push_back(methodWeight);
      dynamic_cast<MethodBase*>(fMethods.back())->ReadWeightsFromStream(istr);
   }
}

Double_t TMVA::MethodCompositeBase::GetMvaValue()
{
   Double_t MvaValue=0;
   for (UInt_t i=0;i< fMethods.size(); i++)
      MvaValue+=fMethods[i]->GetMvaValue()*fMethodWeight[i];
   return MvaValue;
}
