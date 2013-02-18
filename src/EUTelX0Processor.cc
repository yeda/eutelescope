// Version: $Id$
#include "EUTelX0Processor.h"
#include <cmath>
using namespace marlin;
using namespace eutelescope;
using namespace std;

std::string EUTelX0Processor::_histoResidualX = "ResidualX";
std::string EUTelX0Processor::_histoResidualXPlane1 = "ResidualXPlane1";
std::string EUTelX0Processor::_histoResidualXPlane2 = "ResidualXPlane2";
std::string EUTelX0Processor::_histoResidualXPlane3 = "ResidualXPlane3";
std::string EUTelX0Processor::_histoResidualXPlane4 = "ResidualXPlane4";
std::string EUTelX0Processor::_histoResidualYPlane1 = "ResidualYPlane1";
std::string EUTelX0Processor::_histoResidualYPlane2 = "ResidualYPlane2";
std::string EUTelX0Processor::_histoResidualYPlane3 = "ResidualYPlane3";
std::string EUTelX0Processor::_histoResidualYPlane4 = "ResidualYPlane4";
std::string EUTelX0Processor::_histoResidualY = "ResidualY";
std::string EUTelX0Processor::_histoResidualXZ = "ResidualXZ";
std::string EUTelX0Processor::_histoResidualYZ = "ResidualYZ";
std::string EUTelX0Processor::_histoResidualXY = "ResidualYZ";

EUTelX0Processor::EUTelX0Processor()
  :Processor("EUTelX0Processor"),
  _trackColName(""),
  _cutValue1(0.0),
  _cutValue2(0.0),
  _debug(false),
  _debugCount(0),
  _eventNumber(0),
  _histoData(),
  _histoThing(),
  _hitInfo(),
  _inputHitCollectionVec(NULL), 
  _inputTrackCollectionVec(NULL), 
  _inputHitColName(""),
  _inputHitCollectionName(""),
  _inputTrackColName(""),
  _projectedHits(),
  _referenceHitCollectionName(""),
  _referenceHitVec(NULL),
  _residual(),
  _residualAngle(),
  _residualProfile(),
  _runNumber(0),
  _trackCollectionName("")
{
  streamlog_out(DEBUG1) << "Constructing the EUTelX0Processor, setting all values to zero or NULL" << std::endl;
}

void EUTelX0Processor::init()
{
  streamlog_out(DEBUG1) << "Running EUTelX0Processor::init()" << std::endl;
  _debug = false; 
  int nobins = 100, nobinsangle = 100;//Number of bins in the histograms
  double minbin = -0.4, maxbin = 0.4;//Maximum and minimum bin values
  double minbinangle = -25, maxbinangle = 25, minbinalpha = -5, maxbinalpha = 5;
  std::vector<double> empty;  
  
  AIDA::IHistogram1D * ResidualX = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualX",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualX->setTitle("ResidualX");
  _histoThing.insert(make_pair("ResidualX",ResidualX));
  _histoData["ResidualX"] = empty;
 
  AIDA::IHistogram1D * ResidualXPlane1 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualXPlane1",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualXPlane1->setTitle("ResidualXPlane1");
  _histoThing.insert(make_pair("ResidualXPlane1",ResidualXPlane1));
  _histoData["ResidualXPlane1"] = empty;
 
  AIDA::IHistogram1D * ResidualXPlane2 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualXPlane2",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualXPlane2->setTitle("ResidualXPlane2");
  _histoThing.insert(make_pair("ResidualXPlane2",ResidualXPlane2));
  _histoData["ResidualXPlane2"] = empty;
 
  AIDA::IHistogram1D * ResidualXPlane3 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualXPlane3",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualXPlane3->setTitle("ResidualXPlane3");
  _histoThing.insert(make_pair("ResidualXPlane3",ResidualXPlane3));
  _histoData["ResidualXPlane3"] = empty;
 
  AIDA::IHistogram1D * ResidualXPlane4 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualXPlane4",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualXPlane4->setTitle("ResidualXPlane4");
  _histoThing.insert(make_pair("ResidualXPlane4",ResidualXPlane4));
  _histoData["ResidualXPlane4"] = empty;
 
  AIDA::IHistogram1D * ResidualYPlane1 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualYPlane1",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualYPlane1->setTitle("ResidualYPlane1");
  _histoThing.insert(make_pair("ResidualYPlane1",ResidualYPlane1));
  _histoData["ResidualYPlane1"] = empty;
 
  AIDA::IHistogram1D * ResidualYPlane2 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualYPlane2",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualYPlane2->setTitle("ResidualYPlane2");
  _histoThing.insert(make_pair("ResidualYPlane2",ResidualYPlane2));
  _histoData["ResidualYPlane2"] = empty;
 
  AIDA::IHistogram1D * ResidualYPlane3 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualYPlane3",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualYPlane3->setTitle("ResidualYPlane3");
  _histoThing.insert(make_pair("ResidualYPlane3",ResidualYPlane3));
  _histoData["ResidualYPlane3"] = empty;
 
  AIDA::IHistogram1D * ResidualYPlane4 = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualYPlane4",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualYPlane4->setTitle("ResidualYPlane4");
  _histoThing.insert(make_pair("ResidualYPlane4",ResidualYPlane4));
  _histoData["ResidualYPlane4"] = empty;
  
  AIDA::IHistogram1D * ResidualY = AIDAProcessor::histogramFactory(this)->createHistogram1D("ResidualY",nobins,minbin,maxbin);//Create a histogram for the residual
  ResidualY->setTitle("ResidualY");
  _histoThing.insert(make_pair("ResidualY",ResidualY));
  _histoData["ResidualY"] = empty;
 
  AIDA::IHistogram1D * ThetaFront = AIDAProcessor::histogramFactory(this)->createHistogram1D("ThetaFront",nobinsangle,minbinangle,maxbinangle);//Create a histogram for the residual
  ThetaFront->setTitle("ThetaFront");
  _histoThing.insert(make_pair("ThetaFront",ThetaFront));
  _histoData["ThetaFront"] = empty;
 
  
  AIDA::IHistogram1D * PhiFront = AIDAProcessor::histogramFactory(this)->createHistogram1D("PhiFront",nobinsangle,minbinangle,maxbinangle);//Create a histogram for the residual
  PhiFront->setTitle("PhiFront");
  _histoThing.insert(make_pair("PhiFront",PhiFront));
  _histoData["PhiFront"] = empty;
 
  
  AIDA::IHistogram1D * AlphaFront = AIDAProcessor::histogramFactory(this)->createHistogram1D("AlphaFront",nobinsangle,minbinalpha,maxbinalpha);//Create a histogram for the residual
  AlphaFront->setTitle("AlphaFront");
  _histoThing.insert(make_pair("AlphaFront",AlphaFront));
  _histoData["AlphaFront"] = empty;
 
  
  AIDA::IHistogram1D * Theta432 = AIDAProcessor::histogramFactory(this)->createHistogram1D("Theta432",nobinsangle,minbinangle,maxbinangle);//Create a histogram for the residual
  Theta432->setTitle("Theta432");
  _histoThing.insert(make_pair("Theta432",Theta432));
  _histoData["Theta432"] = empty;
 
  
  AIDA::IHistogram1D * Phi432 = AIDAProcessor::histogramFactory(this)->createHistogram1D("Phi432",nobinsangle,minbinangle,maxbinangle);//Create a histogram for the residual
  Phi432->setTitle("Phi432");
  _histoThing.insert(make_pair("Phi432",Phi432));
  _histoData["Phi432"] = empty;
 
  
  AIDA::IHistogram1D * Alpha432 = AIDAProcessor::histogramFactory(this)->createHistogram1D("Alpha432",nobinsangle,minbinalpha,maxbinalpha);//Create a histogram for the residual
  Alpha432->setTitle("Alpha432");
  _histoThing.insert(make_pair("Alpha432",Alpha432));
  _histoData["Alpha432"] = empty;
 

  _inputHitCollectionVec = new LCCollectionVec(LCIO::TRACKERHIT);//Used to store the values of the hit events
  _inputTrackCollectionVec = new LCCollectionVec(LCIO::TRACK);//Used to store the values of the hit events
  // readin parameter for input hit collection name (default alignedHit)
  //registerInputCollection (LCIO::TRACKERHIT, "InputHitCollectionName", "The name of the input hit collection", _inputHitCollectionName, string("correctedHit"));//Used to store the values of the hit events
  registerInputCollection(LCIO::TRACKERHIT,"InputTrackCollectionName",
                           "Collection name for corrected particle positions",
                           _trackColName, string ("alignedHit"));

  
  registerInputCollection(LCIO::TRACK,"OutputTrackCollectionName",
                           "Collection name for fitted tracks",
                           _inputTrackColName, string ("track"));

  registerOptionalParameter("ReferenceCollection","This is the name of the reference it collection (init at 0,0,0)", _referenceHitCollectionName, static_cast< string > ( "referenceHit" ) );//Necessary for working out which layer the particle is detected in
  registerProcessorParameter("CutValue","Used to determine cuts in the system, measured in XXX", _cutValue1, static_cast< double > (50000.0));
}

void EUTelX0Processor::processRunHeader(LCRunHeader *run)
{
  streamlog_out(DEBUG1) << "Running EUTelX0Processor::processRunHeader(LCRunHeader *run) with run = " << run << std::endl;
  run++;
  _runNumber++;
}

void EUTelX0Processor::printTrackParameters( Track* eventtrack ){
    streamlog_out(DEBUG0) << "Type: " << eventtrack->getType() << std::endl << std::endl
    << "***TRACK PARAMETERS***" << std::endl << "(Note: In EUTelTestFitter the track parameters are not yet instantiated, all currently set to zero)" << std::endl
    << "D0: " << eventtrack->getD0() << std::endl
    << "Omega: " << eventtrack->getOmega() << std::endl
    << "Phi: " << eventtrack->getPhi() << std::endl
    << "Z0: " << eventtrack->getZ0() << std::endl
    << "Tan Lambda: " << eventtrack->getTanLambda() << std::endl 
    << "Covariance Matrix of Track Parameters: (This is a square matrix with parameter order: D0, Phi, Omega, Z0, TanLambda)" << std::endl;
    std::vector< float > covmatrix = eventtrack->getCovMatrix();
    streamlog_out(DEBUG0) << covmatrix[0] << ", " << covmatrix[1] << ", " << covmatrix[3] << ", " << covmatrix[6] << ", " << covmatrix[10] << std::endl
                          << covmatrix[1] << ", " << covmatrix[2] << ", " << covmatrix[4] << ", " << covmatrix[7] << ", " << covmatrix[11] << std::endl
                          << covmatrix[3] << ", " << covmatrix[4] << ", " << covmatrix[5] << ", " << covmatrix[8] << ", " << covmatrix[12] << std::endl
                          << covmatrix[6] << ", " << covmatrix[7] << ", " << covmatrix[8] << ", " << covmatrix[9] << ", " << covmatrix[13] << std::endl
                          << covmatrix[10] << ", " << covmatrix[11] << ", " << covmatrix[12] << ", " << covmatrix[13] << ", " << covmatrix[14] << std::endl;    
    streamlog_out(DEBUG0) << std::endl
    << "***ADDITIONAL INFORMATION***" << std::endl
    << "Reference Point: " << eventtrack->getReferencePoint()[0] << ", " <<  eventtrack->getReferencePoint()[1] << ", " << eventtrack->getReferencePoint()[2] << std::endl
    << "Chi2: " << eventtrack->getChi2() << std::endl
    << "Degrees of Freedom: " << eventtrack->getNdf() << std::endl
    << "dE/dx: " << eventtrack->getdEdx() << std::endl
    << "dE/dx error: " << eventtrack->getdEdxError() << std::endl << std::endl;
  streamlog_out(DEBUG0) << std::endl << "***TRACK HITS***" << std::endl;
  std::vector< TrackerHit* > trackhits = eventtrack->getTrackerHits();
  int tracknumber(0);
  for(std::vector< TrackerHit* >::iterator it = trackhits.begin(); it != trackhits.end(); ++it){
    streamlog_out(DEBUG0) << "Track Hit " << tracknumber << ":" << std::endl;
    tracknumber++;
    printHitParameters(*it);
  }
}

void EUTelX0Processor::printHitParameters( TrackerHit* trackhit ){
  streamlog_out(DEBUG0) << "Type: " << trackhit->getType() << std::endl
    << "Time of hit (in nanoseconds): " << trackhit->getTime() << std::endl
    << "dE/dx (in GeV): " << trackhit->getdEdx() << std::endl
    << "Position (x, y, z and measured in mm): " << trackhit->getPosition()[0] << ", "
    << trackhit->getPosition()[1] << ", " << trackhit->getPosition()[2] << std::endl
    << "Covariance of the position: " << std::endl;
  std::vector< float > hitcov = trackhit->getCovMatrix();
  cout << "hitcov size = " << hitcov.size() << std::endl;  
}

double EUTelX0Processor::kinkEstimate(Track* track){

  return 0;
}

void EUTelX0Processor::processEvent(LCEvent *evt)
{
  streamlog_out(DEBUG0) << "Running EUTelX0Processor::processEvent(LCEvent *evt) with evt = " << evt << std::endl;
  //Take track from input parameter
  //Work out kink angle from track
  //Put kink angle into histogram
  //Check for last event
    //Fit gaussian to histogram
    //Extract sigma value from histogram
    //Deduce radiation length from sigma value - See Nadler and Fruwurth paper

  //THIS IS WHERE THE FUTURE CODE IS BEING DEVELOPED, PLEASE DO NOT TOUCH
  try{
    LCCollection* trackcollection = evt->getCollection(_trackCollectionName);
    Track* eventtrack = dynamic_cast< Track* >(trackcollection->getElementAt(_eventNumber));
    streamlog_out(DEBUG0) << "Here is all the information about the track in run " << _runNumber << ", event " << _eventNumber << std::endl << std::endl;
    printTrackParameters( eventtrack );
    double kinkangle = kinkEstimate(eventtrack);
    dynamic_cast< AIDA::IHistogram1D* >(_histoThing["Kink Angle"])->fill(kinkangle);
    
  } catch(std::exception &e){
    if(e.what() == "DataNotAvailableException"){
      streamlog_out(WARNING4) << "Could not get collection '" << _trackCollectionName << "' from event " << _eventNumber << ", Skipping Event" << std::endl;
    } else{
      streamlog_out(ERROR5) << "std::exception occured in EUTelX0Processor, here is some information which might help:" << std::endl
        << "Run Number: " << _runNumber << std::endl
        << "Event Number: " << _eventNumber << std::endl
        << "Throw occured somewhere within the try block which starts with the line: 'LCCollection* trackcollection = evt->getCollection(_trackCollectionName);'" << std::endl
        << "I hope that helps, if not then please submit a bug report or add more exception handling as appropriate, program will now exit" << std::endl;
      exit(1);
    }
  } catch(...){
    streamlog_out(ERROR9) << "Unknown exception occured in EUTelX0Processor, here is some information which might help:" << std::endl
    << "Run Number: " << _runNumber << std::endl
    << "Event Number: " << _eventNumber << std::endl
    << "Throw occured somewhere within the try block which starts with the line: 'LCCollection* trackcollection = evt->getCollection(_trackCollectionName);'" << std::endl
    << "I hope that helps, if not then please submit a bug report or add more exception handling as appropriate, program will now exit" << std::endl;
    exit(1);
  }
  
/*
*/

  try{
    std::vector< std::string > *eventCollectionNames = const_cast<std::vector< std::string >* >(evt->getCollectionNames());
    _referenceHitVec = dynamic_cast < LCCollectionVec * > (evt->getCollection(_referenceHitCollectionName));//Create the reference hit vector (used for figuring out what layer the hit is in)
    streamlog_out(DEBUG0) << "Event Collection Names:" << std::endl;
    for(std::vector< std::string >::iterator it = (*eventCollectionNames).begin(); it != (*eventCollectionNames).end(); ++it){
      streamlog_out(DEBUG0) << *it << " (" << evt->getCollection(*it)->getNumberOfElements() << " elements)" << std::endl;
    }
    LCCollection* col = evt->getCollection(_trackColName);//Create the collection of alignedHits for this event
    threePointResolution(col);
  }
  catch(...){//Not sure what argument I could pass to this catch statement yet
    streamlog_out(WARNING) << "Could not get collection from event, Skipping Event" << std::endl;//Not sure on this verbosity level
  }

  _eventNumber++;
  
}

void EUTelX0Processor::createResiduals(LCCollection *trackCollection){
  int sizeElement = trackCollection->getNumberOfElements();
  for(int i = 0; i < sizeElement; ++i){
    Track* trackElement = dynamic_cast< Track* >(trackCollection->getElementAt(i));
    const vector< TrackerHit* > hits = trackElement->getTrackerHits();
    int sizeHits = hits.size();
    map< int, TVector3 > vecHits;
    for(int j = 0; j < sizeHits; ++j){
      if(hits[j]->getType() >= 32){  //Insist on a fitted hit
        TVector3 temp(hits[j]->getPosition()[0],hits[j]->getPosition()[1],hits[j]->getPosition()[2]);
        int layer = guessSensorID(hits[j]->getPosition());
        vecHits[layer] = temp;
      }
    }
    //Now we have all the hits of a track, we have to work out what the residual of the track is
    //Start with the forward residuals (TODO: (Phillip Hamnett) add the backward residuals too at some point)
    for(int j = 0; j < _noLayers; ++j){
      double dxf,dyf,dzf;
      if(j == 0){
        dxf = vecHits[j+1].x() - vecHits[j].x();
        dyf = vecHits[j+1].y() - vecHits[j].y();
        dzf = vecHits[j+1].z() - vecHits[j].z();
      }
      else if(j == 5){
        dxf = dyf = dzf = 0;
      }
      else{
        dxf = vecHits[j+1].x() - vecHits[j].x();
        dyf = vecHits[j+1].y() - vecHits[j].y();
        dzf = vecHits[j+1].z() - vecHits[j].z();
      }
      if(dxf != 0 || dyf != 0 || dzf != 0){
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualX"]))->fill(dxf/dzf);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualY"]))->fill(dyf/dzf);
//        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing[_histoResidualXY]))->fill(dxf/dyf);
      }
    }
  }
}

void EUTelX0Processor::testtrack(LCCollection *trackCollection){
  if(_debug){
    streamlog_out( DEBUG5 ) << "trackCollection contains " << trackCollection->getNumberOfElements() << " elements" << std::endl;
    streamlog_out( DEBUG5 ) << "The elements are as follows: " << std::endl;
    for(int i = 0; i < trackCollection->getNumberOfElements(); ++i){
      Track* trackElement = dynamic_cast< Track* >(trackCollection->getElementAt(i));
      streamlog_out( DEBUG5 ) << "Element " << i << " contains the following values:" << std::endl;
      streamlog_out( DEBUG5 ) << "D0 = " << trackElement->getD0() << std::endl
           << "Omega = " << trackElement->getOmega() << std::endl
           << "Phi = " << trackElement->getPhi() << std::endl
           << "Z0 = " << trackElement->getZ0() << std::endl
           << "Tan Lambda = " << trackElement->getTanLambda() << std::endl
           << "Reference Point = " << trackElement->getReferencePoint()[0] << ", " <<  trackElement->getReferencePoint()[1] << ", " << trackElement->getReferencePoint()[2] << std::endl
           << "Type = " << trackElement->getType() << std::endl
           << "Chi2 = " << trackElement->getChi2() << std::endl 
           << "Degrees of Freedom = " << trackElement->getNdf() << std::endl 
           << "The covariance matrix is as follows:" << std::endl;
      const vector<float> covMatrix = trackElement->getCovMatrix();
      for(size_t j = 0; j < covMatrix.size(); ++j){
        streamlog_out( DEBUG5 ) << covMatrix[j] << std::endl;
      }
      streamlog_out( DEBUG5 ) <<  "The track hits are finally listed here:" << std::endl;
      const vector<TrackerHit*> hits = trackElement->getTrackerHits();
      for(size_t j = 0; j < hits.size(); ++j){
        streamlog_out( DEBUG5 ) << "Hit " << j << " = " << hits[j]->getPosition()[0] << ", " <<  hits[j]->getPosition()[1] << ", " << hits[j]->getPosition()[2]  << std::endl;
      }
      streamlog_out( DEBUG5 ) << std::endl;
    }
  }
}

int EUTelX0Processor::guessSensorID(const double * hit )
{
  int sensorID = -1;
  double minDistance = numeric_limits< double >::max() ;
  streamlog_out( DEBUG5 ) << "referencehit collection: " << _referenceHitCollectionName << " at "<< _referenceHitVec << std::endl;
  if( _referenceHitVec == 0)
  {
    streamlog_out( MESSAGE5 ) << "_referenceHitVec is empty" << std::endl;
    return 0;
  }

  if( isFirstEvent() )
  {
    // message<DEBUG > ( log() << "number of elements : " << _referenceHitVec->getNumberOfElements() << std::endl );
  }

  for(int ii = 0 ; ii < _referenceHitVec->getNumberOfElements(); ii++)
  {
    EUTelReferenceHit* refhit = static_cast< EUTelReferenceHit*> ( _referenceHitVec->getElementAt(ii) ) ;
    if(refhit == 0 ) continue;
     TVector3 hit3d( hit[0], hit[1], hit[2] );
    TVector3 hitInPlane( refhit->getXOffset(), refhit->getYOffset(), refhit->getZOffset());
    TVector3 norm2Plane( refhit->getAlpha(), refhit->getBeta(), refhit->getGamma() );
    double distance = abs( norm2Plane.Dot(hit3d-hitInPlane) );
    streamlog_out( DEBUG5 ) << " hit " << hit[0] << " "<< hit[1] << " " << hit[2] << std::endl;
    streamlog_out( DEBUG5 ) << " " << refhit->getXOffset() << " " << refhit->getYOffset() << " " << refhit->getZOffset() << std::endl;
    streamlog_out( DEBUG5 ) << " " << refhit->getAlpha() << " " << refhit->getBeta() << " " << refhit->getGamma() << std::endl;
    streamlog_out( DEBUG5 ) << " distance " << distance << std::endl;
    if ( distance < minDistance )
    {
      minDistance = distance;
      sensorID = refhit->getSensorID();
    }
  }
  //Some useful debug printouts:
  bool debug = ( _debugCount>0 );

  if(debug)
  {
    for(int ii = 0 ; ii < _referenceHitVec->getNumberOfElements(); ii++)
    {
      EUTelReferenceHit* refhit = static_cast< EUTelReferenceHit*> ( _referenceHitVec->getElementAt(ii) ) ;
      if(refhit == 0 ) continue;
      // if( sensorID != refhit->getSensorID() ) continue;
      // streamlog_out( DEBUG5 ) << " _referenceHitVec " << _referenceHitVec << " " << _referenceHitCollectionName.c_str() << " " << refhit << " at "
      // << refhit->getXOffset() << " " << refhit->getYOffset() << " " << refhit->getZOffset() << " "
      //<< refhit->getAlpha() << " " << refhit->getBeta() << " " << refhit->getGamma() << std::endl ;
      //message<DEBUG5> ( log() << "iPlane " << refhit->getSensorID() << " hitPos: [" << hit[0] << " " << hit[1] << " " << hit[2] << "] distance: " << minDistance << std::endl );
    }
  }
  return sensorID;
}
 
void EUTelX0Processor::end()
{
  //Clean up memory
  //Set all values to zero or NULL
  //


  //calculateX0();
  _hitInfo.clear();  //This stores the hit position in a TVector3. If there are multiple hits then they are all stored in the vector of TVector3's. The int key refers to the layer number
  _projectedHits.clear();  //int refers to 1 or 2, with 1 being the projection from the 01 plane and 2 being the projection from the 43 plane
  _histoThing.clear();  //This map contains all the histograms that are going to be plotted in this processor
  _residual.clear();  //The pair of doubles refers to the x and y coordinates in layer 2. The vector of TVector3's refers to the positions of the projected hits
  _residualAngle.clear();  //As above but the TVector3 doesn't contain xyz but instead theta, phi and alpha
  _residualProfile.clear(); //TODO(Phillip Hamnett): Can this be joined with _residual? //Used as above but for created a profile histogram
  _inputHitCollectionVec = NULL;  //Stores the information being brought in from the Marlin process which contains information about post-aligned hits
  _inputTrackCollectionVec = NULL;  //Stores the information being brought in from the Marlin process which contains information about post-aligned hits
  _referenceHitVec = NULL;  
}

double EUTelX0Processor::calculateX0()
{
  double X0(0);
  //Two methods for figuring out the material budget, X0, are used. The loss of energy and the scattering angle due to multiple scattering.
  //Potentially might use both a Forward-backward Kalmin filter and a Global Linear Estimator to work out the two above values, might give a more accurate meaurement of X0.
  double sigma_msi(0);  //Highland formula
  double sigma_measi(0);  //Sum of the variance of histogram alpha012 and alpha432
  double E = 5;  //The total particle energy (GeV) TODO: (Phillip Hamnett) is there some way to make this information (and the mass) into the steering file?
  double m = 0.000511;  //Electron Mass (GeV)
  double p = sqrt(E*E - m*m);  //The particle momentum (GeV)
  double theta012(0);  //The polar angle between the momentum vector and the z-axis
  double beta012(0);  //The difference between the azimuthal angle of the momentum vector and the azimuthal angle of a point on the track
  double theta432(0);
  double beta432(0);
  X0 = 100;  //An estimate of X0
  double thickness = 0.00005;  //Thickness of mimosa layer is 50 micrometers
  double X = X0/thickness;
  double BetterX = DBL_MAX;
  double currentlikelihood = DBL_MIN;
  double lastX0 = DBL_MIN;
  while(X0 != BetterX){
    sigma_measi = 5;  //HACK - This is actually the sum of the variances of theta in the forward and backward direction. 
    size_t size = _histoData["Theta012"].size();
    double loglikelihoodX0(0);
    for(size_t i = 0; i < size; ++i){
      theta012 = _histoData["Theta012"][i]*3.1415/180.0;
      theta432 = _histoData["Theta432"][i]*3.1415/180.0;
      beta012 = _histoData["Phi012"][i]*3.1415/180.0;
      beta432 = _histoData["Phi432"][i]*3.1415/180.0;
      double deltatheta = sqrt((theta432 - theta012)*(theta432 - theta012));  //Take the positive difference between the two angles in theta and beta
      double deltabeta = sqrt((beta432 - beta012)*(beta432 - beta012));
      sigma_msi = (0.015*E/(p*p))*sqrt(X/(sin(deltatheta)*cos(deltabeta)))*(1 + 0.038*std::log(X/(sin(deltatheta)*cos(deltabeta))));
      if(sigma_msi != sigma_msi){
        streamlog_out( ERROR5 ) << "Code failed with following values: " << std::endl << "E = " << E << std::endl << "p = " << p << std::endl << "X = " << X << std::endl << "theta012 = " << theta012 << std::endl << "beta012 = " << beta012 << std::endl << "theta432 = " << theta432 << std::endl << "beta432 = " << beta432 << std::endl << "deltatheta = " << deltatheta << std::endl << "deltabeta = " << deltabeta << std::endl << "sigma_msi = " << sigma_msi << std::endl << "sigma_measi = " << sigma_measi << std::endl << "loglikelihoodX0 = " << loglikelihoodX0 << std::endl << std::endl;
      }
      //Maximum log-likelihood:
      loglikelihoodX0 += -std::log(sigma_msi*sigma_msi + sigma_measi*sigma_measi) - (theta012*theta012)/(sigma_msi*sigma_msi + sigma_measi*sigma_measi); 
    }
    if(loglikelihoodX0 > currentlikelihood){
      BetterX = X0;
      currentlikelihood = loglikelihoodX0;
      if(lastX0 < X0){
        X++;
      }
      else{
        X--;
      }
    }
    else{
      if(lastX0 < X0){
        X--;
      }
      else{
        X++;
      }
    }
    X = X0/thickness;
    loglikelihoodX0 = 0;
  }
  return X0;
}

void EUTelX0Processor::threePointResolution(LCCollection *alignedHitCollection){
//This function draws a line between the hits in plane i and i+2
//then it compares where the hit in plane i+1 is with the average of the other two planes
//this is then plotted as a residual for each plane.
  streamlog_out(DEBUG0) << "Starting ThreePointResolution" << std::endl;
  map< int, vector< TVector3> > hitvectortemp;//Temporary map for storing vectors of hit information pre-cut
  int collectionsize = alignedHitCollection->getNumberOfElements();
  streamlog_out(DEBUG0) << "Collectionsize = " << collectionsize << std::endl;
  for(int i = 0; i < collectionsize; ++i){
    TrackerHit* tHit = dynamic_cast<TrackerHit*>(alignedHitCollection->getElementAt(i));//Get the hit from each element of the collection
    const double* pos = tHit->getPosition();//Get the position of the hit in x, y and z
    int layernumber = guessSensorID(pos);//Get the layer number of the hit
    Double_t X = pos[0], Y = pos[1], Z = pos[2];//Store the hit positions as Double_t's to keep ROOT happy
    TVector3 tempvec(X,Y,Z);//Add the positions to a TVector3
    hitvectortemp[layernumber].push_back(tempvec);//Then put it in the temporary map
    if(_debug == true){
      streamlog_out( DEBUG5 )<< "In for loop 'for(int i = 0; i < collectionsize; ++i)':" << std::endl
           << "layernumber = " << layernumber << ", X = " << X << ", Y = " << Y << ", Z = " << Z << std::endl
           << "hitvectortemp[" << layernumber << "].size() = " << hitvectortemp[layernumber].size() << std::endl;
    }
  }
  for(int i = 0; i < _noLayers - 1; ++i){
    streamlog_out(DEBUG0) << "Entering layer " << i << std::endl;
    size_t hitvecisize = hitvectortemp[i].size();
    size_t hitveci2size = hitvectortemp[i+2].size();
    size_t hitveci1size = hitvectortemp[i+1].size();
    streamlog_out(DEBUG0) << "Hit vector sizes are: " << hitvecisize << "," << hitveci2size << "," << hitveci1size << std::endl;
    for(size_t j = 0; j < hitvecisize; ++j){
      TVector3 hiti = hitvectortemp[i][j];
      for(size_t k = 0; k < hitveci2size; ++k){
        TVector3 hiti2 = hitvectortemp[i+2][k];
        for(size_t l = 0; l < hitveci1size; ++l){
          TVector3 hiti1 = hitvectortemp[i+1][l];
          double averagex = (hiti.x() + hiti2.x())/2.0;
          double averagey = (hiti.y() + hiti2.y())/2.0;
          streamlog_out(DEBUG0) << "Does the value fall within cut parameters?" << std::endl
                    << "x^2 + y^2 = " << pow((averagex-hiti1.x()),2) + pow((averagey-hiti1.y()),2) << std::endl
                    << "cutValue1 = " << _cutValue1 << std::endl;
          if(sqrt(pow((averagex-hiti1.x()),2) + pow((averagey-hiti1.y()),2)) < _cutValue1){
            streamlog_out(DEBUG0) << "Filling histograms for residuals" << std::endl;
            (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualX"]))->fill(averagex - hiti1.x());
            (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualY"]))->fill(averagey - hiti1.y());
            if(i == 0){
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualXPlane1"]))->fill(averagex - hiti1.x());
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualYPlane1"]))->fill(averagey - hiti1.y());
            }
            else if(i == 1){
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualXPlane2"]))->fill(averagex - hiti1.x());
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualYPlane2"]))->fill(averagey - hiti1.y());
            }
            else if(i == 2){
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualXPlane3"]))->fill(averagex - hiti1.x());
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualYPlane3"]))->fill(averagey - hiti1.y());
            }
            else if(i == 3){
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualXPlane4"]))->fill(averagex - hiti1.x());
              (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualYPlane4"]))->fill(averagey - hiti1.y());
            }
          }
        }
      }
    }
  }
}

void EUTelX0Processor::basicFitter(LCCollection *alignedHitCollection){
  
  map< int, vector< TVector3> > hitvectortemp;//Temporary map for storing vectors of hit information pre-cut
  int collectionsize = alignedHitCollection->getNumberOfElements();
  for(int i = 0; i < collectionsize; ++i){
    TrackerHit* tHit = dynamic_cast<TrackerHit*>(alignedHitCollection->getElementAt(i));//Get the hit from each element of the collection
    const double* pos = tHit->getPosition();//Get the position of the hit in x, y and z
    int layernumber = guessSensorID(pos);//Get the layer number of the hit
    Double_t X = pos[0], Y = pos[1], Z = pos[2];//Store the hit positions as Double_t's to keep ROOT happy
    TVector3 tempvec(X,Y,Z);//Add the positions to a TVector3
    hitvectortemp[layernumber].push_back(tempvec);//Then put it in the temporary map
    if(_debug == true){
      cout << "In for loop 'for(int i = 0; i < collectionsize; ++i)':" << std::endl
           << "layernumber = " << layernumber << ", X = " << X << ", Y = " << Y << ", Z = " << Z << std::endl
           << "hitvectortemp[" << layernumber << "].size() = " << hitvectortemp[layernumber].size() << std::endl;
    }
  }//End of: for(int i = 0; i < colSize; ++i)
  if(_debug == true){
    cout << "So now, hitvectortemp contains: " << std::endl;
    for(map< int, vector< TVector3 > >::iterator it = hitvectortemp.begin(); it != hitvectortemp.end(); ++it){
      int layer = it->first;
      cout << "Layer " << layer << ":" << std::endl;
      for(vector< TVector3 >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
        cout  << it2->x() << ", " << it2->y() << ", " << it2->z() << std::endl;
      }
    }
  }
  double layerdifference = 0.1;  //This is the radius of a circle in which the two particles must lie
  vector< pair< TVector3, TVector3 > > relatedhits01;
  if(_debug == true){
    cout << "The following hits are within each others acceptable position difference (radius) of " << layerdifference << ":" << std::endl;
  }
  for(vector< TVector3 >::iterator hit0 = hitvectortemp[0].begin(); hit0 != hitvectortemp[0].end(); ++hit0){
    for(vector< TVector3 >::iterator hit1 = hitvectortemp[1].begin(); hit1 != hitvectortemp[1].end(); ++hit1){
      if(sqrt(pow(hit0->x() - hit1->x(),2) + pow(hit0->y() - hit1->y(),2)) < layerdifference){
        if(_debug == true){
          cout << "Layer 0: " << hit0->x() << ", " << hit0->y() << ", " << hit0->z() << std::endl;
          cout << "Layer 1: " << hit1->x() << ", " << hit1->y() << ", " << hit1->z() << std::endl;
        }
        pair< TVector3, TVector3 > relatedhits(*hit0,*hit1);
        relatedhits01.push_back(relatedhits);
      }
    }
  }
  if(_debug == true){
    cout << "The number of related hits in Layers 0 and 1 is:  " << relatedhits01.size() << std::endl;
  }
  //Make a projected point on layer 2 based on all related hits
  vector< TVector3 > projectedhits01;
  for(vector< pair< TVector3, TVector3 > >::iterator it = relatedhits01.begin(); it != relatedhits01.end(); ++it){
    TVector3 projectedhit(2*it->second.x() - it->first.x(), 2*it->second.y() - it->first.y(), 2*it->second.z());
    projectedhits01.push_back(projectedhit);
  }
  if(_debug == true){
    cout << "These are the predicted positions in layer 2 based on layers 0 and 1:" << std::endl;
    for(vector< TVector3 >::iterator it = projectedhits01.begin(); it != projectedhits01.end(); ++it){
      cout << it->x() << ", " << it->y() << ", " << it->z() << std::endl;
    }
  }
  //Clean up
  relatedhits01.clear();
  //Now to repeat on the other side, and then compare results
  vector< pair< TVector3, TVector3 > > relatedhits43;
  if(_debug == true){
    cout << "The following hits are within each others acceptable position difference (radius) of " << layerdifference << ":" << std::endl;
  }
  for(vector< TVector3 >::iterator hit4 = hitvectortemp[4].begin(); hit4 != hitvectortemp[4].end(); ++hit4){
    for(vector< TVector3 >::iterator hit3 = hitvectortemp[3].begin(); hit3 != hitvectortemp[3].end(); ++hit3){
      if(sqrt(pow(hit4->x() - hit3->x(),2) + pow(hit4->y() - hit3->y(),2)) < layerdifference){
        if(_debug == true){
          cout << "Layer 4: " << hit4->x() << ", " << hit4->y() << ", " << hit4->z() << std::endl;
          cout << "Layer 3: " << hit3->x() << ", " << hit3->y() << ", " << hit3->z() << std::endl;
        }
        pair< TVector3, TVector3 > relatedhits(*hit4,*hit3);
        relatedhits43.push_back(relatedhits);
      }
    }
  }
  if(_debug == true){
    cout << "The number of related hits in Layers 4 and 3 is:  " << relatedhits43.size() << std::endl;
  }
  //Make a projected point on layer 2 based on all related hits
  vector< TVector3 > projectedhits43;
  for(vector< pair< TVector3, TVector3 > >::iterator it = relatedhits43.begin(); it != relatedhits43.end(); ++it){
    TVector3 projectedhit(2*it->second.x() - it->first.x(), 2*it->second.y() - it->first.y(), 2*it->second.z());
    projectedhits43.push_back(projectedhit);
  }
  if(_debug == true){
    cout << "These are the predicted positions in layer 2 based on layers 4 and 3:" << std::endl;
    for(vector< TVector3 >::iterator it = projectedhits43.begin(); it != projectedhits43.end(); ++it){
      cout << it->x() << ", " << it->y() << ", " << it->z() << std::endl;
    }
  }
  //Clean up
  relatedhits43.clear();
  cout << "projectedhits01.size() = " << projectedhits01.size() << std::endl;
  cout << "projectedhits43.size() = " << projectedhits43.size() << std::endl;
  //Now we have projected hits in the 2nd layer from both the forward and backward direction. Now we make a residual between the two assuming that anything within a radius of 'layerdifference' of each other should be taken account of only
  for(vector< TVector3 >::iterator hits012 = projectedhits01.begin(); hits012 != projectedhits01.end(); ++hits012){
    for(vector< TVector3 >::iterator hits432 = projectedhits43.begin(); hits432 != projectedhits43.end(); ++ hits432){  
      if(sqrt(pow(hits012->x() - hits432->x(),2) + pow(hits012->y() - hits432->y(),2)) < layerdifference){
        cout << "Hit recorded in histogram" << std::endl;
        double newx = hits012->x() - hits432->x();
        double newy = hits012->y() - hits432->y();
        double newz = 20;  //HACK - find a better way of deducing the z position of layer 2
        Double_t theta012 = (atan(hits012->x()/newz)/3.14)*180;
        Double_t phi012 = (atan(hits012->y()/newz)/3.14)*180;
        Double_t alpha012 = tan(theta012)*tan(phi012)/sqrt(tan(theta012)*tan(theta012) + tan(phi012)*tan(phi012));
        Double_t theta432 = (atan(hits432->x()/newz)/3.14)*180;
        Double_t phi432 = (atan(hits432->y()/newz)/3.14)*180;
        Double_t alpha432 = tan(theta432)*tan(phi432)/sqrt(tan(theta432)*tan(theta432) + tan(phi432)*tan(phi432));
        if(_debug == true){
          cout << newx << ", " << newy << ", " << newz << std::endl;
          cout << theta012 << ", " << phi012 << ", " << alpha012 << std::endl;
        }
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualX"]))->fill(newx);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["ResidualY"]))->fill(newy);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["Theta012"]))->fill(theta012);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["Phi012"]))->fill(phi012);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["Alpha012"]))->fill(alpha012);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["Theta432"]))->fill(theta432);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["Phi432"]))->fill(phi432);
        (dynamic_cast< AIDA::IHistogram1D* > (_histoThing["Alpha432"]))->fill(alpha432);
        _histoData["ResidualX"].push_back(newx);
        _histoData["ResidualY"].push_back(newy);
        _histoData["Theta012"].push_back(theta012);
        _histoData["Phi012"].push_back(phi012);
        _histoData["Alpha012"].push_back(alpha012);
        _histoData["Theta432"].push_back(theta432);
        _histoData["Phi432"].push_back(phi432);
        _histoData["Alpha432"].push_back(alpha432);
      }
    }
  }
  projectedhits01.clear();
  projectedhits43.clear();

}

