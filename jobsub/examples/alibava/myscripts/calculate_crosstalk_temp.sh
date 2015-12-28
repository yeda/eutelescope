#!/bin/zsh

#########################################################################################
#                                                                                       #
# ALiBaVa Analysis - Calculates CrossTalk and does Signal Reconstruction and clustering #
#                                                                                       #
#########################################################################################

# usage: source reco.sh runnumber(s)

# To get pedestal subtracted, common mode and cross talk corrected signal values
# you will need to run these templates
TEMPLATELISTPED=('alibava-convert-ped' 'alibava-pedestal' 'alibava-commonmode' 'alibava-pedestal2')

TEMPLATELISTRECO=('alibava-converter' 'alibava-reco' 'alibava-crosstalk-it1' 'alibava-crosstalk-it2' 'alibava-crosstalk-it3')

for TEMPLATE in $TEMPLATELISTPED; do
	jobsub -c config/dut4_cce_config.cfg -csv runlistfiles/runlist.csv --subdir --naf qsubparameters.cfg $TEMPLATE 4198 

#check if there is any run submitted to the naf
#if there is wait for them to be finished 
while [[ -n `qstat` ]]; do
        echo Waiting for jobs to be finished
        sleep 30
done
if [[ -z `qstat` ]];then
        echo ready to start new job
fi


done


for TEMPLATE in $TEMPLATELISTRECO; do
	jobsub -c config/dut4_cce_config.cfg -csv runlistfiles/runlist.csv --subdir --naf qsubparameters.cfg $TEMPLATE 4197 

#check if there is any run submitted to the naf
#if there is wait for them to be finished 
while [[ -n `qstat` ]]; do
        echo Waiting for jobs to be finished
        sleep 30
done
if [[ -z `qstat` ]];then
        echo ready to start new job
fi


done


