/*

  c++ -o ex1var_compile `root-config --glibs --cflags`\
         -lrooFit -lrooFitCore ex1var_compile.cpp

*/

#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif 

using namespace RooFit ;

void ex1var()
{
  RooWorkspace w("w",kTRUE) ;

  w.factory("Gaussian::g(x[-10,10],mean[-10,10],sigma[3,0.1,10])") ;
  RooDataSet* d = w.pdf("g")->generate(*w.var("x"),10000) ;

  w.pdf("g")->fitTo(*d) ;
  RooPlot* frame = w.var("x")->frame() ;
  d->plotOn(frame) ;
  w.pdf("g")->plotOn(frame) ;
  frame->Draw() ;

  w.var("mean")->Print() ;
  w.var("sigma")->Print() ;
}

int main () 
{
  ex1var () ;
  return 0 ;
}