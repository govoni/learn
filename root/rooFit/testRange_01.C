{
  using namespace RooFit ;

  RooRealVar x ("x", "x", -10, 10) ;   
  RooRealVar meanx ("meanx", "mean of gaussian x", 5., -10., 10.) ; 
  RooRealVar sigmax ("sigmax", "width of gaussian x", 2, 0., 5.) ; 
  RooGaussian gaussx ("gaussx", "gaussian PDF", x, meanx, sigmax) ; 
  RooDataSet * data = gaussx.generate (RooArgSet (x), 10000) ;
  RooDataHist bdata ("bdata","bdata",RooArgList (x), *data) ;
  
  x.setRange ("peak", 3., 7.) ;
//  gaussx.fitTo (bdata, RooFit::Minos (1), RooFit::SumW2Error (kTRUE), Range ("peak"), PrintLevel (-1)) ;
  x.setRange ("side", 0., 3.) ;
  gaussx.fitTo (bdata, RooFit::Minos (1), RooFit::SumW2Error (kTRUE), Range ("side"), PrintLevel (-1)) ;
 
  RooPlot *xplot = x.frame () ;
  bdata.plotOn (xplot, MarkerColor (kRed)) ;
  gaussx.plotOn (xplot) ;
  TCanvas c1 ;
  xplot->Draw () ;
}


/* pezzi sparsi raccolti in giro

RooPlot* frame2 = x.frame(-2,2) ;
data->plotOn(frame1) ; 
pdf->plotOn(frame1) ; 
frame2->Draw() ;


x.setRange(“sb_lo”,-6,-3) ; 
x.setRange(“sb_hi”, 3, 6) ;
RooPlot* xframe2 = x.frame() ; 
data->plotOn(xframe2,CutRange(“sb_lo,sb_hi”)) ; 
model.plotOn(xframe2,ProjectionRange(“sb_lo,sb_hi”)) ;

*/