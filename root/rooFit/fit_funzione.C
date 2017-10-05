// The background function
double background (double* x, double* par)
{
  //parabola
  double val = par[0] * (x[0] - par[1]) * (x[0]- par[1]) + par[2] ;
  return val ;
}



// The signal function
double signal (double* x, double* par)
{
  // gaussiana
  double arg = (x[0] - par[1])/par[2] ;
  double val = par[0] * exp (-0.5*arg*arg) ;
  
  return val ;
}



// Combined background + signal
double sum (double* x, double* par)
{
  double val = background (x, par) + signal (x, &par[3]) ;
  return val ;
}



void fit_funzione ()
{
  // Fill histogram from data in file
  double xMin = 0. ;
  double xMax = 10. ;
  int nBin = 200 ;
  TH1F* histo = new TH1F ("histo","Signal + background data", nBin, xMin, xMax) ;

  std::ifstream inFile ("data_funzione.txt", std::ios::in) ;
  int entry = 0 ;
  while (!inFile.eof ())
  {
    double data ;
    inFile >> data ;
    histo -> Fill (data) ;
    
    ++entry ;
    if (entry%1000000 == 0)
      std::cout << "----- Reading entry " << entry << std::endl ;
  }



  
  // *************
  // Sum Functions
  // *************
  
  // Fit background and signal separately
  double backgroundPar[3] = {-1., 5., 1.} ;
  TF1* backgroundfunc = new TF1 ("backgroundfunc", background, xMin, xMax, 3) ;
  backgroundfunc -> SetParameters (backgroundPar) ;
  backgroundfunc -> SetLineColor (kBlue) ;

  double signalPar[3] = {1., 5., 1.} ;
  TF1* signalfunc = new TF1 ("signalfunc", signal, xMin, xMax, 3) ;
  signalfunc -> SetParameters (signalPar) ;
  signalfunc -> SetLineColor (kRed) ;

  TCanvas* c1 = new TCanvas ("c1", "data") ;
  histo -> Fit ("backgroundfunc", "R", "") ;
  histo -> Fit ("signalfunc", "+", "sames", 4., 6.5) ;
  histo -> DrawCopy () ;


  // Fit data with sum function
  double sumPar[6] ;
  sumPar[0] = backgroundfunc -> GetParameter (0) ;
  sumPar[1] = backgroundfunc -> GetParameter (1) ;
  sumPar[2] = backgroundfunc -> GetParameter (2) ;
  sumPar[3] = signalfunc -> GetParameter (0) ;
  sumPar[4] = signalfunc -> GetParameter (1) ;
  sumPar[5] = signalfunc -> GetParameter (2) ;
  TF1* sumfunc = new TF1 ("sumfunc", sum, xMin, xMax, 6) ;
  sumfunc -> SetParameters (sumPar) ;
  sumfunc -> SetLineColor (kGreen) ;

  TCanvas* c2 = new TCanvas ("c2", "fit funzione") ;  
  histo -> Fit ("sumfunc", "R") ;
  histo -> DrawCopy () ;
  
  
  // Count signal events
  signalfunc -> SetParameter (0, sumfunc -> GetParameter (3)) ;
  signalfunc -> SetParameter (1, sumfunc -> GetParameter (4)) ;
  signalfunc -> SetParameter (2, sumfunc -> GetParameter (5)) ;  
  double nSignal = nBin / (xMax - xMin) * signalfunc -> Integral (xMin, xMax) ;
  std::cout << "\n****** Signal + Background Function ---> " << int(nSignal) << " signal events" << std::endl ;
  
  
  
  // *************
  // Sidebands
  // *************  
  
  TH1F* histo2 = new TH1F ("histo2","Signal + background data", nBin, xMin, xMax) ;
  histo -> Fit ("backgroundfunc", "N", "", 0., 2.) ;
  
  for (int bin = 1 ; bin <= nBin ; ++ bin)
  {
    double x = histo -> GetBinCenter (bin) ;
    double val = backgroundfunc -> Eval (x) ; 
    histo2 -> Fill (x, histo -> GetBinContent (bin) - val) ;  
  }
  
  TCanvas* c3 = new TCanvas ("c3", "fit sidebands") ;  
  histo2 -> Fit ("signalfunc", "", "", 4., 6.5) ;
  histo2 -> Draw () ;
  
  
  // Count signal events  
  nSignal = nBin / (xMax - xMin) * signalfunc -> Integral (xMin, xMax) ;
  std::cout << "\n****** Sidebands ---> " << int(nSignal) << " signal events" << std::endl ;
}
