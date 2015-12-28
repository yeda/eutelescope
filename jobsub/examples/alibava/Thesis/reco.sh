#!/bin/zsh

###########################################################
#                                                         #
# ALiBaVa Analysis - Signal Reconstruction and clustering #
#                                                         #
###########################################################

# usage: source reco.sh runnumber(s)

# To get pedestal subtracted, common mode and cross talk corrected signal values
# you will need to run these templates
TEMPLATELIST=('alibava-converter' 'alibava-reco' 'alibava-applyxtalk')

for TEMPLATE in $TEMPLATELIST; do
	jobsub -c config/noCuts_config.cfg -csv ../runlistfiles/pedestal_runlist_all.csv $TEMPLATE $@
#	jobsub -c config/noCuts_config.cfg -csv ../runlistfiles/pedestal_runlist_all.csv --subdir --naf ../qsubparameters.cfg $TEMPLATE $@

#source ../myscripts/continue_when_all_runs_finished.sh

done


