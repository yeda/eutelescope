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

TString FileBase = TString("/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/gblPython/results/cce-xtalk-telaligned");
TString pdfOutputFile= TString("/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/Thesis/plotting/pdfs/chapCCE-pdf");

TString pdfFolder = TString("pdfs/chapCCE-pdf/");
TString FileName = FileBase + TString("/cce_dutY.root");
TString graphname = TString("dutnumX_dutidY");
TString clugraphname = TString("clusize_dutnumX_dutidY");

using namespace std;

struct GraphEntry{
	TGraphErrors* graph;
	TString irrad;
};


void plotMultiGraph(TString type, vector<int> dutlist, TString pdfname);
void plotTB();
void plotSr();
void convertSrResults();
void plotCluSize();

int main(int argc, char *argv[]){
	gStyle->SetOptStat(0);
	TGaxis::SetMaxDigits(3);

	convertSrResults();
	plotSr();
	plotTB();
	plotCluSize();

	return 0;
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
void plotCluSize(){
	vector<int> dutlist = {0,1,2,3,5,6};
	TString str = TString("TB-CluSize.pdf");
	plotMultiGraph(TString("clusize"), dutlist, str);
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
			if (type == TString("clusize")) tempname = clugraphname;
			else tempname = graphname;
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

	double x1, x2, y1, y2;
	if (type == TString("clusize")) {
		x1=0.15; y1=0.95; x2=0.4; y2=0.7;
	}
	else{
 		x1=0.65; y1=0.45; x2=0.95; y2=0.2;
	}
 	TLegend leg = TLegend(x1, y1, x2, y2);
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

		if (type == TString("tb") || type == TString("clusize")){
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
	if (type == TString("clusize")) mgr->SetTitle(";(-1) Bias Voltage (V); Mean Cluster Size");
	else mgr->SetTitle(";(-1) Bias Voltage (V); Collected Charge (ke)");
	mgr->Draw("ALP");
	leg.Draw();
	PdfName = pdfFolder + pdfname;
	cc->SaveAs(PdfName.Data());
}


