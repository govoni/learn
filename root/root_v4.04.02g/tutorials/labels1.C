// Setting alphanumeric labels in a 1-d histogram
// author; Rene Brun
void labels1()
{
   const Int_t nx = 20;
   char *people[nx] = {"Jean","Pierre","Marie","Odile","Sebastien","Fons","Rene",
      "Nicolas","Xavier","Greg","Bjarne","Anton","Otto","Eddy","Peter","Pasha",
      "Philippe","Suzanne","Jeff","Valery"};
   TCanvas *c1 = new TCanvas("c1","demo bin labels",10,10,900,500);
   c1->SetGrid();
   c1->SetBottomMargin(0.15);
   TH1F *h = new TH1F("h","test",nx,0,nx);
   h->SetFillColor(38);
   for (Int_t i=0;i<5000;i++) {
      h->Fill(gRandom->Gaus(0.5*nx,0.2*nx));
   }
   h->SetStats(0);
   for (i=1;i<=nx;i++) {
      h->GetXaxis()->SetBinLabel(i,people[i-1]);
   }
   h->Draw();
   TPaveText *pt = new TPaveText(0.6,0.7,0.98,0.98,"brNDC");
   pt->SetFillColor(18);
   pt->SetTextAlign(12);
   pt->AddText("Use the axis Context Menu LabelsOption");
   pt->AddText(" \"a\"   to sort by alphabetic order");
   pt->AddText(" \">\"   to sort by decreasing vakues");
   pt->AddText(" \"<\"   to sort by increasing vakues");
   pt->Draw();
}
