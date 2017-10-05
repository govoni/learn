int macro ()
{
  TH2F h1 ("h1", "h1", 10, 0, 10, 10, 0, 10) ;
  
  TF2 *f2 = new TF2 ("f2","xygaus",-4,4,0,8);
  Double_t params2[] = {130, -1.4, 1.8, 1.5, 1} ;
  f2->SetParameters(params2);
  TH2F * h2 = new TH2F ("h2","h2",20,-4,4,20,0,8);
  h2->SetFillColor(4);
  h2->FillRandom("f2",40000);

  TProfile * p2 = h2->ProfileX () ;
  p2->SetLineColor (4) ;
  p2->SetFillColor (0) ;

  TF2 *f3 = new TF2 ("f3","xygaus",-4,4,0,8);
  Double_t params3[] = {150, 2, 0.5, 2, 0.5} ;
  f3->SetParameters(params3);
  TH2F * h3 = new TH2F ("h3","h3",20,-4,4,20,0,8);
  h3->SetFillColor(8);
  h3->FillRandom("f3",40000);

  TProfile * p3 = h3->ProfileX () ;
  p3->SetLineColor (8) ;
  p3->SetFillColor (0) ;
  
  TCanvas * c1 = new TCanvas () ; 

  THStack * test = new THStack ("test", "test") ;
  test->Add (p3) ;
  test->Add (p2) ;

  test->Draw ("histo") ;

  TCanvas * c2 = new TCanvas () ; 

  p3->Draw ("histo") ;
  p2->Draw ("histosame") ;

  TCanvas * c3 = new TCanvas () ; 
  
  TH1F * t1 = new TH1F ("t1", "t1", 10, 0, 10) ;
  t1->Fill (5) ;
  t1->Fill (6) ;
  t1->SetFillColor (3) ;
  TH1F * t2 = new TH1F ("t2", "t2", 10, 0, 10) ;
  t2->Fill (5) ;
  t2->SetFillColor (6) ;
  
  THStack * test2 = new THStack ("test2", "test2") ;
  test2->Add (t1) ;
  test2->Add (t2) ;
  
  test2->Draw ("hist") ;
  


}