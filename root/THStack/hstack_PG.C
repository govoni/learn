void hstack_PG() {
// Example of stacked histograms: class THStack
//
//  Author: Rene Brun

  //PG FILLING
  //PG -------
   
   THStack *hs = new THStack("hs","test stacked histograms");
   //create three 1-d histograms
   TH1F *h1 = new TH1F("h1","test hstack",100,-4,4);
   h1->FillRandom("gaus",20000);
   h1->SetFillColor(kRed);
   h1->SetFillStyle(3001);
   h1->SetMarkerStyle(21);
   h1->SetMarkerColor(kRed);
   hs->Add(h1);
   TH1F *h2 = new TH1F("h2","test hstack",100,-4,4);
   h2->FillRandom("gaus",15000);
   h2->SetFillColor(kBlue);
   h2->SetFillStyle(3011);
   h2->SetMarkerStyle(21);
   h2->SetMarkerColor(kBlue);
   hs->Add(h2);
   TH1F *h3 = new TH1F("h3","test hstack",100,-4,4);
   h3->FillRandom("gaus",10000);
   h3->SetFillColor(kGreen);
   h3->SetFillStyle(3021);
   h3->SetMarkerStyle(21);
   h3->SetMarkerColor(kGreen);
   hs->Add(h3);

  //PG PLOTTING
  //PG --------
   
   TCanvas *c1 = new TCanvas("c1","stacked hists",10,10,1000,800);
   c1->Divide(2,2);

   // in top left pad, draw the stack with defaults
   c1->cd(1);
   hs->Draw();

   // in top right pad, draw the stack in non-stack mode and errors option
   c1->cd(2);
   gPad->SetGrid();
//   hs->Draw("nostack,e1p");
   hs->Draw("nostack");

   //in bottom left, use the list of histograms directly
   c1->cd(3);
   TObjArray * histos = hs->GetStack () ;
//   gPad->SetTheta(3.77);
//   gPad->SetPhi(2.9);
//   hs->Draw("lego1");

   Int_t number = histos->GetEntries();
 
   TH1F * last = (TH1F*) histos->At (number-1) ;
   last->Draw () ;
   for (int i = number-2 ; i >=0 ; --i) 
     {
       TH1F * histo = (TH1F*) histos->At (i) ;
       Style_t origStyle = std::cout << histo->GetFillStyle () << std::endl ;
       Color_t origColor = std::cout << histo->GetFillColor () << std::endl ;
       TH1F * dummy = histo->Clone () ;
       dummy->SetFillStyle (1001) ; 
       dummy->SetFillColor (10) ;        
       dummy->Draw ("same") ;
//       histo->SetFillStyle (origStyle) ; 
//       histo->SetFillColor (origColor) ;        
       histo->Draw ("same") ;
     }


}
