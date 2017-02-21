'''
Created on Jun 8, 2015

@author: kleinwrt
'''
## \file
# main program
#
# telescope track finding and fitting

from eutelpy.telgear import GearSetup
from eutelpy.televent import TelEvent
from eutelpy.teltriplets import TelTriplets
from eutelpy.telfile import TelFile
from eutelpy.telalign import TelMP2alignment
from eutelpy.teltree import TelTree
from toolspy.simpleROOTHists import SimpleHists
import time
import sys, getopt
import os
import math
import numpy as np

def main(argv):
  start = time.clock()
  runnumber = 0
  gearfile = ''
  beamEnergy = 0.0
  gblcuts = ''
  inputdir = ''
  outputdir = ''

  try:
    opts, args = getopt.getopt(argv,"hg:r:e:c:i:o:",["gearfile=","runnumber=","beamenergy=","gblcuts","inputdir","outputdir"])
  except getopt.GetoptError:
    print 'telmain.py -g <gearfile> -r <runnumber> -e <beamenergy> -c <gblcuts> -i <inputdir> -o <outputdir>'
    sys.exit(2)
  for opt, arg in opts:
    if opt == '-h':
      print 'telmain.py -g <gearfile> -r <runnumber> -e <beamenergy> -c <gblcuts> -i <inputdir> -o <outputdir>'
      sys.exit()
    elif opt in ("-g", "--gearfile"):
      gearfile = arg
    elif opt in ("-e", "--beamenergy"):
      beamEnergy = -1.0*float(arg)
    elif opt in ("-r", "--runnumber"):
      runnumber = arg.zfill(6)
    elif opt in ("-c", "--gblcuts"):
      gblcuts = arg
    elif opt in ("-i", "--inputdir"):
      inputdir = arg
    elif opt in ("-o", "--outputdir"):
      outputdir = arg

  print 'Gear file is ', gearfile
  print 'Run number is ', runnumber
  print 'Run beam energy is ', beamEnergy 
  print 'gblcuts is ', gblcuts 
  print 'inputdir is ', inputdir 
  print 'outputdir is ', outputdir 
  
  # create output directory if it doesn't exist
  #outputdir = outputdir + '/run' + runnumber
  if not os.path.exists(outputdir):
    os.makedirs(outputdir)

  # maximum number of events
  #maxEvt = 5000
  maxEvt = 10000000
  # beam energy (*q)
  #beamEnergy = -4.4  # 613
  # lcio data file
  #lciofile = '/nfs/dust/atlas/user/yeda/EUTelescopeOutput/cce-noxtalk-output/lcio/runXXXXXX-hitmaker-local.slcio'
  lciofile = inputdir + '/runXXXXXX-hitmaker-local.slcio'
  dataFile = TelFile(lciofile.replace("XXXXXX",runnumber), 'merged_hits')
  dataFile.open() 
  
  # GEAR file
  #gearfile = "gear-start.xml"

  # Millepede-II alignment
  millepedefile = outputdir + '/milleBinary.dat'
  mp2 = TelMP2alignment(millepedefile)
  mp2.open()  # open file for alignment

  # estimated alignment error (mimosa, DUT)
  alignmentError = (0.005, 0.01)

  # number of events to show event display
  displayEvents = 0

  # ROOT tree file
  #tree = None
  treefile = outputdir + '/dutTree.root'
  tree = TelTree(treefile)
  
  # parse gear file
  g = GearSetup(gearfile, alignmentError)
  bField = g.getBField()
  detector = g.getDetector()
  # create MP2 steering files
  # for mimosa planes determine positions and XY rotations, fix first plane and position of last as Reference
  ##### only telescope
  #parMimosa = ['RRRRRR', '111111', '111111', '111111', '111111', 'RRR001'] 
  #parMimosa = ['RRRRRR', '111001', '111001', '111001', '111001', 'RRR001'] 
  parMimosa = ['RRRRRR', '000000', '000000', '000000', '000000', 'RRRRRR'] 
  #if bField[1] != 0.:
    # for B field in Y fix X position of plane 3 in addition
  #  parMimosa[3] = 'R11001'

  #################
  # DUT Alignment #
  #################
  # for DUT determine positions and (all) rotations
  #parDUT = '111111'
  # for DUT determine X,Z positions
  #parDUT = '101000'
  # for DUT determine X,Z positions and XZ,XY rotations
  parDUT = '101011'
  # for DUT determine X,Z positions and XY rotations
  #parDUT = '101001'

  # combined alignment of DUTs (in common plane, need to have same intersection point with Z-axis in gear file)
  #combDUT = (6, 7)
  # don't combine DUTs
  combDUT = None    

  steerfile = outputdir + '/steer.txt'
  mp2.createSteering(steerfile, detector, parMimosa, parDUT, combDUT)
  #
  # Cuts in (X,Y) for triplet finder, DUT matching 
  #   (peak from true, background from random matches; tail in bending plane for B<>0, electrons)
  #   Doublet definition: X/Y-distances (mean depends on beam direction and Z-distance)
  #   Triplet definition: X/Y-distances of third hit to doublet (RMS = triplet resolution)
  #   Track definition: X/Y-slope differences of pair of triplets (RMS depends on Z-distances)
  #   Track definition: X/Y-position differences of pair of triplets (RMS depends on Z-distances)
  #   DUT matching: X/Y-distances of DUT hit to track
  
  # ((doubletDx_min, doubletDx_max), (doubletDy_min, doubletDy_max), (tripletDx_min, tripletDx_max), (tripletDy_min, tripletDy_max), (dslopeX_min, dslopeX_max), (dslopeY_min, dslopeY_max), (dposX_min, dposX_max), (dposY_min, dposY_max), (DUT-dx, DUT-dY))
  # DUT-dx and DUT-dY are the absolute values of the cuts, i.e. min will be -(DUT-dx) and max will be DUT-dx 
  # same for DUT-dY
  # DUT-dx and DUT-dY is applied to both DUT
  gblcuts = gblcuts.replace('(','')
  gblcuts = gblcuts.replace(')','')
  gblcuts = gblcuts.replace(' ','')
  gblcuts = gblcuts.split(',')
  gblcuts = map(float,gblcuts)
  gblcuts = np.array(gblcuts).reshape(9,2)
  cuts = map(tuple, gblcuts)

  # histograms for cut values
  #hists = None
  hists = SimpleHists(("doubletDx", "doubletDy", "tripletDx", "tripletDy", "dslopeX", "dslopeY", \
                      "dposX", "dposY", "DUT6-dx", "DUT6-dy", "DUT7-dx", "DUT7-dy", "nTriplets", "nMatches", "match/triplet", "nSegments","Pseg","Pgbl","xScat","yScat"))
  #
  numEvt = 0
  numTrack = 0
  while numEvt < maxEvt:
    event = TelEvent(detector, beamEnergy, bField)
    event.read(dataFile)
    if not event.isValid():
      break
    # event read
    #if event.getNumHits() <= 0:
    # only telescope
    if event.getNumDUThits() <= 0:
      continue
    numEvt += 1 
    #event.dump()
    # triplet finder
    finder = TelTriplets(event.getHeader(), detector.keys(), cuts, hists)
    # find tracks and match DUT hits
    event.findTracks(finder, -40.)
    numTrack += event.getNumTracks()
    if numEvt % 1000 == 0:
      print "Processing Event %i    Total Number of tracks = %i" % (numEvt, numTrack)
    # fit segments
    event.fitGBL(mp2.getBinaryFile())
    # display event and finder results
    if numEvt < displayEvents:      
      event.plot(False)
      finder.plotTriplets(False) 
      finder.plotSegments() 

    # analyze event
    event.analyzeScat(hists)
    event.analyzeMom(hists)
    event.analyze(tree)

  dataFile.close() 
  # write and close tree file
  if tree is not None:
    tree.write()
  # plot/save histograms   
  if hists is not None:
    #hists.dump()
    #hists.plot()
    histfile = outputdir + '/cuts.root'
    hists.save(histfile)
  print " Events ", numEvt
  print " Tracks ", numTrack
  end = time.clock()
  print " Time [s] ", end - start  

if __name__ == "__main__":
  main(sys.argv[1:]) 
