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

TString pedRun = TString("run004198");
TString beamRun = TString("run004197");

TString FileBase = TString("/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/Thesis/output");
TCanvas *cc =new TCanvas("cc","",800,600);

TString pdfFolder = TString("pdfs/chapReco-pdf");
using namespace std;

void rawData();
void pedestal();
void commonmode();
void recodata();
void timecut();
void clustering();


int main(int argc, char *argv[]){
	gStyle->SetOptStat(0);
	TGaxis::SetMaxDigits(3);

	rawData();
	pedestal();
	commonmode();
	recodata();
	timecut();
	clustering();

	return 0;
}


void rawData(){

	TH1D *h1D;
	TFile *file;
	TString FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-rawdatahisto.root");
	vector<TString> RunNum;
	RunNum.push_back(pedRun);
	RunNum.push_back(beamRun);


	TString PdfName;
	for (unsigned int i=0; i<RunNum.size(); i++){
		FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-rawdatahisto.root");
		FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
		file = TFile::Open(FileName.Data());
	
		std::cout<< "Running "<< FileName<<std::endl;
		h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/rawdata/hSignal_chip0");
		cc->SetLogy(1);
		h1D->SetAxisRange(0,800,"X");
		h1D->Draw();
		formatCanvas1D(cc);
		formatHisto(h1D);
		h1D->Draw();
		PdfName = pdfFolder + TString("/nocut/")+RunNum[i]+TString("_rawSignal.pdf");
		cc->SaveAs(PdfName.Data());
	
		TH2D *h2D = (TH2D*) file->Get("MyAlibavaDataHistogramMaker/rawdata/hSignal_vs_Time_chip0");
		h2D->SetAxisRange(0,1000,"Y");
		h2D->SetAxisRange(0,30,"X");
		formatHisto(h2D);
		cc->SetLogy(0);
		cc->SetLogz(1);
		formatCanvasColZ(cc);
		h2D->Draw("colz");
		PdfName = pdfFolder + TString("/nocut/")+RunNum[i]+TString("_rawSignalvsTime.pdf");
		cc->SaveAs(PdfName.Data());
	}

}

void pedestal(){

	TH1D *h1D[2][3];
	TFile *file;
	TString PdfName;
	TString FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-pedestalYYY.root");
	FileName.ReplaceAll(TString("XXXXXX"), pedRun);	
	vector<TString> suffix;
	suffix.push_back(TString(""));
	suffix.push_back(TString("2"));
	TString outputPath;

	for (unsigned int j=0; j<suffix.size(); j++){
		TString CurrentFile = FileName;
		CurrentFile.ReplaceAll(TString("YYY"), suffix[j]);
		std::cout<< "Running "<< CurrentFile<<std::endl;
		file = TFile::Open(CurrentFile.Data());
		outputPath = pdfFolder + TString("/nocut/");
		if(j==0){	
			gStyle->SetOptFit(1);
			h1D[j][0] = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/rawdata/Data_chan_chip0_chan33");
			cc->SetLogy(1);
			h1D[j][0]->SetAxisRange(450,600,"X");
			formatCanvas1D(cc);
			formatHisto(h1D[j][0]);
			h1D[j][0]->Draw();
			PdfName = outputPath+pedRun+TString("_rawsignal_chan33_") + suffix[j] +TString(".pdf");
			cc->SaveAs(PdfName.Data());
			gStyle->SetOptFit(0);
		}

		h1D[j][1] = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/hpedestal_chip0");
		cc->SetLogy(0);
		h1D[j][1]->SetAxisRange(400,650,"Y");
		formatCanvas1D(cc);
		formatHisto(h1D[j][1]);
		h1D[j][1]->Draw();
		PdfName = outputPath+pedRun+TString("_pedestal_chip0_") + suffix[j] +TString(".pdf");
		cc->SaveAs(PdfName.Data());


		h1D[j][2] = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/hnoise_chip0");
		cc->SetLogy(0);
		h1D[j][2]->SetAxisRange(0,15,"Y");
		formatCanvas1D(cc);
		formatHisto(h1D[j][2]);
		h1D[j][2]->Draw();
		PdfName = outputPath+pedRun+TString("_noise_chip0_") + suffix[j] +TString(".pdf");
		cc->SaveAs(PdfName.Data());

	}
	
	TLegend *leg = new TLegend(0.2,0.75,0.955,0.91);
	leg->SetFillColor(0);
	leg->SetLineColor(0);

	cc->SetLogy(0);
        formatCanvas1D(cc);
	h1D[0][1]->Draw();
	h1D[1][1]->SetLineColor(kRed);
	h1D[1][1]->Draw("same");
	leg->AddEntry(h1D[0][1],"without common mode subtraction","l");
	leg->AddEntry(h1D[1][1],"with common mode subtraction","l");
	leg->Draw();
	PdfName = outputPath+pedRun+TString("_pedestal_chip0_cmmdComparison.pdf");
	cc->SaveAs(PdfName.Data());

	h1D[0][2]->Draw();
	h1D[1][2]->SetLineColor(kRed);
	h1D[1][2]->Draw("same");
	leg->Draw();
	PdfName = outputPath+pedRun+TString("_noise_chip0_cmmdComparison.pdf");
	cc->SaveAs(PdfName.Data());
}

void commonmode(){
	TH1D *h1D;
	TFile *file;
	TString PdfName;
	TString FileName;
	TString outputPath = TString("pdfs/nocut/");

	FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-pedestal-subtracted-rawdatahisto.root");
	FileName.ReplaceAll(TString("XXXXXX"), pedRun);	
	std::cout<< "Running "<< FileName <<std::endl;
	file = TFile::Open(FileName.Data());
	
	h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/Events/hEvent_1000_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	h1D->Draw();
	PdfName = outputPath+pedRun+TString("_event1000_nocmmd.pdf");
	cc->SaveAs(PdfName.Data());
	
	
	FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-commonmode.root");
	FileName.ReplaceAll(TString("XXXXXX"), pedRun);	
	std::cout<< "Running "<< FileName <<std::endl;
	file = TFile::Open(FileName.Data());

	h1D = (TH1D*) file->Get("Common Mode Correction Values");
	cc->SetLogy(1);
	formatCanvas1D(cc);
	formatHisto(h1D);
	h1D->SetAxisRange(-60,60,"X");
	h1D->Draw();
	PdfName = outputPath+pedRun+TString("_pedcmmd_values.pdf");
	cc->SaveAs(PdfName.Data());
}

void recodata(){

	TH1D *h1D;
	TH2D *h2D;
	TFile *file;
	TString PdfName;
	TString FileName;
	TString outputPath = TString("pdfs/nocut/");

	FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-reco.root");
	FileName.ReplaceAll(TString("XXXXXX"), beamRun);	
	std::cout<< "Running "<< FileName <<std::endl;
	file = TFile::Open(FileName.Data());

	// reco signal	
	h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSignal_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	cc->SetLogy(1);
	h1D->SetAxisRange(-200,600,"X");
	h1D->Draw();
	PdfName = outputPath+beamRun+TString("_recodata_cmmd_signal.pdf");
	cc->SaveAs(PdfName.Data());

	h2D = (TH2D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSignal_vs_Time_chip0");
	formatCanvasColZ(cc);
	formatHisto(h2D);
	cc->SetLogy(0);
	cc->SetLogz(1);
	h2D->SetAxisRange(-200,600,"Y");
	h2D->SetAxisRange(0,30,"X");
	h2D->Draw("colz");
	PdfName = outputPath+beamRun+TString("_recodata_cmmd_signal_vs_time.pdf");
	cc->SaveAs(PdfName.Data());

	TLine line1 = TLine(5,-200,5,600);
	line1.Draw();
	TLine line2 = TLine(15,-200,15,600);
	line2.Draw();
	PdfName = outputPath+beamRun+TString("_recodata_cmmd_signal_vs_time_showTimeCut.pdf");
	cc->SaveAs(PdfName.Data());



	// reco snr
	h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSNR_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	cc->SetLogy(1);
	h1D->SetAxisRange(-50,200,"X");
	h1D->Draw();
	PdfName = outputPath+beamRun+TString("_recodata_cmmd_snr.pdf");
	cc->SaveAs(PdfName.Data());

	h2D = (TH2D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSNR_vs_Time_chip0");
	formatCanvasColZ(cc);
	formatHisto(h2D);
	cc->SetLogy(0);
	cc->SetLogz(1);
	h2D->SetAxisRange(-50,200,"Y");
	h2D->SetAxisRange(0,30,"X");
	h2D->Draw("colz");
	PdfName = outputPath+beamRun+TString("_recodata_cmmd_snr_vs_time.pdf");
	cc->SaveAs(PdfName.Data());
	
	TLine line3 = TLine(5,-50,5,200);
	line3.Draw();
	TLine line4 = TLine(15,-50,15,200);
	line4.Draw();
	PdfName = outputPath+beamRun+TString("_recodata_cmmd_snr_vs_time_showTimeCut.pdf");
	cc->SaveAs(PdfName.Data());

}

void timecut(){
	TH1D *h1D;
	TFile *file;
	TString PdfName;
	TString FileName;
	TString outputPath = TString("pdfs/timecut/");

	FileName = FileBase + TString("/timecut/histogram/XXXXXX-alibava-reco.root");
	FileName.ReplaceAll(TString("XXXXXX"), beamRun);	
	std::cout<< "Running "<< FileName <<std::endl;
	file = TFile::Open(FileName.Data());

	// reco signal	
	h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSignal_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	cc->SetLogy(1);
	h1D->SetAxisRange(-200,600,"X");
	h1D->Draw();
	PdfName = outputPath+beamRun+TString("_recodata_timecut_signal.pdf");
	cc->SaveAs(PdfName.Data());

	// reco snr
	h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSNR_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	cc->SetLogy(1);
	h1D->SetAxisRange(-50,200,"X");
	h1D->Draw();
	PdfName = outputPath+beamRun+TString("_recodata_timecut_snr.pdf");
	cc->SaveAs(PdfName.Data());
}

void clustering(){
	TH1D *h1D;
	TFile *file;
	TString PdfName;
	TString FileName;
	TString outputPath = TString("pdfs/timecut/");

	FileName = FileBase + TString("/timecut/histogram/XXXXXX-alibava-reco.root");
	FileName.ReplaceAll(TString("XXXXXX"), beamRun);	
	std::cout<< "Running "<< FileName <<std::endl;
	file = TFile::Open(FileName.Data());

	// cluster size	
	h1D = (TH1D*) file->Get("MyAlibavaSeedClustering/hClusterSize_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	cc->SetLogy(0);
	h1D->Draw();
	PdfName = outputPath+beamRun+TString("_clustersize_noxtalk.pdf");
	cc->SaveAs(PdfName.Data());


	// cluster size	
	h1D = (TH1D*) file->Get("MyAlibavaSeedClustering/hEta_chip0");
	formatCanvas1D(cc);
	formatHisto(h1D);
	cc->SetLogy(0);
	h1D->Draw();
	PdfName = outputPath+beamRun+TString("_eta_noxtalk.pdf");
	cc->SaveAs(PdfName.Data());

}

