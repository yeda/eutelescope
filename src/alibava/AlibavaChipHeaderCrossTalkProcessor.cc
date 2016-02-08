/*
 * Created by Eda Yildirim
 *  (2014 DESY)
 *
 *  email:eda.yildirim@cern.ch
 */


// alibava includes ".h"
#include "AlibavaChipHeaderCrossTalkProcessor.h"
#include "AlibavaRunHeaderImpl.h"
#include "AlibavaBaseProcessor.h"
#include "AlibavaEventImpl.h"
#include "ALIBAVA.h"
#include "AlibavaPedNoiCalIOManager.h"


// marlin includes ".h"
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include "marlin/Global.h"
#include "marlin/ProcessorMgr.h"

#if defined(USE_AIDA) || defined(MARLIN_USE_AIDA)
// aida includes <.h>
#include <marlin/AIDAProcessor.h>
#include <AIDA/ITree.h>
#endif

// lcio includes <.h>
#include <lcio.h>
#include <UTIL/LCTOOLS.h>
#include <UTIL/CellIDEncoder.h>
#include <UTIL/CellIDDecoder.h>
#include <UTIL/LCTime.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/TrackerDataImpl.h>
#include <IMPL/LCEventImpl.h>


// ROOT includes ".h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TSystem.h"

// system includes <>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <vector>

using namespace std;
using namespace lcio;
using namespace marlin;
using namespace alibava;


AlibavaChipHeaderCrossTalkProcessor::AlibavaChipHeaderCrossTalkProcessor () :
AlibavaBaseProcessor("AlibavaChipHeaderCrossTalkProcessor"),
_rawdataCollectionName("rawdata"),
_databaseFile("database.slcio"),
_crosstalkCollectionName("crosstalk"),
_chanDataHistoName("ChanData"),
_chanDataCorrelationHistoName("ChanDataCorr"),
_channelsUsedForCrosstalkCalculation(),
_maxNEvent(20000)
{
    
    // modify processor description
    _description =
    "AlibavaChipHeaderCrossTalkProcessor calculates the cross talk correction values using chip header information";
    registerInputCollection (LCIO::TRACKERDATA, "InputCollectionName",
                             "Input collection name of alibava clusters",
                             _inputCollectionName, string("chipheader") );
    
    registerProcessorParameter ("RecoDataCollectionName",
                                "The collection name of raw data",
                                _rawdataCollectionName, string ("rawdata"));
    
    registerProcessorParameter ("DatabaseFile",
                                "The filename where the cross talk coefficients will be stored",
                                _databaseFile , string("database.slcio"));
    
    registerProcessorParameter ("CrossTalkCollectionName",
                                "The collection name of cross talk coefficients which will be stored in DatabaseFile",
                                _crosstalkCollectionName, string ("crosstalk"));
    
    registerProcessorParameter ("MaxNEvent",
                                "Max number of events to be used for cross talk calculation",
                                _maxNEvent, int (20000));
    
}


void AlibavaChipHeaderCrossTalkProcessor::init () {
    streamlog_out ( MESSAGE4 ) << "Running init" << endl;
    
    // this method is called only once even when the rewind is active
    // usually a good idea to
    printParameters ();
    
    /* To set of channels to be used
     ex.The format should be like $ChipNumber:StartChannel-EndChannel$
     ex. $0:5-20$ $0:30-100$ $1:50-70$
     means from chip 0 channels between 5-20 and 30-100, from chip 1 channels between 50-70 will be used (all numbers included). the rest will be masked and not used
     Note that the numbers should be in ascending order and there should be no space between two $ character
     */
    if (Global::parameters->isParameterSet(ALIBAVA::CHANNELSTOBEUSED))
        Global::parameters->getStringVals(ALIBAVA::CHANNELSTOBEUSED,_channelsToBeUsed);
    else {
        streamlog_out ( MESSAGE4 ) << "The Global Parameter "<< ALIBAVA::CHANNELSTOBEUSED <<" is not set!" << endl;
    }
    
    
    /* To choose if processor should skip masked events
     ex. Set the value to 0 for false, to 1 for true
     */
    if (Global::parameters->isParameterSet(ALIBAVA::SKIPMASKEDEVENTS))
        _skipMaskedEvents = bool ( Global::parameters->getIntVal(ALIBAVA::SKIPMASKEDEVENTS) );
    else {
        streamlog_out ( MESSAGE4 ) << "The Global Parameter "<< ALIBAVA::SKIPMASKEDEVENTS <<" is not set! Masked events will be used!" << endl;
    }
    
    
}

void AlibavaChipHeaderCrossTalkProcessor::processRunHeader (LCRunHeader * rdr) {
    streamlog_out ( MESSAGE4 ) << "Running processRunHeader" << endl;
    streamlog_out ( MESSAGE4 ) << "Running processRunHeader" << endl;
    
    // Add processor name to the runheader
    auto_ptr<AlibavaRunHeaderImpl> arunHeader ( new AlibavaRunHeaderImpl(rdr)) ;
    arunHeader->addProcessor(type());
    
    // get and set selected chips
    setChipSelection(arunHeader->getChipSelection());
    
    // set channels to be used (if it is defined)
    setChannelsToBeUsed();
    
    // set pedestal and noise values
    setPedestals();
    
    // set number of skipped events to zero (defined in AlibavaBaseProcessor)
    _numberOfSkippedEvents = 0;
    
    _channelsUsedForCrosstalkCalculation.push_back(ALIBAVA::CHIPHEADERLENGTH-2);
    _channelsUsedForCrosstalkCalculation.push_back(ALIBAVA::CHIPHEADERLENGTH-1);
    _channelsUsedForCrosstalkCalculation.push_back(ALIBAVA::CHIPHEADERLENGTH+0); // first channel in raw data
    
    bookHistos();
    fillHistosForPedCalculation();
    storePedestals();
    fillHistosForCrossTalkCalculation();
    calculateCrossTalk ();
    
    
}

void AlibavaChipHeaderCrossTalkProcessor::processEvent (LCEvent * /* anEvent */) {
    
}
void AlibavaChipHeaderCrossTalkProcessor::storePedestals(){
    
    EVENT::IntVec chipSelection = getChipSelection();
    string histoname;
    
    for (unsigned int ichip=0; ichip<chipSelection.size(); ichip++) {
        int chipnum = chipSelection[ichip];
        
        for (unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size(); ichan++) {
            int channum =_channelsUsedForCrosstalkCalculation[ichan];
            string histoname = getChanHistoName( chipnum, channum);
            TH1D * histo = dynamic_cast<TH1D*> (_rootObjectMap[histoname]);
            _pedestal[chipnum][ichan] = histo->GetMean();
            
        }// end of ichan loop
    }// end of ichip loop
}

void AlibavaChipHeaderCrossTalkProcessor::fillHistosForPedCalculation(){
    streamlog_out ( MESSAGE5 ) << "Running through events for pedestal calculation << endl ;
    
    LCReader* lcReader = LCFactory::getInstance()->createLCReader();
    try {
        lcReader->open( Global::parameters->getStringVal("LCIOInputFiles") );
    } catch( IOException& e ){
        streamlog_out ( ERROR1 ) << "Can't open the file: " << e.what() << endl ;
    }
    
    AlibavaEventImpl* alibavaEvent;
    while( (alibavaEvent = static_cast<AlibavaEventImpl*> ( lcReader->readNextEvent() ) ) != 0 ){
        
        if (alibavaEvent->getEventNumber() > _maxNEvent && _maxNEvent!=0) {
            lcReader->close() ;
            return;
        }
        
        LCCollectionVec * chipheaderColVec;
        LCCollectionVec * dataColVec;
        unsigned int noOfChip;
        try
        {
            chipheaderColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( getInputCollectionName() ) ) ;
            dataColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( _rawdataCollectionName ) ) ;
            
            noOfChip = chipheaderColVec->getNumberOfElements();
            
            for ( size_t i = 0; i < noOfChip; ++i )
            {
                TrackerDataImpl * data = dynamic_cast< TrackerDataImpl * > ( dataColVec->getElementAt( i ) ) ;
                
                TrackerDataImpl * chipheader = dynamic_cast< TrackerDataImpl * > ( chipheaderColVec->getElementAt( i ) ) ;
                
                
                int chipnum = getChipNum(chipheader);
                
                if (chipnum != getChipNum(data)) {
                    streamlog_out(ERROR5)<< "Chipheader chipnum and data chip number doesn't match. This is important fix it!"<<endl;
                }
                
                FloatVec datavec;
                datavec = data->getChargeValues();
                
                FloatVec chipheadervec;
                chipheadervec = chipheader->getChargeValues();
                
                // ok, chipheadervec and datavec are constant vectors so we cannot modify them
                // lets copy them to another vector
                vector<float> channelValues;
                channelValues.clear();
                
                
                channelValues.push_back(chipheadervec[_channelsUsedForCrosstalkCalculation[0]]);
                channelValues.push_back(chipheadervec[_channelsUsedForCrosstalkCalculation[1]]);
                
                channelValues.push_back(datavec[0]);
                
                
                string tempHistoName;
                
                for (unsigned int ichan = 0 ; ichan<_channelsUsedForCrosstalkCalculation.size(); ichan++) {
                    int channum =_channelsUsedForCrosstalkCalculation[ichan];
                    tempHistoName = getChanHistoName(chipnum,channum);
                    if ( TH1D * histo = dynamic_cast<TH1D*> (_rootObjectMap[tempHistoName]) )
                        histo->Fill(channelValues[ichan]);
                    
                }
                
                for ( unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size()-1; ichan++) {
                    tempHistoName = getChanCorrelationHistoName(chipnum,_channelsUsedForCrosstalkCalculation[ichan],_channelsUsedForCrosstalkCalculation[2]);
                    if ( TH2D * histo2 = dynamic_cast<TH2D*> (_rootObjectMap[tempHistoName]) )
                        histo2->Fill(channelValues[ichan],channelValues[2]);
                }
                
            }
            
        } catch ( lcio::DataNotAvailableException ) {
            // do nothing again
            streamlog_out( ERROR5 ) << "Collection not found! " << endl;
        }
        
    }// end of loop over events
    lcReader->close() ;
}

void AlibavaChipHeaderCrossTalkProcessor::fillHistosForCrossTalkCalculation(){
    streamlog_out ( MESSAGE5 ) << "Running through events, for cross talk calculation" << endl ;
    
    LCReader* lcReader = LCFactory::getInstance()->createLCReader();
    try {
        lcReader->open( Global::parameters->getStringVal("LCIOInputFiles") );
    } catch( IOException& e ){
        streamlog_out ( ERROR1 ) << "Can't open the file: " << e.what() << endl ;
    }
    
    AlibavaEventImpl* alibavaEvent;
    while( (alibavaEvent = static_cast<AlibavaEventImpl*> ( lcReader->readNextEvent() ) ) != 0 ){
        
        if (alibavaEvent->getEventNumber() > _maxNEvent && _maxNEvent!=0) {
            lcReader->close() ;
            return;
        }
        
        LCCollectionVec * chipheaderColVec;
        LCCollectionVec * dataColVec;
        unsigned int noOfChip;
        try
        {
            chipheaderColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( getInputCollectionName() ) ) ;
            dataColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( _rawdataCollectionName ) ) ;
            
            noOfChip = chipheaderColVec->getNumberOfElements();
            
            
            for ( size_t i = 0; i < noOfChip; ++i )
            {
                TrackerDataImpl * data = dynamic_cast< TrackerDataImpl * > ( dataColVec->getElementAt( i ) ) ;
                
                TrackerDataImpl * chipheader = dynamic_cast< TrackerDataImpl * > ( chipheaderColVec->getElementAt( i ) ) ;
                
                
                int chipnum = getChipNum(chipheader);
                
                if (chipnum != getChipNum(data)) {
                    streamlog_out(ERROR5)<< "Chipheader chipnum and data chip number doesn't match. This is important fix it!"<<endl;
                }
                
                FloatVec datavec;
                datavec = data->getChargeValues();
                
                FloatVec chipheadervec;
                chipheadervec = chipheader->getChargeValues();
                
                // ok, chipheadervec and datavec are constant vectors so we cannot modify them
                // lets copy them to another vector
                vector<float> channelValues;
                channelValues.clear();
                
                
                channelValues.push_back(chipheadervec[_channelsUsedForCrosstalkCalculation[0]]);
                channelValues.push_back(chipheadervec[_channelsUsedForCrosstalkCalculation[1]]);
                
                channelValues.push_back(datavec[0]);
                string tempHistoName;
                string chanvalue;
                
                if (channelValues[0]<_pedestal[chipnum][0]) { // next next channel is low
                    if (channelValues[1]<_pedestal[chipnum][1]) {// next channel is low
                        chanvalue = string("LL");
                    }
                    else {// next channel is high
                        chanvalue = string("LH");
                    }
                }else { // next next channel is high
                    if (channelValues[1]<_pedestal[chipnum][1]) {// next channel is low
                        chanvalue = string("HL");
                    }
                    else {// next channel is high
                        chanvalue = string("HH");
                    }
                }
                
                for (unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size()-1; ichan++) {
                    
                    
                    tempHistoName = getChanCorrelationHistoName(chipnum,_channelsUsedForCrosstalkCalculation[ichan],_channelsUsedForCrosstalkCalculation[2]);
                    tempHistoName+=chanvalue;
                    
                    if ( TH2D * histo2 = dynamic_cast<TH2D*> (_rootObjectMap[tempHistoName]) )
                        histo2->Fill(channelValues[ichan]-_pedestal[chipnum][ichan],channelValues[2]-_pedestal[chipnum][2]);
                    
                }
            }
            
        } catch ( lcio::DataNotAvailableException ) {
            // do nothing again
            streamlog_out( ERROR5 ) << "Collection not found! " << endl;
        }
        
    }// end of loop over events
    lcReader->close() ;
}

void AlibavaChipHeaderCrossTalkProcessor::check (LCEvent * /* evt */ ) {
    // nothing to check here - could be used to fill check plots in reconstruction processor
}

void AlibavaChipHeaderCrossTalkProcessor::fillHistos(TrackerDataImpl * /*trkdata */){
}

void AlibavaChipHeaderCrossTalkProcessor::end() {
    
    streamlog_out ( MESSAGE4 ) << "Successfully finished" << endl;
    
}



void AlibavaChipHeaderCrossTalkProcessor::calculateCrossTalk(){
    
    streamlog_out ( MESSAGE5 ) << "Calculating Cross Talk " << endl ;
    EVENT::IntVec chipSelection = getChipSelection();
    string tempHistoName;
    for (unsigned int ichip=0; ichip<chipSelection.size(); ichip++) {
        int chipnum = chipSelection[ichip];
        
        vector<double> crosstalkCoefficient;
        crosstalkCoefficient.clear();
        
        double mean_nextnextchannel[4][2];
        double mean_nextchannel[4][2];
        string chanvalue[4]= {"LL","LH","HL","HH"};
        TH2D * histo2;
        TH1D * histo2_px;
        TH1D * histo2_py;
        string projectionname;
        string fitname;
        TF1 *histo2_px_Fit;
        TF1 *histo2_py_Fit;

        double min,max;
        min = -500;
        max = 500;
        
        for (int i=0; i<4; i++){
            // this is for next next channel
            tempHistoName = getChanCorrelationHistoName(chipnum,_channelsUsedForCrosstalkCalculation[0],_channelsUsedForCrosstalkCalculation[2]);
            tempHistoName+=chanvalue[i];
            histo2 = dynamic_cast<TH2D*> (_rootObjectMap[tempHistoName]);
            
            //            mean_nextnextchannel[i][0] = histo2->GetMean(1);
            //            mean_nextnextchannel[i][1] = histo2->GetMean(2);
            
            projectionname = tempHistoName+string("_px");
            histo2_px = histo2->ProjectionX(projectionname.c_str(),0,-1);
            fitname = projectionname+string("_fit");
            histo2_px_Fit= new TF1(fitname.c_str(),"gaus",min,max);
            histo2_px->Fit(histo2_px_Fit,"QR");
            _rootObjectMap.insert(make_pair(projectionname,histo2_px));
            
            projectionname = tempHistoName+string("_py");
            histo2_py = histo2->ProjectionY(projectionname.c_str(),0,-1);
            fitname = projectionname+string("_fit");
            histo2_py_Fit= new TF1(fitname.c_str(),"gaus",min,max);
            histo2_py->Fit(histo2_py_Fit,"QR");
            _rootObjectMap.insert(make_pair(projectionname,histo2_py));
            
            mean_nextnextchannel[i][0] = histo2_px_Fit->GetParameter(1);
            mean_nextnextchannel[i][1] = histo2_py_Fit->GetParameter(1);
            
            // this is for next channel
            tempHistoName = getChanCorrelationHistoName(chipnum,_channelsUsedForCrosstalkCalculation[1],_channelsUsedForCrosstalkCalculation[2]);
            tempHistoName+=chanvalue[i];
            histo2 = dynamic_cast<TH2D*> (_rootObjectMap[tempHistoName]);
            //            mean_nextchannel[i][0] = histo2->GetMean(1);
            //          mean_nextchannel[i][1] = histo2->GetMean(2);
            
            projectionname = tempHistoName+string("_px");
            histo2_px = histo2->ProjectionX(projectionname.c_str(),0,-1);
            fitname = projectionname+string("_fit");
            histo2_px_Fit= new TF1(fitname.c_str(),"gaus",min,max);
            histo2_px->Fit(histo2_px_Fit,"QR");
            _rootObjectMap.insert(make_pair(projectionname,histo2_px));
            
            projectionname = tempHistoName+string("_py");
            histo2_py = histo2->ProjectionY(projectionname.c_str(),0,-1);
            fitname = projectionname+string("_fit");
            histo2_py_Fit= new TF1(fitname.c_str(),"gaus",min,max);
            histo2_py->Fit(histo2_py_Fit,"QR");
            _rootObjectMap.insert(make_pair(projectionname,histo2_py));
            
            mean_nextchannel[i][0] = histo2_px_Fit->GetParameter(1);
            mean_nextchannel[i][1] = histo2_py_Fit->GetParameter(1);
            
        }
        
        
        // in order to cancel the effect of next next channel
        // keep the next channel constant and get mean of mean values to calculate b1
        
        /*
         // b1_nextchannelLow = (meany_LL + meany_HL)/ (meanx_LL + meanx_HL)
         double b1_nextchannelLow = (mean_nextchannel[0][1] + mean_nextchannel[2][1])/ (mean_nextchannel[0][0] + mean_nextchannel[2][0]);
         
         // b1_nextchannelHigh = (meany_LH + meany_HH)/ (meanx_LH + meanx_HH)
         double b1_nextchannelHigh = (mean_nextchannel[1][1] + mean_nextchannel[3][1])/ (mean_nextchannel[1][0] + mean_nextchannel[3][0]);
         
         double b1 = (b1_nextchannelLow + b1_nextchannelHigh) /2.0;
         
         double x1 = (mean_nextchannel[0][0] + mean_nextchannel[2][0])/2;
         double x2 =( mean_nextchannel[1][0] + mean_nextchannel[3][0])/2;
         
         double y1 = (mean_nextchannel[0][1] + mean_nextchannel[2][1])/2;
         double y2 = (mean_nextchannel[1][1] + mean_nextchannel[3][1])/2;
         double b1 = (y2-y1)/(x2-x1);
         */
        
        double b1_LL= mean_nextchannel[0][1]/ mean_nextchannel[0][0];
        double b1_LH= mean_nextchannel[1][1]/ mean_nextchannel[1][0];
        double b1_HL= mean_nextchannel[2][1]/ mean_nextchannel[2][0];
        double b1_HH= mean_nextchannel[3][1]/ mean_nextchannel[3][0];
        
        double b1= (b1_LL+b1_LH+b1_HL+b1_HH)/4.0;
        
        
        // in order to cancel the effect of next channel
        // keep the next channel constant calculate the difference in channel introduced by next next channel
        
        // b2_nextchannelLow = fabs(meany_LL - meany_HL)/ fabs(meanx_LL - meanx_HL)
        double b2_nextchannelLow = fabs(mean_nextnextchannel[0][1]-mean_nextnextchannel[2][1]) / fabs(mean_nextnextchannel[0][0] - mean_nextnextchannel[2][0]);
        
        // b2_nextchannelHigh = fabs(meanx_LH - meanx_HH)/ fabs(meany_LH - meany_HH))
        double b2_nextchannelHigh = fabs(mean_nextnextchannel[1][1]-mean_nextnextchannel[3][1]) / fabs(mean_nextnextchannel[1][0] - mean_nextnextchannel[3][0]);
        
        
        double b2 = (b2_nextchannelLow + b2_nextchannelHigh) /2.0;
        
        /*
         // calculate mean value of deviation because of channel 14
         double deviation = (fabs(mean_nextchannel[0][1] - mean_nextchannel[2][1]) + fabs(mean_nextchannel[1][1] - mean_nextchannel[3][1])) / 4.0;
         // calculate mean value of channel 14
         double mean_14 = (fabs(mean_nextnextchannel[0][0])+fabs(mean_nextnextchannel[1][0])+fabs(mean_nextnextchannel[2][0])+fabs(mean_nextnextchannel[3][0]))/4.0;
         
         double b2 = deviation /mean_14;
         */
        
        crosstalkCoefficient.push_back(b1);
        crosstalkCoefficient.push_back(b2);
        
        streamlog_out ( MESSAGE5 ) << "Chip "<<ichip<<" b1 "<<b1<<" b2 "<<b2 << endl;
        _crosstalkCoefficients[ichip].push_back(b1);
        _crosstalkCoefficients[ichip].push_back(b2);
        
        AlibavaPedNoiCalIOManager man;
        man.addToFile(_databaseFile,_crosstalkCollectionName, ichip, crosstalkCoefficient);
        
    }
    
}


void AlibavaChipHeaderCrossTalkProcessor::bookHistos(){
    
    string tmp_string;
    stringstream tempHistoTitle;
    string tempHistoName,tempFitName;
    
    string calculationFolder, datafolder;
    calculationFolder = _crosstalkCollectionName;
    datafolder = calculationFolder+string("/Data");
    
    AIDAProcessor::tree(this)->mkdir(calculationFolder.c_str());
    AIDAProcessor::tree(this)->mkdir(datafolder.c_str());
    EVENT::IntVec chipSelection = getChipSelection();
    
    TH1D * histo1D;
    TH2D * histo2D;
    histo1D = new TH1D ("histo1D","",1000,0,1000);
    histo2D= new TH2D ("histo2D","",1000,0,1000,1000,0,1000);
    
    for (unsigned int i=0; i<chipSelection.size(); i++) {
        unsigned int ichip=chipSelection[i];
        AIDAProcessor::tree(this)->cd(datafolder.c_str());
        /here
        
        // plot channel histogram
        for (unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size(); ichan++) {
            
            // histogram of channels
            tempHistoTitle.clear();
            tempHistoTitle.str( std::string() );
            
            tempHistoName = getChanHistoName(ichip,_channelsUsedForCrosstalkCalculation[ichan]);
            tempHistoTitle<<tempHistoName<<";ADCs;Number of Entries";
            
            TH1D * chanDataHisto = (TH1D*) histo1D->Clone(tempHistoName.c_str());
            _rootObjectMap.insert(make_pair(tempHistoName, chanDataHisto));
            tmp_string = tempHistoTitle.str();
            chanDataHisto->SetTitle(tmp_string.c_str());
        }
        AIDAProcessor::tree(this)->cd(calculationFolder.c_str());
        // plot correlations histogram
        for (unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size()-1; ichan++) {
            
            tempHistoName = getChanCorrelationHistoName(ichip,_channelsUsedForCrosstalkCalculation[ichan],_channelsUsedForCrosstalkCalculation[2]);
            tempHistoTitle.clear();
            tempHistoTitle.str( std::string() );
            tempHistoTitle<<tempHistoName<<";Channel "<<_channelsUsedForCrosstalkCalculation[ichan]<<" (ADCs);Channel "<<_channelsUsedForCrosstalkCalculation[2]<<" (ADCs)";
            
            TH2D * chanCorrelationHisto = (TH2D*)histo2D->Clone(tempHistoName.c_str());
            _rootObjectMap.insert(make_pair(tempHistoName, chanCorrelationHisto));
            tmp_string = tempHistoTitle.str();
            chanCorrelationHisto->SetTitle(tmp_string.c_str());
            
        }
        
        
        AIDAProcessor::tree(this)->cd(calculationFolder.c_str());
        string chanvalue;
        for (int inextnext=0; inextnext<2; inextnext++) {
            for (int inext=0; inext<2; inext++) {
                
                if (inextnext==0 && inext==0)
                    chanvalue = string("LL");
                if (inextnext==1 && inext==0)
                    chanvalue = string("HL");
                if (inextnext==0 && inext==1)
                    chanvalue = string("LH");
                if (inextnext==1 && inext==1)
                    chanvalue = string("HH");
                
                
                for (unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size()-1; ichan++) {
                    
                    
                    tempHistoName = getChanCorrelationHistoName(ichip,_channelsUsedForCrosstalkCalculation[ichan],_channelsUsedForCrosstalkCalculation[2]);
                    tempHistoName+=chanvalue;
                    
                    tempHistoTitle.clear();
                    tempHistoTitle.str( std::string() );
                    tempHistoTitle<<tempHistoName<<";Channel "<<_channelsUsedForCrosstalkCalculation[ichan]<<" (ADCs);Channel "<<_channelsUsedForCrosstalkCalculation[2]<<" (ADCs)";
                    
                    TH2D * chanCorrelationHisto = (TH2D*)histo2D->Clone(tempHistoName.c_str());
                    _rootObjectMap.insert(make_pair(tempHistoName, chanCorrelationHisto));
                    tmp_string = tempHistoTitle.str();
                    chanCorrelationHisto->SetTitle(tmp_string.c_str());
                }
            }
        }
        
        
        
    } // end of chip loop
    
    delete histo1D;
    delete histo2D;
    
    streamlog_out ( MESSAGE1 )  << "End of Booking histograms. " << endl;
}

string AlibavaChipHeaderCrossTalkProcessor::getChanHistoName(unsigned int ichip, unsigned int ichan){
    stringstream s;
    string tempHistoName;
    
    tempHistoName = _chanDataHistoName;
    s<< tempHistoName <<"_chip"<<ichip<<"_chan" << ichan;
    return s.str();
}

string AlibavaChipHeaderCrossTalkProcessor::getChanCorrelationHistoName(unsigned int ichip, unsigned int ichan, unsigned int iichan){
    stringstream s;
    string tempHistoName;
    
    tempHistoName = _chanDataCorrelationHistoName;
    s<< tempHistoName<<"_chip"<<ichip<<"_chan" << ichan<<"vs" << iichan;
    return s.str();
}





