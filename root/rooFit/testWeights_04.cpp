/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore  \
    -o testWeights_04 testWeights_04.cpp
*/

#include "TH1.h"
#include "TExec.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>

#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooFFTConvPdf.h"
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif 

using namespace RooFit ;


double weight_pg (const std::vector<TF1*> & epss,    // eps for each species
                  const std::vector<double> & xsecs, // relative cross section
                  int index,                         // index of the species
                  double x)                          // variable value
{
  double weight = 0. ;
  for (int i = 0 ; i < epss.size () ; ++i)
    {
      weight += xsecs.at (i) * epss.at (i)->Eval (x) ;    
    }
  if (weight == 0) return 1. ;    
  weight = xsecs.at (index) * epss.at (index)->Eval (x) / weight ;
  return weight ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


double vsum (const std::vector<double> & vect)
{
  double sum = 0. ;
  for (int i = 0 ; i < vect.size () ; ++i)
    sum += vect.at (i) ;
  return sum ;  
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


std::pair<double, double>
get_coord (std::vector<double> weights)
{
  std::pair<double, double> coord ;
  int size = weights.size () ;
  double theta = 2 * M_PI / size ;
  coord.first = 0. ;
  coord.second = 0. ;
  for (int i = 0 ; i < size ; ++i)
    {
      coord.first += weights.at (i) * cos (theta*i) ;
      coord.second += weights.at (i) * sin (theta*i) ;
    }
  return coord ;  
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int main (int argc, char ** argv)
{
  gROOT->SetStyle ("Plain") ;	
  gStyle->SetOptStat ("mr") ;
  gStyle->SetCanvasDefH (500) ;
  gStyle->SetCanvasDefW (500) ;
  gStyle->SetOptFit (1111) ;
  gStyle->SetStatFont (42) ;
  gStyle->SetStatFontSize (0.1) ;
  gStyle->SetStatTextColor (1) ;
  gStyle->SetStatFormat ("6.4g") ;
  gStyle->SetStatBorderSize (1) ;
  gStyle->SetStatH (0.06) ;
  gStyle->SetStatW (0.3) ;
  gStyle->SetPalette (1) ;


// create epss for the three models
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  // variables definition
  RooRealVar x ("x", "x", -10, 10) ; 

  // first function definition
  RooRealVar mean_1 ("mean_1", "mean of gaussian _1", 5., -10., 10.) ; 
  RooRealVar sigma_1 ("sigma_1", "width of gaussian _1", 2, 0., 5.) ; 
  RooGaussian func_1 ("func_1", "gaussian PDF", x, mean_1, sigma_1) ; 

  // second function definition
  RooPolynomial func_2 ("constant", "flat function", x, RooArgList ()) ;

  // third function definition
  RooRealVar mean_3 ("mean_3", "mean of gaussian _3", -5., -10., 10.) ; 
  RooRealVar sigma_3 ("sigma_3", "width of gaussian _3", 2, 0., 5.) ; 
  RooGaussian func_3 ("func_3", "gaussian PDF", x, mean_3, sigma_3) ; 

  // plot the distributions
  RooPlot *xplot = x.frame () ;
  func_1.plotOn (xplot, LineColor (kRed)) ;
  func_2.plotOn (xplot, LineColor (kBlue)) ;
  func_3.plotOn (xplot, LineColor (kGreen)) ;
  TCanvas c1 ;
  xplot->Draw () ;
  c1.Print ("WT_distros.eps","eps") ;
  
// generate the three datasets
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  RooDataSet * data_1 = func_1.generate (RooArgSet (x), 10000) ;
  RooDataSet * data_2 = func_2.generate (RooArgSet (x), 10000) ;
  RooDataSet * data_3 = func_3.generate (RooArgSet (x), 10000) ;

  // plot the distributions
  RooPlot *xplot_1 = x.frame () ;
  data_1->plotOn (xplot_1, MarkerColor (kRed)) ;
  func_1.plotOn (xplot_1, LineColor (kRed)) ;
  data_2->plotOn (xplot_1, MarkerColor (kBlue)) ;
  func_2.plotOn (xplot_1, LineColor (kBlue)) ;
  data_3->plotOn (xplot_1, MarkerColor (kGreen)) ;
  func_3.plotOn (xplot_1, LineColor (kGreen)) ;
  xplot_1->Draw () ;
  c1.Print ("WT_distros_and_data.eps","eps") ;
  

// calculate the weights for each event and put it into the diagram
// start with the three cross-sections equal to each other
// for some reasons I have to do it all new cause roofit is crap
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  std::vector<TF1*> epss ;
  //PG FIXME this may become a vector of maps, w/ (eps, xsec)
  TF1 fg_1 ("fg_1","TMath::Gaus(x,[0],[1],1)",-10.,10.) ; 
  fg_1.SetParameters (5., 2.) ; // perche' questo funziona??
  epss.push_back (&fg_1) ;
  TF1 fg_2 ("fg_2","[0]",-10.,10.) ; 
  fg_2.SetParameter (0, 1/20.) ;
  epss.push_back (&fg_2) ;
  TF1 fg_3 ("fg_3","TMath::Gaus(x,[0],[1],1)",-10.,10.) ; 
  fg_3.SetParameters (-5., 2.) ;
  epss.push_back (&fg_3) ;
  std::vector<double> xsecs (3, 1.) ;

  // plot for safety
  c1.cd () ;
  fg_1.SetLineColor (kRed) ; fg_1.Draw () ;
  fg_2.SetLineColor (kBlue) ; fg_2.Draw ("same") ;
  fg_3.SetLineColor (kGreen) ; fg_3.Draw ("same") ;
  c1.Print ("WT_functions.eps","eps") ;

  std::vector<double> weights ;
  weights.push_back (weight_pg (epss, xsecs, 0, 0.5)) ;
  weights.push_back (weight_pg (epss, xsecs, 1, 0.5)) ;
  weights.push_back (weight_pg (epss, xsecs, 2, 0.5)) ;

  std::cout << "test _1: " << weights.at (0) << "\n" ;
  std::cout << "test _2: " << weights.at (1) << "\n" ;
  std::cout << "test _3: " << weights.at (2) << "\n" ;

  std::pair<double, double> point = get_coord (weights) ;
  std::cout << "(" << point.first << "," << point.second << ")\n" ;
  
  for (int i = 0 ; i < epss.size () ; ++i)
    {
      std::cout << "integral " << i << ": " << epss.at (i)->Integral (-10., 10.) << "\n" ;
    }

  TGraph points_1 ;
  TH1F sum_1 ("sum_1","sum_1", 100, 0., 2.) ;
  TH2F checkW_1_d1 ("checkW_1_d1", "check weight 1 on dataset _1", 50, -10., 10., 50, 0., 1.) ;
  checkW_1_d1.SetStats (0) ;
  TH2F checkW_1_d2 ("checkW_1_d2", "check weight 2 on dataset _1", 50, -10., 10., 50, 0., 1.) ;
  checkW_1_d2.SetStats (0) ;
  TH2F checkW_1_d3 ("checkW_1_d3", "check weight 3 on dataset _1", 50, -10., 10., 50, 0., 1.) ;
  checkW_1_d3.SetStats (0) ;
  for (int iEvent = 0 ; iEvent < data_1->numEntries () ; ++iEvent)
    {
      double x = data_1->get (iEvent)->getRealValue ("x") ;
      std::vector<double> weights ;
      weights.push_back (weight_pg (epss, xsecs, 0, x)) ;
      weights.push_back (weight_pg (epss, xsecs, 1, x)) ;
      weights.push_back (weight_pg (epss, xsecs, 2, x)) ;
      checkW_1_d1.Fill (x, weights.at (0)) ;
      checkW_1_d2.Fill (x, weights.at (1)) ;
      checkW_1_d3.Fill (x, weights.at (2)) ;
      sum_1.Fill (vsum (weights)) ;
      std::pair<double, double> point = get_coord (weights) ;
      points_1.SetPoint (iEvent, point.first, point.second) ;    
    }
  
  TGraph points_2 ;
  TH1F sum_2 ("sum_2","sum_2", 100, 0., 2.) ;
  TH2F checkW_2_d1 ("checkW_2_d1", "check weight 1 on dataset _2", 50, -10., 10., 50, 0., 1.) ;
  checkW_2_d1.SetStats (0) ;
  TH2F checkW_2_d2 ("checkW_2_d2", "check weight 2 on dataset _2", 50, -10., 10., 50, 0., 1.) ;
  checkW_2_d2.SetStats (0) ;
  TH2F checkW_2_d3 ("checkW_2_d3", "check weight 3 on dataset _2", 50, -10., 10., 50, 0., 1.) ;
  checkW_2_d3.SetStats (0) ;
  for (int iEvent = 0 ; iEvent < data_2->numEntries () ; ++iEvent)
    {
      double x = data_2->get (iEvent)->getRealValue ("x") ;
      std::vector<double> weights ;
      weights.push_back (weight_pg (epss, xsecs, 0, x)) ;
      weights.push_back (weight_pg (epss, xsecs, 1, x)) ;
      weights.push_back (weight_pg (epss, xsecs, 2, x)) ;
      checkW_2_d1.Fill (x, weights.at (0)) ;
      checkW_2_d2.Fill (x, weights.at (1)) ;
      checkW_2_d3.Fill (x, weights.at (2)) ;
      sum_2.Fill (vsum (weights)) ;
      std::pair<double, double> point = get_coord (weights) ;
      points_2.SetPoint (iEvent, point.first, point.second) ;    
    }
    
  TGraph points_3 ;
  TH1F sum_3 ("sum_3","sum_3", 100, 0., 2.) ;
  TH2F checkW_3_d1 ("checkW_3_d1", "check weight 1 on dataset _3", 50, -10., 10., 50, 0., 1.) ;
  checkW_3_d1.SetStats (0) ;
  TH2F checkW_3_d2 ("checkW_3_d2", "check weight 2 on dataset _3", 50, -10., 10., 50, 0., 1.) ;
  checkW_3_d2.SetStats (0) ;
  TH2F checkW_3_d3 ("checkW_3_d3", "check weight 3 on dataset _3", 50, -10., 10., 50, 0., 1.) ;
  checkW_3_d3.SetStats (0) ;
  for (int iEvent = 0 ; iEvent < data_3->numEntries () ; ++iEvent)
    {
      double x = data_3->get (iEvent)->getRealValue ("x") ;
      std::vector<double> weights ;
      weights.push_back (weight_pg (epss, xsecs, 0, x)) ;
      weights.push_back (weight_pg (epss, xsecs, 1, x)) ;
      weights.push_back (weight_pg (epss, xsecs, 2, x)) ;
      checkW_3_d1.Fill (x, weights.at (0)) ;
      checkW_3_d2.Fill (x, weights.at (1)) ;
      checkW_3_d3.Fill (x, weights.at (2)) ;
      sum_3.Fill (vsum (weights)) ;
      std::pair<double, double> point = get_coord (weights) ;
      points_3.SetPoint (iEvent, point.first, point.second) ;    
    }

  sum_1.Draw () ;
  sum_2.Draw ("same") ;
  sum_3.Draw ("same") ;
  c1.Print ("WT_sum.eps","eps") ;

  TCanvas c2 ;
  c2.Divide (3,3) ;
  c2.cd (1) ; checkW_1_d1.Draw ("col") ;
  c2.cd (2) ; checkW_1_d2.Draw ("col") ;
  c2.cd (3) ; checkW_1_d3.Draw ("col") ;
  c2.cd (4) ; checkW_2_d1.Draw ("col") ;
  c2.cd (5) ; checkW_2_d2.Draw ("col") ;
  c2.cd (6) ; checkW_2_d3.Draw ("col") ;
  c2.cd (7) ; checkW_3_d1.Draw ("col") ;
  c2.cd (8) ; checkW_3_d2.Draw ("col") ;
  c2.cd (9) ; checkW_3_d3.Draw ("col") ;
  c2.Print ("WT_weights_table.eps","eps") ;

  TH2F bkg ("bkg","", 10, -1., 1., 10, -1., 1.) ;
  bkg.SetStats (0) ;
  points_1.SetMarkerStyle (4) ;    
  points_1.SetMarkerSize (0.5) ;    
  points_1.SetMarkerColor (kRed) ;    
  points_2.SetMarkerStyle (5) ;    
  points_2.SetMarkerSize (0.5) ;    
  points_2.SetMarkerColor (kBlue) ;    
  points_3.SetMarkerStyle (29) ;    
  points_3.SetMarkerSize (0.5) ;    
  points_3.SetMarkerColor (kGreen) ;
  TCanvas c3 ("c3","",900,300) ;
  c3.Divide (3,1) ;
  c3.cd (1) ;
  bkg.Draw () ;
  points_1.Draw ("Psame") ;    
  c3.cd (2) ;
  bkg.Draw () ;
  points_2.Draw ("Psame") ;    
  c3.cd (3) ;
  bkg.Draw () ;
  points_3.Draw ("Psame") ;    
  c3.Print ("WT_weightsDistrib.eps","eps") ;


  return 0 ;

}