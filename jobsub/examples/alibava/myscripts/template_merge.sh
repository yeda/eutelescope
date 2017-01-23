#!/bin/zsh

################################
#                              #
# ALiBaVa Analysis - Pedestal  #
#                              #
################################

# usage: source merge_and_createhits_nafsubmission.sh runnumber(s)

# To merge alibava and telescope clusters and then create hits
# you will need to run these templates

RUNLIST=
CONFIG=
CSV=

TEMPLATELIST=('merger' 'hitmaker-local')

for TEMPLATE in $TEMPLATELIST; do
jobsub

#check if there is any run submitted to the naf
#if there is wait for them to be finished 
source myscripts/continue_when_all_runs_finished.sh

# get user input to continue
#source myscripts/ask_me_to_continue.sh

done

root
