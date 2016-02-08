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
_inputDataCollectionName("rawdata"),
_outputDataCollectionName("rawdata_crosstalkcorrected"),
_chanDataHistoName("ChipHeaderData"),
_chanDataCorrelationHistoName("Data_Correlation"),
_channelsUsedForCrosstalkCalculation(),
_nIteration(-1)
{
    
    // modify processor description
    _description =
    "AlibavaChipHeaderCrossTalkProcessor calculates the cross talk correction values using chip header information";
    
    
    // first of register the input collection
    registerInputCollection (LCIO::TRACKERDATA, "InputChipHeaderCollectionName",
                             "Input Chip header collection name, its last channel will be used to calculate crosstalk",
                             _inputCollectionName, string("chipheader") );
    
    // Output collection name
    registerOutputCollection (LCIO::TRACKERDATA, "OutputChipHeaderCollectionName",
                              "Output chip header collection name which will be cross talk corrected",
                              _outputCollectionName, string("chipheader_crosstalkcorrected") );
 
    // Output collection name
    registerProcessorParameter ("OutputDataCollectionName",
                              "Output data collection name which will be cross talk corrected",
                              _outputDataCollectionName, string("rawdata_crosstalkcorrected") );
 
    // The collection name of data
    registerProcessorParameter ("InputDataCollectionName",
                                "Input data collaction name, its first channel will be used to calculate crosstalk. It should be raw data (without pedestal subtraction)",
                                _inputDataCollectionName, string ("rawdata"));
    
    // number of iterations
    registerProcessorParameter ("NumberOfIteratins",
                                "The number of iterations to calculate crosstalk",
                                _maxNIteration,int (3));
    // number of iterations
    registerProcessorParameter ("MaxNumberOfEventsToCalculateCrossTalk",
                                "The number of events that will be used to calculate crosstalk. If set to zero, it will use all events",
                                _maxNEvent,int (0));
    
}


void AlibavaChipHeaderCrossTalkProcessor::init () {
    streamlog_out ( MESSAGE4 ) << "Running init" << endl;
    // this method is called only once even when the rewind is active
    
    /* To choose if processor should skip masked events
     ex. Set the value to 0 for false, to 1 for true
     */
    if (Global::parameters->isParameterSet(ALIBAVA::SKIPMASKEDEVENTS))
        _skipMaskedEvents = bool ( Global::parameters->getIntVal(ALIBAVA::SKIPMASKEDEVENTS) );
    else {
        streamlog_out ( MESSAGE4 ) << "The Global Parameter "<< ALIBAVA::SKIPMASKEDEVENTS <<" is not set! Masked events will be used!" << endl;
    }

    
    // usually a good idea to
    printParameters ();
    _nIteration = 0;
	_maxNIteration++;
}

void AlibavaChipHeaderCrossTalkProcessor::processRunHeader (LCRunHeader * rdr) {
    streamlog_out ( MESSAGE4 ) << "Running processRunHeader" << endl;
    
    // Add processor name to the runheader
    auto_ptr<AlibavaRunHeaderImpl> arunHeader ( new AlibavaRunHeaderImpl(rdr)) ;
    arunHeader->addProcessor(type());
    
    // get and set selected chips
    setChipSelection(arunHeader->getChipSelection());
    
    _nIteration = 0;
    streamlog_out( MESSAGE5 ) << "Iteration "<<_nIteration<< endl;
    
    
    _channelsUsedForCrosstalkCalculation.push_back(ALIBAVA::CHIPHEADERLENGTH-2);
    _channelsUsedForCrosstalkCalculation.push_back(ALIBAVA::CHIPHEADERLENGTH-1);
    _channelsUsedForCrosstalkCalculation.push_back(ALIBAVA::CHIPHEADERLENGTH+0);
    
    // set all pedestal values to zero
    for (int ichip=0; ichip<ALIBAVA::NOOFCHIPS; ichip++) {
        for (unsigned int ichan=0; ichan<ALIBAVA::CHIPHEADERLENGTH+ALIBAVA::NOOFCHANNELS; ichan++) {
            _pedestal[ichip][ichan]=0.0;
        }
    }
    
    // set all crosstalk coefficient values to zero
    for (int ichip=0; ichip<ALIBAVA::NOOFCHIPS; ichip++) {
    	for (int i=0; i<2; i++) {
        	_crosstalkCoefficient[ichip][i]=0.0;
	}
    }

    while (_nIteration<_maxNIteration) {
        runThroughEvents();
        if(_nIteration!=0) calculateCrossTalk();
        storePedestals();
        _nIteration++;
    }
    
    
}

void AlibavaChipHeaderCrossTalkProcessor::runThroughEvents(){
    streamlog_out ( MESSAGE5 ) << "Running through events, iteration " << _nIteration << endl ;
    
    bookHistos();
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
        
        LCCollectionVec * chipeaderColVec;
        LCCollectionVec * dataColVec;
        unsigned int noOfChip;
        try
        {
            chipeaderColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( getInputCollectionName() ) ) ;
            dataColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( _inputDataCollectionName ) ) ;
            
            noOfChip = chipeaderColVec->getNumberOfElements();
            
    LCCollectionVec * newChipHeaderColVec= new LCCollectionVec(LCIO::TRACKERDATA);
    LCCollectionVec* newDataColVec = new LCCollectionVec(LCIO::TRACKERDATA);
               CellIDEncoder<TrackerDataImpl> chipIDEncoder(ALIBAVA::ALIBAVADATA_ENCODE,newDataColVec);
            
            for ( size_t i = 0; i < noOfChip; ++i )
            {
                TrackerDataImpl * data = dynamic_cast< TrackerDataImpl * > ( dataColVec->getElementAt( i ) ) ;
                
                TrackerDataImpl * chipheader = dynamic_cast< TrackerDataImpl * > ( chipeaderColVec->getElementAt( i ) ) ;
                
                
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
                vector<float> allchannels;
                allchannels.clear();
                for (unsigned int i=0; i<chipheadervec.size(); i++)
                    allchannels.push_back(chipheadervec[i]);
                
                for (unsigned int i=0; i<datavec.size(); i++)
                    allchannels.push_back(datavec[i]);
                
                if (_nIteration!=0) {
                    for (unsigned int ichan=0; ichan<allchannels.size(); ichan++) {
                        allchannels[ichan] = allchannels[ichan]-_pedestal[chipnum][ichan];
                    }
                }
                
                if (_nIteration>1){
                    allchannels = correctCrossTalk(chipnum, allchannels);
                }
		if (_nIteration==_maxNIteration){ 
	            FloatVec newdatavec;
        	    newdatavec.clear();
	            FloatVec newchipheadervec;
	            newchipheadervec.clear();
            
	            for (int i=0; i<ALIBAVA::CHIPHEADERLENGTH+ALIBAVA::NOOFCHANNELS;i++){
	                if(i<ALIBAVA::CHIPHEADERLENGTH){
	                    newchipheadervec.push_back(allchannels[i]+_pedestal[chipnum][i]);
	                }else{
	                    newdatavec.push_back(allchannels[i]+_pedestal[chipnum][i]);
	                }
	            }
  
 
	            TrackerDataImpl * newDataImpl = new TrackerDataImpl();
	            newDataImpl->setChargeValues(newdatavec);
	            chipIDEncoder[ALIBAVA::ALIBAVADATA_ENCODE_CHIPNUM] = chipnum;
	            chipIDEncoder.setCellID(newDataImpl);
	            newDataColVec->push_back(newDataImpl);
            
	            TrackerDataImpl * newChipHeaderImpl = new TrackerDataImpl();
	            newChipHeaderImpl->setChargeValues(newchipheadervec);
	            newChipHeaderColVec->push_back(newChipHeaderImpl);
            
		}
                fillHistos(chipnum, allchannels);
		
            }
	        alibavaEvent->addCollection(newChipHeaderColVec, getOutputCollectionName());
	        alibavaEvent->addCollection(newDataColVec, _outputDataCollectionName);
            

        } catch ( lcio::DataNotAvailableException ) {
            // do nothing again
            streamlog_out( ERROR5 ) << "Collection not found! " << endl;
        }
        
    }// end of loop over events
    lcReader->close() ;
}
void AlibavaChipHeaderCrossTalkProcessor::processEvent (LCEvent * anEvent) {
    // here we apply the cross talk correction to all data
    
    AlibavaEventImpl * alibavaEvent = static_cast<AlibavaEventImpl*> (anEvent);

    if (_skipMaskedEvents && (alibavaEvent->isEventMasked()) ) {
        _numberOfSkippedEvents++;
        return;
    }

    LCCollectionVec * chipeaderColVec;
    LCCollectionVec * dataColVec;

    LCCollectionVec * newChipHeaderColVec= new LCCollectionVec(LCIO::TRACKERDATA);
    LCCollectionVec* newDataColVec = new LCCollectionVec(LCIO::TRACKERDATA);
    CellIDEncoder<TrackerDataImpl> chipIDEncoder(ALIBAVA::ALIBAVADATA_ENCODE,newDataColVec);

    unsigned int noOfChip;
    try
    {
        chipeaderColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( getInputCollectionName() ) ) ;
        dataColVec = dynamic_cast< LCCollectionVec * > ( alibavaEvent->getCollection( _inputDataCollectionName ) ) ;
        
        noOfChip = chipeaderColVec->getNumberOfElements();
        
        
        for ( size_t i = 0; i < noOfChip; ++i )
        {
            TrackerDataImpl * data = dynamic_cast< TrackerDataImpl * > ( dataColVec->getElementAt( i ) ) ;
            
            TrackerDataImpl * chipheader = dynamic_cast< TrackerDataImpl * > ( chipeaderColVec->getElementAt( i ) ) ;
            
            
            int chipnum = getChipNum(chipheader);
            
            if (chipnum != getChipNum(data)) {
                streamlog_out(ERROR5)<< "Chipheader chipnum and data chip number doesn't match. This is important fix it!"<<endl;
            }
            
            FloatVec datavec;
            datavec = data->getChargeValues();
            
            FloatVec chipheadervec;
            chipheadervec = chipheader->getChargeValues();
            
            // ok, chipheadervec and datavec are constant vectors so we cannot modify them
            // lets copy them to another vector and subtract pedestals
            vector<float> allchannels;
            allchannels.clear();
            for (unsigned int i=0; i<chipheadervec.size(); i++)
                allchannels.push_back(chipheadervec[i] - _pedestal[chipnum][i]);
            
            for (unsigned int i=0; i<datavec.size(); i++)
                allchannels.push_back(datavec[i] - _pedestal[chipnum][ALIBAVA::CHIPHEADERLENGTH+i]);

            allchannels = correctCrossTalk(chipnum, allchannels);

            FloatVec newdatavec;
            newdatavec.clear();
            FloatVec newchipheadervec;
            newchipheadervec.clear();
            
            for (int i=0; i<ALIBAVA::CHIPHEADERLENGTH+ALIBAVA::NOOFCHANNELS;i++){
                if(i<ALIBAVA::CHIPHEADERLENGTH){
                    newchipheadervec.push_back(allchannels[i]+_pedestal[chipnum][i]);
                }else{
                    newdatavec.push_back(allchannels[i]+_pedestal[chipnum][i]);
                }
            }
  
            
            TrackerDataImpl * newDataImpl = new TrackerDataImpl();
            newDataImpl->setChargeValues(newdatavec);
            chipIDEncoder[ALIBAVA::ALIBAVADATA_ENCODE_CHIPNUM] = chipnum;
            chipIDEncoder.setCellID(newDataImpl);
            newDataColVec->push_back(newDataImpl);
            
            TrackerDataImpl * newChipHeaderImpl = new TrackerDataImpl();
            newChipHeaderImpl->setChargeValues(newchipheadervec);
            newChipHeaderColVec->push_back(newChipHeaderImpl);
            
        }
        alibavaEvent->addCollection(newChipHeaderColVec, getOutputCollectionName());
        alibavaEvent->addCollection(newDataColVec, _outputDataCollectionName);
        
        
    } catch ( lcio::DataNotAvailableException ) {
        // do nothing again
        streamlog_out( ERROR5 ) << "Collection ("<<getInputCollectionName()<<") not found! " << endl;
    }

    
}


void AlibavaChipHeaderCrossTalkProcessor::fillHistos(int chipnum, vector<float> alldata){
    
    string tempHistoName;
    int nextnextchan = _channelsUsedForCrosstalkCalculation[0];
    int nextchan = _channelsUsedForCrosstalkCalculation[1];
    int datachan = _channelsUsedForCrosstalkCalculation[2];
    
    vector<float> channelValuessUsedForCrosstalkCalculation;
    channelValuessUsedForCrosstalkCalculation.clear();
    channelValuessUsedForCrosstalkCalculation.push_back(alldata[nextnextchan]);
    channelValuessUsedForCrosstalkCalculation.push_back(alldata[nextchan]);
    channelValuessUsedForCrosstalkCalculation.push_back(alldata[datachan]);
    
    for (unsigned int ichan = 0 ; ichan<alldata.size(); ichan++) {
        tempHistoName = getChanHistoName(chipnum,ichan);
        if ( TH1D * histo = dynamic_cast<TH1D*> (_rootObjectMap[tempHistoName]) )
            histo->Fill(alldata[ichan]);
    }
    
    for ( unsigned int ichan=0; ichan<_channelsUsedForCrosstalkCalculation.size()-1; ichan++) {
        tempHistoName = getChanCorrelationHistoName(chipnum,_channelsUsedForCrosstalkCalculation[ichan],_channelsUsedForCrosstalkCalculation[2]);
        if ( TH2D * histo2 = dynamic_cast<TH2D*> (_rootObjectMap[tempHistoName]) )
            histo2->Fill(channelValuessUsedForCrosstalkCalculation[ichan],channelValuessUsedForCrosstalkCalculation[2]);
    }
    
    if (_nIteration!=0) {
        string chanvalue;
        if (channelValuessUsedForCrosstalkCalculation[0]<0) { // next next channel is low
            if (channelValuessUsedForCrosstalkCalculation[1]<0) {// next channel is low
                chanvalue = string("LL");
            }
            else {// next channel is high
                chanvalue = string("LH");
            }
        }else { // next next channel is high
            if (channelValuessUsedForCrosstalkCalculation[1]<0) {// next channel is low
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
                histo2->Fill(channelValuessUsedForCrosstalkCalculation[ichan],channelValuessUsedForCrosstalkCalculation[2]);
            
        }
    }// end of if
    
}

void AlibavaChipHeaderCrossTalkProcessor::bookHistos(){
    
    string tmp_string;
    stringstream tempHistoTitle;
    string tempHistoName,tempFitName;
    
    string iterationFolder, datafolder;
    iterationFolder = getInputCollectionName()+string("_it")+to_string(_nIteration);
    datafolder = iterationFolder+string("/Data");
    
    AIDAProcessor::tree(this)->mkdir(iterationFolder.c_str());
    AIDAProcessor::tree(this)->mkdir(datafolder.c_str());
    EVENT::IntVec chipSelection = getChipSelection();
    
    TH1D * histo1D;
    TH2D * histo2D;
    if (_nIteration==0) {
        histo1D = new TH1D ("histo1D","",1000,0,1000);
        histo2D= new TH2D ("histo2D","",1000,0,1000,1000,0,1000);
    }
    else{
        histo1D = new TH1D ("histo1D","",1000,-500,500);
        histo2D= new TH2D ("histo2D","",1000,-500,500,1000,-500,500);
    }
    
    for (unsigned int i=0; i<chipSelection.size(); i++) {
        unsigned int ichip=chipSelection[i];
        AIDAProcessor::tree(this)->cd(datafolder.c_str());
        
        // plot channel histogram
        for (unsigned int ichan=0; ichan<ALIBAVA::CHIPHEADERLENGTH+ALIBAVA::NOOFCHANNELS; ichan++) {
            
            // histogram of channels
            tempHistoTitle.clear();
            tempHistoTitle.str( std::string() );
            
            tempHistoName = getChanHistoName(ichip,ichan);
            tempHistoTitle<<tempHistoName<<";ADCs;NumberofEntries";
            
            TH1D * chanDataHisto = (TH1D*) histo1D->Clone(tempHistoName.c_str());
            _rootObjectMap.insert(make_pair(tempHistoName, chanDataHisto));
            tmp_string = tempHistoTitle.str();
            chanDataHisto->SetTitle(tmp_string.c_str());
        }
        AIDAProcessor::tree(this)->cd(iterationFolder.c_str());
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
        
        
        if (_nIteration!=0) {
            AIDAProcessor::tree(this)->cd(iterationFolder.c_str());
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
        } // end of if
        
        
        
    } // end of chip loop
    
    delete histo1D;
    delete histo2D;
    
    streamlog_out ( MESSAGE1 )  << "End of Booking histograms. " << endl;
}



void AlibavaChipHeaderCrossTalkProcessor::check (LCEvent * /* evt */ ) {
    // nothing to check here - could be used to fill check plots in reconstruction processor
}

void AlibavaChipHeaderCrossTalkProcessor::fillHistos(TrackerDataImpl * /*trkdata */){
}

void AlibavaChipHeaderCrossTalkProcessor::end() {
    
    streamlog_out ( MESSAGE4 ) << "Successfully finished" << endl;
    
}

string AlibavaChipHeaderCrossTalkProcessor::getChanHistoName(unsigned int ichip, unsigned int ichan){
    stringstream s;
    string tempHistoName;
    
    tempHistoName = _chanDataHistoName + string("_it") + to_string(_nIteration);
    s<< tempHistoName <<"_chip"<<ichip<<"_chan" << ichan;
    return s.str();
}

string AlibavaChipHeaderCrossTalkProcessor::getChanCorrelationHistoName(unsigned int ichip, unsigned int ichan, unsigned int iichan){
    stringstream s;
    string tempHistoName;
    
    tempHistoName = _chanDataCorrelationHistoName + string("_it") + to_string(_nIteration);
    s<< tempHistoName<<"_chip"<<ichip<<"_chan" << ichan<<"vs" << iichan;
    return s.str();
}

void AlibavaChipHeaderCrossTalkProcessor::storePedestals(){
    
    EVENT::IntVec chipSelection = getChipSelection();
    string histoname;
    
    for (unsigned int ichip=0; ichip<chipSelection.size(); ichip++) {
        int chipnum = chipSelection[ichip];
        
        for (unsigned int ichan=0; ichan<ALIBAVA::CHIPHEADERLENGTH+ALIBAVA::NOOFCHANNELS; ichan++) {
            string histoname = getChanHistoName( chipnum, ichan);
            TH1D * histo = dynamic_cast<TH1D*> (_rootObjectMap[histoname]);
            _pedestal[chipnum][ichan] += histo->GetMean();
            
        }// end of ichan loop
    }// end of ichip loop
}




vector<float> AlibavaChipHeaderCrossTalkProcessor::correctCrossTalk(int chipnum, vector<float> allchannels){
   
     double b1 = _crosstalkCoefficient[chipnum][0];
     double b2 = _crosstalkCoefficient[chipnum][1];

/*
     for (unsigned int ichan=allchannels.size()-1; ichan>1;ichan--) {
     // y[n] = x[n] - b1*x[n-1]-b2*x[n-2]
     // there will be no corrections for channel 0
     // which corresponds to first channel of chip header. These are not used anywhere
     
         allchannels[ichan]= allchannels[ichan]-b1*allchannels[ichan-1]-b2*allchannels[ichan-2];
         allchannels[ichan-1]= allchannels[ichan-1]+b1*allchannels[ichan-1];
	allchannels[ichan-2]=allchannels[ichan-2]+b2*allchannels[ichan-1];
}
  */       

     for (unsigned int ichan=allchannels.size()-1; ichan>0;ichan--) {
 // y[n] = x[n] - b1*x[n-1]
     // there will be no corrections for channel 0
     // which corresponds to first channel of chip header. These are not used anywhere
     
         allchannels[ichan]= allchannels[ichan]-b1*allchannels[ichan-1];
         allchannels[ichan-1]= allchannels[ichan-1]+b1*allchannels[ichan-1];
     }
    
    return allchannels;
}




void AlibavaChipHeaderCrossTalkProcessor::calculateCrossTalk(){
    
    streamlog_out ( MESSAGE5 ) << "Calculating Cross Talk, iteration " << _nIteration << endl ;
    EVENT::IntVec chipSelection = getChipSelection();
    string tempHistoName;
    for (unsigned int ichip=0; ichip<chipSelection.size(); ichip++) {
        int chipnum = chipSelection[ichip];
        
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
        
        _crosstalkCoefficient[chipnum][0] = b1;
        _crosstalkCoefficient[chipnum][1] = b2;
        
        
        streamlog_out( MESSAGE5 ) <<"Iteration "<< _nIteration << " chip"<<chipnum<<" b1 = "<<b1 <<" b2 = "<< b2 << endl;
        
    }
    
}






