#!/bin/bash

DUTLIST=(0 10 1 2 3 5 6)
TEMPLATELIST=('telescope-filter' 'merger' 'hitmaker-local')
#TEMPLATELIST=('alibava-converter' 'alibava-reco' 'alibava-applyxtalk' 'telescope-converter' 'telescope-clustering' 'telescope-filter' 'merger' 'hitmaker-local')
#TEMPLATELIST=('alibava-converter' 'alibava-reco' 'telescope-converter' 'telescope-clustering' 'telescope-filter' 'merger-noxtalk' 'hitmaker-local')

for Template in "${TEMPLATELIST[@]}"
do

for DUT in "${DUTLIST[@]}"
do

if [ $DUT = 0 ]; then
RUNLIST=(114 120 124 128 132 149 150 153 154 155 156 157)
BONDS='$0:19-33$ $0:35-121$ $1:23-35$ $1:39-76$ $1:78-125$'
TIMECUTMIN=25.0
TIMECUTMAX=35.0
fi

if [ $DUT = 10 ]; then
RUNLIST=(669 671 672)
BONDS='$0:19-33$ $0:35-121$ $1:23-35$ $1:39-76$ $1:78-125$'
TIMECUTMIN=7.0
TIMECUTMAX=17.0
fi

if [ $DUT = 1 ]; then
RUNLIST=(1004 1274 1010 1014 1016 1020 1024 1028 1273 1041 1049)
BONDS='$0:20-22$ $0:24-29$ $0:31-50$ $0:56-122$ $1:24-112$ $1:114-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 2 ]; then
RUNLIST=(2006 2011 2012 2019 2020 2027 2075 2301 2305 2310 2314 2348) 
BONDS='$0:24-126$ $1:24-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 3 ]; then
RUNLIST=(3004 3008 3009 3040 3036 3032 3028 3024 3020 3016) 
BONDS='$0:20-22$ $0:24-29$ $0:31-50$ $0:56-122$ $1:24-112$ $1:114-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi


if [ $DUT = 5 ]; then
RUNLIST=(5064 5057 5056 5048 5024 5031 5032 5039 5040 5047)
BONDS='$0:20-122$ $1:24-127$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 6 ]; then
RUNLIST=(6029)
#RUNLIST=(6004 6005 6012 6013 6020 6021 6028 6029 6036 6037)
BONDS='$0:20-122$ $1:24-127$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

CONFIG=config/dut${DUT}_cce_config.cfg
cp config/noCuts_config.cfg $CONFIG
sed -i "/Bonds/ c\Bonds=$BONDS" $CONFIG 
sed -i "/TimeCutMin/ c\TimeCutMin=$TIMECUTMIN" $CONFIG 
sed -i "/TimeCutMax/ c\TimeCutMax=$TIMECUTMAX" $CONFIG 
sed -i "/EventIDDifference/ c\EventIDDifference=@EventIDDiff@" $CONFIG 



CSV=runlistfiles/cce_runlist_all.csv
CONFIG=config/dut${DUT}_cce_config.cfg
echo Config file: $CONFIG

jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $Template $RUNLIST

done # DUTLIST

#source myscripts/continue_when_all_runs_finished.sh

#source myscripts/bla.sh
source myscripts/ask_me_to_continue.sh

done # TEMPLATELIST
#root -q -b 'lam_plots_cluster.C(1)' 
#root -q -b 'lam_plots_cluster.C(2)'
#root -q -b 'lam_plots_cluster.C(3)'
#root -q -b 'lam_plots_cluster.C(4)'
#root -q -b 'lam_plots_cluster.C(5)'
#root -q -b 'lam_plots_cluster.C(6)'
#
#root -q -b 'lam_plots_hit.C(1)' 
#root -q -b 'lam_plots_hit.C(2)'
#root -q -b 'lam_plots_hit.C(3)'
#root -q -b 'lam_plots_hit.C(4)'
#root -q -b 'lam_plots_hit.C(5)'
#root -q -b 'lam_plots_hit.C(6)'

########## produce hit plots
#for DUT in "${DUTLIST[@]}"
#do

#mv pdfs/lam_plots_hit_dut${DUT}.pdf pdfs/lam_plots_hit_dut${DUT}_idDiff${EVENTIDDIFF}.pdf
#done # DUTLIST

############# move result files
#for runnum in "${ALLRUNLIST[@]}"
#do

#OUTPUTPATH="/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output"
#mv ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-merger.slcio ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-merger_idDiff${EVENTIDDIFF}.slcio
#mv ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-hitmaker-local.slcio ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-hitmaker-local_idDiff${EVENTIDDIFF}.slcio
#mv ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-merger.root ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-merger_idDiff${EVENTIDDIFF}.root
#mv ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-hitmaker-local.root ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-hitmaker-local_idDiff${EVENTIDDIFF}.root


