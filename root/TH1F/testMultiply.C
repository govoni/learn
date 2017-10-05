int testMultiply ()
{
  TH1F * num = new TH1F ("num", "num", 10, 0, 10) ;
  for (int i = 0 ; i < 10 ; ++i) 
    for (int j = 0 ; j < 100 ; ++j) num->Fill (i) ;

  TH1F * den = new TH1F ("den", "den", 10, 0, 10) ;
  for (int i = 0 ; i < 10 ; ++i) 
    for (int j = 0 ; j < 64 ; ++j) den->Fill (i) ;

  TCanvas * c1 = new TCanvas () ;
  num->Draw ("E") ;

  TCanvas * c2 = new TCanvas () ;
  den->Draw ("E") ;

  TH1F * ratio = (TH1F *) num->Clone ("ratio") ;
  ratio->Sumw2 () ; //PG se non chiamo questo,
                    //PG l'errore del ratio e' ricalcolato poissoniano
                    //PG se lo faccio, viene propagato ok
  ratio->Divide (den) ;
  
  TCanvas * c3 = new TCanvas () ;
  ratio->Draw ("E") ;

  cout << num->GetBinError (3) / num->GetBinContent (3) << endl ;
  cout << den->GetBinError (3) / den->GetBinContent (3) << endl ;
  cout << ratio->GetBinError (3) / ratio->GetBinContent (3) << endl ;

  double e_num = num->GetBinError (3) / num->GetBinContent (3) ;
  double e_den = den->GetBinError (3) / den->GetBinContent (3) ;
  double e_rat = ratio->GetBinError (3) / ratio->GetBinContent (3) ;

  cout << sqrt (ratio->GetBinContent (3)) << " " << ratio->GetBinError (3) << endl ;
  cout << sqrt (e_num * e_num + e_den * e_den) << endl ;
  
  return 0 ;
}