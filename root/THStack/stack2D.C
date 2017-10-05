{
  TH2F * uno = new TH2F ("uno", "uno", 10,0,10,10,0,10) ;	
  TH2F * due = new TH2F ("due", "due", 10,0,10,10,0,10) ;	
	
  uno->Fill (2,3) ;	
  due->Fill (4,6) ;	
	
  THStack * tot = new THStack () ;
  tot->Add (uno) ;
  tot->Add (due) ;
	
  tot->Draw ("LEGO2") ;	
	
  TH2F * totH = (TH2F *) tot->GetStack ()->Last () ;
  totH->SetTitle ("tot") ;
  totH->Draw ("LEGO2") ;	
  
	
	
}