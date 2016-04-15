#include <iostream>
#include <fstream>
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
#include "TGraphErrors.h"
#include "TMath.h"
#include <cstdlib>

#include "lam_histos.h"
using namespace std;

TString InputPath = TString("output/notcomb-xtalk");
TString OutputPath = TString("results/notcomb-xtalk");
vector<LAmeas*> measurements;
TString csvfilename = TString("../runlistfiles/lam_file_info.csv");
TFile *fout;
TCanvas *cc = new TCanvas("cc","cc",800,600);
TString dutNum;	
vector<FileInfo> fileinfo; 

int main(int argc, char *argv[]){
	dutNum = TString(argv[1]);
	TString iteration = TString(argv[2]);
	measurements.clear();
	
	TString fout_name = OutputPath + TString("/lam_dut")+dutNum+TString(".root");
	fout = new TFile(fout_name.Data(),"RECREATE");

	fileinfo = readCsvFile();
	createMeasurements( fileinfo );

	lam_plot_eachtrack(iteration);
	//lam_plot_eachdata(iteration);
	
//	writeObjects();
	createGraphs();
	return 0;
}

void lam_plot_eachdata(TString iteration){
	double mean_locxslope[50];
	double mean_locxslope_err[50];
	double mean_clusize[50];
	double mean_clusize_err[50];
	int i=0;

	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){ 
		fill(mean_locxslope, mean_locxslope+50, 0);
		fill(mean_locxslope_err, mean_locxslope_err+50, 0);
		fill(mean_clusize, mean_clusize+50, 0);
		fill(mean_clusize_err, mean_clusize_err+50, 0);

		i=0;
		for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	
	
			if(measurements[imeas]->getDutID() != fileinfo[ifile].DutID || measurements[imeas]->getB() != fileinfo[ifile].B || measurements[imeas]->getBias() != fileinfo[ifile].Bias) continue;
	
			fileinfo[ifile].print();

	   		TTree* treeDUT= getTree(fileinfo[ifile].RunNum, iteration);
			setBranches(treeDUT);
		
			TString cut = TString("dutID==")+TString::Itoa(fileinfo[ifile].DutID,10);
			treeDUT->Draw("(-1 * locxslope * 180.0 / TMath::Pi())>>hslope(100,-25,25)", cut.Data());
			TH1D* hslope = (TH1D*)gDirectory->Get("hslope");
			TF1 *fitfunc = fit_langaus(hslope);
			mean_locxslope[i] = fitfunc->GetParameter(1);
			mean_locxslope_err[i] = fitfunc->GetParError(1);
//			mean_locxslope[i] = hslope->GetMean();
//			mean_locxslope_err[i] = hslope->GetMeanError();
	
			treeDUT->Draw("clustersize>>hclu(20,0,20)", cut.Data());
			TH1D* hclu = (TH1D*)gDirectory->Get("hclu");
			mean_clusize[i] = hclu->GetMean();
			mean_clusize_err[i] = hclu->GetMeanError();
	
			i++;		
		}
		
		TGraphErrors* gr = new TGraphErrors(i,mean_locxslope,mean_clusize,mean_locxslope_err,mean_clusize_err);
		TString grname = measurements[imeas]->getGraphName();
		gr->SetName(grname.Data());
		grname = grname+TString("; Mean Incidence Angle(degrees); Mean Cluster Size");
		gr->SetTitle(grname.Data());
		measurements[imeas]->setGraph(gr);
					
	}
	fitGraphs();
}


void lam_plot_eachtrack(TString iteration){
	
	for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	
		fileinfo[ifile].print();

   		TTree* treeDUT= getTree(fileinfo[ifile].RunNum, iteration);
		setBranches(treeDUT);
	
		LAmeas* currentMeas =  getMeasurement(fileinfo[ifile].DutID, fileinfo[ifile].B, fileinfo[ifile].Bias );	
		TProfile* currentProfile = currentMeas->getProfile();

		Long64_t nentries = treeDUT->GetEntries();
		for (Long64_t ientry=0; ientry<nentries;ientry++) {
      			treeDUT->GetEntry(ientry);
			if ( dutID==currentMeas->getDutID() ){
				double slope = locxslope * 180.0 / TMath::Pi();
				currentProfile->Fill(slope,clustersize);
			}	
   		}
	}

	fitProfiles();
}

void createGraphs(){
	vector<int> storedBias;
	storedBias.clear();

	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		int abias = measurements[imeas]->getBias();
		bool alreadthere = false;
		for (unsigned int ibias=0; ibias<storedBias.size(); ibias++){
			if(storedBias[ibias]==abias) {
				alreadthere = true;
				break;
			}
		}
		if ( alreadthere==false ) storedBias.push_back(abias);	
	}
	
	for (unsigned int ibias=0; ibias<storedBias.size(); ibias++){
		vector<int> dutid;
		
		// store dut ids that measured at this bias voltage
		for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
			if (measurements[imeas]->getBias() == storedBias[ibias]){
				int curid = measurements[imeas]->getDutID();
				
				// is dut id stored
				bool alreadthere = false;
				for (unsigned int iID = 0 ; iID<dutid.size(); iID++){
					if( dutid[iID]==curid) alreadthere = true;
				}

				if ( alreadthere==false ) dutid.push_back(curid);
			}
		}

		for (unsigned int iId=0; iId<dutid.size(); iId++){
			
			double magfield[10];
			double magfielderr[10];
			double LA[10];
			double LAerr[10];

			int i = 0;
			for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
				if (measurements[imeas]->getBias() == storedBias[ibias] && measurements[imeas]->getDutID() == dutid[iId]){
					magfield[i] = measurements[imeas]->getB();
					magfielderr[i] = 0.0;
					LA[i] = measurements[imeas]->getLA();
					LAerr[i] = measurements[imeas]->getLA();
		
					i++;
				}
			}		

			TGraphErrors* gr = new TGraphErrors(i,magfield,LA,magfielderr,LAerr);
			TString title = TString("graph_dut")+TString::Itoa(dutid[iId],10)+TString("_V")+TString::Itoa(storedBias[ibias],10);
			gr->SetMarkerStyle(20);
			gr->SetTitle(title.Data());
			gr->SetName(title.Data());
			
			title = OutputPath + TString("/dut_")+dutNum+TString("_")+TString::Itoa(dutid[iId],10)+TString("_V")+TString::Itoa(storedBias[ibias],10)+TString(".pdf");
			cc->cd();
			gr->Draw("ALP");
			cc->SaveAs(title.Data());

			fout->cd();
			gr->Write();
		
		}
	}
}




void writeObjects(){
	fout->cd();
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		TProfile* profile = measurements[imeas]->getProfile();
		if (profile->Integral() != 0) profile->Write();
		TGraphErrors* gr = measurements[imeas]->getGraph();
		if (gr->Integral() != 0) gr->Write();
	}

}
void fitGraphs(){
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		TGraphErrors* graph = measurements[imeas]->getGraph();
		TString hname = measurements[imeas]->getGraphFitName();
		TF1* f = new TF1(hname.Data(),fit_func,-20,20,4);
		
  		f->SetParNames("LA", "a", "b", "sigma");
  		f->SetParLimits(3, 0.1, 10.);
  		f->SetParameters(0,1,1.13,5);
		graph->Fit(hname.Data(),"RQ");
		cout<< hname << " created"<< " LA= "<<f->GetParameter(0)<<" +/- "<< f->GetParError(0)<<endl;
		measurements[imeas]->setLA(f->GetParameter(0));
		measurements[imeas]->setLAerror(f->GetParError(0));
	}

}

void fitProfiles(){
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		TProfile* profile = measurements[imeas]->getProfile();
		TString hname = measurements[imeas]->getProfileFitName();
		TF1* f = new TF1(hname.Data(),fit_func,-20,20,4);
		
  		f->SetParNames("LA", "a", "b", "sigma");
  		f->SetParLimits(3, 0.1, 5.);
  		f->SetParLimits(0, -10.0, 10.0);
  		f->SetParameters(0,1,1.13,5);
		profile->Fit(hname.Data(),"RQ");
		cout<< hname << " created"<< " LA= "<<f->GetParameter(0)<<" +/- "<< f->GetParError(0)<<endl;
		measurements[imeas]->setLA(f->GetParameter(0));
		measurements[imeas]->setLAerror(f->GetParError(0));
	}

}

bool isMeasStored(LAmeas *ameas){
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		if ( measurements[imeas]->getDutID()==ameas->getDutID() && measurements[imeas]->getB()==ameas->getB() && measurements[imeas]->getBias()==ameas->getBias() ){
			return true;
		}
	}
	return false;
}

void createMeasurements(vector<FileInfo> fileinfo){
	for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	
		LAmeas* ameas = new LAmeas();
		ameas->setDutID( fileinfo[ifile].DutID );
		ameas->setB( fileinfo[ifile].B );
		ameas->setBias( fileinfo[ifile].Bias );
		
		if (isMeasStored(ameas)==false) {
			TString tempname = ameas->getProfileName();
			TProfile *tempprofile = new TProfile(tempname.Data(),tempname.Data(),25,-25,25);
			ameas->setProfile ( tempprofile );
			measurements.push_back(ameas);
		}
		else{
			delete ameas;			
		}
	}		
	
}

LAmeas* getMeasurement(int dutID, double b, int bias){
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		if ( measurements[imeas]->getDutID()==dutID && measurements[imeas]->getB()==b && measurements[imeas]->getBias()==bias ){
			return measurements[imeas];
		}
	}
	
	cout<< "There is no measurement stored in with DutID= "<<dutID<< " B= "<<b<<" V= "<<bias<<endl;
	cout<< "Returning empty LAmeas "<<endl;
	return new LAmeas();
}





vector<FileInfo> readCsvFile(){
	int dutnum = atoi(dutNum.Data());

	vector<FileInfo> fileinfo;
	ifstream csv_file(csvfilename.Data());
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
		
		getline(csv_file,info.Irrad,';');

		getline(csv_file,s,';'); // Rotation
		getline(csv_file,s,';'); // RotationSensor
		getline(csv_file,s,';'); // RotationStage
		getline(csv_file,s,';'); // NegRotation

		getline(csv_file,s,';');
		info.Bias = atoi(s.c_str());
		info.Bias =-1*info.Bias;
 	
		getline(csv_file,s,';'); // SensorTemp
		getline(csv_file,s,';'); // BeamEnergy

		getline(csv_file,s,';'); // B
		info.B = atof(s.c_str());

		getline(csv_file,s,';'); // CalculatedB
		getline(csv_file,s,';'); // Current on Power supply

		if (info.RunNum != 0 && info.DutID !=0 && info.DutNum == dutnum) {
			fileinfo.push_back(info);
			info.print();
		}	
	}
	return fileinfo;
}
   	
void setBranches(TTree* treeDUT){
   // Set branch addresses.
   treeDUT->SetBranchAddress("runnum",&runnum);
   treeDUT->SetBranchAddress("evtnum",&evtnum);
   treeDUT->SetBranchAddress("Pseg", &Pseg);
   treeDUT->SetBranchAddress("Pgbl", &Pgbl);
   treeDUT->SetBranchAddress("hitnum",&hitnum);
   treeDUT->SetBranchAddress("dutID",&dutID);
   treeDUT->SetBranchAddress("locxpos",&locxpos);
   treeDUT->SetBranchAddress("locypos",&locypos);
   treeDUT->SetBranchAddress("locxslope",&locxslope);
   treeDUT->SetBranchAddress("locyslope",&locyslope);
   treeDUT->SetBranchAddress("clustersize",&clustersize);
   treeDUT->SetBranchAddress("clusterxchan",clusterxchan);
   treeDUT->SetBranchAddress("clusterychan",clusterychan);
   treeDUT->SetBranchAddress("clustercharge",clustercharge);

}

Double_t fit_func(Double_t *x, Double_t *par){
  
  const Int_t    n     = 100; // Number of integration step
  const Double_t xmin = x[0] - 5.0*par[3];
  const Double_t xmax = x[0] + 5.0*par[3];
  const Double_t h    = (xmax - xmin)/(n - 1);
  
  Double_t sum = 0;
  Double_t x_  = xmin;
  
  Double_t Lari_sum = 0;
  Double_t Gaus_sum = 0;

  for (Int_t i=1; i<n; i++) 
    {
      Lari_sum =  par[1]*TMath::Abs(TMath::Tan(x_*pi/180) - TMath::Tan(par[0]*pi/180)) + par[2];
      Gaus_sum =  1/TMath::Sqrt(2*pi)/(par[3])*TMath::Exp(-(x[0] - x_ )*(x[0] - x_ )/(2*par[3]*par[3]))*h;
      sum += Lari_sum*Gaus_sum;
      x_ += h;
    } 
  return   sum;
}

TTree* getTree(int RunNum, TString iteration){
	TString input_rootfile = InputPath + TString("/runXXXXXX/dutTree_YYYY.root");
	TString runname = TString::Itoa(RunNum,10);
	while (runname.Length()<6) runname= TString("0")+runname;

	input_rootfile.ReplaceAll(TString("XXXXXX"),runname);
	input_rootfile.ReplaceAll(TString("YYYY"),iteration);

	TFile *fin = TFile::Open(input_rootfile.Data());
	TTree* treeDUT= (TTree*) fin->Get("treeDUT");
	return treeDUT;
}	


