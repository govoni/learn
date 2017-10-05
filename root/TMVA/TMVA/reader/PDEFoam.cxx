// @(#)root/foam:$Name: V03-09-06-01 $:$Id: PDEFoam.cxx,v 1.7 2008/08/08 16:35:37 alexvoigt Exp $
// Author: S. Jadach <mailto:Stanislaw.jadach@ifj.edu.pl>, P.Sawicki <mailto:Pawel.Sawicki@ifj.edu.pl>

#define isTMVA

#include <TStyle.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>


#ifdef isTMVA
#include "TMVA/PDEFoam.h"
#else
#include "../inc/PDEFoam.h"
#endif

#include "TH1.h"
#include "TMath.h"
#include "TVectorT.h"

ClassImp(TMVA::PDEFoam);

static const Float_t gHigh= 1.0e150;
static const Float_t gVlow=-1.0e150;

//________________________________________________________________________________________________
#ifndef isTMVA
TMVA::PDEFoam::PDEFoam() : ::TFoam()
#else
#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
TMVA::PDEFoam::PDEFoam() : ::TFoam()
#else
TMVA::PDEFoam::PDEFoam() : TMVA::TFoam()
#endif
#endif
{
   xmin=0;
   xmax=0;
   fdistr = new TFDISTR();
   timer = new Timer(fNCells, "PDEFoam", kTRUE);
}

//_________________________________________________________________________________________________
#ifndef isTMVA
TMVA::PDEFoam::PDEFoam(const Char_t* Name) : ::TFoam(Name)
#else
#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
TMVA::PDEFoam::PDEFoam(const Char_t* Name) : ::TFoam(Name)
#else
TMVA::PDEFoam::PDEFoam(const Char_t* Name) : TMVA::TFoam(Name)
#endif
#endif
{
   fdistr = new TFDISTR();
   timer = new Timer(fNCells, "PDEFoam", kTRUE);
}

//_______________________________________________________________________________________________
TMVA::PDEFoam::~PDEFoam()
{
   delete timer;
   delete fdistr;
}

//_______________________________________________________________________________________________
void TMVA::PDEFoam::Explore(TFoamCell *cell)
{
// Internal subprogram used by Initialize.
// It explores newly defined cell with help of special short MC sampling.
// As a result, estimates of true and drive volume is defined/determined
// Average and dispersion of the weight distribution will is found along
// each edge and the best edge (minimum dispersion, best maximum weight)
// is memorized for future use.
// The optimal division point for eventual future cell division is
// determined/recorded. Recorded are also minimum and maximum weight etc.
// The volume estimate in all (inactive) parent cells is updated.
// Note that links to parents and initial volume = 1/2 parent has to be
// already defined prior to calling this routine.

   Double_t wt, dx, xBest, yBest;
   Double_t intOld, driOld;

   Long_t iev;
   Double_t nevMC;
   Int_t i, j, k;
   Int_t nProj, kBest;
   Double_t ceSum[5], xproj;

   TFoamVect  cellSize(fDim);
   TFoamVect  cellPosi(fDim);

   cell->GetHcub(cellPosi,cellSize);

   TFoamCell  *parent;

   Double_t *xRand = new Double_t[fDim];

   Double_t *volPart=0;

   cell->CalcVolume();
   dx = cell->GetVolume();
   intOld = cell->GetIntg(); //memorize old values,
   driOld = cell->GetDriv(); //will be needed for correcting parent cells


   /////////////////////////////////////////////////////
   //    Special Short MC sampling to probe cell      //
   /////////////////////////////////////////////////////
   ceSum[0]=0;
   ceSum[1]=0;
   ceSum[2]=0;
   ceSum[3]=gHigh;  //wtmin
   ceSum[4]=gVlow;  //wtmax
   //
   for(i=0;i<fDim;i++) ((TH1D *)(*fHistEdg)[i])->Reset(); // Reset histograms
   fHistWt->Reset();
   //

   Double_t nevEff=0.;

   // DD 10.Jan.2008
   // use new routine that fills edge histograms directly from the 
   // input distributions, w/o MC sampling
   if (fNSampl==0) {
      Double_t cellPosiarr[fDim], cellSizearr[fDim];
      for (Int_t idim=0; idim<fDim; idim++) {
	 cellPosiarr[idim]=cellPosi[idim];
	 cellSizearr[idim]=cellSize[idim];
      }
      // not jet implemented:
      // fRho->FillEdgeHist(fDim, fHistEdg, cellPosiarr, cellSizearr, ceSum, dx);
   }   
   else {
      // ||||||||||||||||||||||||||BEGIN MC LOOP|||||||||||||||||||||||||||||
      for(iev=0;iev<fNSampl;iev++){
	 MakeAlpha();               // generate uniformly vector inside hypercube
       
	 if(fDim>0){
	    for(j=0; j<fDim; j++)
	       xRand[j]= cellPosi[j] +fAlpha[j]*(cellSize[j]);
	 }
       
	 wt=dx*Eval(xRand);
       
	 nProj = 0;
	 if(fDim>0) {
	    for(k=0; k<fDim; k++) {
	       xproj =fAlpha[k];
	       ((TH1D *)(*fHistEdg)[nProj])->Fill(xproj,wt);
	       nProj++;
	    }
	 }
	 //
	 fNCalls++;
	 ceSum[0] += wt;    // sum of weights
	 ceSum[1] += wt*wt; // sum of weights squared
	 ceSum[2]++;        // sum of 1
	 if (ceSum[3]>wt) ceSum[3]=wt;  // minimum weight;
	 if (ceSum[4]<wt) ceSum[4]=wt;  // maximum weight
	 // test MC loop exit condition
	 nevEff = ceSum[0]*ceSum[0]/ceSum[1];
	 if( nevEff >= fNBin*fEvPerBin) break;
      }   // ||||||||||||||||||||||||||END MC LOOP|||||||||||||||||||||||||||||
   }
   //------------------------------------------------------------------
   //---  predefine logics of searching for the best division edge ---
   for(k=0; k<fDim;k++){
      fMaskDiv[k] =1;                       // default is all
      if( fInhiDiv[k]==1) fMaskDiv[k] =0; // inhibit some...
   }
   // Note that predefined division below overrule inhibition above
   kBest=-1;
   Double_t rmin,rmax,rdiv;
   if(fOptPRD) {          // quick check
      for(k=0; k<fDim; k++) {
	 rmin= cellPosi[k];
	 rmax= cellPosi[k] +cellSize[k];
	 if( fXdivPRD[k] != 0) {
	    Int_t n= (fXdivPRD[k])->GetDim();
	    for(j=0; j<n; j++) {
	       rdiv=(*fXdivPRD[k])[j];
	       // check predefined divisions is available in this cell
	       if( (rmin +1e-99 <rdiv) && (rdiv< rmax -1e-99)) {
		  kBest=k;
		  xBest= (rdiv-cellPosi[k])/cellSize[k] ;
		  goto ee05;
	       }
	    }
	 }
      }//k
   }
ee05:
   //------------------------------------------------------------------
   fNEffev += (Long_t)nevEff;
   nevMC          = ceSum[2];
   Double_t intTrue = ceSum[0]/(nevMC+0.000001);
   Double_t intDriv=0.;
   Double_t intPrim=0.;
   
   switch(fOptDrive){
   case 1:                       // VARIANCE REDUCTION
      if(kBest == -1) Varedu(ceSum,kBest,xBest,yBest); // determine the best edge,
      //intDriv =sqrt( ceSum[1]/nevMC -intTrue*intTrue ); // Older ansatz, numerically not bad
      intDriv =sqrt(ceSum[1]/nevMC) -intTrue; // Foam build-up, sqrt(<w**2>) -<w>
      intPrim =sqrt(ceSum[1]/nevMC);          // MC gen. sqrt(<w**2>) =sqrt(<w>**2 +sigma**2)
      break;
   case 2:                       // WTMAX  REDUCTION
      if(kBest == -1) Carver(kBest,xBest,yBest);  // determine the best edge
      intDriv =ceSum[4] -intTrue; // Foam build-up, wtmax-<w>
      intPrim =ceSum[4];          // MC generation, wtmax!
      break;
   default:
      Error("Explore", "Wrong fOptDrive = \n" );
   }//switch
   //=================================================================================
   //hist_Neff_distrib.Fill( fLastCe/2.0+0.01, nevEff+0.01);  //
   //hist_kBest_distrib.Fill( kBest+0.50, 1.0 ); //  debug
   //hist_xBest_distrib.Fill( xBest+0.01, 1.0 ); //  debug
   //=================================================================================
   cell->SetBest(kBest);
   cell->SetXdiv(xBest);
   cell->SetIntg(intTrue);
   cell->SetDriv(intDriv);
   cell->SetPrim(intPrim);
   // correct/update integrals in all parent cells to the top of the tree
   Double_t  parIntg, parDriv;
   for(parent = cell->GetPare(); parent!=0; parent = parent->GetPare()){
      parIntg = parent->GetIntg();
      parDriv = parent->GetDriv();
      parent->SetIntg( parIntg   +intTrue -intOld );
      parent->SetDriv( parDriv   +intDriv -driOld );
   }
   delete [] volPart;
   delete [] xRand;
   //cell->Print();
} // PDEFoam::Explore

//_______________________________________________________________________________________________
void TMVA::PDEFoam::ResetCellIntegrals(void)
{
   // Reset Cell Integrals
   for(Long_t iCell=0; iCell<=fLastCe; iCell++) {
      fCells[iCell]->SetIntg(0.);
      fCells[iCell]->SetPrim(0.);
      fCells[iCell]->SetDriv(0.);
   }
}

//_______________________________________________________________________________________________
void TMVA::PDEFoam::DisplayCellContent(void)
{
   Double_t total_counts = 0;
   Double_t max = GetCellEntries(fCells[0]);
   Double_t min = GetCellEntries(fCells[0]);

   // Reset Cell Integrals
   for(Long_t iCell=0; iCell<=fLastCe; iCell++) {
      total_counts += GetCellEntries(fCells[iCell]);
      
      if (GetCellEntries(fCells[iCell]) < min)
	 min = GetCellEntries(fCells[iCell]);
      if (GetCellEntries(fCells[iCell]) > max)
	 max = GetCellEntries(fCells[iCell]);

//       std::cout << "cell["<< iCell <<"]: Entries=" << GetCellEntries(fCells[iCell]) 
// 		<< " Intg=" << fCells[iCell]->GetIntg() 
// 		<< " Prim=" << fCells[iCell]->GetPrim()
// 		<< " Driv=" << fCells[iCell]->GetDriv() << std::endl;
   }

   std::cout << "PDEFoam-DEBUG: Total Events in Foam: " << total_counts << std::endl;
   std::cout << "PDEFoam-DEBUG: min cell entry: " << min << std::endl;
   std::cout << "PDEFoam-DEBUG: max cell entry: " << max << std::endl;
}

//_______________________________________________________________________________________________
void TMVA::PDEFoam::CalcCellDiscr(){
   // calc discriminator and its error from cells

   // loop over cells
   for(Long_t iCell=0; iCell<=fLastCe; iCell++) {
      Double_t N_sig = fCells[iCell]->GetDriv(); // get number of signal events
      Double_t N_bg  = fCells[iCell]->GetPrim(); // get number of bg events
      if (N_sig+N_bg > 1e-10){
	 fCells[iCell]->SetIntg( N_sig/(N_sig+N_bg) ); // set discriminator
	 fCells[iCell]->SetDriv( 
	    TMath::Sqrt( TMath::Power ( N_sig/TMath::Power(N_sig+N_bg,2),2)*N_sig +
			 TMath::Power ( N_bg /TMath::Power(N_sig+N_bg,2),2)*N_bg    ) ); // set discriminator error
      }
      else {
	 fCells[iCell]->SetIntg( 0.5 ); // set discriminator
	 fCells[iCell]->SetDriv( 1. );  // set discriminator error
      }
   }
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellDiscr(Double_t *xvec){
   // Get discriminator of cell (calculated in CalcCellDiscr())

   // transform xvec
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) 
      txvec[i]=this->VarTransform(i, xvec[i]);

   // find cell
   TFoamCell *cell= FindCell(txvec);

   if (!cell) return -999.;

   return cell->GetIntg();
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellDiscrError(Double_t *xvec){
   // Get discriminator error of cell (calculated in CalcCellDiscr())

   // transform xvec
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) 
      txvec[i]=this->VarTransform(i, xvec[i]);

   // find cell
   TFoamCell *cell= FindCell(txvec);

   if (!cell) return -999.;

   return cell->GetDriv();
}

//_______________________________________________________________________________________________
void TMVA::PDEFoam::AddCellEvents(Double_t *xvec, Bool_t IsSignal, Double_t SigWeight, Double_t BgWeight){
   // Set number of N_Sig (or N_Bg) to cell (similar to SetCell())
   // 0) transform xvec to number [0,1]
   // 1) Loop over cell to find cell that contains xvec
   // 2) Set N_bg or N_sig (times weight) to this cell

   // 0) transform to cooodinates [0,1]
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++)
      txvec[i]=this->VarTransform(i,xvec[i]);

   // 1) xfind cell that contains xvec
   TFoamCell *cell= FindCell(txvec);
   if (!cell) return;

   // 2) Set N_Sig or N_Bg to this cell
   if (IsSignal)
      cell->SetDriv(cell->GetDriv() + 1*SigWeight); // fill signal event to 'Driv'
   else
      cell->SetPrim(cell->GetPrim() + 1*BgWeight);  // fill bg event to 'Prim'
}

//_______________________________________________________________________________________________
void TMVA::PDEFoam::SetCell(Double_t *xvec, Double_t value, Double_t weight)
{ // Set weight to cell
  // 0) transform xvec to number [0,1]
  // 1) Loop over cell to find cell that contains xvec
  // 2) Set value/weight to this cell (to calculate mean and rms)

   char name[]={"Foam::SetCell: "};
#ifdef DEBUG
   cout<<name<<" value= "<< value << endl;
   cout<<name<<"weight= "<< weight << endl;
   cout<<name<<"Print-out xvec: " << endl;
   for(Long_t i=0; i<GetTotDim(); i++) {
      cout<<name<<" xvec["<<i<<"]="<<xvec[i]<<endl;
   }
#endif 
   //
   // transform to cooodinates [0,1]
   //
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) {
      txvec[i]=this->VarTransform(i,xvec[i]);
   }
   //
#ifdef DEBUG
   cout<<name;
   for(Long_t i=0; i<GetTotDim(); i++) cout<<" t["<<i<<"]= "<< txvec[i];
   cout<< endl;
#endif

   TFoamCell *cell= FindCell(txvec);

   if (!cell) {
      std::cout<<name<<"ERROR: No cell found ! "<<std::endl; 
      return;
   }

#ifdef DEBUG
   cout<<name<<" Cell found:"<<endl;
   cell->Print("1");
#endif

//AV>>
   // Add events to cell
   // Intg: Number of events times weight
   // Driv: 
   // Prim: Sum of weights (equals to value=1)
   //  cell->AddWeight(value,weight);
   cell->SetIntg(cell->GetIntg() + value*weight); 
   cell->SetDriv(cell->GetDriv() + value*value*weight*weight); 
   cell->SetPrim(cell->GetPrim() + weight);
//AV<<
   return;
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellMean(Double_t *xvec)
{ // Get mean of cell
  //  char name[]={"Foam::GetCellMean: "};
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) txvec[i]=this->VarTransform(i,xvec[i]);

   TFoamCell *cell= FindCell(txvec);

   if (!cell) {
      char name[]={"Foam::GetCellMean: "};
      std::cout<<name<<"ERROR: No cell found ! "<<std::endl; 
      return -999.;
   }

   return GetCellMean(cell);
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellRMS(Double_t *xvec)
{ // Get RMS of cell that contains xvec
  //  char name[]={"Foam::GetCellMean: "};
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) txvec[i]=this->VarTransform(i,xvec[i]);

   TFoamCell *cell= FindCell(txvec);

   if (!cell) {
      char name[]={"Foam::GetCellMean: "};
      std::cout<<name<<"ERROR: No cell found ! "<<std::endl; 
      return -999.;
   }

   return GetCellRMS(cell);
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellEntries(Double_t *xvec)
{ // Get number of entries in cell that contains xvec
  //  char name[]={"Foam::GetCellEntries: "};
   Double_t txvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) txvec[i]=this->VarTransform(i,xvec[i]);

   TFoamCell *cell= FindCell(txvec);

   if (!cell) {
      char name[]={"Foam::GetCellEntries: "};
      std::cout<<name<<" No cell found ! "<<std::endl; 
      return -999.;
   }

   return GetCellEntries(cell);
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellDensity(Double_t *xvec)
{ // Get density (=number of entries / volume) of cell that contains xvec

   char name[]={"Foam::GetCellDensity: "};
   Double_t txvec[GetTotDim()];
   for(Int_t i=0; i<GetTotDim(); i++) txvec[i]=this->VarTransform(i,xvec[i]);

   TFoamCell *cell= FindCell(txvec);

   if (!cell) {
      std::cout<<name<<"ERROR: No cell found ! "<<std::endl; 
      return -999.;
   }

   Double_t volume = cell->GetVolume();
   if (volume>0){
      //return cell->GetEntries()/volume;
      return GetCellEntries(cell)/volume; 
   }
   else{
      std::cout<<name<<"ERROR: Volume negative or 0 ! "<<std::endl; 
      return -999.;
   }
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellDiscr(TFoamCell* cell){
   return cell->GetIntg();
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellDiscrError(TFoamCell* cell){
   return cell->GetDriv();
}

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellMean(TFoamCell* cell){
   Double_t val=0.;
   if (cell->GetPrim()!=0) {
      val = cell->GetIntg()/cell->GetPrim(); 
   }
   return val;
};

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellRMS(TFoamCell* cell){
   Double_t val=0.;
   if ((cell->GetPrim()!=0) && (cell->GetDriv()-cell->GetIntg()>0)) 
      val = sqrt((cell->GetDriv()-cell->GetIntg())/cell->GetPrim()); 
   return val;
};

//_______________________________________________________________________________________________
Double_t TMVA::PDEFoam::GetCellEntries(TFoamCell* cell){
   return cell->GetPrim();
};

//_______________________________________________________________________________________________
#ifndef isTMVA
TFoamCell* TMVA::PDEFoam::FindCell(Double_t *xvec)
#else
#if ROOT_VERSION_CODE > ROOT_VERSION(5,20,0)
TFoamCell* TMVA::PDEFoam::FindCell(Double_t *xvec)
#else
TMVA::TFoamCell* TMVA::PDEFoam::FindCell(Double_t *xvec)
#endif
#endif
{
   // static char name[]={"Foam::FindCell: "};

   // Find cell that contains xvec
   //
   //
   // loop to find cell that contains xvec
   // start from root Cell, uses binary tree to
   // find cell quickly
   //
   // DD 10.12.2007 
   // cleaned up a bit and improved performance

   TFoamVect  cellPosi (GetTotDim()), cellSize (GetTotDim());
   TFoamVect  cellPosi0(GetTotDim()), cellSize0(GetTotDim());

   TFoamCell *cell, *cell0;

   cell=fCells[0]; // start with root cell
   Int_t idim=0;
   while (1==1) { //go down binary tree until cell is found

      cell->GetHcub(cellPosi,cellSize);
      //std::cout << "GetStat=" << cell->GetStat() << std::endl;
      //cell->Print("1");

      Int_t incell=0;

      if(cell->GetStat()==1) { // cell is active
	 //to avoid rounding errors in cell position caluclation
	 const Double_t xsmall=1.e-10; 
	 for(Long_t i=0; i<GetTotDim(); i++) {
	    if (xvec[i] > cellPosi[i]-xsmall && xvec[i] <= cellPosi[i]+cellSize[i]+xsmall) incell++;
	    else
	       break;
	 }

// 	 if (incell!=GetTotDim()) {
// 	    std::cout << "COORDINATES OUTSIDE OF CELL " << std::endl;
// 	    for(Long_t i=0; i<GetTotDim(); i++) {
// 	       std::cout << "CellPosi["<<i<<"]=" << cellPosi[i] << " CellSize["<<i<<"]=" << cellSize[i] << " xvec["<<i<<"]=" << xvec[i]; 
// 	       std::cout << (xvec[i] > cellPosi[i]-xsmall && xvec[i] <= cellPosi[i]+cellSize[i]+xsmall ? " +" : " <===") << std::endl;
// 	    }
// 	 }
	 break;
      }
      idim=cell->GetBest();  // dimension that changed
      cell0=cell->GetDau0(); 
      //std::cout << "cell0=" << cell0 << std::endl;
      cell0->GetHcub(cellPosi0,cellSize0);

      if (xvec[idim]<=cellPosi0[idim]+cellSize0[idim])
	 cell=cell0;
      else
	 cell=(cell->GetDau1());
   } 
   return cell;
}

//_________________________________________________________________________________________
TH2D* TMVA::PDEFoam::Project(Int_t idim1, Int_t idim2, char *opt)
{ // project variable idim1 and variable idim2 to histogram
  // Option: Project mean, rms, nev, discr, discr_error

   char name[]="PDEFoam::Project: ";
   const bool debug=false;
   char hname[100]; char htit[100];
   const Int_t nbin=100;
 
   sprintf(htit,"%s var%d vs var%d",opt,idim1,idim2);
   sprintf(hname,"h%s_%d_vs_%d",opt,idim1,idim2);
   if (debug) {
      std::cout<<name<<" Project var"<<idim1<<" and var"<<idim2<<" to histo: "<<std::endl; 
      std::cout<<name<<hname<<" nbin="<<nbin
	       <<" xmin["<<idim1<<"]="<<xmin[idim1]<<" xmax["<<idim1<<"]="<<xmax[idim1]
	       <<" xmin["<<idim2<<"]="<<xmin[idim2]<<" xmax["<<idim2<<"]="<<xmax[idim2]
	       <<std::endl;
   }

   TH2D* h1=(TH2D*)gDirectory->Get(hname);
   if (h1) delete h1;
   h1= new TH2D(hname, htit, nbin, xmin[idim1], xmax[idim2], nbin, xmin[idim1], xmax[idim2]);

   if (!h1) std::cout<<name<<"ERROR: Can not create histo"<<hname<<std::endl;
   else if (debug) h1->Print();

   if (debug) std::cout << "histogram created/ loaded" << std::endl;

   //AV>> // faster!!
   TH1D *h1_px = h1->ProjectionX("_px", 1, nbin, ""); // exclude overflow and underflow bin
   TH1D *h1_py = h1->ProjectionY("_py", 1, nbin, "");
   //AV<<

   Double_t xvec[2];
   //
   // need to loop here also over other dimensions, but how
   //
   for(Int_t ibinx=1; ibinx<=nbin; ibinx++) { //loop over  x-bins
      for(Int_t ibiny=1; ibiny<=nbin; ibiny++) { //loop over y-bins
	 //AV>>
	 //xvec[idim1]=h1->ProjectionX()->GetBinCenter(ibinx);
	 //xvec[idim2]=h1->ProjectionY()->GetBinCenter(ibiny);
	 xvec[idim1]=h1_px->GetBinCenter(ibinx);
	 xvec[idim2]=h1_py->GetBinCenter(ibiny);
	 //AV<<
	 if (debug) {
	    std::cout<< " xvec= "<<xvec[idim1]<<" "<<xvec[idim2]<<" "<<std::endl; 
	 }

	 Double_t var=0.;
	 //cout<<" dimension here is fDim= "<<fDim<<endl;
	 //for(Int_t idim=1; idim<=fDim; idim++) {//loop other dimension, how ?? 
	 Double_t nev = this->GetCellEntries(xvec); // problem?

	 if (strcmp(opt,"mean")==0)
	    var=this->GetCellMean(xvec); 
	 else if (strcmp(opt,"rms") ==0) 
	    var=this->GetCellRMS(xvec);
	 else if (strcmp(opt,"nev") ==0) {
	    // DD 27.11.2007
	    // for testing: normalize number of events to cellsize
	    TFoamCell *cell= FindCell(xvec);
	    if (!cell) { 
	       std::cout<<name<<" No cell found ! "<<std::endl; 
	       continue; 
	    }
	    Double_t vol=cell->GetVolume();
	    if (vol<1e-20) {
	       std::cout<<name<<" Volume too small! " <<std::endl; 
	       continue;
	    }
	    var=nev/(cell->GetVolume());
	 }
	 else if (strcmp(opt,"discr") ==0) {
	    var=this->GetCellDiscr(xvec);
	 }
	 else if (strcmp(opt,"discr_error") ==0) {
	    var=this->GetCellDiscrError(xvec);
	 }

	 Int_t ibin=h1->GetBin(ibinx, ibiny);

	 if (debug) {
	    std::cout<<ibin<<" "<<ibinx<<" "<<ibiny<<
	       " xvec= "<<xvec[idim1]<<" "<<xvec[idim2]<<" "<<" nev= "<<nev<<" var= "<<var<<std::endl; 
	 }

	 if (nev!=0) h1->SetBinContent(ibin,var);	 
      }
   }
   return h1;
}

//_________________________________________________________________________________________
TH1D* TMVA::PDEFoam::Draw1Dim(char *opt, Int_t nbin){
   // in case one has just 1 dimestion -> draw 1-dimensional histogram

   if (!(strcmp(opt,"mean")==0 || strcmp(opt,"nev")==0 || strcmp(opt,"rms")==0 || strcmp(opt,"discr")==0 || strcmp(opt,"discr_error")==0)
       || GetTotDim()!=1 )
      return 0;

   char name[]="PDEFoam::Draw1Dim: ";
   const bool debug = false;
   char hname[100]; char htit[100];
 
   sprintf(htit,"1-dimensional Foam: %s", opt);
   sprintf(hname,"h%s",opt);
   if (debug) {
      std::cout<<name<<hname<<" nbin="<<nbin
	       <<" xmin["<<0<<"]="<<xmin[0]<<" xmax["<<0<<"]="<<xmax[0]
	       <<std::endl;
   }

   TH1D* h1=(TH1D*)gDirectory->Get(hname);
   if (h1) delete h1;
   h1= new TH1D(hname, htit, nbin, xmin[0], xmax[0]);

   if (!h1) std::cout<<name<<"ERROR: Can not create histo"<<hname<<std::endl;
   else if (debug) h1->Print();

   if (debug) std::cout << "histogram created/ loaded" << std::endl;

   Double_t xvec[GetTotDim()]; // GetTotDim()==1
   for(Long_t i=0; i<GetTotDim(); i++) 
      xvec[i] = 0.;

   // loop over all bins
   for(Int_t ibinx=1; ibinx<=nbin; ibinx++) { //loop over  x-bins
      xvec[0]=h1->GetBinCenter(ibinx);

      // transform xvec
      Double_t txvec[GetTotDim()];
      for(Long_t i=0; i<GetTotDim(); i++) 
	 txvec[i]=this->VarTransform(i, xvec[i]);

      for(Long_t iCell=0; iCell<=fLastCe; iCell++) { // loop over all active cells
	    
	 if(!(fCells[iCell]->GetStat())) continue; // cell not active -> continue
	    
	 // get cell position and dimesions
	 TFoamVect  cellPosi(GetTotDim()), cellSize(GetTotDim());
	 fCells[iCell]->GetHcub(cellPosi,cellSize);

	 // compare them with txvec
	 const Double_t xsmall = 1.e-10; 
	 if (!( (txvec[0]>cellPosi[0]-xsmall) && (txvec[0]<=cellPosi[0]+cellSize[0]+xsmall) ) )
	    continue; 

	 Double_t vol = fCells[iCell]->GetVolume();
	 if (vol<1e-10) {
	    std::cout << name <<"PDEFoam::Project: ERROR: Volume too small! " <<std::endl; 
	    continue;
	 }

	 // get cell value (depending on the option)
	 Double_t var = 0.;

	 if (strcmp(opt,"mean")==0 || strcmp(opt,"nev")==0)
	    //var = GetCellMean(fCells[iCell]); 
	    var = GetCellEntries(fCells[iCell]); 
	 else if (strcmp(opt,"rms")==0) 
	    var = GetCellRMS(fCells[iCell]);
	 else if (strcmp(opt,"discr")==0)
	    var = GetCellDiscr(fCells[iCell]);
	 else if (strcmp(opt,"discr_error")==0)
	    var = GetCellDiscrError(fCells[iCell]);
	 else
	    std::cout << "PDEFoam::Project: ERROR: unknown option: " << opt << std::endl;	    

	 // filling value to histogram
	 //h1->AddBinContent(ibin, var/vol);
	 h1->SetBinContent(ibinx, var + h1->GetBinContent(ibinx));

	 if (debug) {
	    std::cout << "ibin=" << ibinx
		      << " cellID=" << iCell
		      << " xvec=["<<xvec[0]<< "] "<<" value="<< var
		      << " txvec=["<<txvec[0]<<", "<<txvec[0]<<"]" << std::endl; 
	 }
      }
   }
   return h1;
}

//_________________________________________________________________________________________
TH2D* TMVA::PDEFoam::Project2(Int_t idim1, Int_t idim2, char *opt, Int_t nbin)
{  // project variable idim1 and variable idim2 to histogram
   // Options: mean, rms, nev, discr, discr_error
   // Problem: too slow for high values of nbin (i.e. nbin>100)!

   if (!(strcmp(opt,"mean")==0 || strcmp(opt,"nev")==0 || strcmp(opt,"rms")==0 || strcmp(opt,"discr")==0 || strcmp(opt,"discr_error")==0)
       || (idim1>=GetTotDim()) || (idim1<0) 
       || (idim1>=GetTotDim()) || (idim2<0) )
      return 0;

   char name[]="PDEFoam::Project: ";
   const bool debug = false;
   char hname[100]; char htit[100];
 
   sprintf(htit,"%s var%d vs var%d",opt,idim1,idim2);
   sprintf(hname,"h%s_%d_vs_%d",opt,idim1,idim2);
   if (debug) {
      std::cout<<name<<" Project var"<<idim1<<" and var"<<idim2<<" to histo: "<<std::endl; 
      std::cout<<name<<hname<<" nbin="<<nbin
	       <<" xmin["<<idim1<<"]="<<xmin[idim1]<<" xmax["<<idim1<<"]="<<xmax[idim1]
	       <<" xmin["<<idim2<<"]="<<xmin[idim2]<<" xmax["<<idim2<<"]="<<xmax[idim2]
	       <<std::endl;
   }

   TH2D* h1=(TH2D*)gDirectory->Get(hname);
   if (h1) delete h1;
   h1= new TH2D(hname, htit, nbin, xmin[idim1], xmax[idim2], nbin, xmin[idim1], xmax[idim2]);

   if (!h1) std::cout<<name<<"ERROR: Can not create histo"<<hname<<std::endl;
   else if (debug) h1->Print();

   if (debug) std::cout << "histogram created/ loaded" << std::endl;

   TH1D *h1_px = h1->ProjectionX("_px", 1, nbin, ""); // exclude overflow and underflow bin
   TH1D *h1_py = h1->ProjectionY("_py", 1, nbin, "");

   Double_t xvec[GetTotDim()];
   for(Long_t i=0; i<GetTotDim(); i++) 
      xvec[i] = 0.;

   // loop over all bins
   for(Int_t ibinx=1; ibinx<=nbin; ibinx++) {    //loop over  x-bins
      for(Int_t ibiny=1; ibiny<=nbin; ibiny++) { //loop over y-bins
	 xvec[idim1]=h1_px->GetBinCenter(ibinx); // fill idim1 and idim2 correctly
	 xvec[idim2]=h1_py->GetBinCenter(ibiny); // (the other dimesions are uninteresting)

	 // transform xvec
	 Double_t txvec[GetTotDim()];
	 for(Long_t i=0; i<GetTotDim(); i++) 
	    txvec[i]=this->VarTransform(i, xvec[i]);

	 Int_t ibin=h1->GetBin(ibinx, ibiny);

	 for(Long_t iCell=0; iCell<=fLastCe; iCell++) { // loop over all active cells
	    
	    if(!(fCells[iCell]->GetStat())) continue; // cell not active -> continue
	    
	    // get cell position and dimesions
	    TFoamVect  cellPosi(GetTotDim()), cellSize(GetTotDim());
	    fCells[iCell]->GetHcub(cellPosi,cellSize);

	    // compare them with txvec
	    const Double_t xsmall = 1.e-10; 
	    if (!( (txvec[idim1]>cellPosi[idim1]-xsmall) && (txvec[idim1]<=cellPosi[idim1]+cellSize[idim1]+xsmall) &&
		   (txvec[idim2]>cellPosi[idim2]-xsmall) && (txvec[idim2]<=cellPosi[idim2]+cellSize[idim2]+xsmall)  ) )
	       continue; 

	    Double_t vol = fCells[iCell]->GetVolume();
	    if (vol<1e-10) {
	       std::cout << name <<"PDEFoam::Project: ERROR: Volume too small! " <<std::endl; 
	       continue;
	    }

	    // get cell value (depending on the option)
	    Double_t var = 0.;

	    if (strcmp(opt,"mean")==0 || strcmp(opt,"nev")==0)
	       //var = GetCellMean(fCells[iCell]); 
	       var = GetCellEntries(fCells[iCell]); 
	    else if (strcmp(opt,"rms")==0) 
	       var = GetCellRMS(fCells[iCell]);
	    else if (strcmp(opt,"discr")==0)
	       var = GetCellDiscr(fCells[iCell]);
	    else if (strcmp(opt,"discr_error")==0)
	       var = GetCellDiscrError(fCells[iCell]);
	    else
	       std::cout << "PDEFoam::Project: ERROR: unknown option: " << opt << std::endl;	    

	    // filling value to histogram
	    //h1->AddBinContent(ibin, var/vol);
	    h1->SetBinContent(ibin, (var) + h1->GetBinContent(ibin));

	    if (debug) {
	       std::cout << "ibin=" << ibin << " ibinx=" << ibinx << " ibiny=" << ibiny 
			 << " cellID=" << iCell
			 << " xvec=["<<xvec[idim1]<<", "<<xvec[idim2]<<"] "<<" value="<< var
			 << " txvec=["<<txvec[idim1]<<", "<<txvec[idim2]<<"]" << std::endl; 
	    }
	 }
      }
   }
   return h1;
}

//_________________________________________________________________________________________
TH2D* TMVA::PDEFoam::ProjectMC(Int_t idim1, Int_t idim2, char *opt, Int_t nevents, Int_t nbin)
{ // project variable idim1 and variable idim2 to histogram by using MC generation
  // Option: Project mean, rms, nev, discr, discr_error
  // NOT TESTED JET !

   char hname[100]; char htit[100];
   sprintf(htit,"%s var%d vs var%d",opt,idim1,idim2);
   sprintf(hname,"h%s_%d_vs_%d",opt,idim1,idim2);
   
   TH2D* h1 = (TH2D*)gDirectory->Get(hname);
   if (h1) delete h1;
   h1= new TH2D(hname, htit, nbin, xmin[idim1], xmax[idim2], nbin, xmin[idim1], xmax[idim2]);

   Double_t *MCvect = new Double_t[GetTotDim()];
   
   // generate MC events according to foam
   for(Long_t loop=0; loop<nevents; loop++){
      MakeEvent();                              // reades fPrimary (from cell buildup)
      GetMCvect(MCvect);
      h1->Fill(MCvect[idim1], MCvect[idim2]);
   }

   return h1;
}

//_________________________________________________________________________________________
// !!! test function !!!
void TMVA::PDEFoam::InitCellVariables(){
   for(Long_t iCell=0; iCell<=fLastCe; iCell++) { // loop over all active cells
      if(!(fCells[iCell]->GetStat())) continue;   // cell not active -> continue

      // works:
//       std::stringstream name;
//       std::stringstream capt;
//       name << "hist" << iCell;
//       capt << "histogramm of cell " << iCell;
//       TH1D *h1 = new TH1D(name.str().c_str(), capt.str().c_str(), 100, -3., 3.);
//       h1->FillRandom("gaus", 10000);
//       fCells[iCell]->SetElement(h1);

      TVectorD *vec = new TVectorD(3);
      (*vec)(0) = iCell;
      (*vec)(1) = iCell + 1;
      (*vec)(2) = iCell - 1;
      fCells[iCell]->SetElement(vec);
   }
}

//_________________________________________________________________________________________
// !!! test function !!!
TVectorD* TMVA::PDEFoam::GetFirstCellElement(){
   TObject *ref=0;

   for(Long_t iCell=0; iCell<=fLastCe; iCell++) { // loop over all active cells
      if(!(fCells[iCell]->GetStat())) continue;   // cell not active -> continue
      ref = fCells[iCell]->GetElement();
      if (ref==0) continue;
   }
   return dynamic_cast<TVectorD*>(ref);
}

//_________________________________________________________________________________________
void TMVA::PDEFoam::OutputGrow(Bool_t finished){
   Int_t modulo = Int_t(fNCells/100);
   if (fLastCe%modulo == 0) timer->DrawProgressBar( fLastCe );
}

//_________________________________________________________________________________________
void TMVA::PDEFoam::RootPlot2dim(Char_t *filename, std::string what, Bool_t CreateCanvas = kTRUE)
//void PDEFoam::RootPlot2dim(std::ofstream& outfile, std::string what)
{
   // Debugging tool which plots 2-dimensional cells as rectangles
   // in C++ format readable for root

   bool plotmean = false;
   bool plotnevents = false;
   bool plotrms = false;
   bool plotcellnumber = false;
   bool plotdiscr = false;     // plot discriminator (Tancredi Carli's Method)
   bool plotdiscrerr = false;  // plot discriminator error (Tancredi Carli's Method)
   bool fillcells = true;

   if (what == "mean")
      plotmean = true;
   else if (what == "nevents")
      plotnevents = true;
   else if (what == "rms")
      plotrms = true;
   else if (what == "cellnumber")
      plotcellnumber = true;
   else if (what == "discr")
      plotdiscr = true;
   else if (what == "discrerr")
      plotdiscrerr = true;
   else if (what == "nofill") {
      plotcellnumber = true;
      fillcells = false;
   }
   else {
      plotmean = true;
      std::cerr << "WARNING: Unknown option, plotting mean!" << std::endl;
   }

//    std::cerr << "fLastCe=" << fLastCe << std::endl;

   std::ofstream outfile(filename, std::ios::out);
   Double_t x1,y1,x2,y2,x,y;
   Long_t   iCell;
   Double_t offs =0.1;
   Double_t lpag   =1-2*offs;
   outfile<<"{" << std::endl;
   //outfile<<"cMap = new TCanvas(\"Map1\",\" Cell Map \",600,600);"<<std::endl;
   if (CreateCanvas) 
      outfile<<"cMap = new TCanvas(\""<< fName <<"\",\"Cell Map for "<< fName <<"\",600,600);"<<std::endl;
   //
   outfile<<"TBox*a=new TBox();"<<std::endl;
   outfile<<"a->SetFillStyle(0);"<<std::endl;  // big frame
   outfile<<"a->SetLineWidth(4);"<<std::endl;
   outfile<<"a->SetLineColor(2);"<<std::endl;
   outfile<<"a->DrawBox("<<offs<<","<<offs<<","<<(offs+lpag)<<","<<(offs+lpag)<<");"<<std::endl;
   //
   outfile<<"TText*t=new TText();"<<std::endl;  // text for numbering
   outfile<<"t->SetTextColor(4);"<<std::endl;
   if(fLastCe<51)
      outfile<<"t->SetTextSize(0.025);"<<std::endl;  // text for numbering
   else if(fLastCe<251)
      outfile<<"t->SetTextSize(0.015);"<<std::endl;
   else
      outfile<<"t->SetTextSize(0.008);"<<std::endl;
   //
   outfile<<"TBox *b1=new TBox();"<<std::endl;  // single cell
   if (fillcells) {
      outfile <<"gStyle->SetPalette(1, 0);"<<std::endl;
      outfile <<"b1->SetFillStyle(1001);"<<std::endl;
      outfile<<"TBox *b2=new TBox();"<<std::endl;  // single cell
      outfile <<"b2->SetFillStyle(0);"<<std::endl;     
   }
   else {
      outfile <<"b1->SetFillStyle(0);"<<std::endl;
   }
   //

   int lastcell = fLastCe;
   //   if (lastcell > 10000)
   //     lastcell = 10000;

   if (fillcells)
      gStyle->SetPalette(1, 0);

   Double_t zmin = 1.;  // minimal value (for color calculation)
   Double_t zmax = 1.6; // maximal value (for color calculation)

   if (plotmean) {
      zmin = 1.;
      zmax = 1.6;
   }
   else if (plotnevents) {
      zmin = 9999.;
      zmax = -9999.;
      Double_t value=0.;
      for(iCell=1; iCell<=lastcell; iCell++) {
	 if( fCells[iCell]->GetStat() == 1) {
	    //value = fCells[iCell]->GetEntries();
	    value = GetCellEntries(fCells[iCell]);
	    if (value<zmin)
	       zmin=value;
	    if (value>zmax)
	       zmax=value;
	 }
      }
   }
   else if (plotrms) {
      zmin = 0;
      zmax = 2;
   }
   else if (plotcellnumber) {
      zmin = 0;
      zmax = lastcell;
   }
   else if (plotdiscr) {
      zmin = 0.; // minimal value for discriminator
      zmax = 1.; // maximal value for discriminator
   }
   else if (plotdiscrerr) {
      zmin = 0.;             // minimal value for discriminator error
      zmax = TMath::Sqrt(2); // maximal value for discriminator error
   }

// 	std::cerr << "zmin=" << zmin << std::endl;
// 	std::cerr << "zmax=" << zmax << std::endl;
   	      
   // Next lines from THistPainter.cxx
   Int_t ndivz = abs(gStyle->GetNumberContours());
   Double_t dz = zmax - zmin;
   Double_t scale = ndivz/dz;
   
   Int_t ncolors  = gStyle->GetNumberOfColors();

// 	std::cerr << "ndivz=" << ndivz
// 		  << ", dz=" << dz
// 		  << ", scale=" << scale
// 		  << ", ncolors=" << ncolors 
// 		  << ", fDim=" << GetTotDim() << std::endl;

   if(GetTotDim()==2) {
      TFoamVect  cellPosi(GetTotDim()); TFoamVect  cellSize(GetTotDim());
      outfile << "// =========== Rectangular cells  ==========="<< std::endl;
      for(iCell=1; iCell<=lastcell; iCell++) {
	 if( fCells[iCell]->GetStat() == 1) {
	    fCells[iCell]->GetHcub(cellPosi,cellSize);
	    x1 = offs+lpag*(cellPosi[0]);             y1 = offs+lpag*(cellPosi[1]);
	    x2 = offs+lpag*(cellPosi[0]+cellSize[0]); y2 = offs+lpag*(cellPosi[1]+cellSize[1]);

	    if (fillcells) {
	       Double_t value = 0;

	       if (plotmean) {
		  value = GetCellMean(fCells[iCell]);
	       }
	       else if (plotnevents) {
		  value = GetCellEntries(fCells[iCell]);
	       }
	       else if (plotrms) {
		  value = GetCellRMS(fCells[iCell]);
	       }
	       else if (plotdiscr) {
		  value = GetCellDiscr(fCells[iCell]);
	       }
	       else if (plotdiscrerr) {
		  value = GetCellDiscrError(fCells[iCell]);
	       }
	       else if (plotcellnumber) {
		  value = iCell;
	       }      

	       Int_t color    = Int_t(0.01+(value-zmin)*scale);
	       Int_t thecolor = Int_t((color+0.99)*Float_t(ncolors)/Float_t(ndivz));
	       if (thecolor > ncolors-1) thecolor = ncolors-1;

	       outfile << "b1->SetFillColor(" << gStyle->GetColorPalette(thecolor) << ");" << std::endl;
	       //outfile << "b->SetFillColor(2);" << endl;
	    }

	    //     cell rectangle
	    //            if(lastcell<=2000)
	    outfile<<"b1->DrawBox("<<x1<<","<<y1<<","<<x2<<","<<y2<<");"<<std::endl;
	    if (fillcells)
	       outfile<<"b2->DrawBox("<<x1<<","<<y1<<","<<x2<<","<<y2<<");"<<std::endl;


	    //     cell number
	    if(lastcell<=250) {
	       x = offs+lpag*(cellPosi[0]+0.5*cellSize[0]); y = offs+lpag*(cellPosi[1]+0.5*cellSize[1]);
	       outfile<<"t->DrawText("<<x<<","<<y<<","<<"\""<<iCell<<"\""<<");"<<std::endl;
	    }
	 }
      }
      outfile<<"// ============== End Rectangles ==========="<< std::endl;
   }//kDim=2
   //
   //
   outfile << "}" << std::endl;
   outfile.flush();
   outfile.close();
}

//________________________________________________________________________________________________
void TMVA::PDEFoam::SetVolumeFraction(Double_t vfr){ 
   // set VolFrac to TFDISTR
   fdistr->SetVolumeFraction(vfr);
};

//________________________________________________________________________________________________
void TMVA::PDEFoam::InputEvent(std::vector<Double_t>& eigvecprojs, Bool_t isSignal, Double_t weight, Double_t boostweight){
   // input event to TFDISTR
   fdistr->InputEvent(eigvecprojs, isSignal, weight, boostweight);
};

//________________________________________________________________________________________________
void TMVA::PDEFoam::Create(){ // create TFoam
   SetRho(fdistr);      // give filled distribution to Foam
   TFoam::Initialize(); // create foam cells from fdistr
}; 

//________________________________________________________________________________________________
void TMVA::PDEFoam::Init(){ 
   // initialize TFDISTR
   fdistr->Initialize(GetTotDim());
};

//________________________________________________________________________________________________
void TMVA::PDEFoam::FillDiscriminator(Bool_t val){
   fdistr->FillDiscriminator(val);
};

//________________________________________________________________________________________________
void TMVA::PDEFoam::PrintDensity(){
   // print density
   fdistr->PrintDensity();
};
 
//________________________________________________________________________________________________
ostream& TMVA::operator<< (ostream& os, const TMVA::PDEFoam& pdefoam)
{ 
   // write PDEFoam
   pdefoam.PrintStream(os);
   return os; // Return the output stream.
}

//________________________________________________________________________________________________
istream& TMVA::operator>> (istream& istr, TMVA::PDEFoam& pdefoam)
{ 
   // read PDEFoam
   pdefoam.ReadStream(istr);
   return istr;
}

//_______________________________________________________________________
void TMVA::PDEFoam::ReadStream(istream & istr)
{
   // read Class Variables: xmin, xmax
   for(Int_t i=0; i<GetTotDim(); i++) 
      istr >> xmin[i];
   for(Int_t i=0; i<GetTotDim(); i++) 
      istr >> xmax[i];

   // inherited class variables: fLastCe, fNCells, fDim[GetTotDim()]
   istr >> fLastCe;
   istr >> fNCells;
   istr >> fDim;

   std::cout << "PDEFoam::ReadStream: fdim=" << fDim << std::endl;

   // all cells: **fCells (loop)
//    for(Long_t iCell=0; iCell<=fLastCe; iCell++) {
//       // ?
//    }
}
//_______________________________________________________________________
void TMVA::PDEFoam::PrintStream(ostream & ostr) const
{
   // write class variables: xmin, xmax
   for(Int_t i=0; i<GetTotDim(); i++) 
      ostr << xmin[i] << std::endl;
   for(Int_t i=0; i<GetTotDim(); i++) 
      ostr << xmax[i] << std::endl;

   // inherited class variables: fLastCe, fNCells, fDim[GetTotDim()]
   ostr << fLastCe << std::endl;
   ostr << fNCells << std::endl;
   ostr << fDim    << std::endl;

   // all cells: **fCells (loop)
//    for(Long_t iCell=0; iCell<=fLastCe; iCell++) {
//       ostr << fCells[iCell]->GetPrim() << std::endl;
//       ostr << fCells[iCell]->GetIntg() << std::endl;
//       ostr << fCells[iCell]->GetDriv() << std::endl;
//       ostr << fCells[iCell]->GetVolume() << std::endl; // exists no setter!!
//       ostr << fCells[iCell]->GetBest() << std::endl;
//       ostr << fCells[iCell]->GetXdiv() << std::endl;
//       ostr << fCells[iCell]->GetStat() << std::endl;
//       ostr << fCells[iCell]->GetSerial() << std::endl;
//       ostr << fCells[iCell]->fDim << std::endl; // ???
//       ostr << fCells[iCell]-> << std::endl;
//    }
}

////////////////////////////////////////////////////////////////////////////////
//       End of Class TFoam                                                   //
////////////////////////////////////////////////////////////////////////////////

ClassImp(TMVA::NodeAndDistance);

TMVA::NodeAndDistance::NodeAndDistance(const TMVA::BinarySearchTreeNode* thenode, int nDim, Double_t *Xarg){
   node = thenode;
   distance = 0;

   const std::vector<Float_t>& values = node->GetEventV();

   for (Int_t idim = 0; idim < nDim; idim++) {
      //      if (debug2)
      //	std::cerr << "values[" << idim << "] = " << values[idim] << std::endl;
      
      distance += TMath::Power(Xarg[idim] - values[idim], 2);
   }
};

////////////////////////////////////////////////////////////////////////////////
//       Begin of Class TFDISTR                                               //
////////////////////////////////////////////////////////////////////////////////

//#define DEBUG

//________________________________________________________________________________________________
void TMVA::TFDISTR::PrintDensity(){

   char name[]={"EventfromDensity::PrintDensity"};

   std::cout<<name<<" filename= "<<filename<<std::endl;
   std::cout<<name<<" treename= "<<treename<<std::endl;
   std::cout<<name<<" Volume fraction to search for events: "<<volfraction<<std::endl;
   //  cout<<name<<" ilay= "<<ilay<<endl;
   //cout<<name<<" Variable: "<<varname<<"["<<ilay<<"]="<<endl;
   std::cout<<name<<" Binary Tree: "<<bst<<std::endl;

   if (!bst) std::cout<<name<<" Binary tree not found ! "<<std::endl;

   std::cout<<name<<" volfraction= "<<volfraction<<std::endl;

   for (Int_t idim=0; idim<kDim; idim++){
      std::cout<<name<<idim<<" xmin["<<idim<<"]= "<<xmin[idim]
	       <<" xmax["<<idim<<"]= "<<xmax[idim]<<std::endl;
   }
}

//________________________________________________________________________________________________
void TMVA::TFDISTR::Initialize(Int_t ndim){//init function
   std::string name = "EventfromDensity::PreInitialize:";
#ifndef isTMVA
   std::cout<<name<<" Dimensions= "<<ndim<<std::endl;
   std::cout<<name<<" Volume fraction to search for events: "<<volfraction<<std::endl;
#endif
   this->SetDim(ndim);
 
   for (Int_t idim=0; idim<kDim; idim++) {
      char vname[100]; sprintf(vname,"Eig%d",idim);
//#ifndef isTMVA
//    std::cout << name << " newtree Variables: " << vname << std::endl;
//    vars.push_back(TMVA::VariableInfo(vname, vars.size()+1, 'F', 0, 0, 0 ));
//#else
      vars.push_back(0.); // fill with zeros
//#endif
   }
//#ifndef isTMVA
//  vars.push_back(TMVA::VariableInfo("weight", vars.size()+1, 'F', 0, 0, 0 )); // not necessary in new TMVA framework
//#endif

   if (bst) delete bst;
   bst = new TMVA::BinarySearchTree();
  
#ifndef isTMVA
   if (!bst)
      std::cout << name << " can not create binary tree !" << std::endl;
   else
      std::cout << name << " Binary tree created" << std::endl;
#endif
  
   bst->SetPeriode(kDim + 1);
}

//________________________________________________________________________________________________
void TMVA::TFDISTR::InputEvent(std::vector<Double_t>& eigvecprojs, Bool_t isSignal, Double_t weight, Double_t boostweight) {
//#ifndef isTMVA
//  TMVA::Event *event = new TMVA::Event(vars);
//#else
   TMVA::Event *event = new TMVA::Event(vars, isSignal, weight, boostweight);
//#endif

   // eigvecprojs contains all variables for that event: [x1,...,x_kdim]
   // eigvecprojs.size() == kdim
   for (unsigned i = 0; i < eigvecprojs.size(); i++)
      event->SetVal(i, eigvecprojs[i]); // copy eigvecprojs[] to 'event'

//#ifndef isTMVA
//  event->SetVal(kDim, weight); // set last variable in 'event' to weight
//#endif

   bst->Insert(event); // insert to BinarySearchTree
}

//________________________________________________________________________________________________
bool operator< (const TMVA::NodeAndDistance& x, const TMVA::NodeAndDistance& y) {
   return (x.distance) < (y.distance);
}

//________________________________________________________________________________________________
// DD 10.Jan.2008
// new method to fill edge histograms directly, without MC sampling
void TMVA::TFDISTR::FillEdgeHist(Int_t nDim, TObjArray *myfHistEdg , Double_t *cellPosi, Double_t *cellSize, Double_t *ceSum, Double_t ceVol) {

   //  cout << "inside FillEdgeHist" << endl;

   Double_t wt;
   std::vector<Double_t> lb(nDim+1);
   std::vector<Double_t> ub(nDim+1);
   Double_t valuesarr[nDim];
  
   for (Int_t idim=0; idim<nDim; idim++){
      lb[idim]=xmin[idim]+(xmax[idim]-xmin[idim])*cellPosi[idim];
      ub[idim]=xmin[idim]+(xmax[idim]-xmin[idim])*(cellPosi[idim]+cellSize[idim]);
      //    cout << idim << " " << lb[idim] << " " << ub[idim] << endl;
   }
   lb[nDim] = -1.e19;
   ub[nDim] = +1.e19;

   TMVA::Volume volume(&lb, &ub);
   std::vector<const TMVA::BinarySearchTreeNode*> nodes;
   if (!bst) {
      std::cout<<"PDEFoam-DEBUG: Binary tree not found ! "<<std::endl; 
      exit (1);
   };
   bst->SearchVolume(&volume, &nodes);
   //  cout << "nodes.size: " << nodes.size() << endl;
   for (std::vector<const TMVA::BinarySearchTreeNode*>::iterator inode = nodes.begin(); inode != nodes.end(); inode++) {
      const std::vector<Float_t>& values = (*inode)->GetEventV();
      for (Int_t idim=0; idim<nDim; idim++) {
	 valuesarr[idim]=(values[idim]-xmin[idim])/(xmax[idim]-xmin[idim]);
	 //      cout << " " << valuesarr[idim] << endl;
      }
      wt = (TFDISTR::Density(nDim, valuesarr)) * ceVol;
      for (Int_t idim=0; idim<nDim; idim++) 
	 ((TH1D *)(*myfHistEdg)[idim])->Fill((values[idim]-xmin[idim])/(xmax[idim]-xmin[idim]), wt);
      ceSum[0] += wt;    // sum of weights
      ceSum[1] += wt*wt; // sum of weights squared
      ceSum[2]++;        // sum of 1
      if (ceSum[3]>wt) ceSum[3]=wt;  // minimum weight;
      if (ceSum[4]<wt) ceSum[4]=wt;  // maximum weight
      //    cout << ceSum[2] << " " << wt << " " << ceVol << endl;
   }
}

//________________________________________________________________________________________________
Double_t TMVA::TFDISTR::Density(int nDim, Double_t *Xarg){
   assert(nDim == kDim);

   Bool_t simplesearch=true; // constant volume for all searches
   Bool_t usegauss=true;     // false: use Teepee function, true: use Gauss with sigma=0.3
   Int_t targetevents=0;     // if >0: density is defined as inverse of volume that includes targetevents
   //                           (this option should probably not be used together simplesearch=false) 
   char name[]={" DensityFromEvents: "};
   bool debug0 =false;

   bool debug=false;
   //bool debug=true;
   //bool debug2=true;

   Double_t volscale=1.;
   Double_t lasthighvolscale=0.;
   Double_t lastlowvolscale=0.;
   Int_t nit=0;

   // probevolume relative to hypercube with edge length 1:
   Double_t probevolume_inv;

   static Int_t nev=0;  nev++;
   //
   //  if (debug) cout<<name<<" ilay= "<<ilay<<" New Event: nev= "<<nev<<endl;

   //make a variable transform, since Foam only knows about x=[0,1]

   if (debug) for (Int_t idim=0; idim<nDim; idim++) 
      std::cout<<nev<<name<<idim<<" unscaled in: Xarg= "<<Xarg[idim]<<std::endl;   

   Double_t wbin;
   //    Double_t volbin=0.;
   for (Int_t idim=0; idim<nDim; idim++){
      if (debug)
	 std::cerr << "xmin[" << idim << "] = " << xmin[idim]
		   << ", xmax[" << idim << "] = " << xmax[idim] << std::endl;
      wbin=xmax[idim]-xmin[idim]; //volbin+=wbin;
      Xarg[idim]=xmin[idim]+wbin*Xarg[idim];
   }

   if (debug0) for (Int_t idim=0; idim<nDim; idim++) 
      std::cout<<nev<<name<<idim<<" scaled in: Xarg= "<<Xarg[idim]<<std::endl;   

   //create volume around point to be found

   std::vector<Double_t> lb(nDim + 1);
   std::vector<Double_t> ub(nDim + 1);

volumesearchstart:
   probevolume_inv = pow((volfraction/2/volscale), nDim); 

   for (Int_t idim = 0; idim < kDim; idim++) {
      lb[idim] = Xarg[idim];
      ub[idim] = Xarg[idim];
   }

   for (Int_t idim = 0; idim < nDim; idim++) {
      Double_t volsize=(xmax[idim] - xmin[idim]) / volfraction * volscale;    
      lb[idim] -= volsize;
      ub[idim] += volsize;
      if (debug) {
	 std::cerr << "lb[" << idim << "]=" << lb[idim] << std::endl;
	 std::cerr << "ub[" << idim << "]=" << ub[idim] << std::endl;
      }
   }
   lb[kDim] = -1.e19;
   ub[kDim] = +1.e19;

   TMVA::Volume volume(&lb, &ub);
      
   std::vector<const TMVA::BinarySearchTreeNode*> nodes;

   if (!bst) {
      std::cout<<name<<" Binary tree not found ! "<<std::endl; 
      exit (1);
   };
   //  static Int_t bst_size=bst->GetNNodes();
   bst->SearchVolume(&volume, &nodes);

   // normalized density: (number of counted events) / volume / (total number of events)
   // should be ~1 on average
   Double_t count=(Double_t) (nodes.size()); // number of events found
   // add a small number, to stabilize cell-division algorithm

   if (targetevents>0 && nit<25) {
      nit++;
      if (count<targetevents) {   // repeat search with increased volume if not enough events found
	 lastlowvolscale=volscale;
	 if (lasthighvolscale>0.)
	    volscale+=(lasthighvolscale-volscale)/2;
	 else
	    volscale*=1.5;
	 goto volumesearchstart;
      }
      else if (count>targetevents) { // repeat search with decreased volume if too many events found
	 lasthighvolscale=volscale;
	 if (lastlowvolscale>0.)
	    volscale-=(volscale-lastlowvolscale)/2;
	 else
	    volscale*=0.5;
	 goto volumesearchstart;
      }
   }

   if (targetevents>0 && count!=targetevents)
      std::cout << "WARNING: number of target events not reached within 25 iterations. count==" << count << std::endl;

   //   //  cout << volscale << " " <<count << endl;

   //   return (count/volscale); 

   //  std::vector<NodeAndDistance> nodesanddistances;
   //  for (vector<const TMVA::BinarySearchTreeNode*>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
   //    nodesanddistances.push_back(NodeAndDistance(*inode, nDim, Xarg));
  
   //   //  if (debug)
   //   //    std::cerr << "Number of points found in volume: " << nodes.end() - nodes.begin() << std::endl;

   //  std::sort(nodesanddistances.begin(), nodesanddistances.end());
   // sorting not needed, since we take all events anyway

   Double_t N_sig = 0;
   if (filldiscr){ // calc number of signal events in nodes
      N_sig = 0;
      // sum over all nodes->IsSignal;
      for(Int_t j=0; j<count; j++)  N_sig += (nodes.at(j))->IsSignal();
   }

   if (simplesearch){
      if (filldiscr)
	 return (N_sig/(count+0.1))*probevolume_inv; // fill:  (N_sig/N_total) / (cell_volume)
      else 
	 return ((count+0.1)*probevolume_inv); // fill:  N_total / cell_volume
   }
  
   Double_t density = 0.;
   Double_t normalised_distance = 0;

   if (count==0)
      return (0.1*probevolume_inv);

   //  Int_t usednodes;
   unsigned usednodes;

   for (usednodes = 0; usednodes < nodes.size(); usednodes++) { // loop over all nodes
      const std::vector<Float_t>& values = nodes[usednodes]->GetEventV();
      for (Int_t idim = 0; idim < nDim; idim++) 
	 normalised_distance += pow( ( values[idim]-Xarg[idim] )/ (ub[idim]-lb[idim]) * 2 , 2);
      normalised_distance = sqrt(normalised_distance/nDim);
      if (usegauss)
	 density+=TMath::Gaus(normalised_distance, 0, 0.3, kFALSE);    // Gaus Kernel with sigma=0.3
      else  
	 density+=(1-normalised_distance);  // Teepee kernel
   }
   density /= usednodes;

   if (filldiscr)
      density = (N_sig/density)*probevolume_inv; // fill:  (N_sig/N_total) / (cell_volume)
   else 
      density *= probevolume_inv; // fill:  N_total / cell_volume

   return density;

   //   if (debug0) cout<<"The weight is "<<weight<<endl;

   //   return weight;
   //   //return Xarg[0] + 2*Xarg[1];
}

//________________________________________________________________________________________________
TH2D* TMVA::TFDISTR::MakeHistogram(Int_t nbinsx, Int_t nbinsy) {
   // Only works in 2D for now

   //  const bool debug = true;
   const bool debug = false;

   TH2D *foamhist = new TH2D("foamhist", "Distributon",
			     nbinsx, xmin[0], xmax[0],
			     nbinsy, xmin[1], xmax[1]);
   if (debug)
      std::cerr << "TFDIST::MakeHistogram, nbinsx=" << nbinsx
		<< ", nbinsy=" << nbinsy
		<< ", xmin[0]=" << xmin[0]
		<< ", xmax[0]=" << xmax[0]
		<< ", xmin[1]=" << xmin[1]
		<< ", xmax[1]=" << xmax[1]
		<< std::endl;

   Double_t xvec[2];

   for (Int_t i = 1; i < nbinsx+1; i++) { // ROOT's weird bin numbering
      for (Int_t j = 1; j < nbinsy+1; j++) {
	 xvec[0] = foamhist->GetXaxis()->GetBinCenter(i);
	 xvec[1] = foamhist->GetYaxis()->GetBinCenter(j);

	 // Transform to foam-internal coordinate space, that this->Density expects
	 xvec[0] = (xvec[0] - xmin[0]) / (xmax[0] - xmin[0]);
	 xvec[1] = (xvec[1] - xmin[1]) / (xmax[1] - xmin[1]);

	 // Note: this->Density changes xvec, need to print them before!
	 if (debug)
	    std::cerr << "xvec[0]=" << xvec[0]
		      << ", xvec[1]=" << xvec[1]
		      << std::endl;

	 Int_t ibin = foamhist->GetBin(i, j);
	 Double_t var = this->Density(2, xvec);

	 foamhist->SetBinContent(ibin, var);

	 if (debug)
	    std::cerr << "i=" << i
		      << ", j=" << j
		      << ", var=" << var
		      << ", ibin=" << ibin
		      << std::endl;
      }
   }

   return foamhist;
}

//________________________________________________________________________________________________
// ostream& TMVA::operator<< (ostream& os, const TMVA::TFDISTR& distr)
// { 
//    // write TFDISTR
// //   os << fdistr;   
//    return os; // Return the output stream.
// }

//________________________________________________________________________________________________
// istream& TMVA::operator>> (istream& istr, TMVA::TFDISTR& distr)
// { 
//    // read TFDISTR from an istream
// //   istr >> fdistr;
//    return istr;
// }


ClassImp(TMVA::TFDISTR)

////////////////////////////////////////////////////////////////////////////////
//       End of Class TFDISTR                                                 //
////////////////////////////////////////////////////////////////////////////////
