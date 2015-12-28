#!/bin/bash

#EVENTIDDIFFLIST=("0" "-1" "1" "-2" "2")
#EVENTIDDIFFLIST=("0")

#DUTLIST=(10 0)
DUTLIST=(10 0 1 2 3 5 6)

#for EVENTIDDIFF in "${EVENTIDDIFFLIST[@]}"
#do

TEMPLATELIST=('alibava-applyxtalk' 'merger' 'hitmaker-local')
#TEMPLATELIST=('alibava-noxtalk' 'merger-noxtalk' 'hitmaker-local')
for Template in "${TEMPLATELIST[@]}"
do

for DUT in "${DUTLIST[@]}"
do

if [ $DUT = 0 ]; then
#RUNLIST=(124 132 150 153)
RUNLIST=(114 120 124 128 132 149 150 153 154 155 156 157)
fi

if [ $DUT = 10 ]; then
#RUNLIST=(672)
RUNLIST=(669 671 672)
fi

if [ $DUT = 1 ]; then
RUNLIST=(1004 1274 1010 1014 1016 1020 1024 1028 1273 1041 1049)
fi

if [ $DUT = 2 ]; then
RUNLIST=(2006 2011 2012 2019 2020 2027 2075 2301 2305 2310 2314 2348) 
fi

if [ $DUT = 3 ]; then
RUNLIST=(3004 3008 3009 3040 3036 3032 3028 3024 3020 3016) 
fi

if [ $DUT = 4 ]; then
#RUNLIST=(4197)
fi


if [ $DUT = 5 ]; then
RUNLIST=(5064 5057 5056 5048 5024 5031 5032 5039 5040 5047)
fi

if [ $DUT = 6 ]; then
RUNLIST=(6004 6005 6012 6013 6020 6021 6028 6029 6036 6037)
fi

#echo "Running EventIdDiff=${EVENTIDDIFF}, DUT=${DUT}"
source myscripts/cce_rundut.sh $DUT all 
#source myscripts/cce_rundut.sh $DUT all $EVENTIDDIFF

OUTPUTPATH="/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output"

CSV=runlistfiles/cce_runlist_all.csv
CONFIG=config/dut${DUT}_cce_config.cfg

jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $Template $RUNLIST

#for runnum in "${RUNLIST[@]}"
#do

#mv ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-merger.slcio ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-merger_idDiff${EVENTIDDIFF}.slcio
#mv ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-hitmaker-local.slcio ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-hitmaker-local_idDiff${EVENTIDDIFF}.slcio
#mv ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-merger.root ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-merger_idDiff${EVENTIDDIFF}.root
#mv ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-hitmaker-local.root ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-hitmaker-local_idDiff${EVENTIDDIFF}.root


#done # RUNLIST
#mv pdfs/cce_plots_hit_dut${DUT}_B000.pdf pdfs/cce_plots_hit_dut${DUT}_B000_idDiff${EVENTIDDIFF}.pdf

done # DUTLIST

source myscripts/continue_when_all_runs_finished.sh
done # TEMPLATELIST
#done # EVENTIDDIFFLIST
