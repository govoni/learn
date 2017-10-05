// @(#)root/tmva $Id: MethodPDEFoam.h,v 1.3 2008/08/08 16:33:28 alexvoigt Exp $
// Author: Andreas Hoecker, Xavier Prudent, Joerg Stelzer, Helge Voss, Kai Voss 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodPDEFoam                                                         *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Analysis of PDEFoam discriminant (PDEFoam or Mahalanobis approach)        *
 *                                                                                *
 * Original author of this PDEFoam-Discriminant implementation:                   *
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
 *                                                                                *
 **********************************************************************************/

#ifndef ROOT_TMVA_MethodPDEFoam
#define ROOT_TMVA_MethodPDEFoam

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MethodPDEFoam                                                        //
//                                                                      //
// Analysis of PDEFoam discriminant                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TRandom3.h"

#ifndef ROOT_TMVA_MethodBase
#include "TMVA/MethodBase.h"
#endif

#ifndef ROOT_TMVA_PDEFoam
#include "TMVA/PDEFoam.h"
#endif

#define FOAM_NUMBER 2

namespace TMVA {

  class MethodPDEFoam : public MethodBase {

  public:
      
    MethodPDEFoam( const TString& jobName, 
		   const TString& methodTitle, 
		   DataSetInfo& dsi,
		   const TString& theOption = "PDEFoam",
		   TDirectory* theTargetDir = 0 );

    MethodPDEFoam( DataSetInfo& dsi, 
		   const TString& theWeightFile,  
		   TDirectory* theTargetDir = NULL );

    virtual ~MethodPDEFoam( void );
    
    // training method
    void Train( void );

    using MethodBase::WriteWeightsToStream;
    using MethodBase::ReadWeightsFromStream;

    // write weights to stream
    void WriteWeightsToStream( std::ostream & o) const;

    // read weights from stream
    void ReadWeightsFromStream( std::istream & i );

    // calculate the MVA value
    Double_t GetMvaValue();

    // ranking of input variables
    const Ranking* CreateRanking(){ return 0; };

  protected:

    // make ROOT-independent C++ class for classifier response (classifier-specific implementation)
    void MakeClassSpecific( std::ostream&, const TString& ) const;

    // get help message text
    void GetHelpMessage() const;

    // calculate Xmin and Xmax for Foam
    void CalcXminXmax();

    // get Vector from Event
    std::vector<Double_t> GetVect(const Event*);

  private:

    // the option handling methods
    void DeclareOptions();
    void ProcessOptions();
  
    // nice output
    void PrintCoefficients( void );

    // options to be used
    Bool_t        fSigBgSeparated;  // Separate Sig and Bg, or not
    Double_t      fFrac;            // Fraction used for calc of Xmin, Xmax
    Double_t      fDiscrErrCut;     // cut on discrimant error
    Int_t         fVolFrac;         // volume fraction (used for density calculation during buildup)
    Int_t         fnCells;          // Number of Cells  (500)
    Int_t         fnSampl;          // Number of MC events per cell in build-up (1000)
    Int_t         fnBin;            // Number of bins in build-up (100)
    Int_t         fOptRej;          // Wted events for OptRej=0; wt=1 for OptRej=1 (default)
    Int_t         fOptDrive;        // (D=2) Option, type of Drive =1,2 for Variance,WtMax driven reduction
    Int_t         fEvPerBin;        // Maximum events (equiv.) per bin in buid-up (1000) 
    Int_t         fChat;            // Chat level

    Bool_t        fCompress;        // compress XML file
     
    std::vector<Double_t> Xmin, Xmax; // range for histograms and foams

    // foams and densities
    PDEFoam* foam[FOAM_NUMBER]; // foam[0]=signal, if Sig and BG are Seperated; else foam[0]=signal/bg
                                // foam[1]=background, if Sig and BG are Seperated; else it is not used
    TRandom3 *PseRan; // random generator for PDEFoams

    // default initialisation called by all constructors
    void InitPDEFoam( void );

    ClassDef(MethodPDEFoam,0); // Analysis of PDEFoam discriminant (PDEFoam or Mahalanobis approach) 
  };

} // namespace TMVA

#endif // MethodPDEFoam_H
