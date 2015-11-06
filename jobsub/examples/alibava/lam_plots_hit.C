#include <iostream.h>
#include <vector>
#include "TString.h"
#include "TH2D.h"
#include "TFile.h"

using namespace std;

void lam_plots_hit(int dutnum){

TString filepath = TString("/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output/histogram/runXXXXXX-hitmaker-local.root");

TString csvfilename = TString("runlistfiles/lam_file_info.csv");
vector<int> runnums;
vector<string> rotationstring;
vector<string> Bfieldstring;

ifstream csv_file(csvfilename.Data());
string firstline;
getline(csv_file,firstline);
// DutNum;RunNum;DutID;Irrad;Rotation;RotationSensor;RotationStage;NegRotation;BiasVolt;SensorTemp;BeamEnergy;B;CalculatedB;CurrentPS
string s;
int DutNum, RunNum, DutID, BiasVolt;
double Rotation, RotationSensor, RotationStage, NegRotation, SensorTemp, BeamEnergy, B, CalculatedB, CurrentPS; 
string Irrad;
while(!csv_file.eof()){
	getline(csv_file,s,';');
	DutNum = atoi(s.c_str());

	getline(csv_file,s,';');
	RunNum = atoi(s.c_str());

	getline(csv_file,s,';');
	DutID = atoi(s.c_str());

	getline(csv_file,Irrad,';');

	getline(csv_file,s,';');
	Rotation = atof(s.c_str());

	getline(csv_file,s,';');
	RotationSensor = atof(s.c_str());

	getline(csv_file,s,';');
	RotationStage = atof(s.c_str());

	getline(csv_file,s,';');
	string rotation_string=s;
	NegRotation = atof(s.c_str());

	getline(csv_file,s,';');
	BiasVolt = atoi(s.c_str());
	
	getline(csv_file,s,';');
	SensorTemp = atof(s.c_str());

	getline(csv_file,s,';');
	BeamEnergy = atof(s.c_str());

	getline(csv_file,s,';');
	string Bstring = s;
	B = atof(s.c_str());

	getline(csv_file,s,';');
	CalculatedB = atof(s.c_str());

	getline(csv_file,s,';');
	CurrentPS = atof(s.c_str());

	if (DutNum == dutnum && DutID == 6) {
		runnums.push_back(RunNum);
		rotationstring.push_back(rotation_string);	
		Bfieldstring.push_back(Bstring);
	}
}
//runnums.pop_back();

TFile *f;
TH2D *h;
TCanvas *cc = new TCanvas("cc","",800,600);

//TString Bname = TString::Itoa(int(Bfield*100),10);
//while (Bname.Length()<3) Bname= TString("0")+Bname;
TString pdffilename = TString("pdfs/lam_plots_hit_dut")+TString::Itoa(dutnum,10)+TString(".pdf");
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
		TString title = run+TString("_B")+TString(Bfieldstring[j])+TString("_R")+ TString(rotationstring[j])+ TString("_")+hist;
		h->SetTitle(title.Data());	
		h->Draw("colz");

	}		
	cc->Update();
	cc->Print(pdffilename.Data());
	cc->Clear();
}


cc->Print((pdffilename+TString(")")).Data());


}



