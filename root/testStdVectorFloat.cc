// c++ -o crea.exe testStdVectorFloat.cc `root-config --glibs --cflags`


#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include <vector>

// #ifdef __MAKECINT__
// #pragma link C++ class vector<float>+;
// #endif

int main(){
// int testStdVectorFloat() {

TFile out("out.root","RECREATE");
TTree* tree = new TTree("tree","tree");
std::vector<double> var; // ---> così funziona!
//std::vector<float> var; // ---> così si blocca in esecuzione! Se compilato! Se interpretato funziona ... mah!
tree->Branch("var",&var);

for (int i=0; i<100; i++) {
 var.clear();
 var.push_back(1);
 var.push_back(2);
 var.push_back(3);
 tree->Fill();
}
out.Write();
}