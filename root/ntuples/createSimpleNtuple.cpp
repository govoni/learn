/*
c++ -o createSimpleNtuple `root-config --glibs --cflags` -lm createSimpleNtuple.cpp
*/


#include <iostream>
#include <string>

#include <TFile.h>
#include <TNtuple.h>

int main (int argc, char ** argv)
{
  TNtuple nt ("nt", "nt", "x") ;
  for (int i = 0 ; i < 10 ; ++i)
  {
    nt.Fill (i) ;

  }

  TFile f ("simpleNtuple.root", "recreate") ;
  nt.Write () ;
  f.Close () ;

  return 0 ;
}