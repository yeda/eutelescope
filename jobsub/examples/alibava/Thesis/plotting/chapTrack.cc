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
TString gblRun = TString("run004079");

TString FileBase = TString("/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output");
TString GblOutputFileBase = TString("/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/gblPython/output/notcomb-xtalk-telaligned");
TCanvas *cc =new TCanvas("cc","",800,600);

TString pdfFolder = TString("pdfs/chapTrack-pdf/");
using namespace std;

void synchronization();
void track_algorithm();

int main(int argc, char *argv[]){
	gStyle->SetOptStat(0);
	TGaxis::SetMaxDigits(3);

	synchronization();
	track_algorithm();

	return 0;
}

void track_algorithm(){
	TH1D *h1D;
	TH2D *h2D;
	TString PdfName;

	TFile *file;
	TString FileName;
	vector<TString> itnum = {"1","3"};
	for (unsigned int it=0; it<itnum.size(); it++){
		FileName = GblOutputFileBase + TString("/XXXXXX/cuts_Y.root");
		FileName.ReplaceAll(TString("XXXXXX"), gblRun);
		FileName.ReplaceAll(TString("Y"), itnum[it]);
		
		file = TFile::Open(FileName.Data());
		
		vector<TString> histvec = {"doubletDx", "doubletDy", "tripletDx", "tripletDy", "dslopeX", "dslopeY", "dposX", "dposY", "DUT6-dx", "DUT7-dx"};
		vector<TString> Xaxislabel = {"dx (mm)", "dy (mm)", "dx (mm)", "dy (mm)", "dslopeX (degrees)", "dslopeY (degrees)", "dx (mm)", "dy (mm)", "dx (mm)", "dx (mm)"};
		for(unsigned int ihist=0; ihist<histvec.size(); ihist++){
			h1D = (TH1D*) file->Get(histvec[ihist].Data());

			formatCanvas1D(cc);
			formatHisto(h1D);
			//cc->SetLogz(1);
			h1D->SetXTitle(Xaxislabel[ihist].Data());
			h1D->SetYTitle("Number of Entries");
			h1D->Draw();
			PdfName = pdfFolder+gblRun+TString("_it")+itnum[it]+TString("_")+histvec[ihist]+TString(".pdf");
			cc->SaveAs(PdfName.Data());
		}

	}

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
	h2D->SetYTitle("dx (mm)");
	formatCanvasColZ(cc);
	formatHisto(h2D);
	//cc->SetLogz(1);
	h2D->Draw("colz");
	PdfName = pdfFolder+beamRun+TString("_hSyncX_d2_d7.pdf");
	cc->SaveAs(PdfName.Data());

}

