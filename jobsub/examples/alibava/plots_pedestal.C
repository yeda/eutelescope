#include <iostream.h>
#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TFile.h"

using namespace std;
void plots_pedestal(int dutnum){

TString filepath = TString("/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output/histogram/runXXXXXX-alibava-pedestal2.root");
TString csvfilename = TString("runlistfiles/ped_file_info.csv");

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
runnums.pop_back();

TFile *f;
TH1D *h;
TCanvas *cc = new TCanvas("cc","",800,600);
TString pdffilename = TString("pdfs/plots_pedestal_dut")+TString::Itoa(dutnum,10)+TString(".pdf");
cc->Print((pdffilename+TString("(")).Data());

TString foldername=TString("MyAlibavaPedestalNoiseProcessor/");
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
	for (int ihist=0; ihist<2; ihist++){
		if (ihist==0) histname = TString("hpedestal_chip");
		else histname = TString("hnoise_chip");

		for (int ichip=0; ichip<2; ichip++){
			cc->cd(ihist*2+ichip+1);
			TString hist = foldername+histname;
			hist+=ichip;
			h= (TH1D*)f->Get(hist.Data());
			cout<<"Got histogram "<<hist<<endl;
			TString title = run+histname;
			h->SetTitle(title.Data());
			h->Draw();
		}		

	}		
	cc->Update();
	cc->Print(pdffilename.Data());
	cc->Clear();
}


cc->Print((pdffilename+TString(")")).Data());


}



