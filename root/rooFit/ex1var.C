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

  // signal like
  w.factory ("Gaussian::g(x[-10,10],mean[-1,-10,10],sigma[1.5,0.1,10])") ;
  RooDataSet* d_g = w.pdf ("g")->generate (*w.var ("x"),500) ;

  // bkg like
  w.factory ("Exponential::p(x[-10,10],expo[-0.2,-2,-0.05])") ;
  RooDataSet* d_p = w.pdf ("p")->generate (*w.var ("x"),500) ;

  // sum the two
  w.factory ("SUM::sb(g1frac[0.3] * g, g2frac[0.7] * p") ;
  RooDataSet* d_sb = w.pdf ("sb")->generate (*w.var ("x"),500) ;

  // access x and change its binning for the binned generation
  RooRealVar * x = w.var ("x") ;
  x->setBins (100) ;
  
  // generation in billed way
  RooDataHist* d_bin = w.pdf("g")->generateBinned(*w.var ("x"),500) ;

  // fitting
  w.pdf ("g")->fitTo (*d_g) ;
  w.pdf ("p")->fitTo (*d_p) ;
  w.pdf ("sb")->fitTo (*d_sb) ;

  // create nll
  RooAbsReal * nll_sb = w.pdf ("sb")->createNLL (*d_sb) ;
  RooMinuit m_sb (*nll_sb) ;
  m_sb.migrad () ;
  m_sb.hesse () ;
  m_sb.minos (*w.var ("mean")) ;
  RooFitResult * r_sb = m_sb.save () ;

  // plotting

  // signal
  TCanvas * c1 = new TCanvas () ;
  RooPlot * frame_g = w.var ("x")->frame () ;
  d_g->plotOn (frame_g) ;
  w.pdf("g")->plotOn (frame_g) ;
  frame_g->Draw () ;

  // bkg
  TCanvas * c2 = new TCanvas (); ;
  RooPlot * frame_p = w.var ("x")->frame () ;
  d_p->plotOn (frame_p) ;
  w.pdf ("p")->plotOn (frame_p) ;
  frame_p->Draw () ;

  // both
  TCanvas * c3 = new TCanvas (); ;
  RooPlot * frame_sb = w.var ("x")->frame () ;
//  w.pdf ("sb")->plotOn (frame_sb) ;
//  w.pdf ("sb")->plotOn (frame_sb,Components ("g"), LineStyle (kDashed)) ;
//  w.pdf ("sb")->plotOn (frame_sb,Components ("p"), LineStyle (kDashed)) ;
  d_sb->plotOn (frame_sb) ;
  w.pdf ("sb")->plotOn (frame_sb,VisualizeError (*r_sb)) ;
  frame_sb->Draw () ;

  // nll both
  TCanvas * c4 = new TCanvas (); ;
  RooPlot * frame_par = w.var ("mean")->frame () ;
  nll_sb->plotOn (frame_par) ;
  frame_par->Draw () ;

  w.var("mean")->Print () ;
  w.var("sigma")->Print () ;
}
