// The background function
double background (double* x, double* par)
{
 //esponenziale
 double val = par[0] * exp( -(x[0] - par[1]) / par[2] ) ;
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



void fit_sidebands ()
{
 // Fill histogram from data in file
 double xMin = 0. ;
 double xMax = 200. ;
 int nBin = 200 ;
 TH1F* histo = new TH1F ("histo","Signal + background data", nBin, xMin, xMax) ;

 std::ifstream inFile ("data_sidebands.txt", std::ios::in) ;
 while (!inFile.eof ())
 {
   double data ;
   inFile >> data ;
   histo -> Fill (data) ;
 }




 // *************
 // Sidebands
 // *************  
 
 // Fit background 
 double backgroundPar[3] = {1., 0., 100.} ;
 TF1* backgroundfunc = new TF1 ("backgroundfunc", background, xMin, xMax, 3) ;
 backgroundfunc -> SetParameters (backgroundPar) ;
 backgroundfunc -> FixParameter (1, 0.) ;
 backgroundfunc -> SetLineColor (kBlue) ;
 
  
 TCanvas* c1 = new TCanvas ("c1", "data") ;
 histo -> Fit ("backgroundfunc", "", "", 0., 120.) ;
 histo -> DrawCopy () ;



 TH1F* histo2 = new TH1F ("histo2","Signal + background data", nBin, xMin, xMax) ;
 for (int bin = 1 ; bin <= nBin ; ++ bin)
 {
   double x = histo -> GetBinCenter (bin) ;
   double val = backgroundfunc -> Eval (x) ; 
   histo2 -> Fill (x, histo -> GetBinContent (bin) - val) ;  
 }


 double signalPar[3] = {1., 135., 1.} ;
 TF1* signalfunc = new TF1 ("signalfunc", signal, xMin, xMax, 3) ;
 signalfunc -> SetParameters (signalPar) ;
 signalfunc -> SetLineColor (kRed) ;

 double signalPar_2[3] = {1., 170., 1.} ;
 TF1* signalfunc_2 = new TF1 ("signalfunc_2", signal, xMin, xMax, 3) ;
 signalfunc_2 -> SetParameters (signalPar_2) ;
 signalfunc_2 -> SetLineColor (kRed) ;
 
 
 TCanvas* c3 = new TCanvas ("c3", "fit sidebands") ;  
 histo2 -> Fit ("signalfunc", "", "", 120., 155.) ;
 histo2 -> Fit ("signalfunc_2", "+", "", 150., 190.) ;
 histo2 -> Draw () ;


 // Count signal events  
 double nSignal = nBin / (xMax - xMin) * signalfunc -> Integral (xMin, xMax) ;
 std::cout << "\n****** Sidebands ---> " << int(nSignal) << " signal events" << std::endl ; 
 double nSignal_2 = nBin / (xMax - xMin) * signalfunc_2 -> Integral (xMin, xMax) ;
 std::cout << "\n****** Sidebands ---> " << int(nSignal_2) << " signal_2 events" << std::endl ; 
}