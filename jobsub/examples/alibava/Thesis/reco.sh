#!/bin/zsh

###########################################################
#                                                         #
# ALiBaVa Analysis - Signal Reconstruction and clustering #
#                                                         #
###########################################################

# usage: source reco.sh runnumber(s)

# To get pedestal subtracted, common mode and cross talk corrected signal values
# you will need to run these templates
#TEMPLATELIST=('alibava-converter' 'alibava-rawdatahisto' 'alibava-reco' 'alibava-applyxtalk')
#TEMPLATELIST=('alibava-reco')


# To calculate cross-talk
TEMPLATELIST=('alibava-crosstalk-it1' 'alibava-crosstalk-it2' 'alibava-crosstalk-it3')


for TEMPLATE in $TEMPLATELIST; do
# ChapReco NoCut
#	jobsub -c config/ChapReco-nocut.cfg -csv ../runlistfiles/lam_runlist_all.csv $TEMPLATE $@

# ChapReco TimeCut
	jobsub -c config/ChapReco-timecut.cfg -csv ../runlistfiles/lam_runlist_all.csv $TEMPLATE $@

#	jobsub -c config/noCuts_config.cfg -csv ../runlistfiles/lam_runlist_all.csv --subdir --naf ../qsubparameters.cfg $TEMPLATE $@

source ../myscripts/continue_when_all_runs_finished.sh

done


