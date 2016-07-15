#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include "TString.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "TVectorD.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include <cstdlib>

#include "formatting.h"

TString FileBase = TString("/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/gblPython/results/notcomb-xtalk-telaligned");

TString pdfFolder = TString("pdfs/chapLA-pdf/");
TString FileName = FileBase + TString("/lam_dutY_it3.root");
TString profilename = TString("profile_dutX_BY_VZ");
TString LACanname = TString("LA_dutX_VZ");

using namespace std;

struct GraphEntry{
	TGraphErrors* graph;
	TString irrad;
};

struct ProfileEntry{
	TProfile* graph;
	TString irrad;
};

void plotMultiGraph(TString type, vector<int> dutlist, TString pdfname);


int main(int argc, char *argv[]){
	gStyle->SetOptStat(0);
	TGaxis::SetMaxDigits(3);


	return 0;
}

vector<TProfile*> getProfiles(TFile* f){
	TProfile* pr;
	vector<TProfile*> vec_pr;
	vec_pr.clean(); 
	
	TIter next(fin->GetListOfKeys());
   	TKey *key;
   	while ((key = (TKey*)next())) {
      		TString classname = TString(key->GetClassName());
		if (classname == TString("TProfile")){
			TString pr_name = TString(key->GetTitle());
			if (pr_name.First("profile") != -1) {
				pr = (TProfile*)key->ReadObj();
				vec_pr.push_back(pr);
   			}
		}
	}

	return vec_pr;
}


void plot_LAcombined(int dutnum, int dutid, int bias){

	TProfile* pr;
	TF1* fit_pr;
	int icolor =0;
	TString finname = FileName;
	finname.ReplaceAll(TString("Y"), TString::Itoa(dutnum,10));
	TFile* fin = TFile::Open(finname.Data());

   	
	vector<TProfile*> vec_pr = getProfiles(fin);

	// Now create canvas for each dut and bias combination
	for (unsigned int i=0; i<measuredDut.size(); i++){
		for(unsigned j=0; j<measuredBias.size(); j++){
			icolor=0;
			string canvasname = string("LA_dut")+ to_string(measuredDut[i])+string("_V")+to_string(measuredBias[j]);
			TCanvas *cc = new TCanvas(canvasname.c_str(),canvasname.c_str(),800, 600);
			cc->cd();

	gStyle->SetOptStat(0);
		        TLegend* leg = new TLegend(0.40,0.63,0.65,0.82);
		        leg->SetHeader("Magnetic Field (T)");
		        leg->SetFillColor(0);

			for (unsigned int imeas=0; imeas< measurements.size(); imeas++){
				if (measurements[imeas]->getDutID() == measuredDut[i] && measurements[imeas]->getBias() == measuredBias[j]){
					pr = (TProfile*) measurements[imeas]->getProfile();
					fit_pr = (TF1*) measurements[imeas]->getFitProfile();
					if (s==TString("ITK")){
						if (dutNum == TString("1") && measuredDut[i]==6) pr->SetAxisRange(1.2,1.6,"Y");
						if (dutNum == TString("1") && measuredDut[i]==7) pr->SetAxisRange(1.05,1.25,"Y");
						if (dutNum == TString("2") && measuredDut[i]==6) pr->SetAxisRange(1.0,1.15,"Y");
						if (dutNum == TString("2") && measuredDut[i]==7) pr->SetAxisRange(1.0,1.1,"Y");
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
					pr->SetLineColor(color[icolor]);
					TString title;
					title = TString("LAM ")+ measurements[imeas]->getIrrad()+TString("; Incidence Angle (degrees); Cluster Size");
					pr->SetTitle(title.Data());
					fit_pr->SetLineColor(color[icolor]);
					std::ostringstream ss;
					ss<< string("B= ")<< fixed<< setprecision(2)<<measurements[imeas]->getB() << string(", LA= ")<<fixed<<setprecision(2)<<fit_pr->GetParameter(0)<< string(" +/- ")<<fixed<<setprecision(2)<<fit_pr->GetParError(0);
					title = TString(ss.str());
	
			                leg->AddEntry(pr,title.Data(),"l");

		        		gStyle->SetOptStat(0);
					pr->Draw("same");
					icolor++;
				}
			}

			leg->Draw();
			TString title = OutputPath + TString("/LA_dut_")+dutNum+TString("_")+TString::Itoa(measuredDut[i],10)+TString("_V")+TString::Itoa(measuredBias[j],10)+TString(".pdf");
		
			cc->SaveAs(title.Data());
			fout->cd();
			cc->Write();
		}

	}
	
}



void convertSrResults(){

TCanvas *cc =new TCanvas("cc","",800,600);
	vector<int> srlist = {0,2,3,6,7};
	TString infilename = FileBase + TString("/cce_sr_dutY.root");
	TString filename, outfilename;
	TFile *file;
	for (unsigned int i=0; i<srlist.size(); i++ ){
		filename = infilename;
		filename.ReplaceAll(TString("Y"), TString::Itoa(srlist[i],10));
		file = TFile::Open(filename.Data());
		
		outfilename = FileName;
		outfilename.ReplaceAll(TString("Y"), TString::Itoa(10+srlist[i],10));
		TFile* fout = new TFile(outfilename.Data(),"RECREATE");

		TCanvas * c = (TCanvas*)file->Get("CollCharge25Deg");
		for (unsigned int j=0; j<2; j++){
			TPad *pad = (TPad*)c->GetPad(j+1);
			TGraphErrors *gr = (TGraphErrors*)pad->FindObject("Graph");	
			TString tempname = graphname;
			tempname.ReplaceAll(TString("X"), TString::Itoa(10+srlist[i],10));
//			if (srlist[i]==6 && j==0) tempname.ReplaceAll(TString("Y"), TString::Itoa(7,10));
//			else if (srlist[i]==6 && j==1) tempname.ReplaceAll(TString("Y"), TString::Itoa(6,10));
//			else tempname.ReplaceAll(TString("Y"), TString::Itoa(j+6,10));
			tempname.ReplaceAll(TString("Y"), TString::Itoa(j+6,10));
			TGraphErrors *grc = new TGraphErrors(gr->GetN(),gr->GetX(),gr->GetY(),gr->GetEX(),gr->GetEY());
			for (unsigned int k=0; k<grc->GetN(); k++){
				grc->GetX()[k] *= 1000.0;
				grc->GetEX()[k] *= 1000.0;
				grc->GetY()[k] *= 1000.0;
				grc->GetEY()[k] *= 1000.0;
			}
			fout->cd();
			grc->Write(tempname.Data());
		}
		fout->Close();
	}

}



void plotSr(){
	vector<int> dutlist = {0,10,2,12,3,13,6,16,17};
	TString str = TString("SrComp_org.pdf");
	plotMultiGraph(TString("sr"), dutlist, str);
}


void plotTB(){
	vector<int> dutlist = {0,1,2,3,5,6};
	TString str = TString("TB-Data.pdf");
	plotMultiGraph(TString("tb"), dutlist, str);
}

void plotMultiGraph(TString type, vector<int> dutlist, TString pdfname){
TCanvas *cc =new TCanvas("cc","",800,600);
	TMultiGraph *mgr = new TMultiGraph();
	TGraphErrors *gr;
	TFile *file;
	TString PdfName;
	
	TString dutfile;
	TString tempname;

	map<int, GraphEntry> sorting;

	for (unsigned int idut=0; idut<dutlist.size(); idut++){
		dutfile = FileName;
		dutfile.ReplaceAll(TString("Y"), TString::Itoa(dutlist[idut],10));
		file = TFile::Open(dutfile.Data());

		for (int iid=6; iid<8; iid++){
			tempname = graphname;
			tempname.ReplaceAll(TString("X"), TString::Itoa(dutlist[idut],10));
			tempname.ReplaceAll(TString("Y"), TString::Itoa(iid,10));

			gr = (TGraphErrors*)file->Get(tempname.Data());
			formatGraph(type, gr, dutlist[idut], iid);

			GraphEntry anentry;
			anentry.graph = gr;
			anentry.irrad = getIrrad(dutlist[idut], iid);
			sorting.insert(make_pair(sortGraph(type, dutlist[idut], iid), anentry) );
			mgr->Add(gr);
		}

	}

	TLegend leg = TLegend(0.65, 0.45, 0.95, 0.2);
	leg.SetFillColor(0);
	leg.SetHeader("Fluence (1MeV n_{eq}/cm^{2})");
//	leg.SetLineColor(0);
	leg.SetNColumns(2);

	if (type == TString("sr")){
		leg.AddEntry("","beam","");
		leg.AddEntry("","Sr^{90}","");
	}	
	for (map< int, GraphEntry >::iterator i_entry=sorting.begin(); i_entry!= sorting.end(); i_entry++){
		GraphEntry anentry = i_entry->second;
		TString title = anentry.irrad;
		leg.AddEntry(anentry.graph, title.Data(), "lp");

		if (type == TString("tb")){
			if (i_entry->first == 0 || i_entry->first == 2 || i_entry->first == 12 || i_entry->first == 14){
				leg.AddEntry("","","");
			}
		}

		if (type == TString("sr")){
			if (i_entry->first == 15 || i_entry->first == 17){
				leg.AddEntry("","","");
			}
		}

	}


	formatCanvas1D(cc);
	mgr->SetTitle(";(-1) Bias Voltage (V); Collected Charge (ke)");
	mgr->Draw("ALP");
	leg.Draw();
	PdfName = pdfFolder + pdfname;
	cc->SaveAs(PdfName.Data());
}


