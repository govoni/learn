{
 // display the various 2-d drawing options
   //Author: Rene Brun
   
   gROOT->Reset();
   TF2 *f2 = new TF2("f2","xygaus",-4,4,-4,4);
   Double_t params[] = {130, 0., 0.5, 0., 0.5} ;
   f2.SetParameters(params);
   TH2F h2("h2","xygaus + xygaus(5) + xylandau(10)",100,-4,4,100,-4,4);
   h2.SetFillColor(46);
   h2.FillRandom("f2",40000);

   TCanvas c1 ;
   h2.SetStats (0) ;
   h2.DrawCopy ("col") ;
   h2.SetLineColor (2) ;
   gStyle->SetPalette (8) ;

   h2.SetContour (2) ;   
   h2.SetContourLevel (1, 0.7 * h2.GetMaximum ()) ;   
   
   TH2F * norm = (TH2F*) h2.DrawNormalized ("cont3 same");
   
} 