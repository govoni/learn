  TH1F test ("test", "test", 4, 0, 4) ;
  test.SetBinContent (1, 2.) ;
  test.SetBinContent (2, 2.) ;
  test.SetBinContent (3, 2.) ;
  test.SetBinContent (4, 2.) ;

  test.SetBinError (1, 2.) ;
  test.SetBinError (2, 2.) ;
  test.SetBinError (3, 2.) ;
  test.SetBinError (4, 2.) ;
  	
  TH1F id ("id", "id", 4, 0, 4) ;
  id.SetBinContent (1, 1.) ;
  id.SetBinContent (2, 1.) ;
  id.SetBinContent (3, 1.) ;
  id.SetBinContent (4, 1.) ;
	
  TH1F * ratio = (TH1F *) id.Clone ("ratio") ;
  ratio->Divide (&test) ;

  ratio->Draw () ;
