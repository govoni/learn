using namespace RooFit ;

void ex4()
{
  TFile* f = TFile::Open("model.root") ;
  RooWorkspace* w = f->Get("w") ;
  w->exportToCint() ;

  RooAbsReal* nll = w::model.createNLL(w::data) ;

  RooMinuit m(*nll) ;
  m.migrad() ;
  m.hesse() ;

  m.minos(w::Nsig) ;
  w::Nsig.Print() ;

  w::Nsig.setConstant(kTRUE) ;
  m.minos(w::Nsig) ;
  w::Nsig.Print() ;

  RooPlot* frame = w::Nsig.frame(0,1000) ;
  nll->plotOn(frame,ShiftToZero()) ;
  frame->SetMinimum(0) ;
  frame->Draw() ;
}
