#include "TFile.h"
#include "TTree.h"
//#include "TBrowser.h"

//
// to run the example, do:
// .x prova.cpp   to execute with the CINT interpreter
//

class tento{

 int _uno;

public:

 tento(int uno = 0): _uno(uno) {};
 int getUno() {return _uno;};
 int setUno(int uno) {_uno = uno;};
 
};

void prova_w()
{
   //create a Tree file treeProva.root
   
   //create the file, the Tree and a few branches with 
   //a subset of gctrak
   TFile f("treeProva.root","recreate");
   TTree tempo("tempo","tentativo");

   tento oggetto;

   tempo.Branch("oggetto",&oggetto,"oggetto di prova");
   
   for (int i=0; i<10; ++i) 
    {
      oggetto.setUno(i);
      tempo.Fill();     
    }
   tempo.Write();
}

void prova_r()
{
   //note that we use "new" to create the TFile and TTree objects !
   //because we want to keep these objects alive when we leave 
   //this function.
   TFile f("treeProva.root");
   TTree *tempo = (TTree*)f->Get("tempo");  // si puo' fare in altro modo??

//   static Float_t destep;
// perche' static?

   tento oggetto;
   TBranch *b_oggetto = tempo->GetBranch("oggetto");
   b_oggetto->SetAddress(&oggetto);
   
      //read only the destep branch for all entries
   Int_t nentries = (Int_t)tempo->GetEntries();


   for (Int_t i=0; i<nentries; ++i) 
     {
	b_oggetto->GetEntry(i); 
        cout << oggetto.getUno() << endl;
     }
  
   }   

void prova() {
   prova_w();
   prova_r();
}
