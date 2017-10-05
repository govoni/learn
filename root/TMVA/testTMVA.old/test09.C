
class IClassifierReader ;

typedef struct {
   Float_t j1eta, j1phi, j1p ;
   Float_t j2eta, j2phi, j2p ;
   Float_t Deta, Dphi, Minv ;
} VARS ;

void makePlot (TH1F * histo, IClassifierReader * classifier,
               TTree * sample, std::string varName)
{

  VARS vars ;
//  sample->SetBranchAddress ("vars",&vars,"j1eta/F:j1phi/F:j1p/F:j2eta/F:j2phi/F:j2p/F:Deta/F:Dphi/F:Minv/F") ;
  sample->SetBranchAddress ("vars", &vars) ;
  

}


//PG ------------------------------------


int test09 ()
{
  TFile * output = new TFile ("./outputTMVA.root") ;
  
  //PG correlation among variables
  TH2F * CorrelationMatrixB = (TH2F *) output->Get ("CorrelationMatrixB") ;
  TH2F * CorrelationMatrixS = (TH2F *) output->Get ("CorrelationMatrixS") ;

  //PG network
  TH1F * MLP_effBvsS = (TH1F*) output->Get ("Method_MLP/MLP/MVA_MLP_effBvsS") ;

  //PG cuts
  TH1F * CUTS_effBvsS = (TH1F*) output->Get ("Method_Cuts/Cuts/MVA_Cuts_effBvsS") ;

  //PG efficiencies
  TCanvas * c1 = new TCanvas () ;

  MLP_effBvsS->GetXaxis ()->SetTitle ("signal efficiency") ;
  MLP_effBvsS->GetYaxis ()->SetTitle ("background efficiency") ;
  MLP_effBvsS->SetMarkerStyle (8) ;
  MLP_effBvsS->SetMarkerColor (8) ;
  MLP_effBvsS->SetStats (0) ;
  MLP_effBvsS->Draw ("P") ;

  CUTS_effBvsS->GetXaxis ()->SetTitle ("signal efficiency") ;
  CUTS_effBvsS->GetYaxis ()->SetTitle ("background efficiency") ;
  CUTS_effBvsS->SetMarkerStyle (24) ;
  CUTS_effBvsS->SetMarkerColor (9) ;
  CUTS_effBvsS->SetStats (0) ;
  CUTS_effBvsS->Draw ("Psame") ;

  TLegend* leg1 = new TLegend (0.2,0.8,0.4,0.89) ;
  leg1->AddEntry (MLP_effBvsS,"MLP","p") ;
  leg1->AddEntry (CUTS_effBvsS,"CUTS","p") ;
  leg1->Draw () ;
  
  c1->Update () ;

  gROOT->ProcessLine (".L weights/TMVAtest_Cuts.class.C") ;
  gROOT->ProcessLine (".L weights/TMVAtest_MLP.class.C") ;

  std::vector<std::string> vinputVars ;
  vinputVars.push_back ("vars.Deta") ;
  vinputVars.push_back ("vars.Dphi") ;
  vinputVars.push_back ("vars.Minv") ;

  ReadMLP MLPtester (vinputVars) ;
  ReadCuts CUTtester (vinputVars) ;

  TFile s_file ("data/H150_VBF.root") ;
  TTree * signal = (TTree *) s_file.Get ("shrinked") ;
  TFile b_file ("data/H150_NOVBF.root") ;
  TTree * bkg = (TTree *) b_file.Get ("shrinked") ;
  
  TH1F * HS_MLP_Deta = new TH1F ("HS_MLP_Deta", "HS_MLP_Deta", 100, 0., 7.) ;
  makePlot (&HS_MLP_Deta, &MLPtester, signal, "var.Deta") ;

}