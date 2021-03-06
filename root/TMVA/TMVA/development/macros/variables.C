#include "tmvaglob.C"

// this macro plots the distributions of the different input variables
// used in TMVA (e.g. running TMVAnalysis.C).  Signal and Background are overlayed.



const TString directories[TMVAGlob::kNumOfMethods] = { "input_variables",
                                                       "decorrelated_input_variables",
                                                       "principal_component_analyzed_input_variables" };

const TString titles[TMVAGlob::kNumOfMethods] = { "TMVA Input Variables",
                                                  "Decorrelated TMVA Input Variables",
                                                  "Principal Component Transformed TMVA Input Variables" };

const TString outfname[TMVAGlob::kNumOfMethods] = { "variables",
                                                    "variables_decorr",
                                                    "variables_pca" };

// input: - Input file (result from TMVA),
//        - normal/decorrelated/PCA
//        - use of TMVA plotting TStyle
void variables( TString fin = "TMVA.root", TMVAGlob::TypeOfPlot type = TMVAGlob::kNormal, bool useTMVAStyle=kTRUE )
{
   // set style and remove existing canvas'
   TMVAGlob::Initialize( useTMVAStyle );
   
   // checks if file with name "fin" is already open, and if not opens one
   TFile* file = TMVAGlob::OpenFile( fin );  
   
   TDirectory* dir = (TDirectory*)gDirectory->Get( directories[type] );
   if (dir==0) {
      cout << "Could not locate directory '" << directories[type] << "' in file: " << fin << endl;
      return;
   }
   dir->cd();
 
   // how many plots are in the directory?
   Int_t noPlots = ((dir->GetListOfKeys())->GetEntries()) / 2;
 
   // define Canvas layout here!
   // default setting
   Int_t xPad;  // no of plots in x
   Int_t yPad;  // no of plots in y
   Int_t width; // size of canvas
   Int_t height;
   switch (noPlots) {
   case 1:
      xPad = 1; yPad = 1; width = 500; height = width; break;
   case 2:
      xPad = 2; yPad = 1; width = 600; height = 0.7*width; break;
   case 3:
      xPad = 3; yPad = 1; width = 800; height = 0.7*width; break;
   case 4:
      xPad = 2; yPad = 2; width = 600; height = width; break;
   default:
      xPad = 3; yPad = 2; width = 800; height = 0.7*width; break;
   }
   Int_t noPad = xPad * yPad ;   

   // this defines how many canvases we need
   const Int_t noCanvas = 1 + (Int_t)((noPlots - 0.001)/noPad);
   TCanvas **c = new TCanvas*[noCanvas];
   for (Int_t ic=0; ic<noCanvas; ic++) c[ic] = 0;

   cout << "--- Found: " << noPlots << " plots; will produce: " << noCanvas << " canva(s)" << endl;

   // counter variables
   Int_t countCanvas = 0;
   Int_t countPad    = 1;

   // loop over all objects in directory
   TIter next(dir->GetListOfKeys());
   TKey *key;
   while ((key = (TKey*)next())) {

      // make sure, that we only look at histograms
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;
      TH1 *sig = (TH1*)key->ReadObj();
      TString hname= sig->GetName();

      // check for all signal histograms
      if (hname.Contains("__S")){ // found a new signal plot

         // create new canvas
         if ((c[countCanvas]==NULL) || (countPad>noPad)) {
            cout << "--- Book canvas no: " << countCanvas << endl;
            char cn[20];
            sprintf( cn, "canvas%d", countCanvas+1 );
            c[countCanvas] = new TCanvas( cn, titles[type], 
                                          countCanvas*50+200, countCanvas*20, width, height ); 
            // style
            c[countCanvas]->SetBorderMode(0);
            c[countCanvas]->SetFillColor(0);

            c[countCanvas]->Divide(xPad,yPad);
            countPad = 1;
         }       

         // save canvas to file
         c[countCanvas]->cd(countPad);
         countPad++;

         // find the corredponding backgrouns histo
         TString bgname = hname;
         bgname.ReplaceAll("__S","__B");
         TH1 *bgd = (TH1*)dir->Get(bgname);
         if (bgd == NULL) {
            cout << "ERROR!!! couldn't find backgroung histo for" << hname << endl;
            exit;
         }

         // this is set but not stored during plot creation in MVA_Factory
         TMVAGlob::SetSignalAndBackgroundStyle( sig, bgd );

         // chop off "signal" 
         TString title(sig->GetTitle());
         title.ReplaceAll("signal","");
         sig->SetTitle( TString( titles[type] ) + title );
         TMVAGlob::SetFrameStyle( sig, 1.2 );

         // finally plot and overlay       
         Float_t sc = 1.1;
         if (countPad==2) sc = 1.3;
         sig->SetMaximum( TMath::Max( sig->GetMaximum(), bgd->GetMaximum() )*sc );
         sig->Draw();

         bgd->Draw("same");
         sig->GetXaxis()->SetTitle( title );
         sig->GetYaxis()->SetTitleOffset( 1.35 );
         sig->GetYaxis()->SetTitle("Normalized");

         // redraw axes
         sig->Draw("sameaxis");

         // Draw legend
         if (countPad==2){
            TLegend *legend= new TLegend( gPad->GetLeftMargin(), 
                                          1-gPad->GetTopMargin()-.18, 
                                          gPad->GetLeftMargin()+.4, 
                                          1-gPad->GetTopMargin() );
            legend->AddEntry(sig,"Signal","F");
            legend->AddEntry(bgd,"Background","F");
            legend->Draw("same");
            legend->SetBorderSize(1);
            legend->SetMargin( 0.3 );
         } 

         // save canvas to file
         if (countPad > noPad) {
            c[countCanvas]->Update();
            TString fname = Form( "plots/%s_c%i", outfname[type].Data(), countCanvas+1 );
            TMVAGlob::imgconv( c[countCanvas], &fname[0] );
            //        TMVAGlob::plot_logo(); // don't understand why this doesn't work ... :-(
            countCanvas++;
         }
      }
   }

   if (countPad <= noPad) {
      c[countCanvas]->Update();
      TString fname = Form( "plots/%s_c%i", outfname[type].Data(), countCanvas+1 );
      TMVAGlob::imgconv( c[countCanvas], &fname[0] );
   }
}
