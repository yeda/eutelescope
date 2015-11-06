#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TF1.h"
#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <cstdlib>

#include "langaufit.h"
using namespace std;

//Declaration of leaves types
   Int_t           runum;
   Int_t           evtnum;
   Int_t           hitnum;
   Int_t           dutID;
   Float_t         locxpos;
   Float_t         locypos;
   Float_t         locxslope;
   Float_t         locyslope;
   Int_t           clustersize;
   Int_t           clusterxchan[10];
   Int_t           clusterychan[10];
   Float_t         clustercharge[10];


struct FileInfo{ 
	int DutNum;  
	int RunNum;
	int DutID;   
	int Bias;
	string Irrad;
	TH1D *hHitAmp;
	TF1 *fLangau;
	void print(){
		cout<<"DutNum "<< DutNum <<" RunNum "<<RunNum<<" DutID "<<DutID<< " Bias "<<Bias<<" Irrad "<<Irrad<<endl;
	}
};

void setBranches(TTree* treeDUT);
vector<FileInfo> readCsvFile(TString csv_file_name, int dutNum);
TF1* fit_langaus(TH1D *hist);
void printFitPrameters(vector<FileInfo> fileinfo);
void printExcelVersion(vector<FileInfo> fileinfo);
void cce_plot(TString dutNum, TString iteration);
void for_Susanne();

int main(int argc, char *argv[]){
	TString dutNum = TString(argv[1]);
	TString iteration = TString(argv[2]);
	cce_plot(dutNum, iteration);
	//for_Susanne();
	return 0;
}
void for_Susanne(){
		TString input_rootfile = TString("output/run000155/dutTree.root");
		TFile *fin = TFile::Open(input_rootfile.Data());
   		TTree* treeDUT= (TTree*) fin->Get("treeDUT");
		setBranches(treeDUT);
		TCanvas *cc =new TCanvas("cc","",800,600);	
		TString histoname = TString("hHitpos_vs_SeedCharge");
		TProfile *hHitAmp = new TProfile(histoname.Data(),histoname.Data(),10, 0, 0.0745);
   		Long64_t nentries = treeDUT->GetEntries();
		for (Long64_t ientry=0; ientry<nentries;ientry++) {
      			treeDUT->GetEntry(ientry);
			if (dutID==6){
				double totcharge=0;
				for(int iclu=0; iclu<clustersize; iclu++){
					totcharge += clustercharge[iclu];
				}
				
				if(locxpos>0){
				double hitpos = fmod(double(locxpos),0.0745); 
				//double hitpos = fmod(double(locxpos-0.0745/2.0),0.0745); 
				hHitAmp->Fill(hitpos,clustercharge[0]);}
			}	
   		}
		hHitAmp->Draw();
		cc->SaveAs("Susanne.C");
}	
void cce_plot(TString dutNum, TString iteration){
	TString fout_name = TString("cce_dut")+dutNum+TString(".root");
	TFile *fout = new TFile(fout_name.Data(),"RECREATE");

	
	TString csvfilename = TString("../runlistfiles/cce_file_info.csv");
	vector<FileInfo> fileinfo = readCsvFile(csvfilename, atoi(dutNum.Data()));
	
	for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	
		fileinfo[ifile].print();
		TString input_rootfile = TString("output/runXXXXXX/dutTree_YYYY.root");
       	 	TString runname = TString::Itoa(fileinfo[ifile].RunNum,10);
        	while (runname.Length()<6) runname= TString("0")+runname;

		input_rootfile.ReplaceAll(TString("XXXXXX"),runname);
		input_rootfile.ReplaceAll(TString("YYYY"),iteration);
		
		TFile *fin = TFile::Open(input_rootfile.Data());
   		TTree* treeDUT= (TTree*) fin->Get("treeDUT");
		setBranches(treeDUT);
	
		TString histoname = TString("hHitAmp_run")+TString::Itoa(fileinfo[ifile].RunNum,10)+TString("_id")+TString::Itoa(fileinfo[ifile].DutID,10);
		fileinfo[ifile].hHitAmp = new TH1D(histoname.Data(),histoname.Data(),100, 0, 500);
   		Long64_t nentries = treeDUT->GetEntries();
		for (Long64_t ientry=0; ientry<nentries;ientry++) {
      			treeDUT->GetEntry(ientry);
			if (dutID==fileinfo[ifile].DutID){
				double totcharge=0;
				for(int iclu=0; iclu<clustersize; iclu++){
					totcharge += clustercharge[iclu];
				}
				fileinfo[ifile].hHitAmp->Fill(totcharge);
			}	
   		}
		fout->cd();
		fileinfo[ifile].fLangau = fit_langaus(fileinfo[ifile].hHitAmp);
		fileinfo[ifile].hHitAmp->Write();
		//fileinfo[ifile].fLangau->Write();
	}
	printFitPrameters(fileinfo);
        double biasvolt[2][20];
	double mp[2][20];
	int nValue[2];
        nValue[0]=0; nValue[1]=0;
	for (unsigned int ifile=0; ifile<fileinfo.size(); ifile++){
		int ichip=5;
                if(fileinfo[ifile].DutID == 6) ichip=0;
                if(fileinfo[ifile].DutID == 7) ichip=1;
		
		biasvolt[ichip][nValue[ichip]] = fabs(fileinfo[ifile].Bias);
                mp[ichip][nValue[ichip]] = fileinfo[ifile].fLangau->GetParameter(1);
		nValue[ichip]= nValue[ichip]+1;
        }

        TGraph *g0 = new TGraph(nValue[0],biasvolt[0],mp[0]);
        TGraph *g1 = new TGraph(nValue[1],biasvolt[1],mp[1]);
	fout->cd();
	g0->Write();
	g1->Write();
	
	printExcelVersion(fileinfo);
}

void printExcelVersion(vector<FileInfo> fileinfo){
	cout<<"        LanGau fit parameters         "<<endl;
	cout<<"**************************************"<<endl;
	ofstream excelfile;
	TString excelfilename = TString("cce_dut")+TString::Itoa(fileinfo[0].DutNum,10)+TString(".txt");
	excelfile.open(excelfilename);
	// First do for Sensor 6
	for (unsigned int ifile=0; ifile<fileinfo.size(); ifile++){
		FileInfo afile = fileinfo[ifile];
		if (afile.DutID == 6){
	 	TF1 *fitfunc = (TF1*)afile.fLangau;
		double mp = fitfunc->GetParameter(1);
		double mperr = fitfunc->GetParError(1);	
		cout<<"Fluence; "<<afile.Irrad<<"; DutID; "<<afile.DutID<<"; Bias; "<< afile.Bias<<"; MP; "<<mp<<"; MPErr; "<<mperr<<";"<<endl;
		excelfile<<"Fluence; "<<afile.Irrad<<"; DutID; "<<afile.DutID<<"; Bias; "<< afile.Bias<<"; MP; "<<mp<<"; MPErr; "<<mperr<<";"<<endl;
		}
	}
	// Then do for Sensor 6
	for (unsigned int ifile=0; ifile<fileinfo.size(); ifile++){
		FileInfo afile = fileinfo[ifile];
		if (afile.DutID == 7){
	 	TF1 *fitfunc = (TF1*)afile.fLangau;
		double mp = fitfunc->GetParameter(1);
		double mperr = fitfunc->GetParError(1);	
		cout<<"Fluence; "<<afile.Irrad<<"; DutID; "<<afile.DutID<<"; Bias; "<< afile.Bias<<"; MP; "<<mp<<"; MPErr; "<<mperr<<";"<<endl;
		excelfile<<"Fluence; "<<afile.Irrad<<"; DutID; "<<afile.DutID<<"; Bias; "<< afile.Bias<<"; MP; "<<mp<<"; MPErr; "<<mperr<<";"<<endl;
		}
	}
	excelfile.close();
}


void printFitPrameters(vector<FileInfo> fileinfo){
	cout<<"        LanGau fit parameters         "<<endl;
	cout<<"**************************************"<<endl;

	for (unsigned int ifile=0; ifile<fileinfo.size(); ifile++){
		FileInfo afile = fileinfo[ifile];
		TH1D *hist = afile.hHitAmp;
	 	TF1 *fitfunc = (TF1*)afile.fLangau;
		double width = fitfunc->GetParameter(0);
		double mp = fitfunc->GetParameter(1);
		double area = fitfunc->GetParameter(2);
		double gsigma = fitfunc->GetParameter(3);
		
		afile.print();
		cout<<"MP= "<<mp<<", Width= "<<width<<", Sigma= "<<gsigma<<", Area= "<< area <<endl;
	}
}


TF1* fit_langaus(TH1D *hist){
   // Here are the Landau * Gaussian parameters:
   //   par[0]=Width (scale) parameter of Landau density
   //   par[1]=Most Probable (MP, location) parameter of Landau density
   //   par[2]=Total area (integral -inf to inf, normalization constant)
   //   par[3]=Width (sigma) of convoluted Gaussian function
   //
   // Variables for langaufit call:
   //   his             histogram to fit
   //   fitrange[2]     lo and hi boundaries of fit range
   //   startvalues[4]  reasonable start values for the fit
   //   parlimitslo[4]  lower parameter limits
   //   parlimitshi[4]  upper parameter limits
   //   fitparams[4]    returns the final fit parameters
   //   fiterrors[4]    returns the final fit errors
   //   ChiSqr          returns the chi square
   //   NDF             returns ndf
	
	TCanvas *c0 = new TCanvas("tmpcanvas","tmpcanvas",800,600);
	double area= hist->Integral();
	//double area= hist->Integral("width");
	// Langaus fit settings
	Double_t fitrange[2];
	fitrange[0]= 0.5*hist->GetMean();
	fitrange[1]= 1.5*hist->GetMean();
 	
	
	TF1 *gausFit= new TF1("tempGausFit","gaus");
	hist->Fit(gausFit,"Q");
	double gausmean = gausFit->GetParameter(1);
	double gaussigma = gausFit->GetParameter(2);

	TF1 *lanFit= new TF1("tempLandauFit","landau");
	hist->Fit(lanFit,"Q");
	double lanmpv = gausFit->GetParameter(1);
	double lanwidth = gausFit->GetParameter(2);
	
	Double_t startvalues[4], parlimitslow[4], parlimitshigh[4];
	Double_t fitparameters[4], fitparameters_error[4];
 
 //   par[0]=Width (scale) parameter of Landau density
	startvalues[0]=lanwidth; parlimitslow[0]=startvalues[0]/5.0; parlimitshigh[0]=2.0*startvalues[0];
 //   par[1]=Most Probable (MP, location) parameter of Landau density
	startvalues[1]=(gausmean+lanmpv)/3.0; parlimitslow[1]=lanmpv/5.0; parlimitshigh[1]=gausmean;
 //   par[2]=Total area (integral -inf to inf, normalization constant)
	startvalues[2]=area; parlimitslow[2]=area/10.0; parlimitshigh[2]=area*10.0;
 //   par[3]=Width (sigma) of convoluted Gaussian function
	startvalues[3]=fabs(gausmean-lanwidth)/4.0; parlimitslow[3]=startvalues[3]/5.0; parlimitshigh[3]=2.0*startvalues[3];
	
	Double_t chisqr;
	Int_t    ndf;
	Double_t LangauPeak, LangauFWHM;
	
	TF1 *fitlangau;
	char tmpstring[8][20]={"Width","MP","Area","Gsigma","chisqr","langaupeak","langauFWHM", "Prob"};
	
	fitlangau = langaufit(hist,fitrange,startvalues,parlimitslow,parlimitshigh,fitparameters,fitparameters_error,&chisqr,&ndf);
 	langaupro(fitparameters,LangauPeak,LangauFWHM);
	
	 
	//hist->Draw();	
 	delete c0;
	delete lanFit;
	delete gausFit;
	return fitlangau;
}





vector<FileInfo> readCsvFile(TString csv_file_name, int dutNum){
	vector<FileInfo> fileinfo;
	ifstream csv_file(csv_file_name.Data());
	string firstline;
	getline(csv_file,firstline);
	// first line: DutNum  RunNum  DutID   Bias    Irrad
	string s;
	while(!csv_file.eof()){
		
		FileInfo info;
		getline(csv_file,s,';');
		info.DutNum = atoi(s.c_str());
		
		getline(csv_file,s,';');
		info.RunNum = atoi(s.c_str());
		
		getline(csv_file,s,';');
		info.DutID = atoi(s.c_str());
		
		getline(csv_file,s,';');
		info.Bias = atoi(s.c_str());
	
		getline(csv_file,info.Irrad,';');
		if (info.DutID !=0 && info.DutNum == dutNum) fileinfo.push_back(info);	
	}
	// for some reason getline reads extra one line 
	// so we remove it from the vector
	//fileinfo.pop_back();
	return fileinfo;
}
   	
void setBranches(TTree* treeDUT){
   // Set branch addresses.
   treeDUT->SetBranchAddress("runnum",&runum);
   treeDUT->SetBranchAddress("evtnum",&evtnum);
   treeDUT->SetBranchAddress("hitnum",&hitnum);
   treeDUT->SetBranchAddress("dutID",&dutID);
   treeDUT->SetBranchAddress("locxpos",&locxpos);
   treeDUT->SetBranchAddress("locypos",&locypos);
   treeDUT->SetBranchAddress("locxslope",&locxpos);
   treeDUT->SetBranchAddress("locyslope",&locypos);
   treeDUT->SetBranchAddress("clustersize",&clustersize);
   treeDUT->SetBranchAddress("clusterxchan",clusterxchan);
   treeDUT->SetBranchAddress("clusterychan",clusterychan);
   treeDUT->SetBranchAddress("clustercharge",clustercharge);

}




