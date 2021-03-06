{
gROOT->Reset();
TH1F *h1 = new TH1F("h1","one bin histo",1,0,1);
TH1F *h2 = new TH1F("h2","three bins histo",3,0,1);
TH1F *h3 = new TH1F("h3","ten bins histo",10,0,1);

h1->Fill(0.1); h2->Fill(0.1); h3->Fill(0.1);
h1->Fill(0.2); h2->Fill(0.2); h3->Fill(0.2);
h1->Fill(0.3); h2->Fill(0.3); h3->Fill(0.3);

cout << endl;
cout << "one bin " << h1->GetMean() << endl;
cout << "three bins " << h2->GetMean() << endl;
cout << "ten bins " << h3->GetMean() << endl;
cout << endl;
}
