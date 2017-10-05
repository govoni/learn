//g++ -o tappl.exe `root-config --cflags` `root-config --libs` `root-config --glibs` tappl.cpp
#include "TRint.h"
#include "TCanvas.h"
#include <iostream>

int main(int argc, char **argv)
{
   TApplication *theApp = new TApplication("ROOT example", &argc, argv);

   // Init Intrinsics, build all windows, and enter event loop
   TCanvas c1("c1","ciao",10,10,100,100);
   TCanvas c2("c2","ciao",20,20,200,200);
   
   cout << "ok?" << endl;
   double answer = 0;
   cin >> answer;
   TCanvas c3("c3","ciao",30,30,300,300);
   
   cout << "esco?" << endl;
   cin >> answer;

   theApp->Run();

   TApplication *theApp2 = new TApplication("ROOT example2", &argc, argv);

   // Init Intrinsics, build all windows, and enter event loop
   TCanvas c4("c4","ciao",10,10,100,100);

   cout << "ok?" << endl;
   cin >> answer;
   
   cout << "esco?" << endl;
   cin >> answer;

   theApp2->Run();

//   c1.Draw();

   return(0);
}
