#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <fstream>
#include <math.h>
#include <vector>
#include <cstdlib>


#include "TString.h"
#include "TH1D.h"
#include "TVectorD.h"
#include "TF1.h"
#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TStyle.h"
#include "TLatex.h"

#include "lam_histos.h"
using namespace std;

TString InputPath = TString("output/notcomb-xtalk-telaligned");
TString OutputPath = TString("results/ITK");
//TString OutputPath = TString("results/notcomb-xtalk-telaligned");
vector<LAmeas*> measurements;
TString csvfilename = TString("../runlistfiles/lam_file_info.csv");
TFile *fout;
TCanvas *cc = new TCanvas("cc","cc",800,600);
TString dutNum;	
vector<FileInfo> fileinfo; 
Color_t color[9] = {kBlack, kGreen+2, kBlue, kMagenta+1, kRed, kOrange-3, kPink-9, kViolet+1, kCyan+2};

const int Nbin_profiles = 25;
double bin_profiles[26]={ -22.0, -18.0, -14.0, -11.0, -9.0, -7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 9.0, 11.0, 14.0, 18.0, 22.0};

int main(int argc, char *argv[]){
	dutNum = TString(argv[1]);
	TString iteration = TString(argv[2]);
	measurements.clear();
	
	TString fout_name = OutputPath + TString("/lam_dut")+dutNum+TString("_it")+iteration+TString(".root");
	fout = new TFile(fout_name.Data(),"RECREATE");

	fileinfo = readCsvFile();
	createMeasurements( fileinfo );

	gStyle->SetOptStat(0);
//	lam_plot_eachtrack(iteration);
	lam_plot_eachtrack_ITK(iteration);	
//	lam_plot_eachdata(iteration);	
//	createGraphs();

	plot_LAcombined(TString("ITK"));
//	plot_LAcombined(TString(""));

//        writetotxt("LA_out.txt");
        writetotxt("LA_ITK_out.txt");

	return 0;
}

void writetotxt(TString f_txt){

	std::ofstream out_txt;
    	out_txt.open(f_txt.Data(), std::ofstream::out | std::ofstream::app);
	 
	for(unsigned int i=0; i<measurements.size(); i++){
		out_txt<<measurements[i]->results_to_txt()<<endl;
	}
}

int sortLegendB(TString s){
	TString Bstr;
	Bstr = s( s.First("B")+1, s.First("V")-s.First("B")-2);
	int bf = Bstr.Atoi();
	
	if(bf==0) return 1;
	else if(bf==25) return 2;
	else if(bf==50) return 3;
	else if(bf==75) return 4;
	else if(bf==100) return 5;
	else return -1;
}

void plot_LAcombined(TString s){
TString _irradString_;	
	TProfile* pr;
	TF1* fit_pr;
	int icolor =0;
	// First find measured Duts and bias
	vector<int> measuredBias;
	vector<int> measuredDut;
	int tmpDut, tmpBias;
 	for (unsigned int i=0;i<measurements.size();i++){
		if (i == 0){
			measuredBias.push_back(measurements[i]->getBias());
			measuredDut.push_back(measurements[i]->getDutID());
		}else{
			tmpDut=measurements[i]->getDutID();
			tmpBias=measurements[i]->getBias();
			bool Dutstored=false;
			bool Biasstored = false;
			for (unsigned int j=0; j<measuredBias.size(); j++){
				if(measuredBias[j] == tmpBias) 
					Biasstored=true;
			}
			for (unsigned int j=0; j<measuredDut.size(); j++){
				if(measuredDut[j] == tmpDut) 
					Dutstored=true;
			}
			
			if (!Dutstored) measuredDut.push_back(tmpDut);
			if (!Biasstored)  measuredBias.push_back(tmpBias);
		}
	}
	
	// Now create canvas for each dut and bias combination
	for (unsigned int i=0; i<measuredDut.size(); i++){
		for(unsigned j=0; j<measuredBias.size(); j++){
			icolor=0;
			string canvasname = string("LA_dut")+ to_string(measuredDut[i])+string("_V")+to_string(measuredBias[j]);
			TCanvas *cc = new TCanvas(canvasname.c_str(),canvasname.c_str(),800, 600);
			formatCanvas1D(cc);	
			cc->cd();

	gStyle->SetOptStat(0);

	map<int, LAmeas*> sorting;
			for (unsigned int imeas=0; imeas< measurements.size(); imeas++){
				if (measurements[imeas]->getDutID() == measuredDut[i] && measurements[imeas]->getBias() == measuredBias[j]){
					pr = (TProfile*) measurements[imeas]->getProfile();
					fit_pr = (TF1*) measurements[imeas]->getFitProfile();

					sorting.insert(make_pair( sortLegendB( pr->GetName() ), measurements[imeas] ) );
					if (s==TString("ITK")){
						if (dutNum == TString("1") && measuredDut[i]==6) pr->SetAxisRange(1.2,1.6,"Y");
						if (dutNum == TString("1") && measuredDut[i]==7) pr->SetAxisRange(1.05,1.25,"Y");
						if (dutNum == TString("2") && measuredDut[i]==6) pr->SetAxisRange(0.95,1.15,"Y");
						if (dutNum == TString("2") && measuredDut[i]==7) pr->SetAxisRange(0.95,1.1,"Y");
						if (dutNum == TString("3") && measuredDut[i]==6) pr->SetAxisRange(1.2,1.7,"Y");
						if (dutNum == TString("3") && measuredDut[i]==7) pr->SetAxisRange(1.1,1.35,"Y");
						if (dutNum == TString("4")) pr->SetAxisRange(1.2,2.5,"Y");
						if (dutNum == TString("5") && measuredDut[i]==6) pr->SetAxisRange(1.4,2.4,"Y");
						if (dutNum == TString("5") && measuredDut[i]==7) pr->SetAxisRange(1.4,2.1,"Y");
						if (dutNum == TString("6") && measuredDut[i]==6) pr->SetAxisRange(1.45,2.5,"Y");
						if (dutNum == TString("6") && measuredDut[i]==7) pr->SetAxisRange(1.4,2.2,"Y");
					}
					else {
						if (dutNum == TString("1") && measuredDut[i]==6) pr->SetAxisRange(1.45,2.0,"Y");
						if (dutNum == TString("1") && measuredDut[i]==7) pr->SetAxisRange(1.3,1.7,"Y");
						if (dutNum == TString("2") && measuredDut[i]==6) pr->SetAxisRange(1.0,1.5,"Y");
						if (dutNum == TString("2") && measuredDut[i]==7) pr->SetAxisRange(1.0,1.5,"Y");
						if (dutNum == TString("3") && measuredDut[i]==6) pr->SetAxisRange(1.3,2.2,"Y");
						if (dutNum == TString("3") && measuredDut[i]==7) pr->SetAxisRange(1.3,1.7,"Y");
						if (dutNum == TString("4")) pr->SetAxisRange(1.8,3.0,"Y");
						if (dutNum == TString("5") && measuredDut[i]==6) pr->SetAxisRange(2.2,3.2,"Y");
						if (dutNum == TString("5") && measuredDut[i]==7) pr->SetAxisRange(1.7,2.8,"Y");
						if (dutNum == TString("6") && measuredDut[i]==6) pr->SetAxisRange(2.4,3.4,"Y");
						if (dutNum == TString("6") && measuredDut[i]==7) pr->SetAxisRange(2.0,3.0,"Y");
					}
					TString title;
					title = TString("; Incidence Angle (degrees); Cluster Size");
//					title = TString("LAM ")+ measurements[imeas]->getIrrad()+TString("; Incidence Angle (degrees); Cluster Size");
					pr->SetTitle(title.Data());
					_irradString_ = measurements[imeas]->getIrrad();
		        		gStyle->SetOptStat(0);
					formatProfile(pr);
					pr->Draw("same");
				}
			}

			double leg_y = 0.92 - 0.05 * sorting.size(); 
		        TLegend* leg = new TLegend(0.29,0.92,0.73,leg_y);

//			leg->SetTextSize(22);
		        leg->SetFillColor(0);
		        leg->SetLineColor(0);
		        leg->SetHeader("Magnetic Field (T)");

		        for (map< int, LAmeas* >::iterator i_entry=sorting.begin(); i_entry!= sorting.end(); i_entry++){	
					LAmeas* lameas= i_entry->second; 
					pr = (TProfile*) lameas->getProfile();
					fit_pr = (TF1*) lameas->getFitProfile();
					pr->SetLineColor(color[i_entry->first -1]);
					fit_pr->SetLineColor(color[i_entry->first -1]);
					std::ostringstream ss;
					ss<< string("B= ")<< fixed<< setprecision(2)<< lameas->getB() << string(", #theta_{L} = ")<<fixed<<setprecision(2)<<fit_pr->GetParameter(0)<< string(" #pm ")<<fixed<<setprecision(2)<<fit_pr->GetParError(0);
					TString title = TString(ss.str());

	            		    	leg->AddEntry(pr,title.Data(),"l");
			}

			if(dutNum != TString("2")) leg->Draw();

        TLatex *latex =new TLatex();
        latex->SetNDC();
        latex->SetTextFont(43);
        latex->SetTextColor(kOrange+7);
        latex->SetTextSize(26);
_irradString_ = TString("#bf{")+_irradString_+TString("}");
        latex->DrawLatex(0.65,0.2,_irradString_.Data());


			TString title = OutputPath + TString("/LA_dut_")+dutNum+TString("_")+TString::Itoa(measuredDut[i],10)+TString("_V")+TString::Itoa(measuredBias[j],10)+TString(".pdf");
			cc->SaveAs(title.Data());
			fout->cd();
			cc->Write();
		}

	}
	
}

void lam_plot_eachtrack(TString iteration){
	
	gStyle->SetOptStat(0);
	for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	

//		fileinfo[ifile].print();
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


void lam_plot_eachtrack_ITK(TString iteration){
	
	gStyle->SetOptStat(0);
	for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	

//		fileinfo[ifile].print();
   		TTree* treeDUT= getTree(fileinfo[ifile].RunNum, iteration);
		setBranches(treeDUT);
	
		LAmeas* currentMeas =  getMeasurement(fileinfo[ifile].DutID, fileinfo[ifile].B, fileinfo[ifile].Bias );	
		TProfile* currentProfile = currentMeas->getProfile();

		Long64_t nentries = treeDUT->GetEntries();
		for (Long64_t ientry=0; ientry<nentries;ientry++) {
      			treeDUT->GetEntry(ientry);
			if ( dutID==currentMeas->getDutID() ){
				int calibID = 10*fileinfo[ifile].DutNum + currentMeas->getDutID();
				double calib = mapCalib[calibID];
				int newclusize =0;
				for (unsigned int i_strip = 0; i_strip<clustersize; i_strip++){
					if (clustercharge[i_strip] * calib > threshold)
						newclusize++;
				}
				double slope = locxslope * 180.0 / TMath::Pi();
				if (newclusize > 0)
					currentProfile->Fill(slope,newclusize);
			}	
   		}
	}

	fitProfiles();
}

void createGraphs(){
/*
	gStyle->SetOptStat(0);
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
			double tanLA[10];
			double LAerr[10];
			double tanLAerr[10];
			TString hist_tit;
			int i = 0;
			for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
				if (measurements[imeas]->getBias() == storedBias[ibias] && measurements[imeas]->getDutID() == dutid[iId]){
					magfield[i] = measurements[imeas]->getB();
					magfielderr[i] = 0.0;
					LA[i] = measurements[imeas]->getLA();
					tanLA[i] = TMath::Tan((TMath::Pi() / 180.0) * measurements[imeas]->getLA());
					LAerr[i] = measurements[imeas]->getLAerror();
					tanLAerr[i] = TMath::Tan((TMath::Pi() / 180.0) *  measurements[imeas]->getLAerror());
					hist_tit = TString("LAM ")+ measurements[imeas]->getIrrad()+TString("; Magnetic Field (T); tan Lorentz Angle");
					//hist_tit = TString("LAM ")+ measurements[imeas]->getIrrad()+TString("; Magnetic Field (T); Lorentz Angle (degrees)");
		
					i++;
				}
			}		

			//TGraphErrors* gr = new TGraphErrors(i,magfield,LA,magfielderr,LAerr);
			TGraphErrors* gr = new TGraphErrors(i,magfield,tanLA,magfielderr,tanLAerr);
			TString title = TString("graph_dut")+TString::Itoa(dutid[iId],10)+TString("_V")+TString::Itoa(storedBias[ibias],10);
			TString tit_can = TString("canvas_")+ title;
			TCanvas *can = new TCanvas(tit_can.Data(),tit_can.Data(), 800,600);
			gStyle->SetOptStat(0);
			can->cd();
			gr->SetMarkerStyle(6);
			gr->SetTitle(hist_tit.Data());
			gr->SetName(title.Data());
			gr->GetXaxis()->SetRangeUser(-0.1, 1.1);
			title = TString("fit_")+title;	
			TF1* fitfunc = new TF1(title.Data(),"pol1", -0.05, 1.05); 
			gr->Fit(fitfunc,"QR");
			fitfunc->SetLineStyle(2);

			gr->Draw("AP");
			
			// LA = p0 * (B-field) + p1
			double p0 = fitfunc->GetParameter(0);
			double p1 = fitfunc->GetParameter(1);
			stringstream ss;
			ss << string("LA = ")<<fixed<<setprecision(2)<< p1 << string(" * B-field + (")<< fixed<<setprecision(2)<<p0<<string(")");	
			
			TLatex *latex =new TLatex();
  			latex->SetNDC();
  			latex->SetTextFont(43);
  			latex->SetTextColor(1);
  			latex->SetTextSize(22);

			
			title = ss.str();
//			cout<<"title "<< title<< endl;
			latex->DrawLatex(0.5,0.8,title.Data());
			double la2T = p1 * 2.0 + p0;
			la2T = TMath::ATan(la2T);
			la2T = la2T * 180.0 / TMath::Pi();
			ss.str("");
			ss << string("LA = ")<<fixed<<setprecision(2)<< la2T << string(" at 2T ");
			title = ss.str();
//			cout<<"title "<< title<< endl;
			latex->DrawLatex(0.5,0.7,title.Data());

			title = OutputPath + TString("/dut_")+dutNum+TString("_")+TString::Itoa(dutid[iId],10)+TString("_V")+TString::Itoa(storedBias[ibias],10)+TString(".pdf");
			can->SaveAs(title.Data());

			fout->cd();
			gr->Write();
			can->Write();
		
		}
	}
*/
}

void fitGraphs(){
	gStyle->SetOptStat(0);
	fout->cd();
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		TGraphErrors* graph = measurements[imeas]->getGraph();
		TString hname = measurements[imeas]->getGraphFitName();
		TF1* f = new TF1(hname.Data(),fit_func,-20,20,4);
		
  		f->SetParNames("LA", "a", "b", "sigma");
  		f->SetParLimits(3, 0.1, 10.);
  		f->SetParameters(0,1,1.13,5);
		graph->Fit(hname.Data(),"RQ");
//		cout<< hname << " created"<< " LA= "<<f->GetParameter(0)<<" +/- "<< f->GetParError(0)<<endl;
		measurements[imeas]->setLA(f->GetParameter(0));
		measurements[imeas]->setLAerror(f->GetParError(0));
		graph->Write();
	}

}

void fitProfiles(){
	gStyle->SetOptStat(0);
	fout->cd();
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		TProfile* profile = measurements[imeas]->getProfile();
		TString hname = measurements[imeas]->getProfileFitName();

		TF1* f = new TF1(hname.Data(),fit_func,-20,20,4);
		
  		f->SetParNames("LA", "a", "b", "sigma");
  		f->SetParLimits(3, 0.1, 5.);
  		f->SetParLimits(0, -5.0, 5.0);
  		f->SetParameters(0,1,1.13,5);
		profile->Fit(hname.Data(),"QR");
//		cout<< hname << " created"<< " LA= "<<f->GetParameter(0)<<" +/- "<< f->GetParError(0)<<endl;
		measurements[imeas]->setLA(f->GetParameter(0));
		measurements[imeas]->setLAerror(f->GetParError(0));
		profile->Write();
	}

}

bool isMeasStored(LAmeas *ameas){
	for (unsigned int imeas=0; imeas<measurements.size(); imeas++){
		if ( measurements[imeas]->getDutNum()==ameas->getDutNum() && measurements[imeas]->getDutID()==ameas->getDutID() && measurements[imeas]->getB()==ameas->getB() && measurements[imeas]->getBias()==ameas->getBias() ){
			return true;
		}
	}
	return false;
}

void createMeasurements(vector<FileInfo> fileinfo){
	for(unsigned int ifile=0; ifile<fileinfo.size(); ifile++){	
		LAmeas* ameas = new LAmeas();
		ameas->setDutNum( fileinfo[ifile].DutNum );
		ameas->setDutID( fileinfo[ifile].DutID );
		ameas->setB( fileinfo[ifile].B );
		ameas->setBias( fileinfo[ifile].Bias );
		
		if (isMeasStored(ameas)==false) {
			TString tempname = ameas->getProfileName();
			TProfile *tempprofile = new TProfile(tempname.Data(),tempname.Data(),21,-21,21);
//			TProfile *tempprofile = new TProfile(tempname.Data(),tempname.Data(),Nbin_profiles, bin_profiles);
			tempprofile->SetStats(false);
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
			//info.print();
		}	
	}
	return fileinfo;
}
   	
void setBranches(TTree* treeDUT){
   // Set branch addresses.
 
   treeDUT->SetBranchAddress("runnum", &runnum);
   treeDUT->SetBranchAddress("evtnum", &evtnum);
   treeDUT->SetBranchAddress("Pseg", &Pseg);
   treeDUT->SetBranchAddress("Pgbl", &Pgbl);
   treeDUT->SetBranchAddress("tel_globalPos_0", tel_globalPos_0);
   treeDUT->SetBranchAddress("tel_globalPos_1", tel_globalPos_1);
   treeDUT->SetBranchAddress("tel_globalPos_2", tel_globalPos_2);
   treeDUT->SetBranchAddress("tel_globalPos_3", tel_globalPos_3);
   treeDUT->SetBranchAddress("tel_globalPos_4", tel_globalPos_4);
   treeDUT->SetBranchAddress("tel_globalPos_5", tel_globalPos_5);
   treeDUT->SetBranchAddress("dut_globalPos", dut_globalPos);
   treeDUT->SetBranchAddress("hitnum", &hitnum);
   treeDUT->SetBranchAddress("dutID", &dutID);
   treeDUT->SetBranchAddress("locxpos", &locxpos);
   treeDUT->SetBranchAddress("locypos", &locypos);
   treeDUT->SetBranchAddress("locxslope", &locxslope);
   treeDUT->SetBranchAddress("locyslope", &locyslope);
   treeDUT->SetBranchAddress("clustersize", &clustersize);
   treeDUT->SetBranchAddress("clusterxchan", clusterxchan);
   treeDUT->SetBranchAddress("clusterychan", clusterychan);
   treeDUT->SetBranchAddress("clustercharge", clustercharge);
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


void lam_plot_eachdata(TString iteration){
	gStyle->SetOptStat(0);
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
	
		//	fileinfo[ifile].print();

	   		TTree* treeDUT= getTree(fileinfo[ifile].RunNum, iteration);
			setBranches(treeDUT);
		
			TString cut = TString("dutID==")+TString::Itoa(fileinfo[ifile].DutID,10);
			treeDUT->Draw("(locxslope * 180.0 / TMath::Pi())>>hslope(100,-25,25)", cut.Data());
			TH1D* hslope = (TH1D*)gDirectory->Get("hslope");
			TF1 *fitfunc = fit_langaus(hslope);
		stringstream ss;	
		ss<<"Fit_DutNum"<< fileinfo[ifile].DutID <<"_RunNum"<<fileinfo[ifile].RunNum;
			string fitname = ss.str();
//			TF1* fitfunc = new TF1(fitname.c_str(),"gaus"); 
//			hslope->Fit(fitfunc,"Q");
			mean_locxslope[i] = fitfunc->GetParameter(1);
			mean_locxslope_err[i] = fitfunc->GetParError(1);
	
			treeDUT->Draw("clustersize>>hclu(20,0,20)", cut.Data());
			TH1D* hclu = (TH1D*)gDirectory->Get("hclu");
			mean_clusize[i] = hclu->GetMean();
			mean_clusize_err[i] = hclu->GetMeanError();
if(fileinfo[ifile].B==0 && fileinfo[ifile].Bias==500)
cout<<"B: "<<fileinfo[ifile].B<< " Bias: "<< fileinfo[ifile].Bias<< " Run: "<<fileinfo[ifile].RunNum<<"-"<<fileinfo[ifile].DutID<<" slope: "<<mean_locxslope[i]<<" clu: "<<mean_clusize[i]<<endl;	
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



