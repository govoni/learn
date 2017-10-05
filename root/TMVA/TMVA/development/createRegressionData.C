// plot the variables
#include "TTree.h"
#include "TArrayD.h"
#include "TStyle.h"
#include "TFile.h"
#include "TRandom.h"
#include "Riostream.h"
#include "TCanvas.h"
#include "TMatrixD.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TBranch.h"
#include <vector>

void plot( TString fname = "RegressionData.root" ) 
{
   TFile* dataFile = TFile::Open( fname );

   if (!dataFile) {
      cout << "ERROR: cannot open file: " << fname << endl;
      return;
   }

   TTree *treeR = (TTree*)dataFile->Get("TreeR");

   TCanvas* c = new TCanvas( "c", "", 0, 0, 550, 550 );

   TStyle *TMVAStyle = gROOT->GetStyle("Plain"); // our style is based on Plain
   TMVAStyle->SetOptStat(0);
   TMVAStyle->SetPadTopMargin(0.02);
   TMVAStyle->SetPadBottomMargin(0.16);
   TMVAStyle->SetPadRightMargin(0.03);
   TMVAStyle->SetPadLeftMargin(0.15);
   TMVAStyle->SetPadGridX(0);
   TMVAStyle->SetPadGridY(0);
   
   TMVAStyle->SetOptTitle(0);
   TMVAStyle->SetTitleW(.4);
   TMVAStyle->SetTitleH(.10);
   TMVAStyle->SetTitleX(.5);
   TMVAStyle->SetTitleY(.9);
   TMVAStyle->SetMarkerStyle(20);
   TMVAStyle->SetMarkerSize(.4);
   TMVAStyle->cd();


   Float_t xmin = treeR->GetMinimum( "var1" );
   Float_t xmax = treeR->GetMaximum( "var1" );
   Float_t ymin = treeR->GetMinimum( "var2" );
   Float_t ymax = treeR->GetMaximum( "var2" );

   Int_t nbin = 100;
   TH2F* frame = new TH2F( "DataR", "Data", nbin, xmin, xmax, nbin, ymin, ymax );


   // project trees
   treeR->Draw( "var2:var1>>DataR", "fvalue", "0" );

   // set style
   frame->SetMarkerSize( 0.2 );
   frame->SetMarkerColor( 4 );

   // legend
   frame->SetTitle( "fvalue in var2:var1 plane " );
   frame->GetXaxis()->SetTitle( "var1" );
   frame->GetYaxis()->SetTitle( "var2" );
   frame->GetZaxis()->SetTitle( "fvalue" );

   frame->SetLabelSize( 0.04, "X" );
   frame->SetLabelSize( 0.04, "Y" );
   frame->SetTitleSize( 0.05, "X" );
   frame->SetTitleSize( 0.05, "Y" );

   // and plot
   frame->Draw("surf");


}


// create the data
void createPlane(Int_t N = 200000)
{
   // the true function around which the "sampled" (i.e. measured) values fluctuate
   // is a plan  z = a*x + b*y + c

   const Float_t a=7, b=9, c=2;
   const Int_t nvar = 2;

   TRandom1 R( 100 );

   Float_t xvar[nvar];
   Float_t fvalue;

   // output flie
   TFile* dataFile = TFile::Open( "RegressionData.root", "RECREATE" );

   // create signal and background trees
   TTree* treeR = new TTree( "TreeR", "TreeR", 1 );   
   for (Int_t ivar=0; ivar<nvar; ivar++) {
      treeR->Branch( TString(Form( "var%i", ivar+1 )).Data(), &xvar[ivar], TString(Form( "var%i/F", ivar+1 )).Data() );
   }
   treeR->Branch( "fvalue", &fvalue, "fvalue/F") ;


   Float_t delta_fvalue;
   
   // event loop
   cout << "start event loop " << endl;
   for (Int_t i=0; i<N; i++) {
      //cout << "i=" << i << endl;
      for (Int_t ivar=0; ivar<nvar; ivar++) {
         xvar[ivar] = R.Rndm()*5.;
         //cout << "xvar["<<ivar<<"] = " << xvar[ivar] << endl;
      }
      delta_fvalue = R.Rndm()-0.5;

      fvalue = a*xvar[0] + b*xvar[1] + c + delta_fvalue;
      
      if (i%1000 == 0) cout << "... event: " << i << " (" << N << ")" << endl;
      
      treeR->Fill();
   }

   
   // write tree
   treeR->Write();
   treeR->Show(0);

   dataFile->Close();
   cout << "created data file: " << dataFile->GetName() << endl;

   plot("RegressionData.root");
}

