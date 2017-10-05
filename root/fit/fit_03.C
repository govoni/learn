/*
- l'incertezza sui parametri del fit non dipende da quella sui punti? 
  se e' uniforme probabilmente no, provo a stringere molto, ma significa fondamentalmente che ogni punto ha lo stesso peso
- maggiore l'incertezza sui punti, minore la dimensione della banda di confidenza, come mai?
*/


int fit_03 ()
{
  
  int nBins = 50 ;
  
  TH1F * firstHisto  = new TH1F ("firstHisto",  "", nBins, -5, 5) ;
  firstHisto->SetMarkerStyle (24) ;
  firstHisto->SetMarkerColor (kBlue) ;
  firstHisto->SetLineColor   (kBlue) ;
  firstHisto->SetFillColor   (kBlue) ;
  firstHisto->SetFillStyle (3004) ;

  TF1 * firstFunc  = new TF1 ("firstFunc",  "[0]", -5, 5) ;
  firstFunc->SetLineColor (kBlue) ;
  firstFunc->SetLineWidth (1) ;
  firstFunc->SetLineStyle (1) ;

  TH1F * secondHisto = new TH1F ("secondHisto", "", nBins, -5, 5) ;
  secondHisto->SetMarkerStyle (29) ;
  secondHisto->SetMarkerColor (kRed) ;
  secondHisto->SetLineColor   (kRed) ;
  secondHisto->SetFillColor   (kRed) ;
  secondHisto->SetFillStyle (3005) ;
  
  TF1 * secondFunc = new TF1 ("secondFunc", "[0]", -5, 5) ;
  secondFunc->SetLineColor (kRed) ;
  secondFunc->SetLineWidth (2) ;
  secondFunc->SetLineStyle (2) ;
  
  TRandom3 r ;
  for (int i = 1 ; i <= nBins ; ++i)
    {
      double point = r.Gaus (4., 1.) ;
      firstHisto->SetBinContent (i, point) ;
      point = r.Gaus (4., 4.) ;
      secondHisto->SetBinContent (i, point) ;
      double sigma = r.Gaus (0., 1.) ;
      firstHisto->SetBinError (i, sigma) ;
      sigma = r.Gaus (0., 4.) ;
      secondHisto->SetBinError (i, sigma) ;    
    }
  
  firstHisto->Fit ("firstFunc", "+") ;  
  TH1F * first_fit_error = new TH1F ("first_fit_error", "", 100, -5, 5) ;
  (TVirtualFitter::GetFitter ())->GetConfidenceIntervals (first_fit_error, 0.68) ;
  first_fit_error->SetFillColor (kBlue) ;
  first_fit_error->SetFillStyle (3004) ;

  secondHisto->Fit ("secondFunc", "+") ;  
  TH1F * second_fit_error = new TH1F ("second_fit_error", "", 100, -5, 5) ;
  (TVirtualFitter::GetFitter ())->GetConfidenceIntervals (second_fit_error, 0.68) ;
  second_fit_error->SetFillColor (kRed) ;
  second_fit_error->SetFillStyle (3005) ;

  TCanvas * c1 = new TCanvas () ;
  c1->DrawFrame (-5, -20, 5, 15) ;
  first_fit_error->Draw ("E3same") ;
  second_fit_error->Draw ("E3same") ;
  secondHisto->Draw ("E1Psames") ;
  firstHisto->Draw ("E1Psames") ;


   

}