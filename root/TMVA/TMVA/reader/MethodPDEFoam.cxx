// @(#)root/tmva $Id: MethodPDEFoam.cxx,v 1.5 2008/08/08 16:33:28 alexvoigt Exp $
// Author: Andreas Hoecker, Xavier Prudent, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate Data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodPDEFoam                                                          *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Original author of this Fisher-Discriminant implementation:                    *
 *      Andre Gaidot, CEA-France;                                                 *
 *      (Translation from FORTRAN)                                                *
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

//_______________________________________________________________________

#include <iomanip>
#include <cassert>

#include "TMath.h"
#include "Riostream.h"
#include "TFile.h"
#include "TXMLFile.h"

#include "TMVA/VariableTransformBase.h"
#include "TMVA/MethodPDEFoam.h"
#include "TMVA/Tools.h"
#include "TMatrix.h"
#include "TMVA/Ranking.h"
#include "TMVA/Types.h"
#include "TMVA/ClassifierFactory.h"

REGISTER_METHOD(PDEFoam)

ClassImp(TMVA::MethodPDEFoam)

//_______________________________________________________________________
TMVA::MethodPDEFoam::MethodPDEFoam( const TString& jobName, const TString& methodTitle, DataSetInfo& dsi, 
                                    const TString& theOption, TDirectory* theTargetDir )
   : MethodBase( jobName, methodTitle, dsi, theOption, theTargetDir )
{
   // init PDEFoam objects
   InitPDEFoam();

   // interpretation of configuration option string
   SetConfigName( TString("Method") + GetMethodName() );
   DeclareOptions();
   ParseOptions();
   ProcessOptions();

   //   InitPDEFoam();
}

//_______________________________________________________________________
TMVA::MethodPDEFoam::MethodPDEFoam( DataSetInfo& dsi, 
                                    const TString& theWeightFile,  
                                    TDirectory* theTargetDir )
   : MethodBase( dsi, theWeightFile, theTargetDir )
{
   // constructor to calculate the Fisher-MVA from previously generatad 
   // coefficients (weight file)

   InitPDEFoam();
   DeclareOptions();

   // after this constructor: ReadWeightsFromStream() is called
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::InitPDEFoam( void )
{
   // default initialization called by all constructors
   SetMethodName( "PDEFoam" );
   SetMethodType( TMVA::Types::kPDEFoam );  
   SetTestvarName();

   // init PDEFoam options
   fSigBgSeparated = kTRUE; // default values for options
   fFrac           = 0.999;
   fDiscrErrCut    = 999.;
   fVolFrac        = 50;
   fnCells         = 1000;
   fnSampl         = 2000;
   fnBin           = 150;
   fOptRej         = 1;
   fOptDrive       = 1;
   fEvPerBin       = 10000;
   fChat           = 0;

   fCompress        = true;

   for(int i=0; i<FOAM_NUMBER; i++){
      foam[i] = NULL;
   };

   PseRan = new TRandom3();  // Create random number generator for PDEFoams
   PseRan->SetSeed(4356);

   SetSignalReferenceCut( 0.0 );
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::DeclareOptions() 
{
   //
   // MethodPDEFoam options:
   //
   DeclareOptionRef( fSigBgSeparated = kTRUE, "SigBgSeparated", "Seperate Signal and Bg" );
   DeclareOptionRef( fFrac = 0.999, "Frac", "Fraction of events in foam" );
   DeclareOptionRef( fDiscrErrCut = 999., "DiscrErrCut", "Cut on discrimant error" );
   DeclareOptionRef( fVolFrac = 50, "VolFrac", "volume fraction (used for density calculation during buildup)");
   DeclareOptionRef( fnCells = 1000, "nCells", "Number of Cells");
   DeclareOptionRef( fnSampl = 2000, "nSampl", "Number of MC events per cell in build-up");
   DeclareOptionRef( fnBin = 150, "nBin", "Number of bins in build-up");
   DeclareOptionRef( fOptRej = 1, "OptRej", "Wted events for OptRej=0; wt=1 for OptRej=1 (default)");
   DeclareOptionRef( fOptDrive = 1, "OptDrive", "(D=2) Option, type of Drive =1,2 for Variance,WtMax driven reduction");
   DeclareOptionRef( fEvPerBin = 10000, "EvPerBin", "Maximum events (equiv.) per bin in buid-up");
   DeclareOptionRef( fChat = 0, "Chat", "Chat level");
   DeclareOptionRef( fCompress = true, "Compress", "Compress XML file");
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::ProcessOptions() 
{
	//std::cout << "PDEFoam-DEBUG: process options" << std::endl;

   // process user options
   MethodBase::ProcessOptions();

   if (!(fFrac>0. && fFrac<=1.))
      fFrac = 0.999;

   CheckForUnusedOptions();
}

//_______________________________________________________________________
TMVA::MethodPDEFoam::~MethodPDEFoam( void )
{
   // destructor
   if (fSigBgSeparated){
     for(int i=0; i<FOAM_NUMBER; i++){
        delete foam[i];
     }
   }
   else {
      delete foam[0];
   }
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::CalcXminXmax(){
   // determine range for all dimensions
   // loop over all testing events -> Get Xmin and Xmax

   Xmin.clear();
   Xmax.clear();
   UInt_t kDim = GetNvar(); // == Data()->GetNVariables();
   Double_t xmin[kDim], xmax[kDim];

   // set default values
   for (UInt_t dim=0; dim<kDim; dim++) {
      xmin[dim]=-1.;
      xmax[dim]=1.;
   }

   Int_t nevoutside = (Int_t)((Data()->GetNTrainingEvents())*(1-fFrac)); // number of events that are outside the range
   Int_t rangehistbins = 1000;                               // number of bins in histos
  
   // loop over all testing singnal and BG events and clac minimal and maximal value of every variable
   for (Long64_t i=0; i<(Data()->GetNTestEvents()); i++) { // events loop
      const Event* ev = Data()->GetTestEvent(i);    
      for (UInt_t dim=0; dim<kDim; dim++) { // variables loop
	 if (ev->GetVal(dim)<xmin[dim])
	    xmin[dim]=ev->GetVal(dim);
	 if (ev->GetVal(dim)>xmax[dim])
	    xmax[dim]=ev->GetVal(dim);
      }
   }

   // Create and fill histograms for each dimension (with same events as before), to determine range 
   // based on number of events outside the range
   TH1F *range_h[kDim]; 
   char text[200];
   for (UInt_t dim=0; dim<kDim; dim++) {
      sprintf(text, "range%i", dim);
      range_h[dim]  = new TH1F(text, "range", rangehistbins, xmin[dim], xmax[dim]);
   }

   // fill all testing events into histos 
   for (Long64_t i=0; i<Data()->GetNTestEvents(); i++) {
      const Event* ev = Data()->GetTestEvent(i);
      for (UInt_t dim=0; dim<kDim; dim++) 
	 range_h[dim]->Fill(ev->GetVal(dim));
   };

   // calc Xmin, Xmax from Histos
   for (UInt_t dim=0; dim<kDim; dim++) { 
      for (Int_t i=1; i<(rangehistbins+1); i++) { // loop over bins 
	 if (range_h[dim]->Integral(0, i) > nevoutside) { // calc left limit (integral over bins 0..i = nevoutside)
	    xmin[dim]=range_h[dim]->GetBinLowEdge(i);
	    break;
	 }
      }
      for (Int_t i=rangehistbins; i>0; i--) { // calc left limit (integral over bins i..max = nevoutside)
	 if (range_h[dim]->Integral(i, (rangehistbins+1)) > nevoutside) {
	    xmax[dim]=range_h[dim]->GetBinLowEdge(i+1);
	    break;
	 }
      }
   }  
   // now xmin[] and xmax[] contain upper/lower limits for every dimension

   // copy xmin[], xmax[] values to the class variable
   Xmin.clear();
   Xmax.clear();
   for (UInt_t dim=0; dim<kDim; dim++) { 
      Xmin.push_back(xmin[dim]);
      Xmax.push_back(xmax[dim]);
   }

   // delete histos
   for (UInt_t dim=0; dim<kDim; dim++) {
      delete range_h[dim];
   }

   return;
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::Train( void )
{
   fLogger << "calculate Xmin and Xmax" << endl;
   CalcXminXmax();

   // select method
   if(fSigBgSeparated){
    
      TString foamcaption[2];
      foamcaption[0] = "SignalFoam";
      foamcaption[1] = "BgFoam";

      for(int i=0; i<FOAM_NUMBER; i++){
	 // create 2 PDEFoams
	 foam[i] = new PDEFoam(foamcaption[i]);

	 // set Options VolFrac, kDim, ...
	 foam[i]->SetkDim(        GetNvar());  // Mandatory!!!
	 foam[i]->SetVolumeFraction(fVolFrac); // Mandatory!!!
	 foam[i]->SetnCells(      fnCells);    // optional
	 foam[i]->SetnSampl(      fnSampl);    // optional
	 foam[i]->SetnBin(        fnBin);      // optional
	 foam[i]->SetOptRej(      fOptRej);    // optional
	 foam[i]->SetOptDrive(    fOptDrive);  // optional
	 foam[i]->SetEvPerBin(    fEvPerBin);  // optional
	 foam[i]->SetChat(        fChat);      // optional

	 // SetPseRan
	 foam[i]->SetPseRan(PseRan);

	 // Init PDEFoam
	 foam[i]->Init();

	 // Set Xmin, Xmax
	 for (UInt_t idim=0; idim<GetNvar(); idim++) { // set same upper/ lower limit in both foams
	    fLogger<<" set foam["<<i<<"]->SetXmin("<<idim<<", "<<Xmin.at(idim)<<")"<<endl;
	    fLogger<<" set foam["<<i<<"]->SetXmax("<<idim<<", "<<Xmax.at(idim)<<")"<<endl;
	    foam[i]->SetXmin(idim,Xmin.at(idim));
	    foam[i]->SetXmax(idim,Xmax.at(idim));
	 }

	 fLogger << "filling Foam[" << i << "] with events" << endl;
	 // loop over all events -> fill PDEFoams
	 if(i==0){ // singal 
	    for (Long64_t k=0; k<Data()->GetNTrainingEvents(); k++) { // loop over singal events // GetNEvtSigTrain
	       const Event* ev = Data()->GetTrainingEvent(k);
	       std::vector<Double_t> xvec = GetVect(ev);
	       if (ev->IsSignal()){
		  foam[i]->InputEvent(xvec, true, 1., 1.); // give event to signal-foam
	       }
	    }
	 }
	 else{ // background
	    for (Long64_t k=0; k<Data()->GetNTrainingEvents(); k++) { // loop over bg events
	       const Event* ev = Data()->GetTrainingEvent(k); 
	       std::vector<Double_t> xvec = GetVect(ev);
	       if (!(ev->IsSignal())){
		  foam[i]->InputEvent(xvec, false, 1., 1.); // give event to bg-foam
	       }
	    }
	 }

	 // debug: output of density specific variables
	 //foam[i]->PrintDensity();

	 fLogger << "build up foam[" << i << "]" << endl;
	 // build foam
	 foam[i]->Create();

	 // Reset Cell Integrals
	 foam[i]->ResetCellIntegrals();

	 // loop over all events -> fill foam cells
	 if(i==0){ // singal 
	    for (Long64_t k=0; k<Data()->GetNTrainingEvents(); k++) {
	       const Event* ev = Data()->GetTrainingEvent(k); 
	       std::vector<Double_t> xvec = GetVect(ev);
	       if (ev->IsSignal()){
		  foam[i]->SetCell(&xvec[0], 1, 1);
	       }
	    } // signal event loop
	 } // signal
	 else{ // bg
	    for (Long64_t k=0; k<Data()->GetNTrainingEvents(); k++) {
	       const Event* ev = Data()->GetTrainingEvent(k); 
	       std::vector<Double_t> xvec = GetVect(ev);
	       if (!(ev->IsSignal())){
		  foam[i]->SetCell(&xvec[0], 1, 1);
	       }
	    } // bg event loop
	 } // bg
	 
	 // debug
	 //foam[i]->DisplayCellContent();

      } // loop over foam[i] 
   } // if (fSigBgSaperated)

   /////////////////////////////////////////////////////////////////////////////
   //  End of first method (signal and bg are in seperate foams).
   //
   //  Begin second method (only one foam, which contains discriminator).
   /////////////////////////////////////////////////////////////////////////////

   else{ // fill ratio N_Sig/(N_Sig+N_Bg) into foam
      // create only 1 PDEFoam
      foam[0] = new PDEFoam("DiscrFoam");

      // set Options
      foam[0]->SetkDim(        GetNvar());  // Mandatory!!!
      foam[0]->SetVolumeFraction(fVolFrac); // Mandatory!!!
      foam[0]->SetnCells(      fnCells);    // optional
      foam[0]->SetnSampl(      fnSampl);    // optional
      foam[0]->SetnBin(        fnBin);      // optional
      foam[0]->SetOptRej(      fOptRej);    // optional
      foam[0]->SetOptDrive(    fOptDrive);  // optional
      foam[0]->SetEvPerBin(    fEvPerBin);  // optional
      foam[0]->SetChat(        fChat);      // optional

      foam[0]->FillDiscriminator(true);     // foam fills discriminator in cells, instead of number of events

      // random generator
      foam[0]->SetPseRan(PseRan);

      // Init PDEFoam
      foam[0]->Init();

      // Set Xmin, Xmax
      for (UInt_t idim=0; idim<GetNvar(); idim++) { // set upper/ lower limit in foams
	 fLogger<<" set foam["<<0<<"]->SetXmin("<<idim<<", "<<Xmin.at(idim)<<")"<<endl;
	 fLogger<<" set foam["<<0<<"]->SetXmax("<<idim<<", "<<Xmax.at(idim)<<")"<<endl;
	 foam[0]->SetXmin(idim,Xmin.at(idim));
	 foam[0]->SetXmax(idim,Xmax.at(idim));
      }

      fLogger << "filling Foam[" << 0 << "] with events" << endl;
      // fill training signal and bg events into this foam
      for (Long64_t k=0; k<Data()->GetNTrainingEvents(); k++) { // loop over all training events
	 const Event* ev = Data()->GetTrainingEvent(k); 
	 std::vector<Double_t> xvec = GetVect(ev);

	 // give event to foam -> insert to BinarySearchTree
	 foam[0]->InputEvent(xvec, ev->IsSignal(), 1., 1.);
      }

      fLogger << "build up foam[" << 0 << "]" << endl;
      // build foam -> difference to other method!!
      foam[0]->Create();

      fLogger << "resetting cell integrals" << endl;
      // Reset Cell Integrals
      foam[0]->ResetCellIntegrals();

      fLogger << "filling foam cells with event numbers" << endl;
      // loop over all training events -> fill foam cells with N_sig and N_Bg
      for (UInt_t k=0; k<Data()->GetNTrainingEvents(); k++) {
	 const Event* ev = Data()->GetTrainingEvent(k); 
	 std::vector<Double_t> xvec = GetVect(ev);
	 foam[0]->AddCellEvents(&xvec[0], ev->IsSignal(), 1., 1.);
      } // event loop
      
      fLogger << "calculate cell discriminator"<< endl;
      // calc discriminator (and it's error) for each cell
      foam[0]->CalcCellDiscr();

   }; // if !fSigBgSeparated

   // nice output:
   PrintCoefficients();
}

//_______________________________________________________________________
Double_t TMVA::MethodPDEFoam::GetMvaValue()
{
   const Event* ev = GetEvent();
   Double_t discr = 0.;
   Double_t discr_error = 0.;

   if (fSigBgSeparated){
      std::vector<Double_t> xvec = GetVect(ev);

      Double_t density_sig, density_bg;
      density_sig = foam[0]->GetCellDensity(&xvec[0]); // get number of events per cell volume (cell that includes xvec)
      density_bg  = foam[1]->GetCellDensity(&xvec[0]);
     
      // calc disciminant
      if ( (density_sig+density_bg) > 0 )
	 discr = density_sig/(density_sig+density_bg);
      else
	 discr = 0.5; // assume 50% signal probability, if no events found (bad assumption, but can be overruled by cut on error)

      // do error estimation 
      Double_t neventsB = foam[1]->GetCellEntries(&xvec[0]);
      Double_t neventsS = foam[0]->GetCellEntries(&xvec[0]);
      Double_t scaleB = 1.;
      Double_t errorS = TMath::Sqrt(neventsS); // estimation of statistical error on counted signal events
      Double_t errorB = TMath::Sqrt(neventsB); // estimation of statistical error on counted background events

      if (neventsS == 0) // no signal events in cell
	 errorS = 1.;
      if (neventsB == 0) // no bg events in cell
	 errorB = 1.;

      if ( (neventsS>1e-10) || (neventsB>1e-10) ) // eq. (5) in paper T.Carli, B.Koblitz 2002
	 discr_error = TMath::Sqrt( TMath::Power ( scaleB*neventsB
						   / TMath::Power((neventsS+scaleB*neventsB),2)
						   * errorS, 2) +
				    TMath::Power ( (scaleB*neventsS)
						   / TMath::Power((neventsS+scaleB*neventsB),2)
						   * errorB, 2) );
      else
	 discr_error = 1.;

      if (discr_error < 1e-10)
	 discr_error = 1.;

      if (discr_error > fDiscrErrCut)   // do not classify, if error too big
	 discr = -1.;
   }

   /////////////////////////////////////////////////////////////////////////////
   //  End of first method (signal and bg are in seperate foams).
   //
   //  Begin second method (only one foam, which contains discriminator).
   /////////////////////////////////////////////////////////////////////////////

   else{ // Signal and Bg not seperated
      std::vector<Double_t> xvec = GetVect(ev);
      
      // get discriminator direct from the foam
      discr       = foam[0]->GetCellDiscr(&xvec[0]);
      discr_error = foam[0]->GetCellDiscrError(&xvec[0]);

      if (discr_error > fDiscrErrCut)   // do not classify, if error too big
	 discr = -1.;
   }

   return discr;
}

//_______________________________________________________________________
std::vector<Double_t> TMVA::MethodPDEFoam::GetVect(const Event *ev){
   std::vector<Double_t> xvec;
   xvec.clear();

   for (UInt_t dim=0; dim<GetNvar(); dim++) // loop over variables in event, and fill xvec with variables
      xvec.push_back(ev->GetVal(dim));

   return xvec;
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::PrintCoefficients( void ) 
{
   // display Fisher coefficients and discriminating power for each variable
   // check maximum length of variable name
   fLogger << kINFO << "Results for PDEFoam coefficients: ..." << Endl;   
}
  
//_______________________________________________________________________
void  TMVA::MethodPDEFoam::WriteWeightsToStream( std::ostream& o ) const
{  

   // save options // evtl. done by TMVA ???
   o << fSigBgSeparated << endl;                 // Seperate Sig and Bg, or not
   o << std::setprecision(12) << fFrac << endl;  // Fraction used for calc of Xmin, Xmax
   o << std::setprecision(12) << fDiscrErrCut << endl; // cut on discrimant error
   o << fVolFrac << endl;                        // volume fraction (used for density calculation during buildup)
   o << fnCells << endl;                         // Number of Cells  (500)
   o << fnSampl << endl;                         // Number of MC events per cell in build-up (1000)
   o << fnBin << endl;                           // Number of bins in build-up (100)
   o << fOptRej << endl;                         // Wted events for OptRej=0; wt=1 for OptRej=1 (default)
   o << fOptDrive << endl;                       // (D=2) Option, type of Drive =1,2 for Variance,WtMax driven reduction
   o << fEvPerBin << endl;                       // Maximum events (equiv.) per bin in buid-up (1000) 
   o << fChat << endl;                           // Chat level
   o << fCompress << endl;                       // Compress XML file

   // save range
   for (UInt_t i=0; i<Xmin.size(); i++) 
      o << std::setprecision(12) << Xmin.at(i) << endl;
   for (UInt_t i=0; i<Xmin.size(); i++) 
      o << std::setprecision(12) << Xmax.at(i) << endl;


   // write foam
   if (fSigBgSeparated){
      for(int i=0; i<FOAM_NUMBER; i++)
	 o << *(foam[i]);
   }
   else
      o << *(foam[0]);
    
   // !!! testing cell Elements !!!
   //foam[0]->InitCellVariables();

   // write foams
   TString rfname( GetWeightFileName() ); rfname.ReplaceAll( ".txt", ".xml" );
   o << "Foams stored in root i/o file: " << rfname << std::endl; 
   TXMLFile *RootFile;
   if (fCompress)
      RootFile = new TXMLFile(rfname, "RECREATE", "foamfile", 9);
   else
      RootFile = new TXMLFile(rfname, "RECREATE");

   if (fSigBgSeparated){
      foam[0]->Write("SignalFoam");
      foam[1]->Write("BgFoam");
   }
   else
      foam[0]->Write("DiscrFoam");

   RootFile->Close();
}
  
//_______________________________________________________________________
void  TMVA::MethodPDEFoam::ReadWeightsFromStream( istream& istr )
{
   // read options
   istr >> fSigBgSeparated;                 // Seperate Sig and Bg, or not
   istr >> fFrac;                           // Fraction used for calc of Xmin, Xmax
   istr >> fDiscrErrCut;                    // cut on discrimant error
   istr >> fVolFrac;                        // volume fraction (used for density calculation during buildup)
   istr >> fnCells;                         // Number of Cells  (500)
   istr >> fnSampl;                         // Number of MC events per cell in build-up (1000)
   istr >> fnBin;                           // Number of bins in build-up (100)
   istr >> fOptRej;                         // Wted events for OptRej=0; wt=1 for OptRej=1 (default)
   istr >> fOptDrive;                       // (D=2) Option, type of Drive =1,2 for Variance,WtMax driven reduction
   istr >> fEvPerBin;                       // Maximum events (equiv.) per bin in buid-up (1000) 
   istr >> fChat;   
   istr >> fCompress;                       // compress XML file
   
   // clear old range and prepare new range
   Xmin.clear();
   Xmax.clear();
   for (UInt_t i=0; i<GetNvar(); i++){
      Xmin.push_back(0.);
      Xmax.push_back(0.);
   }
   // read range
   for (UInt_t i=0; i<GetNvar(); i++) 
      istr >> Xmin.at(i);
   for (UInt_t i=0; i<GetNvar(); i++) 
      istr >> Xmax.at(i);

   TString foamcaption[2];
   foamcaption[0] = "SignalFoam";
   foamcaption[1] = "BgFoam";
   // create foam
   if (fSigBgSeparated){
      for(int i=0; i<FOAM_NUMBER; i++){
	 foam[i] = new PDEFoam(foamcaption[i]);
 	 istr >> *(foam[i]);
      }
   }
   else{
      foam[0] = new PDEFoam("DiscrFoam");
      istr >> *(foam[0]);
   }

   // read foams
   TString rfname( GetWeightFileName() ); rfname.ReplaceAll( ".txt", ".xml" );
   TXMLFile *RootFile= new TXMLFile(rfname, "READ");
   if (fSigBgSeparated){
      foam[0] = (PDEFoam*)RootFile->Get("SignalFoam");
      foam[1] = (PDEFoam*)RootFile->Get("BgFoam");
   }
   else
      foam[0] = (PDEFoam*)RootFile->Get("DiscrFoam");
   
   //RootFile->Close();        // PROBLEM!!  -> save foams before closing file!
                               // TRef of cells are lost -> return NULL after closing file
   // foam[0]->PrintCells();   // foam not loaded after RootFile->Close()
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::MakeClassSpecific( std::ostream& fout, const TString& className ) const
{
   // write PDEFoam-specific classifier response
}

//_______________________________________________________________________
void TMVA::MethodPDEFoam::GetHelpMessage() const
{
}

