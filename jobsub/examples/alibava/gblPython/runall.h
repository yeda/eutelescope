/*
 * Created by Eda Yildirim
 *  (2016 JGU)
 *
 *  email:eda.yildirim@cern.ch
 */


#ifndef RunAll_h
#define RunAll_h

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <map>
#include <array>

#include "TString.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TMultiGraph.h"

using namespace std;

struct LAmeas {
    double LA;
    double LAerr_stat;
    double LAerr_sys;
};

void createGraph_LAvsB();
void createMultiGraph_LAvsB();
void createGraph_LAvsIrrad();
void createGraph_LAvsBias();

void readLAResults();
void readSysErr();
void printvalues();
string remove_last_n_char(string s, unsigned int n);
string get_last_n_char(string s, unsigned int n);
double decodeIrradVal(TString str);
void createOutputFile();

// map<sortnum, map<dutnum, dutid > > irrad_val_sort;
map<int, array<int, 2> > irrad_val_sort={
    {0,array<int, 2>{4,6}}, {1,array<int, 2>{4,7}},
    {2,array<int, 2>{5,6}}, {3,array<int, 2>{5,7}},
    {3,array<int, 2>{6,6}}, {5,array<int, 2>{6,7}},
    {6,array<int, 2>{1,6}}, {7,array<int, 2>{1,7}},
    {8,array<int, 2>{3,6}}, {9,array<int, 2>{3,7}},
    {10,array<int, 2>{2,6}}, {11,array<int, 2>{2,7}}
};

void formatCanvas1D(TCanvas *cc){
    cc->SetRightMargin(0.125);
    cc->SetLeftMargin(0.125);
    cc->SetBottomMargin(0.13);
    cc->SetTopMargin(0.07);
}
void formatCanvasColZ(TCanvas *cc){
    cc->SetRightMargin(0.1);
    cc->SetLeftMargin(0.125);
    cc->SetBottomMargin(0.13);
    cc->SetTopMargin(0.07);
}


void formatGraph(TGraphErrors *gr){
    gr->GetXaxis()->SetLabelSize(0.045);
    gr->GetXaxis()->SetTitleSize(0.05);
    gr->GetXaxis()->SetTitleOffset(1.2);
    
    gr->GetYaxis()->SetLabelSize(0.045);
    gr->GetYaxis()->SetTitleSize(0.05);
    gr->GetYaxis()->SetTitleOffset(1.2);
    
    gr->SetTitle("");
}

TString getIrrad(int DutNum, int DutID){
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

#endif


