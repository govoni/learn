
void TGraph_example()
{
/*
gROOT->Reset();
Double_t x[5];	
Double_t y[5];
for (Int_t i=0; i<5; i++) {x[i]=0.1*i;y[i]=10*sin(x[i]+0.2);}
TGraph *gr = new TGraph (5,x,y);
//gr->GetXaxis()->SetTitle("asse x");
gr->GetXaxis()->SetRangeUser(0.2,0.4);
gr->Draw("AC*");
*/

// from Fabrizio Pierella <Fabrizio.Pierella@cern.ch>

  gROOT->Reset();
  c1 = new TCanvas("c1","Pietro Graph",200,10,700,500);

  Double_t x[5];
  Double_t y[5];
  for (Int_t i=0; i<5; i++) {x[i]=0.1*i;y[i]=10*sin(x[i]+0.2);}
  TGraph *gr = new TGraph (5,x,y);
  gr->Draw("AC*");
  c1->Update();
  gr->GetXaxis()->SetRangeUser(0.2,0.4);
  c1->Modified();


}
