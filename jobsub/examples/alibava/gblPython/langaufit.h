#include <iostream>
#include <fstream>
#include <vector>
#include "TString.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TROOT.h"
#include "TMath.h"
#include <cstdlib>

using namespace std;


/*
	// Langaus fit settings
	Double_t fitrange[2]={20.0,500.0};
 Double_t startvalues[4], parlimitslow[4], parlimitshigh[4];
	Double_t fitparameters[4], fitparameters_error[4];
 
	//   par[0]=Width (scale) parameter of Landau density
	startvalues[0]=10.0; parlimitslow[0]=0.5; parlimitshigh[0]=20.0;
 //   par[1]=Most Probable (MP, location) parameter of Landau density
	startvalues[1]=50.0; parlimitslow[1]=20.0; parlimitshigh[1]=150.0;
 //   par[2]=Total area (integral -inf to inf, normalization constant)
	startvalues[2]=5000.0; parlimitslow[2]=1.; parlimitshigh[2]=1000000.0;
 //   par[3]=Width (sigma) of convoluted Gaussian function
	startvalues[3]=10.0; parlimitslow[3]=0.4; parlimitshigh[3]=20.0;
	
	Double_t chisqr;
	Int_t    ndf;
	Double_t LangauPeak, LangauFWHM;
	TCanvas *c0 = new TCanvas("tmpcanvas","tmpcanvas",800,600);
	
	TH1D *fitpar_vs_time[8];
	TF1 *fitlangau;
	char tmpstring[8][20]={"Width","MP","Area","Gsigma","chisqr","langaupeak","langauFWHM", "Prob"};
 fitlangau = langaufit(hist,fitrange,startvalues,parlimitslow,parlimitshigh,fitparameters,fitparameters_error,&chisqr,&ndf);
 langaupro(fitparameters,LangauPeak,LangauFWHM);
 
 */

Double_t langaufun(Double_t *x, Double_t *par) {
    
    //Fit parameters:
    //par[0]=Width (scale) parameter of Landau density
    //par[1]=Most Probable (MP, location) parameter of Landau density
    //par[2]=Total area (integral -inf to inf, normalization constant)
    //par[3]=Width (sigma) of convoluted Gaussian function
    //
    //In the Landau distribution (represented by the CERNLIB approximation),
    //the maximum is located at x=-0.22278298 with the location parameter=0.
    //This shift is corrected within this function, so that the actual
    //maximum is identical to the MP parameter.
    
    // Numeric constants
    Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
    Double_t mpshift  = -0.22278298;       // Landau maximum location
    
    // Control constants
    Double_t np = 100.0;      // number of convolution steps
    Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    
    // Variables
    Double_t xx;
    Double_t mpc;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow,xupp;
    Double_t step;
    Double_t i;
    
    
    // MP shift correction
    mpc = par[1] - mpshift * par[0];
    
    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];
    
    step = (xupp-xlow) / np;
    
    // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
        xx = xlow + (i-.5) * step;
        fland = TMath::Landau(xx,mpc,par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0],xx,par[3]);
        
        xx = xupp - (i-.5) * step;
        fland = TMath::Landau(xx,mpc,par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }
    
    return (par[2] * step * sum * invsq2pi / par[3]);
}


TF1 *langaufit(TH1D *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF)
{
    // Once again, here are the Landau * Gaussian parameters:
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
    
    Int_t i;
    Char_t FunName[100];
    
    sprintf(FunName,"Fitfcn_%s",his->GetName());
    
    TF1 *ffitold = (TF1*)gROOT->GetListOfFunctions()->FindObject(FunName);
    if (ffitold) delete ffitold;
    
    TF1 *ffit = new TF1(FunName,langaufun,fitrange[0],fitrange[1],4);
    ffit->SetParameters(startvalues);
    ffit->SetParNames("Width","MP","Area","GSigma");
    
    for (i=0; i<4; i++) {
        ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
    }
    
    his->Fit(FunName,"RBQ");   // fit within specified range, use ParLimits, do not print fit results
    
    ffit->GetParameters(fitparams);    // obtain fit parameters
    for (i=0; i<4; i++) {
        fiterrors[i] = ffit->GetParError(i);     // obtain fit parameter errors
    }
    ChiSqr[0] = ffit->GetChisquare();  // obtain chi^2
    NDF[0] = ffit->GetNDF();           // obtain ndf
    
    return (ffit);              // return fit function
    
}


Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM) {
    // Seaches for the location (x value) at the maximum of the
    // Landau-Gaussian convolute and its full width at half-maximum.
    //
    // The search is probably not very efficient, but it's a first try.
    
    Double_t p,x,fy,fxr,fxl;
    Double_t step;
    Double_t l,lold;
    Int_t i = 0;
    Int_t MAXCALLS = 10000;
    
    // Search for maximum
    p = params[1] - 0.1 * params[0];
    step = 0.05 * params[0];
    lold = -2.0;
    l    = -1.0;
    
    while ( (l != lold) && (i < MAXCALLS) ) {
        i++;
        lold = l;
        x = p + step;
        l = langaufun(&x,params);
        if (l < lold) step = -step/10;
        p += step;
    }
    
    if (i == MAXCALLS) return (-1);
    maxx = x;
    fy = l/2;
    
    // Search for right x location of fy
    p = maxx + params[0];
    step = params[0];
    lold = -2.0;
    l    = -1e300;
    i    = 0;
    
    while ( (l != lold) && (i < MAXCALLS) ) {
        i++;
        lold = l;
        x = p + step;
        l = TMath::Abs(langaufun(&x,params) - fy);
        if (l > lold) step = -step/10;
        p += step;
    }
    if (i == MAXCALLS) return (-2);
    fxr = x;
    
    // Search for left x location of fy
    p = maxx - 0.5 * params[0];
    step = -params[0];
    lold = -2.0;
    l    = -1e300;
    i    = 0;
    
    while ( (l != lold) && (i < MAXCALLS) ) {
        i++;
        lold = l;
        x = p + step;
        l = TMath::Abs(langaufun(&x,params) - fy);
        if (l > lold) step = -step/10;
        p += step;
    }
    
    if (i == MAXCALLS) return (-3);
    fxl = x;
    
    FWHM = fxr - fxl;
    return (0);
}

