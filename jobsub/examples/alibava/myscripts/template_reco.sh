#!/bin/zsh

RUNLIST=
CONFIG=
CSV=

TEMPLATELIST=('alibava-converter' 'alibava-reco' 'alibava-applyxtalk')

for TEMPLATE in $TEMPLATELIST; do
jobsub 

#check if there is any run submitted to the naf
#if there is wait for them to be finished 
source myscripts/continue_when_all_runs_finished.sh

# get user input to continue
#source myscripts/ask_me_to_continue.sh

done

root 
