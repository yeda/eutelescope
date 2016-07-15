#include <iostream>
#include <fstream>
#include <sstream>
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
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include <cstdlib>

#include "langaufit.h"
using namespace std;

//Declaration of leaves types
   Int_t           runnum;
   Int_t           evtnum;
   Float_t         Pseg;
   Float_t         Pgbl;
   Float_t         tel_globalPos_0[3];
   Float_t         tel_globalPos_1[3];
   Float_t         tel_globalPos_2[3];
   Float_t         tel_globalPos_3[3];
   Float_t         tel_globalPos_4[3];
   Float_t         tel_globalPos_5[3];
   Float_t         dut_globalPos[3];
   Int_t           hitnum;
   Int_t           dutID;
   Float_t         locxpos;
   Float_t         locypos;
   Float_t         locxslope;
   Float_t         locyslope;
   Int_t           clustersize;
   Int_t           clusterxchan[20];   //[clustersize]
   Int_t           clusterychan[20];   //[clustersize]
   Float_t         clustercharge[20];   //[clustersize]



// global 
#define pi 3.14159265

const double threshold = 3000.0;

struct FileInfo{ 
	int DutNum;  
	int RunNum;
	int DutID;   
	int Bias;
	string Irrad;
	double B;
	void print(){
		cout<<"DutNum "<< DutNum <<" RunNum "<<RunNum<<" DutID "<<DutID<< " Bias "<<Bias<<" Irrad "<<Irrad<< " B "<< B <<endl;
	}
};
map<int, double> mapCalib = 	{ 
					{6,163.685}, {7, 160.472}, 
					{16, 166.237}, {17, 163.602}, 
					{26, 154.325}, {27, 156.122},
					{36, 166.237}, {37, 163.602}, 
					{46, 163.685}, {47, 160.472}, 
					{56, 169.929}, {57, 165.31},
					{66, 169.929}, {67, 165.31}
				};

struct LAmeas{
	int DutNum;
	int DutID;
	double B;
	int Bias;
	double LA;
	double LAerror;
	TProfile* profile;
	TGraphErrors* graph;

	LAmeas(){}
	~LAmeas(){}

	std::string results_to_txt(){
        	std::ostringstream ss;
		ss << DutNum << ";" << DutID << ";" << getIrrad();

		if (DutNum==4 && DutID == 6) ss<<"1";
		else if (DutNum==4 && DutID == 7) ss<<"2";
		
		ss << ";" << B << ";" << Bias << ";" << LA << ";" << LAerror << ";";
		return ss.str();
	}

	void setDutNum(int avalue){ DutNum = avalue; }
	int getDutNum() { return DutNum; } 

	void setDutID(int avalue){ DutID = avalue; }
	int getDutID() { return DutID; } 
	
	void setB(double avalue){ B = avalue; }
	double getB() { return B; } 

	void setBias(int avalue){ Bias = avalue; }
	int getBias() { return Bias; } 
	
	void setLA(double avalue){ LA = avalue; }
	double getLA() { return LA; } 
	
	void setLAerror(double avalue){ LAerror = avalue; }
	double getLAerror() { return LAerror; } 

	TF1* getFitProfile() { 
		TString fitname = TString("fit_")+getProfileName();
		return profile->GetFunction(fitname.Data());
	 }

	void setProfile(TProfile* aprofile){ profile = aprofile; }
	TProfile* getProfile() { return profile; }

	void setGraph(TGraphErrors* agraph){ graph = agraph; }
	TGraphErrors* getGraph() { return graph; }

	TString getGraphName(){
		TString temphistname = TString("graph_dutXXX_BYYY_VZZZ");
		temphistname.ReplaceAll(TString("XXX"),TString::Itoa(DutID,10));
                TString Bname = TString::Itoa(int(B*100),10);
                temphistname.ReplaceAll(TString("YYY"),Bname);
                temphistname.ReplaceAll(TString("ZZZ"),TString::Itoa(Bias,10));
		return temphistname;
	}

	TString getProfileName(){
		TString temphistname = TString("profile_dutXXX_BYYY_VZZZ");
		temphistname.ReplaceAll(TString("XXX"),TString::Itoa(DutID,10));
                TString Bname = TString::Itoa(int(B*100),10);
                temphistname.ReplaceAll(TString("YYY"),Bname);
                temphistname.ReplaceAll(TString("ZZZ"),TString::Itoa(Bias,10));
		return temphistname;
	}

	TString getGraphFitName(){
		TString fitname = TString("fit_") + getGraphName();
		return fitname;
	}

	TString getProfileFitName(){
		TString fitname = TString("fit_") + getProfileName();
		return fitname;
	}
	
	TString getIrrad(){
		TString irrad;
		if ( DutNum==1 && DutID==6) irrad = TString("5.0 #times 10^{14}");
		if ( DutNum==1 && DutID==7) irrad = TString("1.0 #times 10^{15}");

		if ( DutNum==2 && DutID==6) irrad = TString("2.0 #times 10^{15}");
		if ( DutNum==2 && DutID==7) irrad = TString("5.0 #times 10^{15}");

		if ( DutNum==3 && DutID==6) irrad = TString("5.0 #times 10^{14} an.");
		if ( DutNum==3 && DutID==7) irrad = TString("1.0 #times 10^{15} an.");

		if ( DutNum==4) irrad = TString("non-irradiated");

		if ( DutNum==5 && DutID==7) irrad = TString("2.0 #times 10^{14}");
		if ( DutNum==5 && DutID==6) irrad = TString("1.2 #times 10^{14}");

		if ( DutNum==6 && DutID==7) irrad = TString("2.0 #times 10^{14} an.");
		if ( DutNum==6 && DutID==6) irrad = TString("1.2 #times 10^{14} an.");
		
		return irrad;
	}	
};

void formatProfile(TProfile *gr){
    gr->GetXaxis()->SetLabelSize(0.045);
    gr->GetXaxis()->SetTitleSize(0.05);
    gr->GetXaxis()->SetTitleOffset(1.2);
    
    gr->GetYaxis()->SetLabelSize(0.045);
    gr->GetYaxis()->SetTitleSize(0.05);
    gr->GetYaxis()->SetTitleOffset(1.2);
    
//    gr->SetTitle("");

}

void formatCanvas1D(TCanvas *cc){
    cc->SetRightMargin(0.125);
    cc->SetLeftMargin(0.125);
    cc->SetBottomMargin(0.13);
    cc->SetTopMargin(0.07);
}

int decodeDutID(TString s){
	int us = s.First('_');
	s.Remove(us,s.Sizeof());
	s.Remove(0, us-1);
	return s.Atoi();
}

double decodeB(TString s){
	s.Remove(0,s.First('_')+2);
	s.Remove(s.Last('_'),s.Sizeof());	
	return s.Atof()/100;
}

int decodeBias(TString s){
	s.Remove(0,s.Last('_')+2);
	return s.Atoi();
}
template<class T>
std::string to_string(const T& t) {
        std::ostringstream ss;
        ss << t;
        return ss.str();
}
void setBranches(TTree* treeDUT);
vector<FileInfo> readCsvFile();
Double_t fit_func(Double_t *x, Double_t *par);
TTree* getTree(int RunNum, TString iteration);

void createMeasurements(vector<FileInfo> fileinfo);
LAmeas* getMeasurement(int dutID, double b, int bias);
void lam_plot_eachtrack(TString iteration);
void lam_plot_eachtrack_ITK(TString iteration);
void lam_plot_eachdata(TString iteration);
void fitProfiles();
void fitGraphs();
void createGraphs();
void plot_LAcombined(TString s);
int sortLegendB(TString s);
void writetotxt(TString f_txt);

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



