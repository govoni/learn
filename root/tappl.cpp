//g++ -o tappl.exe `root-config --cflags` `root-config --libs` `root-config --glibs` tappl.cpp
#include "TRint.h"
#include "TCanvas.h"
#include <iostream>

int main(int argc, char **argv)
{
   TApplication *theApp = new TApplication("ROOT example", &argc, argv);

   // Init Intrinsics, build all windows, and enter event loop
   TCanvas c1("ciao","ciao",10,10,100,100);
   TCanvas c2("ciao","ciao",20,20,200,200);
   
   cout << "ok?" << endl;
   double answer = 0;
   cin >> answer;
   TCanvas c3("ciao","ciao",30,30,300,300);
   
   cout << "esco?" << endl;
   cin >> answer;
   return 0;

   theApp->Run();

//   c1.Draw();

   return(0);
}
