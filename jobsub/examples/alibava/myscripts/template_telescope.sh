#!/bin/zsh

################################
#                              #
# ALiBaVa Analysis - Pedestal  #
#                              #
################################

# usage: source telescope_reco_nafsubmission.sh runnumber(s)

# To get telescope clusters to be formed and filtered
# you will need to run these templates
RUNLIST=
CONFIG=
CSV=

TEMPLATELIST=('telescope-converter' 'telescope-clustering' 'telescope-filter')

for TEMPLATE in $TEMPLATELIST; do
jobsub

#check if there is any run submitted to the naf
#if there is wait for them to be finished 
source myscripts/continue_when_all_runs_finished.sh

# get user input to continue
#source myscripts/ask_me_to_continue.sh

done
