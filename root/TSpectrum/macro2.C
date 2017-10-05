#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"


 
Int_t npeaks = 30;
Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = par[0] + par[1]*x[0];
   for (Int_t p=0;p<npeaks;p++) {
      Double_t norm  = par[3*p+2];
      Double_t mean  = par[3*p+3];
      Double_t sigma = par[3*p+4];
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
}


void macro2(Int_t np=10) {
   npeaks = np;
   TH1F *h = new TH1F("h","test",9000,0,9000);

   // read the spectrum
   ifstream data ("Th232_1.dat", ios::in) ;
   int j = 0 ; 
   while (!data.eof ()) 
     {
       int dummy ;
       data >> dummy ;
       h->SetBinContent (++j, dummy) ;
     }
   data.close () ;
   cout << "read " << j << " lines\n" ;
   cout << "read " << h->GetNbinsX () << " bins" << endl ;
   
   TH1F *h2 = (TH1F*)h->Clone("h2");
   //Use TSpectrum to find the peak candidates
   TSpectrum *s = new TSpectrum(2*npeaks);
   Int_t nfound = s->Search(h,1,"new");
   printf("Found %d candidate peaks to fitn",nfound);

   TH1F * bkg = s->Background (h) ; 
// const char*	Background(Double_t* spectrum, Int_t ssize, Int_t numberIterations, Int_t direction, Int_t filterOrder, bool smoothing, Int_t smoothWindow, bool compton)

   TCanvas * c2 = new TCanvas () ;
   c2->cd () ;
   h->Draw () ;
   bkg->Draw ("same") ;
   cout << endl ;
}