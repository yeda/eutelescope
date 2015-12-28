{

TH1D *h1D;
TFile *file;
TCanvas *cc =new TCanvas("cc","",800,600);
TString FileBase = TString("../../output/thesis/histogram/XXXXXX-alibava-rawdatahisto.root");
vector<TString> RunNum;
RunNum.push_back(TString("run000602"));
RunNum.push_back(TString("run000613"));


TString PdfName;
for (unsigned int i=0; i<RunNum.size(); i++){
TString FileName = FileBase;
FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
file = TFile::Open(FileName.Data());

std::cout<< "Running "<< FileName<<std::endl;
h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/rawdata/hSignal_chip0");
cc->SetLogy(1);
h1D->SetAxisRange(0,1000,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_rawSignal.pdf");
cc->SaveAs(PdfName.Data());

TH2D *h2D = (TH2D*) file->Get("MyAlibavaDataHistogramMaker/rawdata/hSignal_vs_Time_chip0");
h2D->SetAxisRange(0,1000,"Y");
h2D->SetAxisRange(0,30,"X");
cc->SetLogy(0);
cc->SetLogz(1);
h2D->Draw("colz");
PdfName = TString("pdfs/")+RunNum[i]+TString("_rawSignalvsTime.pdf");
cc->SaveAs(PdfName.Data());


}



FileBase = TString("../../output/thesis/histogram/XXXXXX-alibava-pedestalYYY.root");
RunNum.clear();
RunNum.push_back(TString("run000602"));

vector<TString> suffix;
suffix.push_back(TString(""));
suffix.push_back(TString("2"));

for (unsigned int i=0; i<RunNum.size(); i++){
for (unsigned int j=0; j<suffix.size(); j++){

TString FileName = FileBase;
FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
FileName.ReplaceAll(TString("YYY"), suffix[j]);
file = TFile::Open(FileName.Data());

std::cout<< "Running "<< FileName<<std::endl;
if(j==0){
h1D = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/rawdata/Data_chan_chip0_chan33");
cc->SetLogy(1);
h1D->SetAxisRange(450,600,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_rawSignal_chan") + suffix[j] +TString(".pdf");
cc->SaveAs(PdfName.Data());
}
h1D = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/hpedestal_chip0");
cc->SetLogy(0);
h1D->SetAxisRange(400,650,"Y");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_pedestal_chip0") + suffix[j] +TString(".pdf");
cc->SaveAs(PdfName.Data());


h1D = (TH1D*) file->Get("MyAlibavaPedestalNoiseProcessor/hnoise_chip0");
cc->SetLogy(0);
h1D->SetAxisRange(0,12,"Y");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_noise_chip0") + suffix[j] +TString(".pdf");
cc->SaveAs(PdfName.Data());

}
}


FileBase = TString("../../output/thesis/histogram/XXXXXX-alibava-commonmode.root");
RunNum.clear();
RunNum.push_back(TString("run000602"));


for (unsigned int i=0; i<RunNum.size(); i++){

TString FileName = FileBase;
FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
file = TFile::Open(FileName.Data());

std::cout<< "Running "<< FileName<<std::endl;
h1D = (TH1D*) file->Get("Common Mode Correction Values");
cc->SetLogy(1);
h1D->SetAxisRange(-60,60,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_pedcmmd.pdf");
cc->SaveAs(PdfName.Data());
}

FileBase = TString("../../output/thesis/histogram/XXXXXX-alibava-raw-ped-datahisto.root");
RunNum.clear();
RunNum.push_back(TString("run000602"));


for (unsigned int i=0; i<RunNum.size(); i++){

TString FileName = FileBase;
FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
file = TFile::Open(FileName.Data());

std::cout<< "Running "<< FileName<<std::endl;
h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/Events/hEvent_1000_chip0");
cc->SetLogy(0);
//h1D->SetAxisRange(-60,60,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_eventcmmd.pdf");
cc->SaveAs(PdfName.Data());
}

FileBase = TString("../../output/thesis/histogram/XXXXXX-alibava-reco.root");
RunNum.clear();
RunNum.push_back(TString("run000613"));


for (unsigned int i=0; i<RunNum.size(); i++){

TString FileName = FileBase;
FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
file = TFile::Open(FileName.Data());

std::cout<< "Running "<< FileName<<std::endl;
h1D = (TH1D*) file->Get("Common Mode Correction Values");
cc->SetLogy(1);
h1D->SetAxisRange(-60,60,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_sourcecmmd.pdf");
cc->SaveAs(PdfName.Data());
}

FileBase = TString("../../output/thesis/histogram/XXXXXX-alibava-datahisto.root");
RunNum.clear();
RunNum.push_back(TString("run000613"));

for (unsigned int i=0; i<RunNum.size(); i++){
TString FileName = FileBase;
FileName.ReplaceAll(TString("XXXXXX"), RunNum[i]);
file = TFile::Open(FileName.Data());

std::cout<< "Running "<< FileName<<std::endl;
h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSignal_chip0");
cc->SetLogy(1);
h1D->SetAxisRange(-200,600,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_recoSignal.pdf");
cc->SaveAs(PdfName.Data());

TH2D *h2D = (TH2D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSignal_vs_Time_chip0");
h2D->SetAxisRange(-200,600,"Y");
h2D->SetAxisRange(0,30,"X");
cc->SetLogy(0);
cc->SetLogz(1);
h2D->Draw("colz");
PdfName = TString("pdfs/")+RunNum[i]+TString("_recoSignalvsTime.pdf");
cc->SaveAs(PdfName.Data());

TLine line1 = TLine(5,-200,5,600);
line1.Draw();
TLine line2 = TLine(15,-200,15,600);
line2.Draw();
cc->Modify();
PdfName = TString("pdfs/")+RunNum[i]+TString("_recoSignalvsTime_cuts.pdf");
cc->SaveAs(PdfName.Data());



h2D = (TH2D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSNR_vs_Time_chip0");
h2D->SetAxisRange(-50,200,"Y");
h2D->SetAxisRange(0,30,"X");
cc->SetLogy(0);
cc->SetLogz(1);
h2D->Draw("colz");
PdfName = TString("pdfs/")+RunNum[i]+TString("_recoSNRvsTime.pdf");
cc->SaveAs(PdfName.Data());


h1D = (TH1D*) file->Get("MyAlibavaDataHistogramMaker/recodata_cmmd/hSNR_chip0");
cc->SetLogy(1);
h1D->SetAxisRange(-50,200,"X");
h1D->Draw();
PdfName = TString("pdfs/")+RunNum[i]+TString("_recoSNR.pdf");
cc->SaveAs(PdfName.Data());


}





}
