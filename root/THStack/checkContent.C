void checkContent() {

   
   THStack *hs = new THStack("hs","test stacked histograms");
   //create three 1-d histograms
   TH1F *h1 = new TH1F("h1","test hstack",100,-4,4);
   h1->Sumw2 () ;
   h1->FillRandom("gaus",20000);
   h1->SetFillColor(kRed);
   h1->SetMarkerStyle(27);
   h1->SetMarkerColor(kRed);
   hs->Add(h1);
   TH1F *h2 = new TH1F("h2","test hstack",100,-4,4);
   h2->Sumw2 () ;
   h2->FillRandom("gaus",15000);
   h2->SetFillColor(kBlue);
   h2->SetMarkerStyle(27);
   h2->SetMarkerColor(kBlue);
   hs->Add(h2);
//   TH1F *h3 = new TH1F("h3","test hstack",100,-4,4);
//   h3->Sumw2 () ;
//   h3->FillRandom("gaus",10000);
//   h3->SetFillColor(kGreen);
//   h3->SetMarkerStyle(27);
//   h3->SetMarkerColor(kGreen);
//   hs->Add(h3);
   
   TCanvas *c1 = new TCanvas("c1","stacked hists",10,10,1000,800);
   c1->Divide(2,2);
   // in top left pad, draw the stack with defaults
   c1->cd(1);
   hs->Draw();
   // in top right pad, draw the stack in non-stack mode and errors option
   c1->cd(2);
   TH1F * h_stack = (TH1F *) hs->GetStack ()->Last () ;
   h_stack->Draw ("e1p") ;
   //in bottom left, draw the last histogram of GetStack
   c1->cd(3);
   h1->Draw ("e1p") ;
   //in bottom right, draw the last histogram of GetHists
   c1->cd(4);
   h2->Draw ("e1p") ;
   
   float e1 = h1->GetBinError (50) ;
   float e2 = h2->GetBinError (50) ;
   cout << e1 << " "
        << e2 << " "
        << h_stack->GetBinError (50) << " "
        << sqrt (e1 * e1 + e2 * e2) << endl ;
   
   
   
}
