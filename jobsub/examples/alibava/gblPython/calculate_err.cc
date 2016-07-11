/*
 * Created by Eda Yildirim
 *  (2016 JGU)
 *
 *  email:eda.yildirim@cern.ch
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <map>
#include <utility>
#include <algorithm>
#include <math.h>

#include "TString.h"
#include "TMultiGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"


#include "calculate_err.h"

TString resultfile_name= TString("./lam_error.txt");
TString gbloutDir = TString("./output/notcomb-xtalk-telaligned/run00XXXX");
TString csvfilename = TString("../runlistfiles/lam_file_info.csv");

TString millepede_file_name = TString("/millepede_3.res");
TString root_file_name = TString("/dutTree_3.root");


map<int, map<int, map<double, vector<int> > > > measurements;

using namespace std;

int main(int argc, char *argv[]){
    
    double weighted_error[2];
    readCsvFile();
    ofstream resultfile;
    resultfile.open (resultfile_name.Data());
    resultfile << "DutNum;Voltage;Bfield;Err_DutId_6;Err_DutId_;"<<endl;
    
    map<int, map<int, map<double, vector<int> > > >::iterator it_dut;
    for(it_dut = measurements.begin(); it_dut != measurements.end(); it_dut++) { // loop over duts
        
        map<int, map<double, vector<int> > >::iterator it_voltage;
        for (it_voltage = it_dut->second.begin(); it_voltage != it_dut->second.end(); it_voltage++) { // loop over voltage
            
            map<double, vector<int> >::iterator it_bfield;
            for (it_bfield = it_voltage->second.begin() ; it_bfield != it_voltage->second.end(); it_bfield++) { //loop over bfield
                
                vector<int> runnums = it_bfield->second;
                calculate_LA_error(runnums, weighted_error);
                
                resultfile<<it_dut->first<<";"
                <<it_voltage->first<<";"
                <<it_bfield->first<<";"
                <<weighted_error[0]<<";"
                <<weighted_error[1]<<";"
                <<endl;
                
            } // end of loop over bfield
        }// end of loop over voltage
    } //end of loop over duts
    resultfile.close();

}



void readCsvFile(){
    
    ifstream csv_file(csvfilename.Data());
    string firstline;
    getline(csv_file,firstline);
    // first line: DutNum  RunNum  DutID   Bias    Irrad
    string s;
    int num;
    // map<dutnum, map<voltage, map<bfield, vector<runnums> > > > ameas;
    
    int dutnum, voltage,dutid;
    double bfield;
    int runnum;
    
    while(!csv_file.eof()){
        
        getline(csv_file,s,';');
        dutnum = atoi(s.c_str());
        
        getline(csv_file,s,';');
        runnum = atoi(s.c_str());
        
        getline(csv_file,s,';');
        dutid = atoi(s.c_str());
        
        getline(csv_file,s,';');
        
        getline(csv_file,s,';'); // Rotation
        getline(csv_file,s,';'); // RotationSensor
        getline(csv_file,s,';'); // RotationStage
        getline(csv_file,s,';'); // NegRotation
        
        getline(csv_file,s,';');
        voltage = atoi(s.c_str());
        voltage = -1*voltage;
        
        getline(csv_file,s,';'); // SensorTemp
        getline(csv_file,s,';'); // BeamEnergy
        
        getline(csv_file,s,';'); // B
        bfield = atof(s.c_str());
        
        getline(csv_file,s,';'); // CalculatedB
        getline(csv_file,s,';'); // Current on Power supply
        
        if (runnum != 0 && dutnum !=0 && dutnum !=2 && dutid == 6) {
            measurements[dutnum][voltage][bfield].push_back(runnum);
            //info.print();
        }
    }
}



void calculate_LA_error(vector<int> runnums, double weighted_error[]){
    
    double errors[2];
    double nTrack[2];
    
    double w_sum[2] = {0,0};
    double tot_nTrack[2] = {0,0};
    
    cout<<"runnums ";
    for(unsigned int irun = 0; irun<runnums.size(); irun++){
        cout<<" "<<runnums[irun];
        TString run_folder = gbloutDir;
        run_folder.ReplaceAll(TString("XXXX"), TString::Itoa(runnums[irun],10));
        
        TString millepede_file = run_folder + millepede_file_name;
        readmillepede(millepede_file, errors);
        
        TString root_file = run_folder + root_file_name;
        TFile *f = TFile::Open(root_file.Data());
        TTree* t = (TTree*)f->Get("treeDUT");
        nTrack[0] = t->GetEntries("dutID==6");
        nTrack[1] = t->GetEntries("dutID==7");
        
        for (unsigned int i=0; i<2; i++){
            w_sum[i] = w_sum[i] + (pow(nTrack[i],2.0) * pow(errors[i],2.0));
            tot_nTrack[i] = tot_nTrack[i] + nTrack[i];
        }
        
    }
    cout<<endl;
    for (unsigned int i=0; i<2; i++){
        weighted_error[i] = sqrt(w_sum[i]) / tot_nTrack[i];
        weighted_error[i] = weighted_error[i] * 180.0 / TMath::Pi();
    }
    
}

void readmillepede(TString millepede_res, double errors[]){
    
    ifstream millepede_file(millepede_res.Data());
    string aline;
    getline(millepede_file,aline);
    
    int linecount = 0;
    while(!millepede_file.eof()){
        getline(millepede_file,aline);
        if (linecount == millepede_err_id6) {
            errors[0] = decodeError(aline);
        }
        if (linecount == millepede_err_id7) {
            errors[1] = decodeError(aline);
        }
        linecount++;
    }
    millepede_file.close();
}

double decodeError(string aline){
    
    string buf; // Have a buffer string
    stringstream ss(aline); // Insert the string into a stream
    
    vector<string> tokens; // Create vector to hold our words
    
    while (ss >> buf)
        tokens.push_back(buf);
    
    istringstream iss(tokens[millepede_err_column]);
    double d;
    iss >> d;
    
    return d;
}























