{
  TH1F * prova = new TH1F ("prova","prova",5,0,5) ;
  prova->Fill (0,4) ;
  prova->Fill (1,2) ;
  prova->Fill (2,3) ;
  prova->Fill (3,6) ;
  prova->Fill (4,1) ;

  prova->GetXaxis ()->SetBinLabel (1, "zero") ; 
  prova->GetXaxis ()->SetBinLabel (2, "uno") ; 
  prova->GetXaxis ()->SetBinLabel (3, "due") ; 
  prova->GetXaxis ()->SetBinLabel (4, "tre") ; 
  prova->GetXaxis ()->SetBinLabel (5, "quattro") ; 

  prova->Draw () ;
}