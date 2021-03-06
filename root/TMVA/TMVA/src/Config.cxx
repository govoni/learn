// @(#)root/tmva $Id: Config.cxx,v 1.16 2008/05/10 20:45:30 andreas.hoecker Exp $   
// Author: Andreas Hoecker, Joerg Stelzer, Fredrik Tegenfeldt, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Config                                                                *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation                                                            *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Andreas Hoecker    <Andreas.Hocker@cern.ch>     - CERN, Switzerland       *
 *      Joerg Stelzer      <Joerg.Stelzer@cern.ch>      - CERN, Switzerland       *
 *      Fredrik Tegenfeldt <Fredrik.Tegenfeldt@cern.ch> - Iowa State U., USA      *
 *      Helge Voss         <Helge.Voss@cern.ch>         - MPI-K Heidelberg, GER   *
 *                                                                                *
 * Copyright (c) 2006:                                                            *
 *      CERN, Switzerland                                                         *
 *      Iowa State U., USA                                                        *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://mva.sourceforge.net/license.txt)                                       *
 **********************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Config                                                               //
//                                                                      //
// Singleton class for global configuration settings used by TMVA       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TMVA/Config.h"

ClassImp(TMVA::Config)

TMVA::Config* TMVA::Config::fgConfigPtr = 0;

TMVA::Config& TMVA::gConfig() { return TMVA::Config::Instance(); }

//_______________________________________________________________________
TMVA::Config::Config() :
   fUseColoredConsole    ( kTRUE  ),
   fSilent               ( kFALSE ),
   fWriteOptionsReference( kFALSE ),
   fLogger( "Config" )
{
   // constructor - set defaults
   
   // plotting
   fVariablePlotting.fTimesRMS = 8.0;
   fVariablePlotting.fNbins1D  = 60;
   fVariablePlotting.fNbins2D  = 300;
   fVariablePlotting.fMaxNumOfAllowedVariablesForScatterPlots = 20;
   fVariablePlotting.fNbinsXOfROCCurve                        = 100;

   // IO names
   fIONames.fWeightFileDir           = "weights";
   fIONames.fWeightFileExtension     = "weights";
   fIONames.fOptionsReferenceFileDir = "optionInfo";
}

//_______________________________________________________________________
TMVA::Config::~Config()
{
   // destructor
}
