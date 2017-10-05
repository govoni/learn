/*
c++ `root-config --cflags --glibs` -lrooFit -lrooFitCore -o analyzeSamples_03 analyzeSamples_03.cpp
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


double lr (const vector<vector<TF1*> > & sigPdfs,    // Pdfs for each species
           const vector<double> & sigXsecs,          // relative cross section
           const vector<vector<TF1*> > & bkgPdfs,    // Pdfs for each species
           const vector<double> & bkgXsecs,          // relative cross section
           const vector<float*> & x)                 // variable value
{
  double num = 0. ;
  for (int iSpecies = 0 ; iSpecies < sigPdfs.size () ; ++iSpecies)
    {
      double add = 1. ;
      for (int iVar = 0 ; iVar < x.size () ; ++iVar)
        {
          add *= sigPdfs.at (iSpecies).at (iVar)->Eval (*x.at (iVar)) ;        
        }
      add *= sigXsecs.at (iSpecies) ;
      num += add ;
   }
  
  double den = 0. ;
  for (int iSpecies = 0 ; iSpecies < bkgPdfs.size () ; ++iSpecies)
    {
      double add = 1. ;
      for (int iVar = 0 ; iVar < x.size () ; ++iVar)
        {
          add *= bkgPdfs.at (iSpecies).at (iVar)->Eval (*x.at (iVar)) ;        
        }
      add *= bkgXsecs.at (iSpecies) ;
      den += add ;
   }
  
  if (!den) return -1. ;
  return num / den ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


double weight (const vector<vector<TF1*> > & Pdfs,    // Pdfs for each species
               const vector<double> & xsecs,          // relative cross section
               int index,                             // index of the species
               const vector<float*> & x)              // variable value
{
  double weight = 0. ;
  for (int iSpecies = 0 ; iSpecies < Pdfs.size () ; ++iSpecies)
    {
      double add = 1. ;
      for (int iVar = 0 ; iVar < x.size () ; ++iVar)
        {
          add *= Pdfs.at (iSpecies).at (iVar)->Eval (*x.at (iVar)) ;        
        }
      add *= xsecs.at (iSpecies) ;
      weight += add ;
   }
  
  double add = xsecs.at (index) ;
  for (int iVar = 0 ; iVar < x.size () ; ++iVar)
    {
      add *= Pdfs.at (index).at (iVar)->Eval (*x.at (iVar)) ;        
    }

  weight = add / weight ;
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
  TNtuple * bkg_3NT = (TNtuple *) f.Get ("bkg_3NT") ;
  samples.push_back (bkg_3NT) ;

  vector<double> xsecs ;
  xsecs.push_back (1.) ; // sample 1
  xsecs.push_back (1.) ; // sample 2
  xsecs.push_back (1.) ; // sample 3
  xsecs.push_back (1.) ; // sample 3

  vector<string> vars ; 
  vars.push_back ("x") ;
  vars.push_back ("y") ;

  // ranges for the variables
  vector<pair<double, double> > ranges ;
  ranges.push_back (pair<double, double> (0., 30.)) ;
  ranges.push_back (pair<double, double> (0., 3.14)) ;

  // the hypothesis is that Pdfs are factorized
  vector< vector<TF1*> > Pdfs ; // Pdfs[ispecies][ivar]

  // fit the Pdfs on the one-dim distributions of the data integrated along the other direction
  vector<TF1*> Pdfs_var ;

  TH1F signal_x ("signal_x","signal_x", 30, 0., 30.) ; signal_x.SetStats (0) ;
  signalNT->Draw ("x>>signal_x") ;
  TF1 signal_x_Pdf ("signal_x_Pdf", "gausn", 0., 30.) ;
  signal_x_Pdf.SetLineColor (2) ;
  signal_x_Pdf.SetParameters (signal_x.GetMean (), signal_x.GetRMS ()) ;
  signal_x.Fit ("signal_x_Pdf") ;
  Pdfs_var.push_back (&signal_x_Pdf) ;

  TH1F signal_y ("signal_y","signal_y", 30, 0., 3.14) ; signal_y.SetStats (0) ;
  signalNT->Draw ("y>>signal_y") ;
  TF1 signal_y_Pdf ("signal_y_Pdf", "gausn", 0., 3.14) ;
  signal_y_Pdf.SetLineColor (2) ;
  signal_y_Pdf.SetParameters (signal_y.GetMean (), signal_y.GetRMS ()) ;
  signal_y.Fit ("signal_y_Pdf") ;
  Pdfs_var.push_back (&signal_y_Pdf) ;
  Pdfs.push_back (Pdfs_var) ;
  Pdfs_var.clear () ;

  TH1F bkg_1_x ("bkg_1_x","bkg_1_x", 30, 0., 30.) ; bkg_1_x.SetStats (0) ;
  bkg_1NT->Draw ("x>>bkg_1_x") ;
  TF1 bkg_1_x_Pdf ("bkg_1_x_Pdf", "pol3", 0., 30.) ;
  bkg_1_x_Pdf.SetLineColor (3) ;
//  bkg_1_x_Pdf.SetParameters (/*bkg_1_x.GetMean (), bkg_1_x.GetRMS ()*/) ;
  bkg_1_x.Fit ("bkg_1_x_Pdf") ;
  Pdfs_var.push_back (&bkg_1_x_Pdf) ;

  TH1F bkg_1_y ("bkg_1_y","bkg_1_y", 30, 0., 3.14) ; bkg_1_y.SetStats (0) ;
  bkg_1NT->Draw ("y>>bkg_1_y") ;
  TF1 bkg_1_y_Pdf ("bkg_1_y_Pdf", "[0] * TMath::Exp([1] * x)", 0., 3.14) ;
  bkg_1_y_Pdf.SetLineColor (3) ;
//  bkg_1_y_Pdf.SetParameters (/*bkg_1_y.GetMean (), bkg_1_y.GetRMS ()*/) ;
  bkg_1_y.Fit ("bkg_1_y_Pdf") ;
  Pdfs_var.push_back (&bkg_1_y_Pdf) ;
  Pdfs.push_back (Pdfs_var) ;
  Pdfs_var.clear () ;

  TH1F bkg_2_x ("bkg_2_x","bkg_2_x", 30, 0., 30.) ; bkg_2_x.SetStats (0) ;
  bkg_2NT->Draw ("x>>bkg_2_x") ;
  TF1 bkg_2_x_Pdf ("bkg_2_x_Pdf", "pol1", 0., 30.) ;
  bkg_2_x_Pdf.SetLineColor (4) ;
//  bkg_2_x_Pdf.SetParameters (/*bkg_2_x.GetMean (), bkg_2_x.GetRMS ()*/) ;
  bkg_2_x.Fit ("bkg_2_x_Pdf") ;
  Pdfs_var.push_back (&bkg_2_x_Pdf) ;

  TH1F bkg_2_y ("bkg_2_y","bkg_2_y", 30, 0., 3.14) ; bkg_2_y.SetStats (0) ;
  bkg_2NT->Draw ("y>>bkg_2_y") ;
  TF1 bkg_2_y_Pdf ("bkg_2_y_Pdf", "pol2", 0., 3.14) ;
  bkg_2_y_Pdf.SetLineColor (4) ;
//  bkg_2_y_Pdf.SetParameters (/*bkg_2_y.GetMean (), bkg_2_y.GetRMS ()*/) ;
  bkg_2_y.Fit ("bkg_2_y_Pdf") ;
  Pdfs_var.push_back (&bkg_2_y_Pdf) ;
  Pdfs.push_back (Pdfs_var) ;
  Pdfs_var.clear () ;

  TH1F bkg_3_x ("bkg_3_x","bkg_3_x", 30, 0., 30.) ; bkg_3_x.SetStats (0) ;
  bkg_3NT->Draw ("x>>bkg_3_x") ;
  TF1 bkg_3_x_Pdf ("bkg_3_x_Pdf", "gausn", 0., 30.) ;
  bkg_3_x_Pdf.SetLineColor (5) ;
  bkg_3_x_Pdf.SetParameters (bkg_3_x.GetMean (), bkg_3_x.GetRMS ()) ;
  bkg_3_x.Fit ("bkg_3_x_Pdf") ;
  Pdfs_var.push_back (&bkg_3_x_Pdf) ;

  TH1F bkg_3_y ("bkg_3_y","bkg_3_y", 30, 0., 3.14) ; bkg_3_y.SetStats (0) ;
  bkg_3NT->Draw ("y>>bkg_3_y") ;
  TF1 bkg_3_y_Pdf ("bkg_3_y_Pdf", "gausn", 0., 3.14) ;
  bkg_3_y_Pdf.SetLineColor (5) ;
  bkg_3_y_Pdf.SetParameters (bkg_3_y.GetMean (), bkg_3_y.GetRMS ()) ;
  bkg_3_y.Fit ("bkg_3_y_Pdf") ;
  Pdfs_var.push_back (&bkg_3_y_Pdf) ;
  Pdfs.push_back (Pdfs_var) ;
  Pdfs_var.clear () ;

  // check plot
  TCanvas c0 ("c0", "c0", 600, 1200) ;
  c0.Divide (2,4) ;
  c0.cd (1) ; signal_x.Draw () ;
  c0.cd (2) ; signal_y.Draw () ;
  c0.cd (3) ; bkg_1_x.Draw () ;
  c0.cd (4) ; bkg_1_y.Draw () ;
  c0.cd (5) ; bkg_2_x.Draw () ;
  c0.cd (6) ; bkg_2_y.Draw () ;
  c0.cd (7) ; bkg_3_x.Draw () ;
  c0.cd (8) ; bkg_3_y.Draw () ;
  c0.Print ("distros.eps","eps") ;

  // absorb the Pdfs normalization in the xsections   ---- ---- ---- ---- ---- ---- ---- ----
  vector<double> loc_xsecs = xsecs ;
  for (int iSpecies = 0 ; iSpecies < Pdfs.size () ; ++iSpecies)
    {
      for (int iVar = 0 ; iVar < Pdfs.at (iVar).size () ; ++iVar)
        {
          double integral = Pdfs.at (iSpecies).at (iVar)->Integral (ranges.at (iVar).first, ranges.at (iVar).second) ;
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
          for (int iSpecies = 0 ; iSpecies < Pdfs.size () ; ++iSpecies)
            {
              weights.at (iSpecies) = weight (Pdfs, loc_xsecs, iSpecies, vars_value) ;
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
      sprintf (plotName,"weightsVar_%s.eps",vars.at (iVar).c_str ()) ;
      c2.Print (plotName, "eps") ;
    }

  TGraph polygon ; 
  for (int i = 0 ; i <= samples.size () ; ++i)
    {
      double x = cos (6.28 * i / samples.size ()) ;
      double y = sin (6.28 * i / samples.size ()) ;
      polygon.SetPoint (i, x, y) ;    
    }
  polygon.SetLineColor (kGray + 1) ;    

  TH2F bkg ("bkg","", 10, -1., 1., 10, -1., 1.) ;
  bkg.SetStats (0) ;
  TCanvas c1 ("c1","c1",600,600) ;
  bkg.Draw () ;
  polygon.Draw ("Lsame") ;
  for (int iGraph = 0 ; iGraph < points.size () ; ++iGraph)
    {
//      points.at (iGraph).SetMarkerStyle (4) ;    
//      points.at (iGraph).SetMarkerSize (0.5) ;    
      points.at (iGraph).SetMarkerColor (2+iGraph) ;    
      points.at (iGraph).Draw ("Psame") ;    
    }
  c1.Print ("polygon_2D.eps","eps") ;

  TCanvas c3 ("c3", "c3", 400, 400 * points.size ()) ;
  c3.Divide (1, points.size ()) ;
  vector<TH2F *> singlePolygons ;
  for (int i = 0 ; i < points.size () ; ++i)
    {
      char name[20] ;
      sprintf (name, "sample_%d", i) ;
      TH2F * dummy = new TH2F (name, name, 100, -1., 1., 100, -1., 1.) ;
      fillFromTGraph (*dummy, points.at (i)) ; 
      dummy->SetStats (0) ;
      c3.cd (i+1) ;
      dummy->Draw ("col") ;
      singlePolygons.push_back (dummy) ;
    }
  polygon.Draw ("Lsame") ;
  c3.Print ("polygons.eps","eps") ;

  c1.cd () ;
  bkg.Draw () ;
  for (int i = 0 ; i < points.size () ; ++i)
    {
      singlePolygons.at (i)->SetLineColor (2+i) ;
      singlePolygons.at (i)->SetLineWidth (0.5) ;
      singlePolygons.at (i)->SetContour (5) ;
//      singlePolygons.at (i)->SetContourLevel (1, 0.7 * singlePolygons.at (i)->GetMaximum ()) ;
//      singlePolygons.at (i)->SetContourLevel (2, 0.4 * singlePolygons.at (i)->GetMaximum ()) ;
//      singlePolygons.at (i)->SetContourLevel (3, 0.2 * singlePolygons.at (i)->GetMaximum ()) ;
      singlePolygons.at (i)->Draw ("cont3 same") ;
    }
  polygon.Draw ("Lsame") ;
  c1.Print ("polygon_cont.eps","eps") ;
  
  // calculate the likelihood ratio, given the first sample as singnal

  // the hypothesis is that Pdfs are factorized
  vector< vector<TF1*> > sigPdfs ; // Pdfs[iSpecies][iVar]
  sigPdfs.push_back (Pdfs.at (0)) ;
  vector<double> sigXsecs ;
  sigXsecs.push_back (xsecs.at (0)) ;
  
  vector< vector<TF1*> > bkgPdfs ; // Pdfs[iSpecies][iVar]
  bkgPdfs.push_back (Pdfs.at (1)) ;
  bkgPdfs.push_back (Pdfs.at (2)) ;
  bkgPdfs.push_back (Pdfs.at (3)) ;
  vector<double> bkgXsecs ;
  bkgXsecs.push_back (xsecs.at (1)) ;
  bkgXsecs.push_back (xsecs.at (2)) ;
  bkgXsecs.push_back (xsecs.at (3)) ;

  TH1F lr_sig_h1 ("lr_sig_h1", "lr_sig_h1", 100, 0., 20.) ;
  for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
    samples.at (0)->SetBranchAddress (vars.at (iVar).c_str (), vars_value.at (iVar)) ;
  // loop over signal events
  for (int iEvent = 0 ; iEvent < samples.at (0)->GetEntries () ; ++iEvent)
    {
      samples.at (0)->GetEntry (iEvent) ;
      lr (sigPdfs, sigXsecs, bkgPdfs, bkgXsecs, vars_value) ;
//      lr_sig.Fill (lr (sigPdfs, sigXsecs, bkgPdfs, bkgXsecs, vars_value)) ;      
      lr_sig_h1.Fill (lr (sigPdfs, sigXsecs, bkgPdfs, bkgXsecs, vars_value)) ;      
    } // loop over signal events


  TH1F lr_bkg_h1 ("lr_bkg_h1", "lr_bkg_h1", 100, 0., 20.) ;
  // loop over bkg samples
  for (int iSample = 1 ; iSample < samples.size () ; ++iSample)
    {
      for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
        samples.at (iSample)->SetBranchAddress (vars.at (iVar).c_str (), vars_value.at (iVar)) ;

      // loop over bkg events
      for (int iEvent = 0 ; iEvent < samples.at (iSample)->GetEntries () ; ++iEvent)
        {
          samples.at (iSample)->GetEntry (iEvent) ;
          lr_bkg_h1.Fill (lr (sigPdfs, sigXsecs, bkgPdfs, bkgXsecs, vars_value)) ;      
        } // loop over bkg events
    } // loop over bkg samples

  TCanvas c4 ;
  c4.cd () ;
  c4.SetLogy () ;
  lr_bkg_h1.SetLineColor (kRed) ;
  lr_bkg_h1.SetStats (0) ;
  lr_bkg_h1.Draw () ;
  lr_sig_h1.SetLineColor (kBlue) ;
  lr_sig_h1.SetStats (0) ;
  lr_sig_h1.Draw ("same") ;
  c4.Print ("lr.eps","eps") ;

  TH3F lr_polygon ("lr_polygon", "lr_polygon", 100, -1., 1., 100, -1., 1., 100, 0., 20.) ; 
  // loop over samples
  for (int iSample = 0 ; iSample < samples.size () ; ++iSample)
    {
      for (int iVar = 0 ; iVar < vars.size () ; ++iVar)
        samples.at (iSample)->SetBranchAddress (vars.at (iVar).c_str (), vars_value.at (iVar)) ;

      // loop over events
      for (int iEvent = 0 ; iEvent < samples.at (iSample)->GetEntries () ; ++iEvent)
        {
          samples.at (iSample)->GetEntry (iEvent) ;
          std::vector<double> weights (samples.size (), 1.) ;
          // loop over species
          for (int iSpecies = 0 ; iSpecies < Pdfs.size () ; ++iSpecies)
            {
              weights.at (iSpecies) = weight (Pdfs, loc_xsecs, iSpecies, vars_value) ;
            } // loop over species
          std::pair<double, double> point = get_coord (weights) ;
          lr_polygon.Fill (point.second, point.first, // the inversion is because we are using a TH3F 
                           lr (sigPdfs, sigXsecs, bkgPdfs, bkgXsecs, vars_value)
                           ) ;       
        } // loop over events
    } // loop over samples

  TProfile2D * lr_polygon_profile = lr_polygon.Project3DProfile ("xy") ;
//  TCanvas c5 ;
  c1.cd () ;
  c1.SetLogz () ;
  lr_polygon_profile->SetStats (0) ;
  lr_polygon_profile->Draw ("colz") ;
  c1.Print ("polygon_lr.eps","eps") ;

  f.Close () ;

  for (int iVar = 0 ; iVar < vars_value.size () ; ++iVar)
    delete vars_value.at (iVar) ;

  return 0 ;
}

