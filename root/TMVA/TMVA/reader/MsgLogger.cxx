// @(#)root/tmva $Id: MsgLogger.cxx,v 1.4 2007/09/10 19:50:05 orcohen Exp $
// Author: Attila Krasznahorkay

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MsgLogger                                                             *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Implementation (see header for description)                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch> - CERN, Switzerland   *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

// Local include(s):
#include "TMVA/MsgLogger.h"

// Local include(s):
#include "TMVA/Config.h"

// STL include(s):
#include <iomanip>
#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;

// ROOT include(s):

ClassImp(TMVA::MsgLogger)

// this is the hard-coded maximum length of the source names
static const string::size_type MAXIMUM_SOURCE_NAME_LENGTH = 13;
// this is the hardcoded prefix
static const string PREFIX = "--- ";
// this is the hardcoded suffix
static const string SUFFIX = ": ";

//temporaly disables the logger
static Bool_t fOutputSupressed=kFALSE;
void  TMVA::MsgLogger::SupressOutput()   {fOutputSupressed=kTRUE;}
void  TMVA::MsgLogger::UnsupressOutput() {fOutputSupressed=kFALSE;}

TMVA::MsgLogger::MsgLogger( const TObject* source, EMsgType minType )
   : fObjSource( source ), 
     fStrSource( "" ), 
     fPrefix( PREFIX ), 
     fSuffix( SUFFIX ), 
     fActiveType( kINFO ), 
     fMaxSourceSize( MAXIMUM_SOURCE_NAME_LENGTH ),
     fMinType( minType )
{
   // constructor
   InitMaps();
}

TMVA::MsgLogger::MsgLogger( const string& source, EMsgType minType )
   : fObjSource( 0 ),
     fStrSource( source ), 
     fPrefix( PREFIX ), 
     fSuffix( SUFFIX ), 
     fActiveType( kINFO ), 
     fMaxSourceSize( MAXIMUM_SOURCE_NAME_LENGTH ),
     fMinType( minType )
{
   // constructor
   InitMaps();
}

TMVA::MsgLogger::MsgLogger( EMsgType minType )
   : fObjSource( 0 ), 
     fStrSource( "Unknown" ), 
     fPrefix( PREFIX ), 
     fSuffix( SUFFIX ), 
     fActiveType( kINFO ), 
     fMaxSourceSize( MAXIMUM_SOURCE_NAME_LENGTH ),
     fMinType( minType )
{
   // constructor
   InitMaps();
}

TMVA::MsgLogger::MsgLogger( const MsgLogger& parent ) :
   //   basic_ios< MsgLogger::char_type, MsgLogger::traits_type >( new MsgLogger::__stringbuf_type() ),
   std::basic_ios< MsgLogger::char_type, MsgLogger::traits_type >(),
   std::ostringstream(),
   TObject(),
   fPrefix( PREFIX ), 
   fSuffix( SUFFIX ),
   fMaxSourceSize( MAXIMUM_SOURCE_NAME_LENGTH )
{
   // copy constructor
   InitMaps();
   *this = parent;
}

TMVA::MsgLogger::~MsgLogger() 
{
   // destructor
}

TMVA::MsgLogger& TMVA::MsgLogger::operator= ( const MsgLogger& parent ) 
{
   // assingment operator
   if (&parent != this) {
      fObjSource  = parent.fObjSource;
      fStrSource  = parent.fStrSource;
      fActiveType = parent.fActiveType;
      fMinType    = parent.fMinType;
   }

   return *this;
}

string TMVA::MsgLogger::GetFormattedSource() const
{
   // make sure the source name is no longer than fMaxSourceSize:
   string source_name;
   if (fObjSource) source_name = fObjSource->GetName();
   else            source_name = fStrSource;

   if (source_name.size() > fMaxSourceSize) {
      source_name = source_name.substr( 0, fMaxSourceSize - 3 );
      source_name += "...";
   }
   
   return source_name;
}

string TMVA::MsgLogger::GetPrintedSource() const
{ 
   // the full logger prefix
   string source_name = GetFormattedSource();
   if (source_name.size() < fMaxSourceSize) 
      for (string::size_type i=source_name.size(); i<fMaxSourceSize; i++) source_name.push_back( ' ' );

   return fPrefix + source_name + fSuffix; 
}

void TMVA::MsgLogger::Send() 
{
   // activates the logger writer

   // make sure the source name is no longer than fMaxSourceSize:
   string source_name = GetFormattedSource();

   string message = this->str();
   string::size_type previous_pos = 0, current_pos = 0;

   // slice the message into lines:
   while (kTRUE) {
      current_pos = message.find( '\n', previous_pos );
      string line = message.substr( previous_pos, current_pos - previous_pos );

      std::ostringstream message_to_send;
      // must call the modifiers like this, otherwise g++ get's confused with the operators...
      message_to_send.setf( std::ios::adjustfield, std::ios::left );
      message_to_send.width( fMaxSourceSize );
      message_to_send << source_name << fSuffix << line;
      this->WriteMsg( fActiveType, message_to_send.str() );

      if (current_pos == message.npos) break;
      previous_pos = current_pos + 1;
   }

   // reset the stream buffer:
   this->str( "" );
   return;
}

void TMVA::MsgLogger::WriteMsg( EMsgType type, const std::string& line ) const 
{
   // putting the output string, the message type, and the color
   // switcher together into a single string

   if (type < fMinType || fOutputSupressed) return;
   std::map<EMsgType, std::string>::const_iterator stype;
   if ((stype = fTypeMap.find( type )) == fTypeMap.end()) return;
   if (gConfig().UseColor()) {
      // no text for INFO
      if (type == kINFO) cout << fPrefix << line << endl; // no color for info
      else               cout << fColorMap.find( type )->second << fPrefix << "<" 
                              << stype->second << "> " << line  << "\033[0m" << endl;
   } 
   else {
      if (type == kINFO) cout << fPrefix << line << endl;
      else               cout << fPrefix << "<" << stype->second << "> " << line << endl;
   }

   // take decision to stop if fatal error
   if (type == kFATAL) { cout << "***> abort program execution" << endl; exit(1); }
}

TMVA::MsgLogger& TMVA::MsgLogger::Endmsg( MsgLogger& logger ) 
{
   // end line
   logger.Send();
   return logger;
}

void TMVA::MsgLogger::InitMaps()
{
   // fill maps that assign a string and a color to echo message level
   fTypeMap[kVERBOSE]  = "VERBOSE";
   fTypeMap[kDEBUG]    = "DEBUG";
   fTypeMap[kINFO]     = "INFO";
   fTypeMap[kWARNING]  = "WARNING";
   fTypeMap[kERROR]    = "ERROR";
   fTypeMap[kFATAL]    = "FATAL";
   fTypeMap[kALWAYS]   = "ALWAYS";

   fColorMap[kVERBOSE] = "\033[1;34m";
   fColorMap[kDEBUG]   = "\033[34m";
   fColorMap[kINFO]    = "";
   fColorMap[kWARNING] = "\033[31m";
   fColorMap[kERROR]   = "\033[31m";
   fColorMap[kFATAL]   = "\033[1;41;29m";
   fColorMap[kALWAYS]  = "\033[30m";   
}
