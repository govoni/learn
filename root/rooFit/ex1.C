void ex1()
{
  RooWorkspace w("w",kTRUE) ;

  w.factory("Gaussian::g(x[-10,10],mean[-10,10],sigma[3,0.1,10])") ;
  RooDataSet* d = w::g.generate(w::x,10000) ;
  // access x and change its binning for the binned generation
  w::x.setBins (1000000) ;
  
  RooDataHist* d_bin = w::g.generateBinned(w::x,10000) ;

  w::g.fitTo(*d) ;
  RooPlot* frame = w::x.frame() ;
  d->plotOn(frame) ;
  w::g.plotOn(frame) ;
  frame->Draw() ;

  w::mean.Print() ;
  w::sigma.Print() ;
}
