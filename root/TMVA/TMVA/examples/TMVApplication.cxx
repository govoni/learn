/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVApplication                                                     *
 *                                                                                *
 * This exectutable provides a simple example on how to use the trained           *
 * classifiers within a C++ analysis module                                       *
 *                                                                                *
 * ------------------------------------------------------------------------------ *
 * see also the alternative (slightly faster) way to retrieve the MVA values in   *
 * examples/TMVApplicationAlternative.cxx                                         *
 * ------------------------------------------------------------------------------ *
 **********************************************************************************/

#include <iostream> // Stream declarations

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

using namespace std;

// ---------------------------------------------------------------
// choose MVA methods to be applied
Bool_t Use_Cuts            = 0;
Bool_t Use_CutsD           = 0;
Bool_t Use_CutsGA          = 1;
Bool_t Use_Likelihood      = 1;
Bool_t Use_LikelihoodD     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
Bool_t Use_LikelihoodPCA   = 1; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
Bool_t Use_PDERS           = 0;
Bool_t Use_PDERSD          = 0;
Bool_t Use_PDERSPCA        = 0;
Bool_t Use_KNN             = 1;
Bool_t Use_HMatrix         = 1;
Bool_t Use_Fisher          = 1;
Bool_t Use_FDA_GA          = 0;
Bool_t Use_FDA_MT          = 1;
Bool_t Use_MLP             = 1; // this is the recommended ANN
Bool_t Use_CFMlpANN        = 0; 
Bool_t Use_TMlpANN         = 0; 
Bool_t Use_SVM_Gauss       = 1;
Bool_t Use_SVM_Poly        = 0;
Bool_t Use_SVM_Lin         = 0;
Bool_t Use_BDT             = 1;
Bool_t Use_BDTD            = 0;
Bool_t Use_RuleFit         = 1;
// ---------------------------------------------------------------

int main( void ) 
{
   cout << endl;
   cout << "==> Start TMVApplication" << endl;
 
   //
   // create the Reader object
   //
   TMVA::Reader *reader = new TMVA::Reader("!Color");    

   // create a set of variables and declare them to the reader
   // - the variable names must corresponds in name and type to 
   // those given in the weight file(s) that you use
   Float_t var1, var2;
   Float_t var3, var4;
   reader->AddVariable( "var1+var2", &var1 );
   reader->AddVariable( "var1-var2", &var2 );
   reader->AddVariable( "var3",      &var3 );
   reader->AddVariable( "var4",      &var4 );

   //
   // book the MVA methods
   //
   string dir    = "weights/";
   string prefix = "TMVAnalysis";

   if (Use_Cuts)          reader->BookMVA( "Cuts method",          dir + prefix + "_Cuts.weights.txt" );
   if (Use_CutsD)         reader->BookMVA( "CutsD method",         dir + prefix + "_CutsD.weights.txt" );
   if (Use_CutsGA)        reader->BookMVA( "CutsGA method",        dir + prefix + "_CutsGA.weights.txt" );
   if (Use_Likelihood)    reader->BookMVA( "Likelihood method",    dir + prefix + "_Likelihood.weights.txt" );
   if (Use_LikelihoodD)   reader->BookMVA( "LikelihoodD method",   dir + prefix + "_LikelihoodD.weights.txt" );
   if (Use_LikelihoodPCA) reader->BookMVA( "LikelihoodPCA method", dir + prefix + "_LikelihoodPCA.weights.txt" );
   if (Use_PDERS)         reader->BookMVA( "PDERS method",         dir + prefix + "_PDERS.weights.txt" );
   if (Use_PDERSD)        reader->BookMVA( "PDERSD method",        dir + prefix + "_PDERSD.weights.txt" );
   if (Use_PDERSPCA)      reader->BookMVA( "PDERSPCA method",      dir + prefix + "_PDERSPCA.weights.txt" );
   if (Use_KNN)           reader->BookMVA( "KNN method",           dir + prefix + "_KNN.weights.txt" );
   if (Use_HMatrix)       reader->BookMVA( "HMatrix method",       dir + prefix + "_HMatrix.weights.txt" );
   if (Use_Fisher)        reader->BookMVA( "Fisher method",        dir + prefix + "_Fisher.weights.txt" );
   if (Use_MLP)           reader->BookMVA( "MLP method",           dir + prefix + "_MLP.weights.txt" );
   if (Use_CFMlpANN)      reader->BookMVA( "CFMlpANN method",      dir + prefix + "_CFMlpANN.weights.txt" );
   if (Use_TMlpANN)       reader->BookMVA( "TMlpANN method",       dir + prefix + "_TMlpANN.weights.txt" );
   if (Use_BDT)           reader->BookMVA( "BDT method",           dir + prefix + "_BDT.weights.txt" );
   if (Use_BDTD)          reader->BookMVA( "BDTD method",          dir + prefix + "_BDTD.weights.txt" );
   if (Use_RuleFit)       reader->BookMVA( "RuleFit method",       dir + prefix + "_RuleFit.weights.txt" );
   if (Use_SVM_Gauss)     reader->BookMVA( "SVM_Gauss method",     dir + prefix + "_SVM_Gauss.weights.txt" );
   if (Use_SVM_Poly)      reader->BookMVA( "SVM_Poly method",      dir + prefix + "_SVM_Poly.weights.txt" );
   if (Use_SVM_Lin)       reader->BookMVA( "SVM_Lin method",       dir + prefix + "_SVM_Lin.weights.txt" );
   if (Use_FDA_MT)        reader->BookMVA( "FDA_MT method",        dir + prefix + "_FDA_MT.weights.txt" );
   if (Use_FDA_GA)        reader->BookMVA( "FDA_GA method",        dir + prefix + "_FDA_GA.weights.txt" );
  
   // book output histograms
   UInt_t nbin = 100;
   TH1F *histLk, *histLkD, *histLkPCA, *histPD, *histPDD, *histPDPCA, *histKNN, *histHm, *histFi;
   TH1F *histNn, *histNnC, *histNnT, *histBdt, *histBdtD, *histRf;
   TH1F *histSVMG, *histSVMP, *histSVML;
   TH1F *histFDAMT, *histFDAGA;

   if (Use_Likelihood)    histLk    = new TH1F( "MVA_Likelihood",    "MVA_Likelihood",    nbin,  0, 1 );
   if (Use_LikelihoodD)   histLkD   = new TH1F( "MVA_LikelihoodD",   "MVA_LikelihoodD",   nbin,  0.000001, 0.9999 );
   if (Use_LikelihoodPCA) histLkPCA = new TH1F( "MVA_LikelihoodPCA", "MVA_LikelihoodPCA", nbin,  0, 1 );
   if (Use_PDERS)         histPD    = new TH1F( "MVA_PDERS",         "MVA_PDERS",         nbin,  0, 1 );
   if (Use_PDERSD)        histPDD   = new TH1F( "MVA_PDERSD",        "MVA_PDERSD",        nbin,  0, 1 );
   if (Use_PDERSPCA)      histPDPCA = new TH1F( "MVA_PDERSPCA",      "MVA_PDERSPCA",      nbin,  0, 1 );
   if (Use_KNN)           histKNN   = new TH1F( "MVA_KNN",           "MVA_KNN",           nbin,  0, 1 );
   if (Use_HMatrix)       histHm    = new TH1F( "MVA_HMatrix",       "MVA_HMatrix",       nbin, -0.95, 1.55 );
   if (Use_Fisher)        histFi    = new TH1F( "MVA_Fisher",        "MVA_Fisher",        nbin, -4, 4 );
   if (Use_MLP)           histNn    = new TH1F( "MVA_MLP",           "MVA_MLP",           nbin, -0.25, 1.5 );
   if (Use_CFMlpANN)      histNnC   = new TH1F( "MVA_CFMlpANN",      "MVA_CFMlpANN",      nbin,  0, 1 );
   if (Use_TMlpANN)       histNnT   = new TH1F( "MVA_TMlpANN",       "MVA_TMlpANN",       nbin, -1.3, 1.3 );
   if (Use_BDT)           histBdt   = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.8, 0.8 );
   if (Use_BDTD)          histBdtD  = new TH1F( "MVA_BDTD",          "MVA_BDTD",          nbin, -0.4, 0.6 );
   if (Use_RuleFit)       histRf    = new TH1F( "MVA_RuleFit",       "MVA_RuleFit",       nbin, -2.0, 2.0 );
   if (Use_SVM_Gauss)     histSVMG  = new TH1F( "MVA_SVM_Gauss",     "MVA_SVM_Gauss",     nbin, 0.0, 1.0 );
   if (Use_SVM_Poly)      histSVMP  = new TH1F( "MVA_SVM_Poly",      "MVA_SVM_Poly",      nbin, 0.0, 1.0 );
   if (Use_SVM_Lin)       histSVML  = new TH1F( "MVA_SVM_Lin",       "MVA_SVM_Lin",       nbin, 0.0, 1.0 );
   if (Use_FDA_MT)        histFDAMT = new TH1F( "MVA_FDA_MT",        "MVA_FDA_MT",        nbin, -2.0, 3.0 );
   if (Use_FDA_GA)        histFDAGA = new TH1F( "MVA_FDA_GA",        "MVA_FDA_GA",        nbin, -2.0, 3.0 );

   // book examsple histogram for probability (the other methods are done similarly)
   TH1F *probHistFi, *rarityHistFi;
   if (Use_Fisher) {
      probHistFi   = new TH1F( "PROBA_MVA_Fisher",  "PROBA_MVA_Fisher",  nbin, 0, 1 );
      rarityHistFi = new TH1F( "RARITY_MVA_Fisher", "RARITY_MVA_Fisher", nbin, 0, 1 );
   }

   // Prepare input tree (this must be replaced by your data source)
   // in this example, there is a toy tree with signal and one with background events
   // we'll later on use only the "signal" events for the test in this example.
   //   
   TString fname = "../macros/tmva_example.root";   
   TFile *input = TFile::Open( fname );

   if (!input) {
      cout << "ERROR: could not open data file: " << fname << endl;
      exit(1);
   }

   //
   // prepare the tree
   // - here the variable names have to corresponds to your tree
   // - you can use the same variables as above which is slightly faster,
   //   but of course you can use different ones and copy the values inside the event loop
   //
   TTree* theTree = (TTree*)input->Get("TreeS");
   cout << "--- Select signal sample" << endl;
   Float_t userVar1, userVar2;
   theTree->SetBranchAddress( "var1", &userVar1 );
   theTree->SetBranchAddress( "var2", &userVar2 );
   theTree->SetBranchAddress( "var3", &var3 );
   theTree->SetBranchAddress( "var4", &var4 );

   // efficiency calculator for cut method
   Int_t    nSelCuts = 0, nSelCutsD = 0, nSelCutsGA = 0;
   Double_t effS     = 0.7;

   cout << "--- Processing: " << theTree->GetEntries() << " events" << endl;
   TStopwatch sw;
   sw.Start();
   for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {

      if (ievt%1000 == 0)
         cout << "--- ... Processing event: " << ievt << endl;

      theTree->GetEntry(ievt);

      var1 = userVar1 + userVar2;
      var2 = userVar1 - userVar2;
    
      // 
      // return the MVAs and fill to histograms
      // 
      if (Use_Cuts) {
         // Cuts is a special case: give the desired signal efficienciy
         Bool_t passed = reader->EvaluateMVA( "Cuts method", effS );
         if (passed) nSelCuts++;
      }
      if (Use_CutsD) {
         // Cuts is a special case: give the desired signal efficienciy
         Bool_t passed = reader->EvaluateMVA( "CutsD method", effS );
         if (passed) nSelCutsD++;
      }
      if (Use_CutsGA) {
         // Cuts is a special case: give the desired signal efficienciy
         Bool_t passed = reader->EvaluateMVA( "CutsGA method", effS );
         if (passed) nSelCutsGA++;
      }

      if (Use_Likelihood   )   histLk    ->Fill( reader->EvaluateMVA( "Likelihood method"    ) );
      if (Use_LikelihoodD  )   histLkD   ->Fill( reader->EvaluateMVA( "LikelihoodD method"   ) );
      if (Use_LikelihoodPCA)   histLkPCA ->Fill( reader->EvaluateMVA( "LikelihoodPCA method" ) );
      if (Use_PDERS        )   histPD    ->Fill( reader->EvaluateMVA( "PDERS method"         ) );
      if (Use_PDERSD       )   histPDD   ->Fill( reader->EvaluateMVA( "PDERSD method"        ) );
      if (Use_PDERSPCA     )   histPDPCA ->Fill( reader->EvaluateMVA( "PDERSPCA method"      ) );
      if (Use_KNN          )   histKNN   ->Fill( reader->EvaluateMVA( "KNN method"           ) );
      if (Use_HMatrix      )   histHm    ->Fill( reader->EvaluateMVA( "HMatrix method"       ) );
      if (Use_Fisher       )   histFi    ->Fill( reader->EvaluateMVA( "Fisher method"        ) );
      if (Use_MLP          )   histNn    ->Fill( reader->EvaluateMVA( "MLP method"           ) );
      if (Use_CFMlpANN     )   histNnC   ->Fill( reader->EvaluateMVA( "CFMlpANN method"      ) );
      if (Use_TMlpANN      )   histNnT   ->Fill( reader->EvaluateMVA( "TMlpANN method"       ) );
      if (Use_BDT          )   histBdt   ->Fill( reader->EvaluateMVA( "BDT method"           ) );
      if (Use_BDTD         )   histBdtD  ->Fill( reader->EvaluateMVA( "BDTD method"          ) );
      if (Use_RuleFit      )   histRf    ->Fill( reader->EvaluateMVA( "RuleFit method"       ) );
      if (Use_SVM_Gauss    )   histSVMG  ->Fill( reader->EvaluateMVA( "SVM_Gauss method"     ) );
      if (Use_SVM_Poly     )   histSVMP  ->Fill( reader->EvaluateMVA( "SVM_Poly method"      ) );
      if (Use_SVM_Lin      )   histSVML  ->Fill( reader->EvaluateMVA( "SVM_Lin method"       ) );
      if (Use_FDA_MT       )   histFDAMT ->Fill( reader->EvaluateMVA( "FDA_MT method"        ) );
      if (Use_FDA_GA       )   histFDAGA ->Fill( reader->EvaluateMVA( "FDA_GA method"        ) );

      // retrieve probability instead of MVA output
      if (Use_Fisher       )   {
         probHistFi  ->Fill( reader->GetProba ( "Fisher method" ) );
         rarityHistFi->Fill( reader->GetRarity( "Fisher method" ) );
      }
   }
   sw.Stop();
   cout << "--- End of event loop: "; sw.Print();
   // get elapsed time
   if (Use_Cuts)   cout << "--- Efficiency for Cuts method  : " << double(nSelCuts)/theTree->GetEntries()
                        << " (for a required signal efficiency of " << effS << ")" << endl;
   if (Use_CutsD)  cout << "--- Efficiency for CutsD method : " << double(nSelCutsD)/theTree->GetEntries()
                        << " (for a required signal efficiency of " << effS << ")" << endl;
   if (Use_CutsGA) cout << "--- Efficiency for CutsGA method: " << double(nSelCutsGA)/theTree->GetEntries()
                        << " (for a required signal efficiency of " << effS << ")" << endl;

   // test: retrieve cuts for particular signal efficiency
   TMVA::MethodCuts* mcuts = dynamic_cast<TMVA::MethodCuts*>(reader->FindMVA( "CutsGA method" ));
   if (mcuts) {      
      std::vector<Double_t> cutsMin;
      std::vector<Double_t> cutsMax;
      mcuts->GetCuts( 0.7, cutsMin, cutsMax );
      cout << "--- -------------------------------------------------------------" << endl;
      cout << "--- Retrieve cut values for signal efficiency of 0.7 from Reader" << endl;
      for (UInt_t ivar=0; ivar<cutsMin.size(); ivar++) {
         cout << "... Cut: " 
              << cutsMin[ivar] 
              << " < \"" 
              << mcuts->GetInputVar(ivar) 
              << "\" <= " 
              << cutsMax[ivar] << endl;
      }
      cout << "--- -------------------------------------------------------------" << endl;
   }

   //
   // write histograms
   //
   TFile *target  = new TFile( "TMVApp.root","RECREATE" );
   if (Use_Likelihood   )   histLk    ->Write();
   if (Use_LikelihoodD  )   histLkD   ->Write();
   if (Use_LikelihoodPCA)   histLkPCA ->Write();
   if (Use_PDERS        )   histPD    ->Write();
   if (Use_PDERSD       )   histPDD   ->Write();
   if (Use_PDERSPCA     )   histPDPCA ->Write();
   if (Use_KNN          )   histKNN   ->Write();
   if (Use_HMatrix      )   histHm    ->Write();
   if (Use_Fisher       )   histFi    ->Write();
   if (Use_MLP          )   histNn    ->Write();
   if (Use_CFMlpANN     )   histNnC   ->Write();
   if (Use_TMlpANN      )   histNnT   ->Write();
   if (Use_BDT          )   histBdt   ->Write();
   if (Use_BDTD         )   histBdtD  ->Write();
   if (Use_RuleFit      )   histRf    ->Write();
   if (Use_SVM_Gauss    )   histSVMG  ->Write();
   if (Use_SVM_Poly     )   histSVMP  ->Write();
   if (Use_SVM_Lin      )   histSVML  ->Write();
   if (Use_FDA_MT       )   histFDAMT ->Write();
   if (Use_FDA_GA       )   histFDAGA ->Write();

   // write also probability hists
   if (Use_Fisher) { probHistFi->Write(); rarityHistFi->Write(); }
   target->Close();

   cout << "--- Created root file: \"TMVApp.root\" containing the MVA output histograms" << endl;
  
   delete reader;
    
   cout << "==> TMVApplication is done!" << endl << endl;
} 
