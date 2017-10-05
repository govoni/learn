//g++ -o tappl.exe `root-config --cflags` `root-config --libs` `root-config --glibs` tappl.cpp
#include "TROOT.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TRint.h"
#include "TCanvas.h"
#include "sstream"
#include <iostream>

int main(int argc, char **argv)
{
//   TApplication *theApp = new TApplication("ROOT example", &argc, argv);

   gROOT->SetBatch(kTRUE);

   TCanvas c1("c1","ciao",10,10,100,100);
   TH1F h1("h1","h1",10,0,10);
   
   for (int i=0; i<10; ++i)
   {
     h1.Fill(i);
   }
   
   h1.Draw();
   c1.Print("stats.eps");

   int entry = 2;
   stringstream picture_name_ps;
   picture_name_ps << "stats" << entry << ".ps";                
   c1.Print(picture_name_ps.str());

// modo 1
   gSystem->Exec("pstopnm -ppm -xborder 0 -yborder 0 -portrait stats.eps");
   gSystem->Exec("ppmtogif stats.eps001.ppm>stats.gif");
// modo 2
//   gSystem->Exec("convert stats.ps stats.gif");
   gSystem->Exec("rm -f stats.ps");

/* metodo che ho trovato in roottalk, ma che non capisco
   if (gROOT->IsBatch())  {
      c1.Print("stats.eps");
      gSystem->Exec("pstopnm -ppm -xborder 0 -yborder 0 -portrait stats.eps");
      gSystem->Exec(
         "ppmtogif stats.eps001.ppm>/user/brun/Welcome/devonly/stats/ftpstats.gif");
   } else {
      c1.Print("stats.gif");
   }
*/   
//   c1.Print("prova.gif"); // cosi' non funziona, cannot create gif in batch mode!

   return(0);
}
