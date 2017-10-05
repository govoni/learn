from ROOT import TCanvas, TPad, TH1F

c1 = TCanvas ()
p1 = TPad ("p1","",0,0,1,0.2)
p1.SetFillColor (2)
p2 = TPad ("p1","",0,0.2,1,1)
p2.SetFillColor (4)
p1.Draw()
p2.Draw()
h1 = TH1F ("h1","",10,0,10)
h1.Fill (3)
p1.cd ()
h1.Draw()
p2.cd ()
h1.Draw ()