// @(#)root/tmva $Id: DumpOptionsReference.C,v 1.6 2008/05/16 21:19:18 andreas.hoecker Exp $
/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: DumpOptionsReference                                               *
 *                                                                                *
 * This macro instantiates calls all configurable classes and dumps               *
 * the reference for options. Also dumped (in the same output file) are the       *
 * help messages for classifier tuning.                                           *
 *                                                                                *
 * This macro works together with the script "CreateOptionRef.py" that creates    *
 * the corresponding html reference file.                                         *
 **********************************************************************************/

#include <iostream>

#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"

void DumpOptionsReference()
{
   std::cout << "Start DumpOptionsReference" << std::endl
             << "======================" << std::endl
             << std::endl;

   // Create a new root output file.
   TString outfileName( "Dummy.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory will
   // then run the performance analysis for you.
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/ 
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in 
   // front of the "Silent" argument in the option string
   // Write reference file for options
   (TMVA::gConfig()).SetWriteOptionsReference( kTRUE );

   TMVA::Factory *factory = new TMVA::Factory( "DumpOptionsReference", outputFile, "" );

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
   factory->AddVariable( "var1+var2", 'F' );
   factory->AddVariable( "var1-var2", 'F' );
   factory->AddVariable( "var3",      'F' );
   factory->AddVariable( "var4",      'F' );

   TFile *input(0);
    TString fname = "../macros/tmva_example.root";
    if (!gSystem->AccessPathName( fname )) {
       // first we try to find tmva_example.root in the local directory
       std::cout << "--- DumpOptionsReference    : Accessing " << fname << std::endl;
       input = TFile::Open( fname );
    } 
    if (!input) {
       std::cout << "ERROR: could not open data file" << std::endl;
       exit(1);
    }

    TTree *signal     = (TTree*)input->Get("TreeS");
    TTree *background = (TTree*)input->Get("TreeB");

    // ====== register trees ====================================================
    //
    // the following method is the prefered one:
    // you can add an arbitrary number of signal or background trees
    factory->AddSignalTree    ( signal,     1 );
    factory->AddBackgroundTree( background, 1 );
       
   // This would set individual event weights (the variables defined in the 
   // expression need to exist in the original TTree)
   //    for signal    : factory->SetSignalWeightExpression("weight1*weight2");
   //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");

   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";

   // tell the factory to use all remaining events in the trees after training for testing:
   factory->PrepareTrainingAndTestTree( mycuts, mycutb, "" );  

   // ---- Book MVA methods
   //
   // please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc.
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   // Cut optimisation
   factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
                        "H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   factory->BookMethod( TMVA::Types::kCuts, "CutsD", 
                        "H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

   factory->BookMethod( TMVA::Types::kCuts, "CutsPCA", 
                        "H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

   factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                        "H:!V:FitMethod=GA:EffSel:Steps=30:Cycles=3:PopSize=100:SC_steps=10:SC_rate=5:SC_factor=0.95:VarProp=FSmart" );
   
   factory->BookMethod( TMVA::Types::kCuts, "CutsSA",
                        "H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemperature=IncreasingAdaptive:InitialTemperature=1e+6:MinTemperature=1e-6:Eps=1e-10:UseDefaultScale" );
   
   // Likelihood
   factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood", 
                        "H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=10:NSmoothBkg[0]=10:NSmoothBkg[1]=10:NSmooth=10:NAvEvtPerBin=50" ); 

   // test the decorrelated likelihood
   factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD", 
                        "H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=10:NSmoothBkg[0]=10:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" ); 

   factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA", 
                        "H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=10:NSmoothBkg[0]=10:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 
 
   // test the new kernel density estimator
   factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE", 
                        "H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // test the mixed splines and kernel density estimator (depending on which variable)
   factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX", 
                        "H:!V:!TransformOutput:PDFInterpol[0]=KDE:PDFInterpol[1]=KDE:PDFInterpol[2]=Spline2:PDFInterpol[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // PDE - RS method
   factory->BookMethod( TMVA::Types::kPDERS, "PDERS", 
                        "H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );
   // And the options strings for the MinMax and RMS methods, respectively:
   //      "H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );   
   //      "H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );   

   factory->BookMethod( TMVA::Types::kPDERS, "PDERSkNN", 
                        "H:!V:VolumeRangeMode=kNN:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

   factory->BookMethod( TMVA::Types::kPDERS, "PDERSD", 
                        "H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

   factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA", 
                        "H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );
  
   // K-Nearest Neighbour classifier (KNN)
   factory->BookMethod( TMVA::Types::kKNN, "KNN", 
                        "H:nkNN=400:TreeOptDepth=6:ScaleFrac=0.8:!UseKernel:!Trim" );  

   // H-Matrix (chi2-squared) method
   factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "H:!V" ); 
   
   // Fisher discriminant
   factory->BookMethod( TMVA::Types::kFisher, "Fisher", 
                        "H:!V:!Normalise:CreateMVAPdfs:Fisher:NbinsMVAPdf=50:NsmoothMVAPdf=1" );

   // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
   factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                        "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );
   
   factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                        "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=100:Cycles=3:Steps=20:Trim=True:SaveBestGen=0" );

   factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                        "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:KernelTemperature=IncreasingAdaptive:InitialTemperature=1e+6:MinTemperature=1e-6:Eps=1e-10:UseDefaultScale" );

   factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                        "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

   // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
   factory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:!Normalise:NeuronType=tanh:NCycles=200:HiddenLayers=N+1,N:TestRate=5" );

   // CF(Clermont-Ferrand)ANN
   factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "H:!V:NCycles=500:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  
  
   // Tmlp(Root)ANN
   factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...
  
   // Support Vector Machines using three different Kernel types (Gauss, polynomial and linear)
   factory->BookMethod( TMVA::Types::kSVM, "SVM_Gauss", "H:Sigma=2:C=1:Tol=0.001:Kernel=Gauss" );

   // Boosted Decision Trees (second one with decorrelation)
   factory->BookMethod( TMVA::Types::kBDT, "BDT", 
                        "H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5" );

   // RuleFit -- TMVA implementation of Friedman's method
   factory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
                        "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

 
   // --------------------------------------------------------------------------------------------------

   // help messages are also dumped into files
   factory->PrintHelpMessage();
   
   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> DumpOptionsReference is done!" << std::endl;      

   delete factory;
}
