/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o analyzeSamples analyzeSamples.cpp
*/

#include "TProfile2D.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TNtuple.h"
#include <fstream>
#include <vector>
#include <utility>

#include "RooFormulaVar.h"
#include "RooKeysPdf.h"
#include "RooExponential.h"
#include "RooArgusBG.h"
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
using namespace std ;


double weight (const vector<vector<TF1*> > & pdfs,    // pdfs for each species
               const vector<double> & xsecs,          // relative cross section
               int index,                             // index of the species
               const vector<float*> & x)              // variable value
{
  double weight = 0. ;
  for (int iSpecies = 0 ; iSpecies < pdfs.at (0).size () ; ++iSpecies)
    {
      double add = 1. ;
      for (int iVar = 0 ; iVar < x.size () ; ++iVar)
        {
          add *= pdfs.at (iVar).at (iSpecies)->Eval (*x.at (iVar)) ;        
        }
      add *= xsecs.at (iSpecies) ;
      weight += add ;
   }
  
  double add = xsecs.at (index) ;
  for (int iVar = 0 ; iVar < x.size () ; ++iVar)
    {
      add *= pdfs.at (iVar).at (index)->Eval (*x.at (iVar)) ;        
    }

  weight = add / weight ;
  return weight ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


double weight_pg (const std::vector<TF1*> & pdfs,    // pdfs for each species
                  const std::vector<double> & xsecs, // relative cross section
                  int index,                         // index of the species
                  double x)                          // variable value
{
  double weight = 0. ;
  for (int i = 0 ; i < pdfs.size () ; ++i)
    {
      weight += xsecs.at (i) * pdfs.at (i)->Eval (x) ;    
    }
  if (weight == 0) return 1. ;    
  weight = xsecs.at (index) * pdfs.at (index)->Eval (x) / weight ;
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


void fillFromTGraph (TH2F & output, const TGraph & input) 
{
  for (int i = 0 ; i < input.GetN () ; ++i)
    output.Fill (input.GetX ()[i], input.GetY ()[i]) ;
    
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int main (int argc, char ** argv)
{
  gROOT->SetStyle ("Plain") ;	
  gStyle->SetOptStat ("mr") ;
  gStyle->SetOptFit (1111) ;
  gStyle->SetStatFont(42);
  gStyle->SetStatFontSize(0.1);
  gStyle->SetStatTextColor(1);
  gStyle->SetStatFormat("6.4g");
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatH(0.06);
  gStyle->SetStatW(0.3);
  gStyle->SetPalette(1);


// read the samples ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  TFile f ("samples.root","read") ;
  vector<TNtuple *> samples ;
  TNtuple * signalNT = (TNtuple *) f.Get ("signalNT") ;
  samples.push_back (signalNT) ;
  TNtuple * bkg_1NT = (TNtuple *) f.Get ("bkg_1NT") ;
  samples.push_back (bkg_1NT) ;
  TNtuple * bkg_2NT = (TNtuple *) f.Get ("bkg_2NT") ;
  samples.push_back (bkg_2NT) ;

  vector<double> xsecs ;
  xsecs.push_back (1.) ; // sample 1
  xsecs.push_back (1.) ; // sample 2
  xsecs.push_back (1.) ; // sample 3

  vector<string> vars ; 
  vars.push_back ("x") ;
  vars.push_back ("y") ;

  // ranges for the variables
  vector<pair<double, double> > ranges ;
  ranges.push_back (pair<double, double> (0., 30.)) ;
  ranges.push_back (pair<double, double> (0., 3.14)) ;

  // the hypothesis is that pdfs are factorized
  vector< vector<TF1*> > pdfs ;

  // fit the pdfs on the one-dim distributions of the data integrated along the other direction
  vector<TF1*> pdfs_x ;
  vector<TF1*> pdfs_y ;

  TH1F signal_x ("signal_x","signal_x", 30, 0., 30.) ; signal_x.SetStats (0) ;
  signalNT->Draw ("x>>signal_x") ;
  TF1 signal_x_pdf ("signal_x_pdf", "gausn", 0., 30.) ;
  signal_x_pdf.SetLineColor (2) ;
  signal_x_pdf.SetParameters (signal_x.GetMean (), signal_x.GetRMS ()) ;
  signal_x.Fit ("signal_x_pdf") ;
  pdfs_x.push_back (&signal_x_pdf) ;

  TH1F signal_y ("signal_y","signal_y", 30, 0., 3.14) ; signal_y.SetStats (0) ;
  signalNT->Draw ("y>>signal_y") ;
  TF1 signal_y_pdf ("signal_y_pdf", "gausn", 0., 3.14) ;
  signal_y_pdf.SetLineColor (2) ;
  signal_y_pdf.SetParameters (signal_y.GetMean (), signal_y.GetRMS ()) ;
  signal_y.Fit ("signal_y_pdf") ;
  pdfs_y.push_back (&signal_y_pdf) ;

  TH1F bkg_1_x ("bkg_1_x","bkg_1_x", 30, 0., 30.) ; bkg_1_x.SetStats (0) ;
  bkg_1NT->Draw ("x>>bkg_1_x") ;
  TF1 bkg_1_x_pdf ("bkg_1_x_pdf", "pol3", 0., 30.) ;
  bkg_1_x_pdf.SetLineColor (3) ;
//  bkg_1_x_pdf.SetParameters (/*bkg_1_x.GetMean (), bkg_1_x.GetRMS ()*/) ;
  bkg_1_x.Fit ("bkg_1_x_pdf") ;
  pdfs_x.push_back (&bkg_1_x_pdf) ;

  TH1F bkg_1_y ("bkg_1_y","bkg_1_y", 30, 0., 3.14) ; bkg_1_y.SetStats (0) ;
  bkg_1NT->Draw ("y>>bkg_1_y") ;
  TF1 bkg_1_y_pdf ("bkg_1_y_pdf", "[0] * TMath::Exp([1] * x)", 0., 3.14) ;
  bkg_1_y_pdf.SetLineColor (3) ;
//  bkg_1_y_pdf.SetParameters (/*bkg_1_y.GetMean (), bkg_1_y.GetRMS ()*/) ;
  bkg_1_y.Fit ("bkg_1_y_pdf") ;
  pdfs_y.push_back (&bkg_1_y_pdf) ;

  TH1F bkg_2_x ("bkg_2_x","bkg_2_x", 30, 0., 30.) ; bkg_2_x.SetStats (0) ;
  bkg_2NT->Draw ("x>>bkg_2_x") ;
  TF1 bkg_2_x_pdf ("bkg_2_x_pdf", "pol1", 0., 30.) ;
  bkg_2_x_pdf.SetLineColor (4) ;
//  bkg_2_x_pdf.SetParameters (/*bkg_2_x.GetMean (), bkg_2_x.GetRMS ()*/) ;
  bkg_2_x.Fit ("bkg_2_x_pdf") ;
  pdfs_x.push_back (&bkg_2_x_pdf) ;

  TH1F bkg_2_y ("bkg_2_y","bkg_2_y", 30, 0., 3.14) ; bkg_2_y.SetStats (0) ;
  bkg_2NT->Draw ("y>>bkg_2_y") ;
  TF1 bkg_2_y_pdf ("bkg_2_y_pdf", "pol2", 0., 3.14) ;
  bkg_2_y_pdf.SetLineColor (4) ;
//  bkg_2_y_pdf.SetParameters (/*bkg_2_y.GetMean (), bkg_2_y.GetRMS ()*/) ;
  bkg_2_y.Fit ("bkg_2_y_pdf") ;
  pdfs_y.push_back (&bkg_2_y_pdf) ;

  pdfs.push_back (pdfs_x) ;
  pdfs.push_back (pdfs_y) ;

  // check plot
  TCanvas c1 ;
  c1.Divide (2,3) ;
  c1.cd (1) ; signal_x.Draw () ;
  c1.cd (2) ; signal_y.Draw () ;
  c1.cd (3) ; bkg_1_x.Draw () ;
  c1.cd (4) ; bkg_1_y.Draw () ;
  c1.cd (5) ; bkg_2_x.Draw () ;
  c1.cd (6) ; bkg_2_y.Draw () ;
  c1.Print ("distros.pdf","pdf") ;

  // absorb the pdfs normalization in the xsections   ---- ---- ---- ---- ---- ---- ---- ----
  vector<double> loc_xsecs = xsecs ;
  for (int iVar = 0 ; iVar < pdfs.size () ; ++iVar)
    {
      vector<double> xsecs ;
      for (int iSpecies = 0 ; iSpecies < pdfs.at (iVar).size () ; ++iSpecies)
        {
          double integral = pdfs.at (iVar).at (iSpecies)->Integral (ranges.at (iVar).first, ranges.at (iVar).second) ;
          if (integral != 0) loc_xsecs.at (iSpecies) /= integral ;
        }
    }

  // graphs to be put in the polygon
  std::vector <TGraph> points ;

  // for the ntuple reading
  vector<float *> vars_value ;
  for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
    {
      float * dummy = new float (0) ;
      vars_value.push_back (dummy) ;
    }

  vector<vector<vector<TH2F *> > > det_histos ; // sample species var
  // to monitor weights power
  for (int iSample = 0 ; iSample < samples.size () ; ++iSample)
    {
      vector<vector<TH2F *> > det_histos_samp ;
      for (int iSpecies = 0 ; iSpecies < samples.size () ; ++iSpecies)
        {
          vector<TH2F * > det_histos_spec ;
          for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
            {
              char name[20] ;
              sprintf (name,"h2_sample%d_species%d_%s",iSample, iSpecies, vars.at (iVar).c_str ()) ;
              TH2F * dummy = new TH2F (name, name, 30, ranges.at (iVar).first, ranges.at (iVar).second, 30, 0., 1.) ;
              dummy->SetStats (0) ;
              det_histos_spec.push_back (dummy) ;        
            }
          det_histos_samp.push_back (det_histos_spec) ;  
        }
      det_histos.push_back (det_histos_samp) ;  
    }

  // the main loop ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  cout << "before the main loop" << endl ;  
  int counter = 0 ;
  // loop over samples
  for (int iSample = 0 ; iSample < samples.size () ; ++iSample)
    {
      points.push_back (TGraph ()) ;
      for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
        samples.at (iSample)->SetBranchAddress (vars.at (iVar).c_str (), vars_value.at (iVar)) ;

      // loop over events
      for (int iEvent = 0 ; iEvent < samples.at (iSample)->GetEntries () ; ++iEvent)
        {
          samples.at (iSample)->GetEntry (iEvent) ;
          std::vector<double> weights (samples.size (), 1.) ;
          // loop over species
          for (int iSpecies = 0 ; iSpecies < pdfs.at (0).size () ; ++iSpecies)
            {
              weights.at (iSpecies) = weight (pdfs, loc_xsecs, iSpecies, vars_value) ;
              ++counter ;
              for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
                det_histos.at (iSample).at (iSpecies).at (iVar)->Fill (*vars_value.at (iVar), weights.at (iSpecies)) ;
            } // loop over species
          std::pair<double, double> point = get_coord (weights) ;
          points.back ().SetPoint (iEvent, point.first, point.second) ;                
        } // loop over events
    } // loop over samples

  cout << "after the main loop " << counter << endl ;  

  // plotting ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

//  vector<vector<vector<TH2F *> > > det_histos ; // sample species var

  TCanvas c2 ;
  int nPads = det_histos.size () ;
  c2.Divide (nPads, nPads) ;
  for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
    {
      for (int iSample = 0 ; iSample < samples.size () ; ++iSample)
        {
          for (int iSpecies = 0 ; iSpecies < samples.size () ; ++iSpecies)
            {
              c2.cd (iSample * nPads + iSpecies + 1) ;
              det_histos.at (iSample).at (iSpecies).at (iVar)->Draw ("col") ;
            }
        }
      char plotName[20] ;
      sprintf (plotName,"weightsVar_%s.pdf",vars.at (iVar).c_str ()) ;
      c2.Print (plotName, "pdf") ;
    }

  TGraph polygon ; 
  for (int i = 0 ; i <= samples.size () ; ++i)
    {
      double x = cos (6.28 * i / samples.size ()) ;
      double y = sin (6.28 * i / samples.size ()) ;
      polygon.SetPoint (i, x, y) ;    
    }

  TH2F bkg ("bkg","", 10, -1., 1., 10, -1., 1.) ;
  bkg.SetStats (0) ;
  c1.cd () ;
  bkg.Draw () ;
  polygon.Draw ("Lsame") ;
  for (int iGraph = 0 ; iGraph < points.size () ; ++iGraph)
    {
//      points.at (iGraph).SetMarkerStyle (4) ;    
//      points.at (iGraph).SetMarkerSize (0.5) ;    
      points.at (iGraph).SetMarkerColor (2+iGraph) ;    
      points.at (iGraph).Draw ("Psame") ;    
    }
  c1.Print ("polygon.pdf","pdf") ;


  TCanvas c3 ("c3", "c3", 400, 400 * points.size ()) ;
  c3.Divide (1, points.size ()) ;
  vector<TH2F *> singlePolygons ;
  for (int i = 0 ; i < points.size () ; ++i)
    {
       char name[20] ;
       sprintf (name, "sample_%d", i) ;
       TH2F * dummy = new TH2F (name, name, 50, -1., 1., 50, -1., 1.) ;
       fillFromTGraph (*dummy, points.at (i)) ; 
       dummy->SetStats (0) ;
       c3.cd (i+1) ;
       dummy->Draw ("colz") ;
       polygon.Draw ("Lsame") ;
       singlePolygons.push_back (dummy) ;
    }
  c3.Print ("polygons.pdf","pdf") ;
  
  

  f.Close () ;

  for (int iVar = 0 ; iVar < vars_value.size () ; ++iVar)
    delete vars_value.at (iVar) ;

  return 0 ;
}

