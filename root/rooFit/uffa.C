void uffa()
{
  using namespace RooFit;
  RooMsgService::instance().deleteStream(0);
  RooMsgService::instance().deleteStream(1);
  
  
  
  int nBins = 200;
  double xMin =  0.;
  double xMax = 10.;
  double xMin_signal = 4.;
  double xMax_signal = 6.;
  
  RooRealVar x("x","x",xMin,xMax);
  x.setRange("low",   xMin,       xMin_signal);
  x.setRange("signal",xMin_signal,xMax_signal);
  x.setRange("high",  xMax_signal,xMax);
  
  
  
  TF1* f_bkg = new TF1("f_bkg","exp(-0.25*x)",xMin,xMax);
  TF1* f_sig = new TF1("f_sig","exp(-(x-5.)*(x-5.)/(2*0.2*0.2))",xMin,xMax);
  
  
  
  TH1F* h_bkg = new TH1F("h_bkg","",nBins,xMin,xMax);
  h_bkg -> FillRandom("f_bkg",10000000);
  TH1F* h_sig = new TH1F("h_sig","",nBins,xMin,xMax);
  h_sig -> FillRandom("f_sig",10000000);  
  
  TCanvas* c_template = new TCanvas("c_template","templates",1000,500);
  c_template -> Divide(2);
  
  c_template -> cd(1);
  h_bkg -> Draw();

  c_template -> cd(2);
  h_sig -> Draw();  
  
  
  
  
  TH1F* h_data = new TH1F("h_data","",nBins/4,xMin,xMax);
  h_data -> Sumw2();
  RooDataSet* rooDATADataSet = new RooDataSet("rooDATADataSet","",RooArgSet(x));
  int nBkg = 9000;
  for(int i = 0; i < nBkg; ++i)
  {
    float r = f_bkg -> GetRandom();
    x = r;
    h_data -> Fill(r);
    rooDATADataSet -> add(RooArgSet(x));
  }
  int nSig = 1000;
  for(int i = 0; i < nSig; ++i)
  {
    float r = f_sig -> GetRandom();
    x = r;
    h_data -> Fill(r);
    rooDATADataSet -> add(RooArgSet(x));    
  }
  
  
  
  
  
  
  RooDataHist* rooBKGShapeHisto = new RooDataHist("rooBKGShapeHisto","",RooArgList(x),h_bkg);
  RooHistPdf* rooBKGPdf = new RooHistPdf("rooBKGPdf","",RooArgList(x),*rooBKGShapeHisto,0); 
  RooRealVar* rooNBKG = new RooRealVar("rooNBKG","",0.9,0.9,0.9);

  RooDataHist* rooSIGShapeHisto = new RooDataHist("rooSIGShapeHisto","",RooArgList(x),h_sig);
  RooHistPdf* rooSIGPdf = new RooHistPdf("rooSIGPdf","",RooArgList(x),*rooSIGShapeHisto,0); 
  RooRealVar* rooNSIG = new RooRealVar("rooNSIG","",0.1,0.1,0.1);  
  
  // somma con parametri fissati, il primo RooArgList e' la lista delle funzioni,
  // il secondo e' la lista dei parametri
  RooAddPdf* rooBKGSIGPdf = new RooAddPdf("rooBKGSIGPdf","",RooArgList(*rooBKGPdf,*rooSIGPdf),RooArgList(*rooNBKG,*rooNSIG));  
  
  // definisco il parametro totale per avere l'estensione
  RooRealVar* rooNTot = new RooRealVar("rooNTot","",1.,0.,1000000.);
  // definisco la pdf estesa
  RooExtendPdf* rooTotPdf = new RooExtendPdf("rooTotPdf","",*rooBKGSIGPdf,*rooNTot);
  //RooAddPdf* rooTotPdf = new RooAddPdf("rooTotPdf","",RooArgList(*rooBKGPdf),RooArgList(*rooNTot));
  
  
  
  //rooTotPdf -> fitTo(*rooDATADataSet,Extended(kTRUE));
  rooTotPdf -> fitTo(*rooDATADataSet,Extended(kTRUE),Range("low,high"));
  
  
  
  
  
  
  TCanvas* c_data = new TCanvas("c_data","data",1000,500);
  c_data -> Divide(2);
  
  
  c_data -> cd(1);
  RooPlot* rooDATAPlot = x.frame();
  rooDATADataSet -> plotOn(rooDATAPlot,MarkerSize(0.7),Binning(nBins/4));
  rooTotPdf -> plotOn(rooDATAPlot, LineColor(kRed));  
  rooDATAPlot->Draw();
  
  
  c_data -> cd(2);
  TH1F* h_data_fitSubtracted = (TH1F*)(h_data -> Clone("h_data_fitSubtracted"));
  
  RooAbsReal* rooSidebandIntegral = rooTotPdf -> createIntegral(x,NormSet(x),Range("low,high"));
  RooAbsReal* rooSignalIntegral   = rooTotPdf -> createIntegral(x,NormSet(x),Range("signal"));
  RooAbsReal* rooTotIntegral      = rooTotPdf -> createIntegral(x,NormSet(x));
  
  std::cout << ">>>>>> rooSidebandIntegral: " << rooSidebandIntegral -> getVal() << std::endl;
  std::cout << ">>>>>> rooSignalIntegral: "   << rooSignalIntegral -> getVal() << std::endl;
  std::cout << ">>>>>> rooTotIntegral: "      << rooTotIntegral -> getVal() << std::endl;  
  for(int bin = 1; bin <= h_data_fitSubtracted->GetNbinsX(); ++bin)
  {
    double xMin_bin = h_data_fitSubtracted->GetBinLowEdge(bin);
    double xMax_bin = h_data_fitSubtracted->GetBinLowEdge(bin) + h_data_fitSubtracted->GetBinWidth(bin);
    x.setRange("bin",xMin_bin,xMax_bin);
    
    RooAbsReal* rooBinIntegral = rooTotPdf -> createIntegral(x,NormSet(x),Range("bin"));
    float norm = rooBinIntegral -> getVal() * 10.;
    
    h_data_fitSubtracted -> SetBinContent(bin,h_data_fitSubtracted->GetBinContent(bin) - (norm*rooNTot->getVal()));
  }
  
  h_data_fitSubtracted -> Draw();
  
  
  
  
  std::cout << "****** FIT RESULTS ******" << std::endl;
  std::cout << ">>> nBkg (true): " << nBkg << "   nSig(true): " << nSig << std::endl;
  std::cout << ">>> nTot (fit): " << rooNTot -> getVal() << std::endl;
}
