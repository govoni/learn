{
  using namespace RooFit ;

  RooRealVar x ("x", "x", -10, 10) ;   
  RooRealVar meanx ("meanx", "mean of gaussian x", 5., -10., 10.) ; 
  RooRealVar sigmax ("sigmax", "width of gaussian x", 2, 0., 5.) ; 
  RooGaussian gaussx ("gaussx", "gaussian PDF", x, meanx, sigmax) ; 
  RooDataSet * data = gaussx.generate (RooArgSet (x), 10000) ;
  RooDataHist bdata ("bdata","bdata",RooArgList (x), *data) ;
  
  gaussx.fitTo (bdata, RooFit::Minos (1), RooFit::SumW2Error (kTRUE), Range ("smaller"), PrintLevel (-1)) ;
  char rangeName[20] ;
  for (int i = 0 ; i < 4 ; ++i)
    {
      sprintf (rangeName, "R_%d", i) ;
      x.setRange (rangeName, -5., 5.) ;
      gaussx.fitTo (bdata, RooFit::Minos (1), RooFit::SumW2Error (kTRUE), Range (rangeName), PrintLevel (-1)) ;
    }

  RooPlot *xplot = x.frame () ;
  bdata.plotOn (xplot, MarkerColor (kRed)) ;
  gaussx.plotOn (xplot) ;
  TCanvas c1 ;
  xplot->Draw () ;
}
