
int uncertainties  ()
{
    
  cout << "\n sumw2 on from the beginning, no weights in filling, scaling\n" << endl ;
    
  TH1F *h1 = new TH1F ("h1", "test H1", 1, -2, 2) ; 
  h1->Sumw2 () ;
  TH1F *h2 = new TH1F ("h2", "test H1", 1, -2, 2) ; 
  h2->Sumw2 () ;
  TH1F *h3 = new TH1F ("h3", "test H1", 1, -2, 2) ; 
  TH1F *h4 = new TH1F ("h4", "test H1", 1, -2, 2) ; 
  h4->Sumw2 () ;
  float x ;
  for (int i = 0 ; i < 500 ; ++i) 
    {
      x = gRandom->Gaus (0, 1) ;
      h1->Fill (x) ;
      h2->Fill (x, 0.5) ;      
      h3->Fill (x) ;
      h4->Fill (x, 0.5) ;      
    }
  cout << "h1 " << h1->GetBinContent (1) << " " << h1->GetBinError (1) << " " << sqrt (h1->GetBinContent (1)) << endl ;
  h1->Scale (2.) ;
  cout << "h1 " << h1->GetBinContent (1) << " " << h1->GetBinError (1) << " " << sqrt (h1->GetBinContent (1)) << endl ;

  cout << "\n sumw2 on from the beginning, yes weights in filling, scaling\n" << endl ;

  cout << "h2 " << h2->GetBinContent (1) << " " << h2->GetBinError (1) << " " << sqrt (h2->GetBinContent (1)) << endl ;
  h2->Scale (2.) ;
  cout << "h2 " << h2->GetBinContent (1) << " " << h2->GetBinError (1) << " " << sqrt (h2->GetBinContent (1)) << endl ;

  cout << "\n no sumw2 on from the beginning, no weights in filling, scaling\n" << endl ;
    
  cout << "h3 " << h3->GetBinContent (1) << " " << h3->GetBinError (1) << " " << sqrt (h3->GetBinContent (1)) << endl ;
  h3->Scale (2.) ;
  cout << "h3 " << h3->GetBinContent (1) << " " << h3->GetBinError (1) << " " << sqrt (h3->GetBinContent (1)) << endl ;

  cout << "\n no sumw2 on from the beginning, yes weights in filling, scaling\n" << endl ;

  cout << "h4 " << h4->GetBinContent (1) << " " << h4->GetBinError (1) << " " << sqrt (h4->GetBinContent (1)) << endl ;
  h4->Scale (2.) ;
  cout << "h4 " << h4->GetBinContent (1) << " " << h4->GetBinError (1) << " " << sqrt (h4->GetBinContent (1)) << endl ;

  h4->SetBinErrorOption (TH1::kPoisson) ;
  cout << "h4 " << h4->GetBinContent (1) << " " << h4->GetBinError (1) << " " << sqrt (h4->GetBinContent (1)) << endl ;




}