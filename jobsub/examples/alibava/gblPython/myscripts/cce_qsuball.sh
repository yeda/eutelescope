#!/bin/bash

AlibavaFolder="/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava"
GblFolder="${AlibavaFolder}/gblPython"
InfoFile="${AlibavaFolder}/runlistfiles/cce_file_info.csv"


ALLRUNLIST=(114 120 124 128 132 149 150 153 154 155 156 157 669 671 672 1274 1010 1014 1016 1020 1024 1028 1273 1041 1049 2006 2011 2012 2019 2020 2027 2075 2301 2305 2310 2314 2348 3004 3008 3009 3040 3036 3032 3028 3024 3020 3016 5064 5057 5056 5048 5024 5031 5032 5039 5040 5047 6004 6005 6012 6013 6020 6021 6028 6029 6036 6037)


#ITERATIONLIST=(1)
ITERATIONLIST=(1 2 3 4)

for iteration in "${ITERATIONLIST[@]}"
do

###  CCE


for runnum in $ALLRUNLIST
do

BeamEnergy="4.4"

prev_it=`python -c "print ${iteration}-1"`
GEARFILE="${GblFolder}/output/run${(l:6::0:)runnum}/gear_${runnum}_it${prev_it}.xml"


if [ $iteration = 1 ]; then
GblCuts="((0.5, 0.5), (0.02, 0.02), (0.01, 0.01), (0.1, 0.1), (2.2, 10))"
elif [ $iteration = 2 ]; then
GblCuts="((0.5, 0.5), (0.02, 0.02), (0.01, 0.01), (0.1, 0.1), (1.0, 10))"
elif [ $iteration = 3 ]; then
GblCuts="((0.7, 0.5), (0.02, 0.02), (0.01, 0.01), (0.1, 0.1), (0.2, 10))"
elif [ $iteration = 4 ]; then
GblCuts="((0.5, 0.5), (0.02, 0.02), (0.01, 0.01), (0.1, 0.1), (0.1, 10))"
else
GblCuts="((0.5, 0.5), (0.02, 0.02), (0.01, 0.01), (0.1, 0.1), (0.05, 10))"
fi

source ${GblFolder}/myscripts/gbl_qsub.sh ${runnum} ${GEARFILE} ${BeamEnergy} ${GblCuts} ${iteration}

done # RUNLIST

source ${AlibavaFolder}/myscripts/continue_when_all_runs_finished.sh


done #ITERATIONLIST
