#include "tmvaglob.C"

// this macro serves to assess the convergence of the MLP ANN. 
// It compares the error estimator for the training and testing samples.
// If overtraining occurred, the estimator for the training sample should 
// monotoneously decrease, while the estimator of the testing sample should 
// show a minimum after which it increases.

// input: - Input file (result from TMVA),
//        - use of TMVA plotting TStyle
void annconvergencetest( TString fin = "TMVA.root", bool useTMVAStyle=kTRUE )
{
   // set style and remove existing canvas'
   TMVAGlob::Initialize( useTMVAStyle );

   // checks if file with name "fin" is already open, and if not opens one
   TMVAGlob::OpenFile( fin );  

   TDirectory * dir = (TDirectory*)gDirectory->Get("MLP");
   if (dir==0) {
      cout << "Could not locate directory MLP in file " << fin << endl;
      return;
   }
   dir->cd();  

   TCanvas* c = new TCanvas( "MLPConvergenceTest", "MLP Convergence Test", 150, 0, 600, 580*0.8 ); 
  
   Double_t m1  = estimatorHistTrain->GetMaximum();
   Double_t m2  = estimatorHistTest ->GetMaximum();
   Double_t max = TMath::Max( m1, m2 );
   m1  = estimatorHistTrain->GetMinimum();
   m2  = estimatorHistTest ->GetMinimum();
   Double_t min = TMath::Min( m1, m2 );
   estimatorHistTrain->SetMaximum( max + 0.1*(max - min) );
   estimatorHistTrain->SetMinimum( min - 0.1*(max - min) );
   estimatorHistTrain->SetLineColor( 2 );
   estimatorHistTrain->SetLineWidth( 2 );
   estimatorHistTrain->SetTitle( TString("MLP Convergence test") );
  
   estimatorHistTest->SetLineColor( 4 );
   estimatorHistTest->SetLineWidth( 2 );

   estimatorHistTrain->GetXaxis()->SetTitle( "Epochs" );
   estimatorHistTrain->GetYaxis()->SetTitle( "Estimator" );
   estimatorHistTrain->GetXaxis()->SetTitleOffset( 1.20 );
   estimatorHistTrain->GetYaxis()->SetTitleOffset( 1.65 );

   estimatorHistTrain->Draw();
   estimatorHistTest ->Draw("same");

   // need a legend
   TLegend *legend= new TLegend( 1 - gPad->GetRightMargin() - 0.45, 1-gPad->GetTopMargin() - 0.20, 
                                 1 - gPad->GetRightMargin() - 0.05, 1-gPad->GetTopMargin() - 0.05 );

   legend->AddEntry(estimatorHistTrain,"Training Sample","l");
   legend->AddEntry(estimatorHistTest,"Test sample","l");
   legend->Draw("same");
   legend->SetBorderSize(1);
   legend->SetMargin( 0.3 );

   c->cd();
   TMVAGlob::plot_logo(); // don't understand why this doesn't work ... :-(
   c->Update();

   TString fname = "plots/annconvergencetest";
   TMVAGlob::imgconv( c, fname );
}
