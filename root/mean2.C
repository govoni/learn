{
gROOT->Reset();
TH1F *h1 = new TH1F("h1","one bin histo",1,0,1);
TH1F *h2 = new TH1F("h2","three bins histo",3,0,1);
TH1F *h3 = new TH1F("h3","ten bins histo",10,0,1);

h1->Fill(0.15); h2->Fill(0.15); h3->Fill(0.15);
h1->Fill(0.25); h2->Fill(0.25); h3->Fill(0.25);
h1->Fill(0.35); h2->Fill(0.35); h3->Fill(0.35);

cout << endl;
cout << "one bin " << h1->GetMean() << endl;
cout << "three bins " << h2->GetMean() << endl;
cout << "ten bins " << h3->GetMean() << endl;
cout << endl;
}
