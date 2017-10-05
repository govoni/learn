#include <iostream>

#include "TControlBar.h"

void TMVAGui( const char* fName = "TMVA.root" ) 
{
   // Use this script in order to run the various individual macros
   // that plot the output of TMVA (e.g. running TMVAnalysis.C),
   // stored in the file "TMVA.root"
   // for further documentation, look in the individual macros


   cout << "--- Open TMVAGui for input file: " << fName << endl;
   
   //   gROOT->Reset();
   //   gStyle->SetScreenFactor(2); // if you have a large screen, select 1,2 or 1.4

   // create the control bar
   TControlBar * cbar = new TControlBar( "vertical", "Plotting Scripts", 0, 0 );

   const char* buttonType = "button";
   const char* scriptpath = "../macros";

   // configure buttons
   cbar->AddButton( "Input Variables",
                    Form( ".x %s/variables.C(\"%s\",0)", scriptpath, fName ),
                    "Plots all input variables (macro variables.C)",
                    buttonType );
   
   cbar->AddButton( "Decorrelated Variables",
                    Form( ".x %s/variables.C(\"%s\",1)", scriptpath, fName ),    
                    "Plots all decorrelated input variables (macro variables.C)",
                    buttonType );

   cbar->AddButton( "PCA-transformed Variables",
                    Form( ".x %s/variables.C(\"%s\",2)", scriptpath, fName ),    
                    "Plots all PCA-transformed input variables (macro variables.C)",
                    buttonType );

   cbar->AddButton( "Variable Correlations (scatter profiles)",
                    Form( ".x %s/correlationscatters.C\(\"%s\",0)", scriptpath, fName ), 
                    "Plots signal and background correlation profiles between all input variables (macro correlationscatters.C)",
                    buttonType );

   cbar->AddButton( "   Decorrelated-Variable Correlations (scatter profiles)   ",
                    Form( ".x %s/correlationscatters.C\(\"%s\",1)", scriptpath, fName ), 
                    "Plots signal and background correlation profiles between all decorrelated input variables (macro correlationscatters.C(1))",
                    buttonType );

   cbar->AddButton( "   PCA-transformed Variable Correlations (scatter profiles)   ",
                    Form( ".x %s/correlationscatters.C\(\"%s\",2)", scriptpath, fName ), 
                    "Plots signal and background correlation profiles between all PCA-transformed input variables (macro correlationscatters.C(2))",
                    buttonType );

   cbar->AddButton( "Variable Correlations (summary)",
                    Form( ".x %s/correlations.C(\"%s\")", scriptpath, fName ),
                    "Plots signal and background correlation summaries for all input variables (macro correlations.C)", 
                    buttonType );

   cbar->AddButton( "Output MVA Variables",
                    Form( ".x %s/mvas.C(\"%s\")", scriptpath, fName ),
                    "Plots the output variable of each method (macro mvas.C)",
                    buttonType );

   cbar->AddButton( "Mu-transforms (summary)",
                    Form( ".x %s/mutransform.C(\"%s\")", scriptpath, fName ),
                    "Plots the mu-transformed signal and background MVAs of each method (macro mutransform.C)",
                    buttonType );

   cbar->AddButton( "Background Rejection vs Signal Efficiencies",
                    Form( ".x %s/efficiencies.C(\"%s\")", scriptpath, fName ),
                    "Plots background rejection vs signal efficiencies (macro efficiencies.C)",
                    buttonType );

   cbar->AddButton( "Likelihood Reference Distributiuons (if exist)",
                    Form( ".x %s/likelihoodrefs.C(\"%s\")", scriptpath, fName ), 
                    "Plots to verify the likelihood reference distributions (macro likelihoodrefs.C)",
                    buttonType );

   cbar->AddButton( "Network Architecture (if exists)",
                    Form( ".x %s/network.C(\"%s\")", scriptpath, fName ), 
                   "Plots the MLP weights (macro network.C)",
                    buttonType );

   cbar->AddButton( "Network Convergence Test (if exists)",
                    Form( ".x %s/annconvergencetest.C(\"%s\")", scriptpath, fName ), 
                    "Plots error estimator versus training epoch for training and test samples (macro annconvergencetest.C)",
                    buttonType );

   cbar->AddButton( "Decision Tree (#1)",
                    Form( ".x %s/BDT.C", scriptpath, fName ),
                    "Plots the Decision Tree (#1); to plot other trees (i) call macro BDT.C(i) from command line",
                    buttonType );


   cbar->AddButton( "Quit",   ".q", "Quit", buttonType );

   // set the style 
   cbar->SetTextColor("black");

   // there seems to be a bug in ROOT: font jumps back to default after pressing on >2 different buttons
   // cbar->SetFont("-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
   
   // draw
   cbar->Show();



   gROOT->SaveContext();
}
