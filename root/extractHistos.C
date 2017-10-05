int extractHistos (TString filename)
{
  TFile f (filename) ;
  c1->Draw () ;
  TIter next (c1->GetListOfPrimitives ()) ;
  TObject * obj ;
  while ((obj = next ()))
    {
//      cout << obj->GetName () << endl ;
      if (obj->InheritsFrom ("TH1"))
        {
          cout << obj->GetName () << endl ;
          TString nameFile = "F_" ;
          nameFile += obj->GetName () ;
          nameFile += ".root" ;
          TDirectory * curr = gDirectory ;
          TFile f (nameFile, "recreate") ;
          f.cd () ;
          obj->Write () ;
          f.Close () ;
          curr->cd () ;
        }
    }
}
