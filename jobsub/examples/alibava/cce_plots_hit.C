#include <iostream.h>
#include <vector>
#include "TString.h"
#include "TH2D.h"
#include "TFile.h"
#include "TROOT.h"
#include "TPad.h"

using namespace std;

void cce_plots_hit(int dutnum, double Bfield){

TString filepath = TString("/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output/histogram/runXXXXXX-hitmaker-local.root");

TString csvfilename = TString("runlistfiles/cce_file_info.csv");
vector<int> runnums;
ifstream csv_file(csvfilename.Data());
string firstline;
getline(csv_file,firstline);
// first line: DutNum  RunNum  DutID   Bias    Irrad
string s;
int DutNum, RunNum, DutID,Bias;
string Irrad;
while(!csv_file.eof()){
	getline(csv_file,s,';');
	DutNum = atoi(s.c_str());

	getline(csv_file,s,';');
	RunNum = atoi(s.c_str());

	getline(csv_file,s,';');
	DutID = atoi(s.c_str());

	getline(csv_file,s,';');
	Bias = atoi(s.c_str());
	
	getline(csv_file,Irrad,';');
		
	if (DutNum == dutnum && DutID == 6) runnums.push_back(RunNum);	
}
//runnums.pop_back();


TFile *f;
TH2D *h;
TCanvas *cc = new TCanvas("cc","",800,600);

TString Bname = TString::Itoa(int(Bfield*100),10);
while (Bname.Length()<3) Bname= TString("0")+Bname;
TString pdffilename = TString("pdfs/cce_plots_hit_dut")+TString::Itoa(dutnum,10)+TString("_B")+Bname+TString(".pdf");
cc->Print((pdffilename+TString("(")).Data());

TString foldername=TString("MyAlibavaCorrelator/hSyncX/");
//sort(runnums.begin(),runnums.end());
for (unsigned int j=0; j<runnums.size(); j++){
cc->Divide(2,2);
	TString run = TString::Itoa(runnums[j],10);
	while (run.Length()<6) run= TString("0")+run;

	TString file = filepath;
	file = file.ReplaceAll(TString("XXXXXX"),run);
	f = TFile::Open(file.Data());
	TString histname;
	cout<<"Running file "<< file <<"   "<<j<<"/"<<runnums.size()<<endl;
	for (int ihist=0; ihist<4; ihist++){
		if (ihist==0) histname = TString("hSyncX_d3_d6");
		else if(ihist==1) histname= TString("hSyncX_d3_d7");
		else if(ihist==2) histname= TString("hSyncX_d4_d6");
		else if(ihist==3) histname= TString("hSyncX_d4_d7");

		cc->cd(ihist+1);
		TString hist = foldername+histname;
		h= (TH2D*)f->Get(hist.Data());
		cout<<"Got histogram "<<hist<<endl;
		TString title = run+TString("_")+hist;
		h->SetTitle(title.Data());	
		gPad->SetLogz(1);
		h->Draw("colz");

	}		
	cc->Update();
	cc->Print(pdffilename.Data());
	cc->Clear();
}


cc->Print((pdffilename+TString(")")).Data());


}



