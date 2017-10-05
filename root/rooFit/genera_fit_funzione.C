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



void genera_fit_funzione ()
{
  // Outfile
  std::ofstream outFile ("data_funzione.txt", std::ios::out) ; 


  // Parameters for parabola
  double backgroundPar[3] = {-0.1, 5., 25.} ;
  TF1* backgroundFunz = new TF1 ("backgroundFunz", background, 0., 10., 3) ;
  backgroundFunz -> SetParameters (backgroundPar) ;
  
  // Generate background events
  int nBackground = 1 * 900000 ;
  for (int i = 0 ; i < nBackground ; ++i)
  {
    if (i%100000 == 0)
      std::cout << i << std::endl ;

    double r = backgroundFunz -> GetRandom () ;
    outFile << r << std::endl ;
  }



  // Parameters for gaussian
  double signalPar[3] = {10., 5.25, 1.0} ;
  TF1* signalFunz = new TF1 ("signalFunz", signal, 0., 10., 3) ;
  signalFunz -> SetParameters (signalPar) ;
  
  // Generate signal events
  int nSignal = 1 * 100000 ;
  for (int i = 0 ; i < nSignal ; ++i)
  {
    if (i%100000 == 0)
      std::cout << i << std::endl ;

    double r = signalFunz -> GetRandom () ;
    outFile << r << std::endl ;
  }
  
}
