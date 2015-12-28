#!/bin/zsh

################################
#                              #
# ALiBaVa Analysis - Pedestal  #
#                              #
################################

# usage: source pedestal.sh runnumber(s)

#To calculate Pedestals
# you will need to run these templates
TEMPLATELIST=('alibava-convert-ped' 'alibava-pedestal' 'alibava-commonmode' 'alibava-pedestal2')

for TEMPLATE in $TEMPLATELIST; do
	jobsub -c config/noCuts_config.cfg -csv ../runlistfiles/pedestal_runlist_all.csv $TEMPLATE $@
#	jobsub -c config/noCuts_config.cfg -csv ../runlistfiles/pedestal_runlist_all.csv --subdir --naf ../qsubparameters.cfg $TEMPLATE $@

#source ../myscripts/continue_when_all_runs_finished.sh

done


