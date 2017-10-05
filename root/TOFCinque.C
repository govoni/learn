/////////////////////////////////////////////////////
//
// Dispersion in a Scintillating Slab
//
/////////////////////////////////////////////////////


 /******************************************************\
 *
 * The random generation is made by the function Casual,
 * which takes in input different functions as pointers.
 * It's important the maximum of these functions to be 1.
 * Gauss          is a gaussian
 * ThetaDistr     is a gaussian multiplied by sin
 * PhiDistr       is a flat distribution
 *
 \******************************************************/ 


# include <math.h>
# include </usr/share/root/include/TMath.h>
# include <stdlib.h>
# include <stdio.h>
# include </usr/share/root/include/TROOT.h>
# include </usr/share/root/include/TStyle.h>
# include </usr/share/root/include/TH1.h>
# include </usr/share/root/include/TCanvas.h>
# include <iostream.h>
# define MaxRand 2147483647   // max dei longint


Float_t Gauss(Float_t*, Float_t *, Float_t *);
Float_t PhiDistr(Float_t*, Float_t*, Float_t*);
Float_t ThetaDistr(Float_t*, Float_t*, Float_t*);
Float_t Casual(Float_t (*)(Float_t*, Float_t*, Float_t*), Float_t*, Float_t*, Float_t*, Float_t*);
//Float_t Msg(Float_t);


void main()
{

  Float_t      c = 0.000299792458;         // m/ps
  Float_t      RefIndex = 1.6;
  Float_t      SlabHalfLength = 1.25;      // m

  Float_t      Theta;
  Float_t      SigmaTheta = 1; 
  Float_t      ThetaMin = 0.05;            // rad
  Float_t      ThetaMax = 0.3;             // rad

  Float_t      Time;
  Float_t      TimeCenter = 20;
  Float_t      SigmaTime = 1;
  Float_t      TimeMin = 0;                // ps
  Float_t      TimeMax = 100;              // ps

  Float_t      Phi;
  Float_t      SigmaPhi = 1;
  Float_t      PhiMin = 0.14;              // rad
  Float_t      PhiMax = 3;                 // rad

  Float_t      PMEfficiency = 0.1;         // of 1
  Float_t      PMGain = 1E6; 
  Float_t      PMSigma = 250;              // ps 

  Float_t      Weight;
  Float_t      Arrival;
  Float_t      DiscThres = 1;              // DISCriminator THREShold
  Float_t      store;
  Int_t        EventNumber = 10;
  Int_t        PhotonNumber = 10;

  //////////////////////////
  // histograms' parameters
  //////////////////////////

  Int_t FinBin=100;  Axis_t FinMin=15000; Axis_t FinMax=100000; 
  Int_t DiscBin=100; Axis_t DiscMin=0;    Axis_t DiscMax=20000; 

  //////////////////////////
  // histograms' definition and initialization
  //////////////////////////

  TH1F *FinalDistr = new TH1F("FinalDistr","Signal Shape on a PM",FinBin,FinMin,FinMax);
  FinalDistr->GetXaxis()->SetTitle("Time (ps)");
  FinalDistr->GetYaxis()->SetTitle("Int_Tensity (arbitrary units)");
 
  TH1F *DiscTimeDistr = new TH1F("DiscTimeDistr","Distribution of the discriminated sgn",
                                  DiscBin,DiscMin,DiscMax);
  DiscTimeDistr->GetXaxis()->SetTitle("Time (ps)");
  DiscTimeDistr->GetYaxis()->SetTitle("Frequency ()");

  //////////////////////////
  // histograms' filling
  //////////////////////////

  for (Int_t Event=0; Event < EventNumber; Event++)
    {
      for (Int_t Photon=0; Photon < PhotonNumber; Photon++)
	{

          // il sin(Theta) e' lo jacobiano di cambio di coordinate
	  store = 0;
	  Theta = Casual(ThetaDistr, &ThetaMin, &ThetaMax, &SigmaTheta, &store);
	  Time = Casual(Gauss, &TimeMin, &TimeMax, &SigmaTime, &TimeCenter);
	  Phi = Casual(PhiDistr, &PhiMin, &PhiMax, &SigmaPhi, &store);
  
	  if ((rand()/MaxRand) > PMEfficiency)   // efficiency control
	    {
	      continue;
	    }
 	 	  
          Arrival = SlabHalfLength * RefIndex / (c * ((Float_t)sin((double)Theta)) * ((Float_t)sin((double)Phi))) - SlabHalfLength / c;

	  for (Int_t t=0; t<FinBin; t++)    // loop over bins
	    {
              store = t*(FinMax-FinMin)/FinBin;
	      FinalDistr->Fill(store, Gauss(&store, &PMSigma, &Arrival));
	    }


	  for (Int_t t=0; t<FinBin; t++)    //t scans the histogram to find when it goes over the threshold
	    {
	      if ((FinalDistr->GetBinContent(t)) > DiscThres) 
		{
		  DiscTimeDistr->Fill(t*(FinMax-FinMin)/FinBin);
		  break;
		}
    	    }
	}     
    } 

  //////////////////////////
  // graphics
  //////////////////////////

  TCanvas *c1 = new TCanvas("c1","Fortuna Iuvat Audentes",100,10,900,820);
  c1->SetGridx();
  c1->SetGridy();
 
  /*
  TPad *pad;
  *pad = new TPad("pad","one", 0, 0, 1, 1); 
  pad->Draw();
  */
  //////////////////////////
  // histograms' drawing
  //////////////////////////

  DiscTimeDistr->SetFillColor(38);   // light green
  DiscTimeDistr->Draw();
  c1->Update();
  //cout << "ciao" << endl;
/*
  float a;
  cin >> a;
*/
}




Float_t Gauss(Float_t *x, Float_t *Sigmax, Float_t *xCenter)
{
  return exp(-1*((Float_t)pow((*x-*xCenter),2))/(2*((Float_t)pow((*Sigmax),2))));
}

Float_t ThetaDistr(Float_t *x, Float_t *Sigmax, Float_t *xCenter)
{
  return exp(-1*((Float_t)pow((*x-*xCenter),2))/(2*((Float_t)pow((*Sigmax),2))))*((Float_t)sin((double)*x));
  // this is a gaussian, multiplied by a sin(x), that takes Int_to account the 
  // jacobian factor due to the increasing of density near the poles in polar cohordinates
}


Float_t PhiDistr(Float_t *Phi, Float_t *SigmaPhi, Float_t *PhiCenter)
{
  return 1;
}


Float_t Casual(Float_t (*Func)(Float_t *, Float_t *, Float_t *), Float_t *Minx, 
                               Float_t *Maxx, Float_t *Sigmax, Float_t *xCenter)
{
  Float_t      x,y;
  //  Float_t      MaxRand = 2147483647;   // max dei longint

  while(1)                // to be sure to have a random number at the end
    {
      x = (Float_t) rand();
      x = x * (*Maxx-*Minx) / MaxRand + *Minx;
      y = (Float_t) rand();
      y /= MaxRand;
      if (y < (Func(&x, Sigmax, xCenter)))
    	{
	  return y;
   	}
    }
}
/*
void Msg(Float_t message)
{
  cout<<"\t" << message << endl;
}
*/











