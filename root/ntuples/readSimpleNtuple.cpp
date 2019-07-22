/*
c++ -o readSimpleNtuple `root-config --glibs --cflags` -lm readSimpleNtuple.cpp
*/


#include <iostream>
#include <string>

#include <TFile.h>
#include <TNtuple.h>
#include <TTreeReader.h>

using namespace std ;

int main (int argc, char ** argv)
{
  TFile f ("simpleNtuple.root") ;
  TTreeReader nt ("nt", &f) ;
  TTreeReaderValue<float> var (nt, "x") ;

  while (nt.Next ()) cout << *var << endl ;
  

  return 0 ;
}