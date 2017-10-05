// https://root.cern.ch/root/html/tutorials/math/mathGammaNormal.C.html

Double_t DelphesLogNormal(Double_t mean, Double_t sigma)
{
  Double_t a, b;

  if(mean > 0.0)
  {
    b = TMath::Sqrt(TMath::Log((1.0 + (sigma*sigma)/(mean*mean))));
    a = TMath::Log(mean) - 0.5*b*b;

    return TMath::Exp(a + b*gRandom->Gaus(0, 1));
  }
  else
  {
    return 0.0;
  }
}

// ---- ---- ---- ---- 

int LogNormal () 
{

  float mean = 50. ;
  float sigma = 1400.4 ;
  // generation
  TH1F * gen = new TH1F ("gen", "gen", 100, 0., 2.) ;
  TH1F * gen2 = new TH1F ("gen2", "gen", 100, 0., 300.) ;
  
  for (int i = 0 ; i < 10000 ; ++i)
    { 
      float val = DelphesLogNormal (mean, sigma) ;
      gen->Fill (val / mean) ;
      gen2->Fill (val) ;
    }  
  gen->DrawNormalized () ;
//  gen2->DrawNormalized () ;
  
  return 0. ; 
  
  TF1 * f_ln = new TF1 ("f_ln", "TMath::LogNormal(x,[0],[1],[2])", 0., 2.) ;
  f_ln->SetParameter (0, sigma) ; // sigma
  f_ln->SetParameter (1, mean) ; // theta = mean
  f_ln->SetParameter (2, 5) ;  // m = scale factor that zooms the shifted x coordinate

//  f_ln->Draw ("same") ;
  f_ln->Draw () ;
  
  return 0 ;

  TCanvas * c1 = new TCanvas ;
  c1->DrawFrame (0., 0., 2., 4.) ;
  for (float sigma = 10 ; sigma < 50 ; sigma += 4.0)
    {
      f_ln->SetParameter (0, sigma) ; // sigma
      f_ln->SetParameter (1, 0) ; // theta = mean
      f_ln->SetParameter (2, 1.0) ;  // m = scale factor that zooms the shifted x coordinate
      f_ln->Draw ("same") ;
      c1->Update () ;
      gSystem->Sleep (500) ;
    }

}