#include <iostream>
#include <vector>
#include "TMath.h" 
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TString.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TProfile.h"
#include "lam_histos.h"


using namespace std;

const int N = 3;
Color_t color[9] = {kBlack, kRed, kBlue, kGreen+2, kMagenta+1, kOrange-3, kPink-9, kViolet+1, kCyan+2};

void incidenceAngle();
void lamplot(int dutnum, int iteration);

void plots(){
	gStyle->SetOptStat(0);
	//incidenceAngle();
	lamplot(4,3);
}

void lamplot(int dutnum, int iteration){
	TString finname = TString("results/notcomb-xtalk/lam_dutXXX_itYYY.root");
	finname.ReplaceAll(TString("XXX"),TString::Itoa(dutnum,10));
	finname.ReplaceAll(TString("YYY"),TString::Itoa(iteration,10));
	TFile* fin = TFile::Open(finname.Data());
	vector<TString> profnames = getObjectNames(fin,TString("TProfile"));
	
        TCanvas* cc = new TCanvas("cc_lam","cc",800,600);
	TProfile* prof;
        TLegend* leg = new TLegend(0.50,0.63,0.75,0.82);
        leg->SetHeader("Magnetic Field (T)");
        leg->SetFillColor(0);

	int j=0;
	for(unsigned int i=0; i<profnames.size(); i++){
		if (decodeDutID(profnames[i])==7 && decodeBias(profnames[i])==500){
			prof = (TProfile*)fin->Get(profnames[i].Data());
			prof->SetLineColor(color[j]);
			prof->SetTitle("non irrad. LAM; Incidence Angle (degrees); Cluster Size");
			TString funcname = TString("fit_")+profnames[i];
			TF1* fitfunc = prof->GetFunction(funcname.Data());
			fitfunc->SetLineColor(color[j]);

			std::ostringstream ss;
			ss<< string("B= ")<<decodeB(profnames[i]) << string(", LA= ")<<setprecision(2)<<fitfunc->GetParameter(0)<< string(" +/- ")<<setprecision(2)<<fitfunc->GetParError(0);
			TString title = TString(ss.str());
	
	                leg->AddEntry(prof,title.Data(),"l");

			prof->Draw("same");
			j++;
		}

	}
	leg->Draw();
	cc->SaveAs("lam.pdf");
}


vector<TString> getObjectNames(TFile* fin, TString classname){
	TList* list = fin->GetListOfKeys();
	vector<TString> names;
	names.clear();

	int i=0;
	TObject* key;
	while(key = list->At(i)){
		TObject* obj = fin->Get(key->GetName());
		if(TString(obj->ClassName()) == classname){
			names.push_back(TString(obj->GetName()));
		}
		i++;
	}
	return names;
}




void incidenceAngle(){
	TFile* f[N];
	f[0] = TFile::Open("output/run004056/dutTree_4.root"); // rot=-1.2,  B=1
	f[1] = TFile::Open("output/run004031/dutTree_4.root"); // rot=17.1,  B=1
	f[2] = TFile::Open("output/run004076/dutTree_4.root"); // rot=-18.8,  B=1
	
	TString title[N] = {"-1.2", "17.1", "-18.8"};
	TTree* t[N];
	TH1D* h[N];
	
	TCanvas* cc = new TCanvas("cc_incAngle","cc",800,600);
	for (int i=0; i<N; i++ ){
		t[i] = (TTree*)f[i]->Get("treeDUT");
		TString histname = TString("h")+TString::Itoa(i,10);
		TString drawstr = TString("(locxslope* 180.0 / TMath::Pi() )>>")+ histname +TString("(1000,-25,25)");
		t[i]->Draw(drawstr.Data(),"dutID==7");
		h[i] = (TH1D*)gDirectory->Get(histname.Data());
		h[i]->SetLineWidth(2);
		h[i]->SetLineColor(color[i]);
		h[i]->SetTitle("Incidence angle on DUT; Incidence angle (degrees);");
	}
	cc->cd();
	TLegend* leg = new TLegend(0.50,0.63,0.75,0.82);
	leg->SetHeader("Tilt Angle (degrees)");
	leg->SetFillColor(0);
	
	for (int i=0; i<N; i++ ){
		leg->AddEntry(h[i],title[i].Data(),"l");
		
		if ( i==0 ) h[i]->DrawNormalized();
		else h[i]->DrawNormalized("SAME");
	}
	//h[1]->Draw();
	//h[0]->Draw("SAME");
	//h[2]->Draw("SAME");
	leg->Draw();
	
	cc->SaveAs("pdfs/lam_incidenceAngle.pdf");
}



