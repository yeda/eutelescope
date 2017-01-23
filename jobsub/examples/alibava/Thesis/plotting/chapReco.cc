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
#include "TPaveStats.h"
#include <cstdlib>

#include "formatting.h"

TString pedRun = TString("run004198");
TString beamRun = TString("run004197");

TString EuTelHistoFileBase = TString("/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output/histogram");

TString FileBase = TString("/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/Thesis/output");
TCanvas *cc =new TCanvas("cc","",800,600);


const int N_Comp = 6;
// B=0, Tilt=0
int pedRunList_Comp[N_Comp]={4198, 5025, 6058, 1072, 3073, 2047}; // sorted by irrad
int runRunList_Comp[N_Comp]={4197, 5024, 6057, 1076, 3056, 2046}; // sorted by irrad



TString pdfFolder = TString("pdfs/chapReco-pdf");
using namespace std;

void rawData();
void pedestal();
void commonmode();
void recodata();
void timecut();
void clustering();

void pedestal_Comp();
void signal_Comp();
void hitamp_Comp();
void clusize_Comp();

int main(int argc, char *argv[]){
    gStyle->SetOptStat(0);
    TGaxis::SetMaxDigits(3);
    
     rawData();
     pedestal();
     
commonmode();
  /*   recodata();
     timecut();
     clustering();
   
    pedestal_Comp();
    signal_Comp();
    hitamp_Comp();
    clusize_Comp();
*/
    return 0;
}

void clusize_Comp(){
    
    TH1D *h1D[N_Comp][2];
    TFile *file;
    TString PdfName;
    TString outputPath = pdfFolder + TString("/comp/");
    
    TString FileName = EuTelHistoFileBase + TString("/XXXXXX-alibava-applyxtalk.root");
    TString RunName;
    TString HistoName;
    
    formatCanvas1D(cc);
    cc->SetLogy(0);
    
    
    
    int icolor = 0;
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(runRunList_Comp[j]/1000.0) == 6 || floor(runRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            if(j==0 && id==1) continue;
            
            icolor++;
            
            TString CurrentFile = FileName;
            RunName = TString("run00")+ TString::Itoa(runRunList_Comp[j], 10);
            CurrentFile.ReplaceAll(TString("XXXXXX"), RunName);
            
            
            std::cout<< "Running "<< CurrentFile<<std::endl;
            file = TFile::Open(CurrentFile.Data());
            
            HistoName = TString("MyAlibavaClusterHistogramMaker/alibava_clusters/hClusterSize_chip") + TString::Itoa(id, 10);
            h1D[j][id] = (TH1D*) file->Get(HistoName.Data());
            formatHisto(h1D[j][id]);
            h1D[j][id]->SetLineColor(color[icolor]);
            
            double scale = h1D[j][id]->Integral();
            scale = 1/scale;

            h1D[j][id]->Scale(scale);
            h1D[j][id]->SetAxisRange(0,8,"X");
            h1D[j][id]->SetAxisRange(0,1,"Y");
            h1D[j][id]->SetLineWidth(2);
            
            cc->cd();
            if (j==0 && id==0) h1D[j][id]->Draw();
            else h1D[j][id]->Draw("same");
        }
        
    }
    
    TLegend *leg = new TLegend(0.61,0.46,0.86,0.91);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(runRunList_Comp[j]/1000.0) == 6 || floor(runRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            
            int dutnum = floor(runRunList_Comp[j]/1000.0);
            int dutid = id+6;
            
            TString labelname;
	    if (j==0) labelname = getIrrad(dutnum,dutid);
	    else labelname = getIrrad(dutnum,dutid) + TString(" n_{eq}");

            if(j==0 && id==1) continue;
            else leg->AddEntry(h1D[j][id],labelname.Data(),"l");
        }
    }
    
    cc->cd();
    leg->Draw();
    PdfName = outputPath+TString("CluSizeComparison.pdf");
    cc->SaveAs(PdfName.Data());
    
}


void hitamp_Comp(){
    
    TH1D *h1D[N_Comp][2];
    TFile *file;
    TString PdfName;
    TString outputPath = pdfFolder + TString("/comp/");
    
    TString FileName = EuTelHistoFileBase + TString("/XXXXXX-alibava-applyxtalk.root");
    TString RunName;
    TString HistoName;
    
    formatCanvas1D(cc);
    cc->SetLogy(1);
    
    
    
    int icolor = 0;
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(runRunList_Comp[j]/1000.0) == 6 || floor(runRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            if(j==0 && id==1) continue;
            
            icolor++;
            int calibID = floor(runRunList_Comp[j]/1000.0)*10 + double(id+6);
            double calibrationconst = mapCalib[calibID] /1000.0;
            
            TString CurrentFile = FileName;
            RunName = TString("run00")+ TString::Itoa(runRunList_Comp[j], 10);
            CurrentFile.ReplaceAll(TString("XXXXXX"), RunName);
            
            
            std::cout<< "Running "<< CurrentFile<<std::endl;
            file = TFile::Open(CurrentFile.Data());
            
            HistoName = TString("MyAlibavaClusterHistogramMaker/alibava_clusters/hHitAmplitude_chip") + TString::Itoa(id, 10);
            h1D[j][id] = (TH1D*) file->Get(HistoName.Data());
            formatHisto(h1D[j][id]);
            h1D[j][id]->SetLineColor(color[icolor]);
            h1D[j][id]->Rebin(4);
            
            int nbins = h1D[j][id]->GetXaxis()->GetNbins();
            double new_bins[nbins+1];
            for(int ibin=0; ibin <= nbins; ibin++){
                new_bins[ibin] = h1D[j][id]->GetBinLowEdge(ibin+1) * calibrationconst;
            }
            
            h1D[j][id]->SetBins(nbins, new_bins);
            h1D[j][id]->SetAxisRange(-10,80,"X");
            //            h1D[j][id]->SetAxisRange(70,100,"Y");
            h1D[j][id]->SetXTitle("(-1) Hit Amplitude (ke)");
            
            cc->cd();
            if (j==0 && id==0) h1D[j][id]->Draw();
            else h1D[j][id]->Draw("same");
        }
        
    }
    
    TLegend *leg = new TLegend(0.61,0.46,0.86,0.91);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(runRunList_Comp[j]/1000.0) == 6 || floor(runRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            
            int dutnum = floor(runRunList_Comp[j]/1000.0);
            int dutid = id+6;
            
            TString labelname;
	    if (j==0) labelname = getIrrad(dutnum,dutid);
	    else labelname = getIrrad(dutnum,dutid) + TString(" n_{eq}");

            if(j==0 && id==1) continue;
            else leg->AddEntry(h1D[j][id],labelname.Data(),"l");
        }
    }
    
    cc->cd();
    leg->Draw();
    PdfName = outputPath+TString("HitAmpComparison.pdf");
    cc->SaveAs(PdfName.Data());
    
}



void signal_Comp(){
    
    TH1D *h1D[N_Comp][2];
    TFile *file;
    TString PdfName;
    TString outputPath = pdfFolder + TString("/comp/");
    
    TString FileName = EuTelHistoFileBase + TString("/XXXXXX-alibava-applyxtalk.root");
    TString RunName;
    TString HistoName;
    
    formatCanvas1D(cc);
    cc->SetLogy(1);
    
    
    
    int icolor = 0;
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(runRunList_Comp[j]/1000.0) == 6 || floor(runRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            if(j==0 && id==1) continue;
            
            icolor++;
            int calibID = floor(runRunList_Comp[j]/1000.0)*10 + double(id+6);
            double calibrationconst = mapCalib[calibID] /1000.0;
            
            TString CurrentFile = FileName;
            RunName = TString("run00")+ TString::Itoa(runRunList_Comp[j], 10);
            CurrentFile.ReplaceAll(TString("XXXXXX"), RunName);
            
            
            std::cout<< "Running "<< CurrentFile<<std::endl;
            file = TFile::Open(CurrentFile.Data());
            
            HistoName = TString("MyAlibavaDataHistogramMaker/recodata_xtalk/hSignal_chip") + TString::Itoa(id, 10);
            h1D[j][id] = (TH1D*) file->Get(HistoName.Data());
            formatHisto(h1D[j][id]);
            h1D[j][id]->SetLineColor(color[icolor]);
            h1D[j][id]->Rebin(4);
            
            int nbins = h1D[j][id]->GetXaxis()->GetNbins();
            double new_bins[nbins+1];
            for(int ibin=0; ibin <= nbins; ibin++){
                new_bins[ibin] = h1D[j][id]->GetBinLowEdge(ibin+1) * calibrationconst;
            }
            
            h1D[j][id]->SetBins(nbins, new_bins);
            h1D[j][id]->SetAxisRange(-10,100,"X");
            //            h1D[j][id]->SetAxisRange(70,100,"Y");
            h1D[j][id]->SetXTitle("(-1) Signal (ke)");
            
            cc->cd();
            if (j==0 && id==0) h1D[j][id]->Draw();
            else h1D[j][id]->Draw("same");
        }
        
    }
    
    TLegend *leg = new TLegend(0.6,0.45,0.85,0.9);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(runRunList_Comp[j]/1000.0) == 6 || floor(runRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            
            int dutnum = floor(runRunList_Comp[j]/1000.0);
            int dutid = id+6;
            
            TString labelname;
	    if (j==0) labelname = getIrrad(dutnum,dutid);
	    else labelname = getIrrad(dutnum,dutid) + TString(" n_{eq}");

            if(j==0 && id==1) continue;
            else leg->AddEntry(h1D[j][id],labelname.Data(),"l");
        }
    }
    
    cc->cd();
    leg->Draw();
    PdfName = outputPath+TString("SignalComparison.pdf");
    cc->SaveAs(PdfName.Data());
    
}

void recodata(){
    
    TH1D *h1D;
    TH2D *h2D;
    TFile *file;
    TString PdfName;
    TString FileName;
    TString outputPath = pdfFolder + TString("/nocut/");
    
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
    TString outputPath = pdfFolder + TString("/timecut/");
    
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
    TString outputPath = pdfFolder + TString("/timecut/");
    
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
    h1D->SetXTitle("#eta");
    h1D->Draw();
    PdfName = outputPath+beamRun+TString("_eta_noxtalk.pdf");
    cc->SaveAs(PdfName.Data());
    
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
            h1D[j][0] = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/rawdata/Data_chan_chip0_chan33");
            cc->SetLogy(1);
            h1D[j][0]->SetAxisRange(450,600,"X");
            formatCanvas1D(cc);
            formatHisto(h1D[j][0]);
            gStyle->SetOptFit(1);
            cc->SetRightMargin(0.025);
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
        h1D[j][2]->SetYTitle("Noise (ADCs)");
        h1D[j][2]->Draw();
        PdfName = outputPath+pedRun+TString("_noise_chip0_") + suffix[j] +TString(".pdf");
        cc->SaveAs(PdfName.Data());
        
    }
    
    TLegend *leg = new TLegend(0.2,0.75,0.85,0.91);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    
    cc->SetLogy(0);
    formatCanvas1D(cc);
    
    h1D[0][1]->Draw();
    h1D[1][1]->SetLineColor(kRed);
    h1D[1][1]->Draw("same");
    leg->AddEntry(h1D[0][1],"without cmmd noise subtraction","l");
    leg->AddEntry(h1D[1][1],"with cmmd noise subtraction","l");
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
    TString outputPath;
    outputPath = pdfFolder + TString("/nocut/");
    
    FileName = FileBase + TString("/nocut/histogram/XXXXXX-alibava-pedestal-subtracted-rawdatahisto.root");
    FileName.ReplaceAll(TString("XXXXXX"), pedRun);
    std::cout<< "Running "<< FileName <<std::endl;
    file = TFile::Open(FileName.Data());
    
    h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/Events/hEvent_1777_chip0");
    formatCanvas1D(cc);
    formatHisto(h1D);
    h1D->Draw();
    PdfName = outputPath+pedRun+TString("_event1777_nocmmd.pdf");
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
void pedestal_Comp(){
    
    TH1D *h1D[2][N_Comp][2];
    TFile *file;
    TString PdfName;
    TString outputPath = pdfFolder + TString("/comp/");
    
    TString FileName = EuTelHistoFileBase + TString("/XXXXXX-alibava-pedestal2.root");;
    TString PedRunName;
    TString HistoName;
    
    TCanvas *cc_ped =new TCanvas("cc_ped","",800,600);
    TCanvas *cc_noi =new TCanvas("cc_noi","",800,600);
    formatCanvas1D(cc_ped);
    formatCanvas1D(cc_noi);
    cc_ped->SetLogy(0);
    cc_noi->SetLogy(0);
    
    int icolor = 0;
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(pedRunList_Comp[j]/1000.0) == 6 || floor(pedRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            if(j==0 && id==1) continue;
            
            icolor++;
            int calibID = floor(pedRunList_Comp[j]/1000.0)*10 + double(id+6);
            
            TString CurrentFile = FileName;
            PedRunName = TString("run00")+ TString::Itoa(pedRunList_Comp[j], 10);
            CurrentFile.ReplaceAll(TString("XXXXXX"), PedRunName);
            
            
            std::cout<< "Running "<< CurrentFile<<std::endl;
            file = TFile::Open(CurrentFile.Data());
            
            HistoName = TString("MyAlibavaPedestalNoiseProcessor/hpedestal_chip") + TString::Itoa(id, 10);
            h1D[0][j][id] = (TH1D*) file->Get(HistoName.Data());
            formatHisto(h1D[0][j][id]);
            h1D[0][j][id]->SetLineColor(color[icolor]);
            h1D[0][j][id]->Scale(mapCalib[calibID] /1000.0);
            h1D[0][j][id]->SetAxisRange(70,100,"Y");
            h1D[0][j][id]->SetYTitle("Pedestal (ke)");
            
            cc_ped->cd();
            if (j==0 && id==0) h1D[0][j][id]->Draw();
            else h1D[0][j][id]->Draw("same");
            
            HistoName = TString("MyAlibavaPedestalNoiseProcessor/hnoise_chip") + TString::Itoa(id, 10);
            h1D[1][j][id] = (TH1D*) file->Get(HistoName.Data());
            formatHisto(h1D[1][j][id]);
            h1D[1][j][id]->SetLineColor(color[icolor]);
            h1D[1][j][id]->Scale(mapCalib[calibID] /1000.0);
            h1D[1][j][id]->SetAxisRange(0.5,0.85,"Y");
            h1D[1][j][id]->SetYTitle("Noise (ke)");
            
            cc_noi->cd();
            if (j==0 && id==0) h1D[1][j][id]->Draw();
            else h1D[1][j][id]->Draw("same");
        }
        
    }
    
    TLegend *leg = new TLegend(0.2,0.65,0.85,0.91);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    leg->SetNColumns(2);
    
    for (unsigned int j=0; j<N_Comp; j++){
        if (floor(pedRunList_Comp[j]/1000.0) == 6 || floor(pedRunList_Comp[j]/1000.0)==3) continue;
        for (unsigned int id=0; id<2; id++) {
            
            int dutnum = floor(pedRunList_Comp[j]/1000.0);
            int dutid = id+6;

            TString labelname;
	    if (j==0) labelname = getIrrad(dutnum,dutid);
	    else labelname = getIrrad(dutnum,dutid) + TString(" n_{eq}");
            
            if(j==0 && id==1) leg->AddEntry("","","");
            else leg->AddEntry(h1D[1][j][id],labelname.Data(),"l");
        }
    }
    
    cc_ped->cd();
    leg->Draw();
    PdfName = outputPath+TString("PedestalComparison.pdf");
    cc_ped->SaveAs(PdfName.Data());
    
    cc_noi->cd();
    leg->Draw();
    PdfName = outputPath+TString("NoiseComparison.pdf");
    cc_noi->SaveAs(PdfName.Data());
    
}









