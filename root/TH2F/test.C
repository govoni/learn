Double_t g2(Double_t *x, Double_t *par) {
   Double_t r1 = Double_t((x[0]-par[1])/par[2]);
   Double_t r2 = Double_t((x[1]-par[3])/par[4]);
   return par[0]*TMath::Exp(-0.5*(r1*r1+r2*r2));
}  
Double_t fun2(Double_t *x, Double_t *par) {
   Double_t *p1 = &par[0];
   Double_t result = g2(x,p1);
   return result;
}

int test ()
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1, 0);
  const Int_t npar = 5;
  Double_t f2params[npar] = {100, 0.5, 0.25, 0.5, 0.5};
  TF2 *f2 = new TF2("f2", fun2, 0, 1, 0, 1, npar);
  f2->SetParameters(f2params);
  
  TH2F * h2  = new TH2F ("h2","h2",10, 0, 1, 10, 0, 1) ;
  h2->SetStats (0) ;
  h2->FillRandom ("f2", 100000) ;
  h2->Scale (10. / h2->GetMaximum ()) ; 
  h2->Draw ("cont") ;
 
//  TCanvas* cc2Cont = new TCanvas ("cc2Cont","cc2Cont",600,600);
 // ÊDouble_t levels[100] = {0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0};
//  h2->Draw ("col z") ;
//  Double_t levels[100] = {2.3, 6.0, 7, 8, 9, 10};
//  h2 -> SetContour (2,levels);
//  h2 -> Draw ("cont");
//  gPad->Update();
  
  return 0 ; 
}  