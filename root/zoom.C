{
  TH1F * pippo = new TH1F ("pippo","pippo",10,0,10)  ;
  pippo->Fill (1) ;
  pippo->Fill (2) ;
  pippo->Fill (2) ;
  pippo->Fill (2) ;
  pippo->Fill (3) ;
  pippo->Fill (3) ;
  
  TCanvas * c1 = new TCanvas () ;
  pippo->DrawClone () ;
  
  TPad * zoom = new TPad ("zoom","zoom",0.45,0.45,0.95,0.95) ;
  zoom->Draw () ;
  zoom->cd () ;
  pippo->GetXaxis ()->SetRangeUser (0,4) ;
  pippo->Draw () ;

}