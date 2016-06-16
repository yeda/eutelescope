#include <iostream>
#include <math.h>
#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TVectorD.h"
#include "TF1.h"
#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include <cstdlib>


using namespace std;


void formatCanvas1D(TCanvas *cc){
	cc->SetRightMargin(0.025);
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
}



