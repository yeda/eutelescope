#!/bin/zsh

################################
#                              #
# ALiBaVa Analysis - Pedestal  #
#                              #
################################

# usage: source pedestal_nafsubmission.sh runnumber(s)

#To calculate Pedestals
# you will need to run these templates
RUNLIST=
CONFIG=
CSV=


TEMPLATELIST=('alibava-convert-ped' 'alibava-pedestal' 'alibava-commonmode' 'alibava-pedestal2')


for TEMPLATE in $TEMPLATELIST; do
jobsub 

#check if there is any run submitted to the naf
#if there is wait for them to be finished 
source myscripts/continue_when_all_runs_finished.sh

# get user input to continue
#source myscripts/ask_me_to_continue.sh

done

root -b -q 'plots_pedestal.C(0)' 

