#include <iostream>
#include <math.h>
#include <vector>
#include "TString.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "TLine.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TVectorD.h"
#include "TF1.h"
#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include <cstdlib>

#include "formatting.h"

TString beamRun = TString("run004197");

TString FileBase = TString("/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output");
TCanvas *cc =new TCanvas("cc","",800,600);

TString pdfFolder = TString("pdfs/chapTrack-pdf/");
using namespace std;

void synchronization();


int main(int argc, char *argv[]){
	gStyle->SetOptStat(0);
	TGaxis::SetMaxDigits(3);

	synchronization();

	return 0;
}

void synchronization(){

	TH1D *h1D;
	TH2D *h2D;
	TString PdfName;

	TFile *file;
	TString FileName = FileBase + TString("/histogram/XXXXXX-hitmaker-local.root");
	FileName.ReplaceAll(TString("XXXXXX"), beamRun);	
	file = TFile::Open(FileName.Data());

	h2D = (TH2D*) file->Get("MyAlibavaCorrelator/hSyncX/hSyncX_d2_d7");
	formatCanvasColZ(cc);
	formatHisto(h2D);
	//cc->SetLogz(1);
	h2D->Draw("colz");
	PdfName = pdfFolder+beamRun+TString("_hSyncX_d2_d7.pdf");
	cc->SaveAs(PdfName.Data());

}

