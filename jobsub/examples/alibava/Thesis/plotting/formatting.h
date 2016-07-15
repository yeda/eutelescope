#include <iostream>
#include <math.h>
#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TColor.h"
#include "TF1.h"
#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include <cstdlib>

Color_t color[9] = {kBlack, kRed, kBlue, kGreen+2, kMagenta+1, kOrange-3, kPink-9, kViolet+1, kCyan+2};

using namespace std;


int sortGraph(TString type, int dutnum, int dutid){
	if (type == TString("tb") || type == TString("clusize") ){	
		if (dutnum==0 && dutid==6) return 0;
		if (dutnum==0 && dutid==7) return 2;

		if (dutnum==1 && dutid==6) return 8;
		if (dutnum==1 && dutid==7) return 10;

		if (dutnum==2 && dutid==6) return 12;
		if (dutnum==2 && dutid==7) return 14;
	
		if (dutnum==3 && dutid==6) return 9;
		if (dutnum==3 && dutid==7) return 11;

		if (dutnum==5 && dutid==6) return 4;
		if (dutnum==5 && dutid==7) return 6;

		if (dutnum==6 && dutid==6) return 5;
		if (dutnum==6 && dutid==7) return 7;
	}

	if (type == TString("sr")){	
		if (dutnum==0 && dutid==6) return 0;
		if (dutnum==0 && dutid==7) return 2;

		if (dutnum==10 && dutid==6) return 1;
		if (dutnum==10 && dutid==7) return 3;

		if (dutnum==6 && dutid==6) return 4;
		if (dutnum==6 && dutid==7) return 6;

		if (dutnum==16 && dutid==6) return 5;
		if (dutnum==16 && dutid==7) return 7;

		if (dutnum==3 && dutid==6) return 8;
		if (dutnum==3 && dutid==7) return 10;

		if (dutnum==13 && dutid==6) return 9;
		if (dutnum==13 && dutid==7) return 11;

		if (dutnum==2 && dutid==6) return 12;
		if (dutnum==2 && dutid==7) return 14;

		if (dutnum==12 && dutid==6) return 13;
		if (dutnum==12 && dutid==7) return 15;

		if (dutnum==17 && dutid==6) return 17;
		if (dutnum==17 && dutid==7) return 19;

	}





} 

void formatGraph(TString type, TGraphErrors *gr, int dutnum, int dutid){
	Color_t cl;

	gr->SetMarkerStyle(7);
	gr->SetLineWidth(2);
	gr->SetFillStyle(0);

	if ( (type == "tb"|| type == "clusize") && (dutnum == 3 || dutnum == 6)) gr->SetLineStyle(2);
	else if (type == "sr" && dutnum > 9) gr->SetLineStyle(2);
	else gr->SetLineStyle(1);

	if ( (dutnum==0 || dutnum==10) && dutid==6) cl = color[8];
	if ( (dutnum==0 || dutnum==10) && dutid==7) cl = color[7];
	
	if ( (dutnum==1 || dutnum==11) && dutid==6) cl = color[6];
	if ( (dutnum==1 || dutnum==11) && dutid==7) cl = color[5];

	if ( (dutnum==2 || dutnum==12) && dutid==6) cl = color[4];
	if ( (dutnum==2 || dutnum==12) && dutid==7) cl = color[3];

	if ( (dutnum==3 || dutnum==13) && dutid==6) cl = color[6];
	if ( (dutnum==3 || dutnum==13) && dutid==7) cl = color[5];

	if ( (dutnum==5 || dutnum==15) && dutid==6) cl = color[2];
	if ( (dutnum==5 || dutnum==15) && dutid==7) cl = color[1];

	if ( (dutnum==6 || dutnum==16) && dutid==6) cl = color[2];
	if ( (dutnum==6 || dutnum==16) && dutid==7) cl = color[1];

	if ( (dutnum==7 || dutnum==17) && dutid==6) cl = color[0];
	if ( (dutnum==7 || dutnum==17) && dutid==7) cl = color[0];

	gr->SetMarkerColor(cl);
	gr->SetLineColor(cl);


}



TString getIrrad(int dutnum, int dutid){
	if (dutnum==0 || dutnum==10) return TString("non-irrad.");

	if (dutnum==1 && dutid==6) return TString("5.0x10^{14}");
	if (dutnum==1 && dutid==7) return TString("1.0x10^{15}");

	if (dutnum==17 && dutid==6) return TString("5.0x10^{14} an.");
	if (dutnum==17 && dutid==7) return TString("1.0x10^{15} an.");

	if ((dutnum==2 || dutnum==12) && dutid==6) return TString("2.0x10^{15}");
	if ((dutnum==2 || dutnum==12) && dutid==7) return TString("5.0x10^{15}");

	if ((dutnum==3 || dutnum==13) && dutid==6) return TString("5.0x10^{14} an.");
	if ((dutnum==3 || dutnum==13) && dutid==7) return TString("1.0x10^{15} an.");

	if (dutnum==5 && dutid==6) return TString("1.2x10^{14}");
	if (dutnum==5 && dutid==7) return TString("2.0x10^{14}");

	if ((dutnum==6 || dutnum==16) && dutid==6) return TString("1.2x10^{14} an.");
	if ((dutnum==6 || dutnum==16) && dutid==7) return TString("2.0x10^{14} an.");
}

void formatCanvas1D(TCanvas *cc){
	cc->SetRightMargin(0.125);
	cc->SetLeftMargin(0.125);
	cc->SetBottomMargin(0.13);
	cc->SetTopMargin(0.07);
}
void formatCanvasColZ(TCanvas *cc){
	cc->SetRightMargin(0.1);
	cc->SetLeftMargin(0.125);
	cc->SetBottomMargin(0.13);
	cc->SetTopMargin(0.07);
}


void formatHisto(TH1 *h){
	h->SetLabelSize(0.045,"XY");
	h->SetTitleSize(0.05,"XY");
	h->SetTitleOffset(1.2,"XY");
	h->SetTitle("");
	h->SetLineWidth(2);
}



