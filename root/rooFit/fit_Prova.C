// The background function
double background (double* x, double* par)
{
  //example
  double val = 0.;
  for(int i = 0; i < 20; ++i)
    val += par[i] ;
  
  return val ;
}



void fit_Prova ()
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
  double backgroundPar[20] = {1., 2., 3., 4., 5., 6., 7., 8., 9., 10,
                              11., 12., 13., 14., 15., 16., 17., 18., 19., 20} ;
  TF1* backgroundfunc = new TF1 ("backgroundfunc", background, xMin, xMax, 20) ;
  backgroundfunc -> SetParameters (backgroundPar) ;
  backgroundfunc -> SetLineColor (kBlue) ;
  backgroundfunc -> Draw();
}
