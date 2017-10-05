// @(#)root/ged:$Name: v4-04-02g $:$Id: TGedToolBox.cxx,v 1.4 2004/04/22 16:28:28 brun Exp $
// Author: Marek Biskup, Ilka Antcheva 17/07/2003

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGedToolBox                                                          //
//                                                                      //
// A toolbox is a composite frame that contains TGPictureButtons        //
// arranged in a matrix 4x5                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGedToolBox.h"
#include "TList.h"
#include "TGButton.h"
#include "TGPicture.h"
#include "TGToolBar.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "Riostream.h"

ClassImp(TGedToolBox)

enum {
   kToolModify,
   kToolArc,
   kToolLine,
   kToolArrow,
   kToolButton,
   kToolDiamond,
   kToolEllipse,
   kToolPad,
   kToolPave,
   kToolPLabel,
   kToolPText,
   kToolPsText,
   kToolGraph,
   kToolCurlyLine,
   kToolCurlyArc,
   kToolLatex,
   kToolMarker,
   kToolCutG
};

static ToolBarData_t gToolBoxData[] = {
  // { filename,      tooltip,            staydown,  id,            button}
   { "pointer.xpm",    "Modify",           kFALSE,    kToolModify,   NULL },
   { "arc.xpm",        "Arc",              kFALSE,    kToolArc,      NULL },
   { "line.xpm",       "Line",             kFALSE,    kToolLine,     NULL },
   { "arrow.xpm",      "Arrow",            kFALSE,    kToolArrow,    NULL },
   { "button.xpm",     "Button",           kFALSE,    kToolButton,   NULL },
   { "diamond.xpm",    "Diamond",          kFALSE,    kToolDiamond,  NULL },
   { "ellipse.xpm",    "Ellipse",          kFALSE,    kToolEllipse,  NULL },
   { "pad.xpm",        "Pad",              kFALSE,    kToolPad,      NULL },
   { "pave.xpm",       "Pave",             kFALSE,    kToolPave,     NULL },
   { "pavelabel.xpm",  "Pave Label",       kFALSE,    kToolPLabel,   NULL },
   { "pavetext.xpm",   "Pave Text",        kFALSE,    kToolPText,    NULL },
   { "pavestext.xpm",  "Paves Text",       kFALSE,    kToolPsText,   NULL },
   { "graph.xpm",      "Graph",            kFALSE,    kToolGraph,    NULL },
   { "curlyline.xpm",  "Curly Line",       kFALSE,    kToolCurlyLine,NULL },
   { "curlyarc.xpm",   "Curly Arc",        kFALSE,    kToolCurlyArc, NULL },
   { "latex.xpm",      "Text/Latex",       kFALSE,    kToolLatex,    NULL },
   { "marker.xpm",     "Marker",           kFALSE,    kToolMarker,   NULL },
   { "cut.xpm",        "Graphical Cut",    kFALSE,    kToolCutG,     NULL },
   { 0,                0,                  kFALSE,    0,             NULL }
};
                    
	
//______________________________________________________________________________
TGedToolBox::TGedToolBox(const TGWindow *p, UInt_t w, 
                         UInt_t h, UInt_t options, ULong_t back) 
   : TGToolBar(p, w, h, options, back)
{
   // Create toolbox widget.

   SetLayoutManager(new TGMatrixLayout(this, 1, 0, 2, 2));
   CreateButtons(gToolBoxData);

}

//______________________________________________________________________________
void TGedToolBox::CreateButtons(ToolBarData_t buttons[])
{
   // Adds buttons described by buttons table to the ToolBox.
   // the last element of buttons[] table has zero in the filed
   // fPixmap.
    
   for (int i = 0; buttons[i].fPixmap; i++)
      AddButton(this, &buttons[i], 0);

   return;
}

//______________________________________________________________________________
Bool_t TGedToolBox::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.
   switch (GET_MSG(msg)) {

      case kC_COMMAND:
              
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:
                    
               switch(parm1) {
                 
                  case kToolModify:
                     gROOT->SetEditorMode();
                     break;
                  case kToolArc:
                     gROOT->SetEditorMode("Arc");
                     break;
                  case kToolLine:
                     gROOT->SetEditorMode("Line");
                     break;
                  case kToolArrow:
                     gROOT->SetEditorMode("Arrow");
                     break;
                  case kToolButton:
                     gROOT->SetEditorMode("Button");
                     break;
                  case kToolDiamond:
                     gROOT->SetEditorMode("Diamond");
                     break;
                  case kToolEllipse:
                     gROOT->SetEditorMode("Ellipse");
                     break;
                  case kToolPad:
                     gROOT->SetEditorMode("Pad");
                     break;
                  case kToolPave:
                     gROOT->SetEditorMode("Pave");
                     break;
                  case kToolPLabel:
                     gROOT->SetEditorMode("PaveLabel");
                     break;
                  case kToolPText:
                     gROOT->SetEditorMode("PaveText");
                     break;
                  case kToolPsText:
                     gROOT->SetEditorMode("PavesText");
                     break;
                  case kToolGraph:
                     gROOT->SetEditorMode("PolyLine");
                     break;
                  case kToolCurlyLine:
                     gROOT->SetEditorMode("CurlyLine");
                     break;
                  case kToolCurlyArc:
                     gROOT->SetEditorMode("CurlyArc");
                     break;
                  case kToolLatex:
                     gROOT->SetEditorMode("Text");
                     break;
                  case kToolMarker:
                     gROOT->SetEditorMode("Marker");
                     break;
                  case kToolCutG:
                     gROOT->SetEditorMode("CutG");
                     break;
                  default:
                     break;
               }  // GET_SUBMSG
               break;
            default:
               break;
         }
         default:
            break;
   } // GET_MSG
   return kTRUE;
}

//______________________________________________________________________________
void TGedToolBox::SavePrimitive(ofstream &out, Option_t *option)
{
   // Save the tool box widget as a C++ statement(s) on output stream out
   
   TGToolBar::SavePrimitive(out, option);
   
   // setting layout manager
   out << "   " << GetName() <<"->SetLayoutManager(";
   GetLayoutManager()->SavePrimitive(out,option);
   out << ");"<< endl;

   out << "   " << GetName() <<"->Resize();" << endl;
}
 