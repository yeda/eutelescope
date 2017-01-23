/*
 * Created by Eda Yildirim
 *  (2016)
 *
 *  email:eda.yildirim@cern.ch
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <map>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <iomanip>


#include "TString.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TMath.h"
#include "TAxis.h"
#include "TColor.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TGaxis.h"

#include "runall.h"


using namespace std;

bool is_itk= false;



TString OutputPath;
TString LAresultfile;
TString output_rootfile;
TString Errresultfile = TString("./lam_error.txt");
TString cce_txt_file = TString("./cce_tb_txt.txt");

map<TString,TObject*> rootobjects;
TString LAvsBname = TString("LAvsB_dutnumXX_dutidYY_VZZ");
TString LAvsBname_mgr = TString("LAvsB_VZZ");

TString LAvsIrradname = TString("LAvsIrrad_BXX_VZZ");
TString LAvsCCEname = TString("LAvsCCE_BXX_VZZ");
TString LAvsBiasname = TString("LAvsBias_dutnumXX_dutidYY_BZZ");

// map<dutnum, map<voltage, map<Bfield, map<dutid, LAmeas > > > >
map<int, map<int, map<double, map<int, LAmeas > > > > results;

// map<dutnum, map<dutid, cce> > 
map<int, map<int, double > > cce_results; // biasV =500


// map<dutnum, map<dutid, map<irrad_str, irrad_double > > > irrad_map
map<int, map<int, TString > > irrad_map_str;
map<int, map<int, double > > irrad_map_val;



Color_t color[18] = {kBlack, kGreen+2, kBlue, kMagenta+1, kRed, kOrange-3, kPink-9, kViolet+1, kCyan+2, kBlack, kGreen+2, kBlue, kMagenta+1, kRed, kOrange-3, kPink-9, kViolet+1, kCyan+2};

TCanvas* cc = new TCanvas("cc","cc",800,600);


int main(int argc, char *argv[]){
	if(is_itk) OutputPath = TString("./results/ITK/");
	else OutputPath = TString("./results/notcomb-xtalk-telaligned/");
	
	LAresultfile = OutputPath + TString("LA_out.txt");
	output_rootfile = OutputPath + TString("lam_graphs.root");	

//    reRunData(); 
    gStyle->SetOptStat(0);
//    TGaxis::SetMaxDigits(3);

    readLAResults();
    readSysErr();
    readCCEResults();
 
    printvalues();
    createGraph_LAvsB();
    //createMultiGraph_LAvsB();

    createGraph_LAvsIrrad(0);
    createGraph_LAvsIrrad(0.25);
    createGraph_LAvsIrrad(0.5);
    createGraph_LAvsIrrad(0.75);
    createGraph_LAvsIrrad(1.0);

    createGraph_LAvsCCE(1.0);

    createGraph_LAvsBias();

    createOutputFile();
    
    return 0;
}
void reRunData(){
 	TString runthis = TString("rm ") + LAresultfile;
	system(runthis.Data());
	system("./lam_histos 1 3; ./lam_histos 2 3; ./lam_histos 3 3; ./lam_histos 4 3; ./lam_histos 5 3; ./lam_histos 6 3;");
    
    runthis = TString("rm ") + Errresultfile;
    system(runthis.Data());
    system("./calculate_err");
}

void createGraph_LAvsBias(){
    
    int dutid, dutnum;
    TString tmp_str;
    TString grname;
    double Bfield;
    
    int icount=0;
    double scale = 1.0e14;
    double Bias[2][20];
    double Biaserr[2][20];
    double LA[2][20];
    double LAerr[2][20];
    
    map<int, map<int, map<double, map<int, LAmeas > > > >::iterator it_dutnum;
    for (it_dutnum = results.begin(); it_dutnum!=results.end(); it_dutnum++) {
        dutnum = it_dutnum->first;
        if (dutnum != 5) continue;
        
        map<int, map<double, map<int, LAmeas > > >::iterator it_voltage;
        for (it_voltage = it_dutnum->second.begin(); it_voltage!= it_dutnum->second.end(); it_voltage++) {
            
            map<double, map<int, LAmeas > >::iterator it_bfield;
            for (it_bfield = it_voltage->second.begin(); it_bfield!=it_voltage->second.end(); it_bfield++) {
                if (it_bfield->first != 1.0) continue;
                
                map<int, LAmeas >::iterator it_dutid;
                
                for (it_dutid = it_bfield->second.begin(); it_dutid!=it_bfield->second.end(); it_dutid++) {
                    dutid = it_dutid->first -6;
                    
                    LAmeas ameas = it_dutid->second;
                    
                    Bias[dutid][icount] = it_voltage->first;
                    Biaserr[dutid][icount] = 0;
                    LA[dutid][icount] = fabs(ameas.LA);
                    LAerr[dutid][icount] = ameas.LAerr_stat + ameas.LAerr_sys;
                }
                icount++;

            }
        }
    }
    
    for (int id =0; id<2; id++) {
        TGraphErrors* gr = new TGraphErrors(icount, Bias[id], LA[id], Biaserr[id], LAerr[id] );
        //TString("LAvsBias_dutnumXX_dutidYY_BZZ");
        dutnum = 5;
        dutid = id+6;
        Bfield = 1.0;
        grname = LAvsBiasname;
        grname.ReplaceAll("XX", TString::Itoa(dutnum,10));
        grname.ReplaceAll("YY", TString::Itoa(dutid,10));
        grname.ReplaceAll("ZZ", TString::Itoa(int(Bfield*100),10));
        gr->SetName(grname.Data());
        TString grtitle = grname +TString(";(-1) Bias voltage (V); #left|#theta_{L}#right| (#circ)");
        gr->SetTitle(grtitle.Data());
        gr->GetXaxis()->SetLimits(250,1050);
        gr->SetMarkerStyle(8);
        
        rootobjects.insert(pair<TString,TObject*>(grname,gr));
        
        gStyle->SetOptStat(0);
        cc->cd();
        //cc->SetLogx(1);
        formatCanvas1D(cc);
        formatGraph(gr);
        gr->Draw("ap");
        
        TLatex *latex =new TLatex();
        latex->SetNDC();
        latex->SetTextFont(43);
        latex->SetTextColor(1);
        latex->SetTextSize(26);
        latex->SetTextColor(kOrange+7);

        tmp_str = getIrrad(dutnum,dutid);
        tmp_str = TString("#bf{")+tmp_str+TString("}");
        latex->DrawLatex(0.6,0.7,tmp_str.Data());

        tmp_str = TString("B-field = 1T");
        latex->DrawLatex(0.6,0.60,tmp_str.Data());

        tmp_str = OutputPath +grname +TString(".pdf");
        cc->SaveAs(tmp_str.Data());

    }
    
}



void createGraph_LAvsCCE(double MagField){
    
    int dutid, dutnum;
    TString tmp_str;
    TString grname;
    
    int i_color = 0;
    int icount=0;
    int an_icount=0;

    double CCE[20]={0};
    double CCEerr[20]={0};
    double LA[20]={0};
    double LAerr[20]={0};
    
    double an_CCE[20]={0};
    double an_CCEerr[20]={0};
    double an_LA[20]={0};
    double an_LAerr[20]={0};

	double val_cce;
	double val_cceerr;
	double val_la;
	double val_laerr;

    map<int, map<int, map<double, map<int, LAmeas > > > >::iterator it_dutnum;
    for (it_dutnum = results.begin(); it_dutnum!=results.end(); it_dutnum++) {
        dutnum = it_dutnum->first;
        
        map<int, map<double, map<int, LAmeas > > >::iterator it_voltage;
        for (it_voltage = it_dutnum->second.begin(); it_voltage!= it_dutnum->second.end(); it_voltage++) {
            if (it_voltage->first != 500) continue;
            
            map<double, map<int, LAmeas > >::iterator it_bfield;
            for (it_bfield = it_voltage->second.begin(); it_bfield!=it_voltage->second.end(); it_bfield++) {
                if (it_bfield->first != MagField) continue;
                
                map<int, LAmeas >::iterator it_dutid;
                
                for (it_dutid = it_bfield->second.begin(); it_dutid!=it_bfield->second.end(); it_dutid++) {
                    dutid = it_dutid->first;
                    	if (dutnum == 4 && dutid == 7) continue;
                    
			LAmeas ameas = it_dutid->second;
			val_cce = cce_results[dutnum][dutid] / 1000.0;
                        val_cceerr = 0;
			val_la = fabs(ameas.LA);
                        val_laerr = ameas.LAerr_stat + ameas.LAerr_sys;
                        
                    if (dutnum == 3 || dutnum == 6){
                        an_CCE[an_icount] = val_cce;
                        an_CCEerr[an_icount] = val_cceerr;
                        an_LA[an_icount] = val_la;
                        an_LAerr[an_icount] = val_laerr;
                        an_icount++;
                        
                    }
                    else {
                        CCE[icount] = val_cce;
                        CCEerr[icount] = val_cceerr;
                        LA[icount] = val_la;
                        LAerr[icount] = val_laerr;
                        icount++;
                    }
                }
                
            }
        }
    }
    
    TMultiGraph* gr = new TMultiGraph();

    grname = LAvsCCEname;
    int Bfield = 100;
    grname.ReplaceAll("XX", TString::Itoa(MagField*100,10));
    grname.ReplaceAll("ZZ", TString::Itoa(500,10));
    gr->SetName(grname.Data());
    TString grtitle;

    grtitle = TString(";Collected Charge (ke); #left|#theta_{L}#right| (#circ)");

    gr->SetTitle(grtitle.Data());
 
    
    TGraphErrors* gr1 = new TGraphErrors(icount, CCE, LA, CCEerr, LAerr );
    TGraphErrors* gr2 = new TGraphErrors(an_icount, an_CCE, an_LA, an_CCEerr, an_LAerr );
    formatGraph(gr1);
    gr1->SetMarkerStyle(8);
    gr1->SetMarkerColor(kRed);
    gr1->SetFillColor(kRed);
    gr1->SetFillStyle(3004);
    gr1->GetXaxis()->SetLimits(-1,11);

    formatGraph(gr2);
    gr2->SetMarkerStyle(8);
    gr2->SetMarkerColor(kBlue);
    gr2->SetFillColor(kBlue);
    gr2->SetFillStyle(3005);
    gr2->GetXaxis()->SetLimits(-1,11);

    gr->Add(gr1);
    gr->Add(gr2);
  
    
    rootobjects.insert(pair<TString,TObject*>(grname,gr));
    
    gStyle->SetOptStat(0);
    cc->cd();
    //cc->SetLogx(1);

    formatCanvas1D(cc);
    gr->Draw("a2");
    gr->Draw("p");
    
    
    TLegend* leg;
    if (is_itk)
        leg = new TLegend(0.4,0.4,0.65,0.2);
    else
        leg = new TLegend(0.6,0.4,0.85,0.2);

    
    leg->SetFillColor(0);
    leg->SetLineColor(1);
    
    leg->AddEntry(gr1,"non-annealed","lpf");
    leg->AddEntry(gr2,"annealed","lpf");

 
    leg->Draw();
    tmp_str = OutputPath +grname + TString(".pdf");
    cc->SaveAs(tmp_str.Data());
    
}




void createGraph_LAvsIrrad(double MagField){
    
    int dutid, dutnum;
    TString tmp_str;
    TString grname;
    
    int i_color = 0;
    int icount=0;
    int an_icount=0;

    double scale = 1.0e14;
    double Irrad[20]={0};
    double Irraderr[20]={0};
    double LA[20]={0};
    double LAerr[20]={0};
    
    double an_Irrad[20]={0};
    double an_Irraderr[20]={0};
    double an_LA[20]={0};
    double an_LAerr[20]={0};

	double val_irrad;
	double val_irraderr;
	double val_la;
	double val_laerr;

    map<int, map<int, map<double, map<int, LAmeas > > > >::iterator it_dutnum;
    for (it_dutnum = results.begin(); it_dutnum!=results.end(); it_dutnum++) {
        dutnum = it_dutnum->first;
        
        map<int, map<double, map<int, LAmeas > > >::iterator it_voltage;
        for (it_voltage = it_dutnum->second.begin(); it_voltage!= it_dutnum->second.end(); it_voltage++) {
            if (it_voltage->first != 500) continue;
            
            map<double, map<int, LAmeas > >::iterator it_bfield;
            for (it_bfield = it_voltage->second.begin(); it_bfield!=it_voltage->second.end(); it_bfield++) {
                if (it_bfield->first != MagField) continue;
                
                map<int, LAmeas >::iterator it_dutid;
                
                for (it_dutid = it_bfield->second.begin(); it_dutid!=it_bfield->second.end(); it_dutid++) {
                    dutid = it_dutid->first;
                    if (dutnum == 4 && dutid == 7) continue;
                    	LAmeas ameas = it_dutid->second;
			val_irrad = irrad_map_val[dutnum][dutid] / scale;
                        val_irraderr = 0.1 * irrad_map_val[dutnum][dutid] / scale;

                        if (MagField == 0) 
				val_la = ameas.LA;
                        else 
				val_la = fabs(ameas.LA);

                        val_laerr = ameas.LAerr_stat + ameas.LAerr_sys;
                        
                    if (dutnum == 3 || dutnum == 6){
                        an_Irrad[an_icount] = val_irrad;
                        an_Irraderr[an_icount] = val_irraderr;
                        an_LA[an_icount] = val_la;
                        an_LAerr[an_icount] = val_laerr;
                        an_icount++;
                        
                    }
                    else {
                        Irrad[icount] = val_irrad;
                        Irraderr[icount] = val_irraderr;
                        LA[icount] = val_la;
                        LAerr[icount] = val_laerr;
                        icount++;
                    }
                }
                
            }
        }
    }
    
    TMultiGraph* gr = new TMultiGraph();

    grname = LAvsIrradname;
    int Bfield = 100;
    grname.ReplaceAll("XX", TString::Itoa(MagField*100,10));
    grname.ReplaceAll("ZZ", TString::Itoa(500,10));
    gr->SetName(grname.Data());
    TString grtitle;

    if (MagField == 0) 
	grtitle = TString(";Fluence/10^{14} (1 MeV n_{eq}/cm^{2}); #theta_{L} (#circ)");
    else 
	grtitle = TString(";Fluence/10^{14} (1 MeV n_{eq}/cm^{2}); #left|#theta_{L}#right| (#circ)");

    gr->SetTitle(grtitle.Data());
 
    
    TGraphErrors* gr1 = new TGraphErrors(icount, Irrad, LA, Irraderr, LAerr );
    TGraphErrors* gr2 = new TGraphErrors(an_icount, an_Irrad, an_LA, an_Irraderr, an_LAerr );
    formatGraph(gr1);
    gr1->SetMarkerStyle(8);
    gr1->SetMarkerColor(kRed);
    gr1->SetFillColor(kRed);
    gr1->SetFillStyle(3004);
    gr1->GetXaxis()->SetLimits(-1,11);

    formatGraph(gr2);
    gr2->SetMarkerStyle(8);
    gr2->SetMarkerColor(kBlue);
    gr2->SetFillColor(kBlue);
    gr2->SetFillStyle(3005);
    gr2->GetXaxis()->SetLimits(-1,11);

    gr->Add(gr1);
    gr->Add(gr2);
  
    
    rootobjects.insert(pair<TString,TObject*>(grname,gr));
    
    gStyle->SetOptStat(0);
    cc->cd();
    //cc->SetLogx(1);

    formatCanvas1D(cc);
    gr->Draw("a2");
    gr->Draw("p");
    
    
    TLegend* leg;
    if (is_itk)
        leg = new TLegend(0.4,0.4,0.65,0.2);
    else if(MagField == 0)
        leg = new TLegend(0.6,0.9,0.85,0.7);
    else
        leg = new TLegend(0.6,0.4,0.85,0.2);

    
    leg->SetFillColor(0);
    leg->SetLineColor(1);
    
    leg->AddEntry(gr1,"non-annealed","lpf");
    leg->AddEntry(gr2,"annealed","lpf");

 
    leg->Draw();
    tmp_str = OutputPath +grname + TString(".pdf");
    cc->SaveAs(tmp_str.Data());
    
}


void createMultiGraph_LAvsB(){
    
    TString grname;
    TString tmp_str;
    int i_color = 0;
    
    TString mgr_name = LAvsBname_mgr;
    mgr_name.ReplaceAll("ZZ","500");
    TMultiGraph* mgr = new TMultiGraph(mgr_name,";Magnetic Field (T); tan(#theta_{L})");
    
    TLegend* leg = new TLegend(0.3,0.9,0.7,0.6);
    leg->SetHeader("#theta_{L} at 2T");
    leg->SetFillColor(0);
    
    for (map<int, array<int, 2> >::iterator it_sort = irrad_val_sort.begin(); it_sort!=irrad_val_sort.end(); it_sort++) {
        grname = LAvsBname;
        array<int, 2> arr = it_sort->second;
        grname.ReplaceAll("XX", TString::Itoa(arr[0],10));
        grname.ReplaceAll("YY", TString::Itoa(arr[1],10));
        grname.ReplaceAll("ZZ", "500");
        
        if (rootobjects.find(grname) != rootobjects.end()) {
            TGraphErrors* gr = (TGraphErrors*)rootobjects[grname];
            
            tmp_str = TString("fit_") + grname;
            TF1* fitfunc = new TF1(tmp_str.Data(),"pol1", -0.05, 1.05);
            gr->Fit(fitfunc,"QR");
            fitfunc->SetLineStyle(2);
            fitfunc->SetLineColor(color[i_color]);
            i_color++;
            
            mgr->Add(gr);

            double p0 = fitfunc->GetParameter(0);
            double p1 = fitfunc->GetParameter(1);
            double la2T = p1 * 2.0 + p0;
            la2T = TMath::ATan(la2T);
            la2T = la2T * 180.0 / TMath::Pi();
            
            stringstream ss;
            ss << string("   #theta_{L}(2T) = ")<<fixed<<setprecision(2)<< la2T;
            

            tmp_str = irrad_map_str[arr[0]][arr[1]];
            tmp_str = tmp_str + TString(ss.str());
            leg->AddEntry(gr,tmp_str.Data(),"lp");
        }
    }
   
    cc->cd();
    mgr->Draw("ap");
    leg->Draw();
    tmp_str = mgr_name+TString(".pdf");
    cc->SaveAs(tmp_str.Data());
    
    

    
}


void createGraph_LAvsB(){
    
    int dutid;
    TString tmp_str;
    TString grname;
    
    int i_color = 0;
    
    map<int, map<int, map<double, map<int, LAmeas > > > >::iterator it_dutnum;
    for (it_dutnum = results.begin(); it_dutnum!=results.end(); it_dutnum++) {
        
        
        
        map<int, map<double, map<int, LAmeas > > >::iterator it_voltage;
        for (it_voltage = it_dutnum->second.begin(); it_voltage!= it_dutnum->second.end(); it_voltage++) {
            double B[10]={0};
            double Berr[10]={0};
            double tanLA[10]={0};
            double tanLAerr[10]={0};
            
            
            for (int id=6; id<8; id++) {
                int icount=0;
                map<double, map<int, LAmeas > >::iterator it_bfield;
                for (it_bfield = it_voltage->second.begin(); it_bfield!=it_voltage->second.end(); it_bfield++) {
                    
                    map<int, LAmeas >::iterator it_dutid;
                    for (it_dutid = it_bfield->second.begin(); it_dutid!=it_bfield->second.end(); it_dutid++) {
                        if (it_dutid->first == id) {
                            LAmeas ameas = it_dutid->second;
                            B[icount] = it_bfield->first;
                            Berr[icount] = 0;
                            tanLA[icount] = TMath::Tan(ameas.LA * TMath::Pi() / 180.0);
                            tanLAerr[icount] = TMath::Tan( (ameas.LAerr_stat + ameas.LAerr_sys) *TMath::Pi() / 180.0 );
                            icount++;
                        }
                        
                    }
                }
                
                
                TGraphErrors* gr = new TGraphErrors(icount, B, tanLA, Berr, tanLAerr );
                grname = LAvsBname;
                grname.ReplaceAll("XX", TString::Itoa(it_dutnum->first,10));
                grname.ReplaceAll("YY", TString::Itoa(id,10));
                grname.ReplaceAll("ZZ", TString::Itoa(it_voltage->first,10));
                gr->SetName(grname.Data());
                TString grtitle = grname +TString(";Magnetic field (T); tan(#theta_{L})");
                gr->SetTitle(grtitle.Data());
                gr->GetXaxis()->SetLimits(-0.2,1.2);
                gr->SetMarkerStyle(6);

                rootobjects.insert(pair<TString,TObject*>(grname,gr));

                if (it_voltage->first != 500) continue;

                tmp_str = TString("fit_") + grname;
                TF1* fitfunc = new TF1(tmp_str.Data(),"pol1", -0.05, 1.05);
                fitfunc->SetLineStyle(2);
                gr->Fit(fitfunc,"QR");
                
                //fitfunc->SetLineColor(color[i_color]);
                //i_color++;

                gStyle->SetOptStat(0);
                cc->cd();
                formatCanvas1D(cc);
                formatGraph(gr);
                gr->Draw("ap");
                
                double p0 = fitfunc->GetParameter(0);
                double p0_err = fitfunc->GetParError(0);
                double p1 = fitfunc->GetParameter(1);
                double p1_err = fitfunc->GetParError(1);
                double la2T = p1 * 2.0 + p0;
                la2T = TMath::ATan(la2T);
                la2T = la2T * 180.0 / TMath::Pi();
                double la2T_err = p1_err*2.0+p0_err;
                la2T_err =TMath::ATan(la2T_err) *180.0/ TMath::Pi();
                
                TLatex *latex =new TLatex();
                latex->SetNDC();
                latex->SetTextFont(43);
                latex->SetTextColor(1);
                latex->SetTextSize(20);
                stringstream ss;

                ss << string(" tan(#theta_{L}) = p_{1} #times B-field + p_{0}");
                tmp_str = ss.str();
                latex->DrawLatex(0.5,0.85,tmp_str.Data());

                
                
                ss.str("");
                ss<<string("p_{1} = ")<<fixed<<setprecision(2)<<scientific<< p1 << string(" #pm ")<<fixed<<setprecision(2)<<scientific<<p1_err;
                tmp_str = ss.str();
                latex->DrawLatex(0.56,0.80,tmp_str.Data());

                
                ss.str("");
                ss<<string("p_{0} = ")<< fixed<<setprecision(2)<<scientific<<p0<<string(" #pm ")<<fixed<<setprecision(2)<<scientific<<p0_err;
                tmp_str = ss.str();
                latex->DrawLatex(0.56,0.75,tmp_str.Data());

                ss.str("");
                ss << string("#color[2]{#theta_{L}( 2T ) = ")<<fixed<<setprecision(2)<< la2T << string("#circ #pm ")<< la2T_err<<string("#circ}");
                tmp_str = ss.str();
                //                      cout<<"title "<< title<< endl;
                latex->SetTextColor(kOrange+7);
                latex->SetTextSize(26);
                latex->DrawLatex(0.25,0.25,tmp_str.Data());
                
                tmp_str = getIrrad(it_dutnum->first, id);
                tmp_str = TString("#bf{")+tmp_str+TString("}");
                latex->DrawLatex(0.25,0.35,tmp_str.Data());

                
                tmp_str = OutputPath +grname +TString(".pdf");
                cc->SaveAs(tmp_str.Data());

            }
        }
    }
    
    
}






void printvalues(){
    cout<<"Bias &   B-field (T)	& Sensor number	& Lorentz angle ($^\\circ$) \\\\ "<<endl;
    
    // map<dutnum, map<voltage, map<Bfield, map<dutid, LAmeas > > > >
    map<int, map<int, map<double, map<int, LAmeas > > > >::iterator it_dutnum;
    for (it_dutnum = results.begin(); it_dutnum!=results.end(); it_dutnum++) {
        
        map<int, map<double, map<int, LAmeas > > >::iterator it_voltage;
        for (it_voltage = it_dutnum->second.begin(); it_voltage!= it_dutnum->second.end(); it_voltage++) {
            
            map<double, map<int, LAmeas > >::iterator it_bfield;
            for (it_bfield = it_voltage->second.begin(); it_bfield!=it_voltage->second.end(); it_bfield++) {
                
                map<int, LAmeas >::iterator it_dutid;
                for (it_dutid = it_bfield->second.begin(); it_dutid!=it_bfield->second.end(); it_dutid++) {
                    LAmeas ameas = it_dutid->second;
                    /*
                    cout<<"dutnum"<< it_dutid->first
                        <<" V"<< it_voltage->first
                        <<" B"<< it_bfield->first
                        <<" id"<< it_dutid->first
                    << endl;
                    
                    cout <<"               LA = "<<ameas.LA <<" +/ "<<ameas.LAerr_stat<<"(stat) +/- "<<ameas.LAerr_sys <<"(sys)"<<endl;
                 */
                    
                    
                    
                    // cout<<"Bias &   B-field (T)	& Sensor number	& Lorentz angle ($^\circ$) \\ "<<endl;
                    cout<< it_voltage->first
                    <<" \t & "<< irrad_map_str[ it_dutnum->first ][ it_dutid->first ]
                    <<" \t & $B = "<<fixed<<setprecision(2)<< it_bfield->first
                    <<"$ \t & $"<<fixed<<setprecision(2)<< ameas.LA
                    <<" \\pm "<<fixed<<setprecision(2)<<ameas.LAerr_sys
                    <<" \\text{(syst)} \\pm "<<fixed<<setprecision(2)<<ameas.LAerr_stat
                    <<" \\text{(stat)}$ \\\\ "<<endl;

                }
            }
        }
        
    }

    
    
}


void readCCEResults(){
    	string line;
	string tmpstr;
	int dutnum, dutid;
	double cce;
	ifstream cce_file(cce_txt_file.Data());
	
	getline(cce_file,line);
	while(getline(cce_file,line)){
		// dutnum;dutid;vbias;cce;
		vector<string> values = splitstring(line,';');

		if( values[2] == string("500") ){
			dutnum = atoi(values[0].c_str());
			if (dutnum == 4) continue;
			if (dutnum == 0) dutnum = 4;
			dutid = atoi(values[1].c_str());
			cce = atof(values[3].c_str());
			if(dutid != 0)
				cce_results[dutnum][dutid]=cce;
		}
	}
}

void readSysErr(){
    ifstream file(Errresultfile.Data());
    
    int dutnum,voltage;
    double Bfield, err_6, err_7;
    LAmeas ameas;
    string s;
    
    //DutNum;Voltage;Bfield;Err_DutId_6;Err_DutId_;
    getline(file,s);
    while(!file.eof()){
        getline(file,s,';');
        dutnum = atoi(s.c_str());

        getline(file,s,';');
        voltage = atoi(s.c_str());
        
        getline(file,s,';');
        Bfield = atof(s.c_str());

        getline(file,s,';');
        err_6 = atof(s.c_str());

        getline(file,s,';');
        err_7 = atof(s.c_str());

        if (dutnum != 0 && dutnum != 2) {
            ameas = results[dutnum][voltage][Bfield][6];
            ameas.LAerr_sys = err_6;
            results[dutnum][voltage][Bfield][6] = ameas;
            
            ameas = results[dutnum][voltage][Bfield][7];
            ameas.LAerr_sys = err_7;
            results[dutnum][voltage][Bfield][7] = ameas;
        }
    }
file.close();
}

void readLAResults(){
    // map<dutnum, map<voltage, map<Bfield, map<dutid, value[3] > > > > results
    // value[0] = LA
    // value[1] = LAerr stat
    // value[2] = LAerr syst
    
    // map<dutnum, map<dutid, map<irrad_str, irrad_double > > > irrad_map


    ifstream file(LAresultfile.Data());

    int dutnum, voltage, dutid;
    double Bfield, LA, LAerr_stat, LAerr_sys, irrad_double;
    TString irrad_str;
    string s;


    while(!file.eof()){
        
        getline(file,s,';');
        dutnum = atoi(s.c_str());
        
        getline(file,s,';');
        dutid = atoi(s.c_str());
        
        getline(file,s,';');
        irrad_str = TString(s);
        
        getline(file,s,';');
        Bfield = atof(s.c_str());
 
        getline(file,s,';');
        voltage = atoi(s.c_str());

        getline(file,s,';');
        LA = atof(s.c_str());

        getline(file,s,';');
        LAerr_stat = atof(s.c_str());

        if (dutnum != 0 && dutid !=0 && dutnum != 2) {
            LAmeas ameas;
            ameas.LA = LA;
            ameas.LAerr_stat = LAerr_stat;
            ameas.LAerr_sys = 0;
            results[dutnum][voltage][Bfield][dutid] = ameas;
            
            irrad_double = decodeIrradVal(irrad_str);
 
            irrad_map_val[dutnum][dutid] = irrad_double;
            irrad_map_str[dutnum][dutid] = irrad_str;

        }
    }
file.close();
    return;
}

double decodeIrradVal(TString str){
    string s(str.Data());
    double irrad_val= -1;
    if (remove_last_n_char(s,1) == string("non-irrad")){
        irrad_val = 0.0;
    }
    else if (get_last_n_char(s,3) == string("_An")){
        s = remove_last_n_char(s,3);
        istringstream os(s);
        os >> irrad_val;
    }
    else {
        istringstream os(s);
        os >> irrad_val;
    }
    
    return irrad_val;
}

string remove_last_n_char(string s, unsigned int n){
    string ss = s;
    return ss.substr(0,ss.length()-n);
}
string get_last_n_char(string s, unsigned int n){
    string ss = s;
    return ss.substr(ss.length()-n,ss.length());
}




void createOutputFile(){
    TFile *fout = new TFile(output_rootfile.Data(),"RECREATE");

    fout->cd();
    for (map<TString,TObject*>::iterator it=rootobjects.begin(); it != rootobjects.end(); it++) {
         it->second->Write();
        
    }
    
}






