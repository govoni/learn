{
   //
   // To see the output of this macro, click begin_html <a href="gif/fit1.gif">here</a>. end_html
   //
   gROOT->Reset();
   c1 = new TCanvas("c1","The Fit Canvas",200,10,700,500);
   c1->SetGridx();
   c1->SetGridy();
   c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderMode(-1);
   c1->GetFrame()->SetBorderSize(5);

   gBenchmark->Start("fit1");
   //
   // We connect the ROOT file generated in a previous tutorial
   // (see begin_html <a href="fillrandom.C.html">Filling histograms with random numbers from a function</a>) end_html
   //
   TFile fill("fillrandom.root");

   //
   // The function "ls()" lists the directory contents of this file
   //
   fill.ls();

   //
   // Get object "sqroot" from the file. Undefined objects are searched
   // for using gROOT->FindObject("xxx"), e.g.:
   // TF1 *sqroot = (TF1*) gROOT.FindObject("sqroot")
   //
   sqroot->Print();

   //
   // Now fit histogram h1f with the function sqroot
   //
   h1f->SetFillColor(45);
   h1f->Fit("sqroot");

   // We now annotate the picture by creating a PaveText object
   // and displaying the list of commands in this macro
   //
   fitlabel = new TPaveText(0.6,0.3,0.9,0.80,"NDC");
   fitlabel->SetTextAlign(12);
   fitlabel->SetFillColor(42);
   fitlabel->ReadFile("fit1_C.C");
   fitlabel->Draw();
   c1->Update();
   gBenchmark->Show("fit1");
}
