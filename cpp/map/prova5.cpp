#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
//#include <algorithm>
#include "TB06Step1.h"

int main ()
{
  std::map<int,TB06Step1> calibrators_1 ;

  for (/* loop sugli eventi*/)
    {
       TB06Reco * entry ;
       // controllo se esiste nella mappa
       // un oggetto associato al cristallo
       if ( !calibrators_1.count (entry->MEXTLindex) )
         {
           TB06Step1 dummy (/*mode*/) ;
           calibrators_1[entry->MEXTLindex] = dummy ;         
         }
       calibrators_1[entry->MEXTLindex].fillHisto (entry) ;  
    } /* loop sugli eventi*/

  // loop sulla mappa
  for (std::map<int,TB06Step1>::iterator mapIt = calibrators_1.begin () ;
       mapIt != calibrators_1.end () ;
       ++mapIt)
    {
       int xtalNum = mapIt->first ; /* la chiave della mappa */
       double xcross = mapIt->second.getXcross () ;    
    } // loop sulla mappa

  // i costruttori
  
  
  // primo tipo
  TB06Step1 dummy2 (/*mode*/) ;
  TB06Step1 dummy3 = dummy2 ; // operatore = : operator= (const TBStep01 &)
  TB06Step1 dummy4 (dummy2) ; // copy ctor : TBSteo01 (const TBStep01 &)

}