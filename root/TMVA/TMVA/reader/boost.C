#include <vector>
#include <string>
#include "tmvaglob.C"


// this macro plots the reference distribuions for the Likelihood
// methods for the various input variables used in TMVA (e.g. running
// TMVAnalysis.C).  Signal and Background are plotted separately

// input: - Input file (result from TMVA),
//        - use of TMVA plotting TStyle
void boost( TString fin = "TMVA.root", Bool_t useTMVAStyle = kTRUE )
{
   // set style and remove existing canvas'
   TMVAGlob::Initialize( useTMVAStyle );
  
   // checks if file with name "fin" is already open, and if not opens one
   TFile* file = TMVAGlob::OpenFile( fin );

   // get all titles of the method likelihood
   TList titles;
   UInt_t ninst = TMVAGlob::GetListOfTitles("Method_Boost",titles);
   if (ninst==0) 
   {
      cout << "Could not locate directory 'Method_Boost' in file " << fin << endl;
      return;
   }
   // loop over all titles
   TIter keyIter(&titles);
   TDirectory *lhdir;
   TKey *key;
   while ((key = TMVAGlob::NextKey(keyIter,"TDirectory"))) 
   {
      lhdir = (TDirectory *)key->ReadObj();
      boost( lhdir );
   }
}

void boost( TDirectory *lhdir )
{
   //std::vector<TCanvas*> canvas;
   TCanvas** canvas = new TCanvas*[10];
   Int_t width  = 670;
   Int_t height = 380;

   UInt_t ic = -1;

   TIter next(lhdir->GetListOfKeys());
   TKey *key;
   while ((key = TMVAGlob::NextKey(next,"TH1"))) 
   {
      TH1 *h = (TH1*)key->ReadObj();
      TString hname( h->GetName() );
      if (hname.BeginsWith("Booster_"))
      {
         ic++;
         hname = TString(h->GetName()).ReplaceAll("Booster_","");
         //cout << hname.Data() << endl;
         canvas[ic]=new TCanvas( hname, h->GetTitle(),ic*50+50, ic*20+20, width, height );
         h->Draw();
         canvas[ic]->Update();
         TMVAGlob::imgconv( canvas[ic] , Form("plots/%s",hname.Data()));
      }
   }
}

