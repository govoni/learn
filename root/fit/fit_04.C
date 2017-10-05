/*
- l'incertezza sui parametri del fit non dipende da quella sui punti? 
  se e' uniforme probabilmente no, provo a stringere molto, ma significa fondamentalmente che ogni punto ha lo stesso peso
- maggiore l'incertezza sui punti, minore la dimensione della banda di confidenza, come mai?
*/


int fit_04 ()
{
  
  // initialise the reference histograms
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  
  int n_sig  = 50000 ;
  int n_bkg1 = 150000 ;
  int n_bkg2 = 60000 ;
  
  TH1F * h_sig  = new TH1F ("h_sig",  "", 50, 0., 10.) ;
  h_sig->SetMarkerStyle (24) ;
  h_sig->SetMarkerColor (kBlue) ;
  h_sig->SetLineColor   (kBlue) ;
  h_sig->SetFillColor   (kBlue) ;
  h_sig->SetFillStyle (3004) ;

//  TF1 * firstFunc  = new TF1 ("firstFunc",  "[0] + [1] * x", -5, 5) ;
//  firstFunc->SetLineColor (kBlue) ;
//  firstFunc->SetLineWidth (1) ;
//  firstFunc->SetLineStyle (1) ;

  TH1F * h_bkg1 = new TH1F ("h_bkg1", "", 50, 0., 10.) ;
  h_bkg1->SetMarkerStyle (29) ;
  h_bkg1->SetMarkerColor (kRed) ;
  h_bkg1->SetLineColor   (kRed) ;
  h_bkg1->SetFillColor   (kRed) ;
  h_bkg1->SetFillStyle (3005) ;
  
  TH1F * h_bkg2 = new TH1F ("h_bkg2", "", 50, 0., 10.) ;
  h_bkg2->SetMarkerStyle (28) ;
  h_bkg2->SetMarkerColor (kGreen+2) ;
  h_bkg2->SetLineColor   (kGreen+2) ;
  h_bkg2->SetFillColor   (kGreen+2) ;
  h_bkg2->SetFillStyle (3017) ;
  
//  TF1 * secondFunc = new TF1 ("secondFunc", "[0] + [1] * x", -5, 5) ;
//  secondFunc->SetLineColor (kRed) ;
//  secondFunc->SetLineWidth (2) ;
//  secondFunc->SetLineStyle (2) ;
  
  TRandom3 r ;
  for (int i = 0 ; i < n_sig ; ++i)
    {
      double point = r.Gaus (4., 1.) ;
      h_sig->Fill (point) ;    
    }

  for (int i = 0 ; i < n_bkg1 ; ++i)
    {
      double point = r.Exp (1.) ;
      h_bkg1->Fill (point) ;
    }
  
  for (int i = 0 ; i < n_bkg2 ; ++i)
    {
      double point = r.Uniform (10.) ;
      h_bkg2->Fill (point) ;
    }
  
  TCanvas * c1 = new TCanvas () ;
  double max = h_bkg1->GetMaximum () ;
  if (h_sig->GetMaximum () > max) max = h_sig->GetMaximum () ;
  if (h_bkg2->GetMaximum () > max) max = h_bkg2->GetMaximum () ;
  max *= 1.1 ;
  c1->DrawFrame (0, 0, 10, max) ;
  h_bkg1->Draw ("same") ;
  h_bkg2->Draw ("same") ;
  h_sig->Draw ("Psame") ;


   

}