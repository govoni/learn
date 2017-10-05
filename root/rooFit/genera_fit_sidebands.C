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



void genera_fit_sidebands ()
{
 // Outfile
 std::ofstream outFile ("data_sidebands.txt", std::ios::out) ; 


 // Parameters for exponential
 double backgroundPar[3] = {1, 0., 90.} ;
 TF1* backgroundFunz = new TF1 ("backgroundFunz", background, 0., 200., 3) ;
 backgroundFunz -> SetParameters (backgroundPar) ;

 // Generate background events
 int nBackground = 98000 ;
 for (int i = 0 ; i < nBackground ; ++i)
 {
   if (i%10000 == 0)
     std::cout << i << std::endl ;

   double r = backgroundFunz -> GetRandom () ;
   outFile << r << std::endl ;
 }



 // Parameters for gaussian
 double signalPar[3] = {10., 135., 2.5} ;
 TF1* signalFunz = new TF1 ("signalFunz", signal, 0., 200., 3) ;
 signalFunz -> SetParameters (signalPar) ;

 // Generate signal events
 int nSignal = 2000 ;
 for (int i = 0 ; i < nSignal ; ++i)
 {
   if (i%1000 == 0)
     std::cout << i << std::endl ;

   double r = signalFunz -> GetRandom () ;
   outFile << r << std::endl ;
 }


 // Second peak
 double signalPar_2[3] = {10., 170., 3.} ;
 signalFunz -> SetParameters (signalPar_2) ;

 // Generate signal events
 nSignal = 1500 ;
 for (int i = 0 ; i < nSignal ; ++i)
 {
   if (i%1000 == 0)
     std::cout << i << std::endl ;

   double r = signalFunz -> GetRandom () ;
   outFile << r << std::endl ;
 }

 outFile.close();

}