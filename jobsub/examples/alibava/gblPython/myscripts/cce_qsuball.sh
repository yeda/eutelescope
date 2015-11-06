#!/bin/bash

PRINTALIGNPARAMETERS=(1)

for PRINT in "${PRINTALIGNPARAMETERS[@]}"
do

ITERATIONLIST=(1 2 3 4)

for iteration in "${ITERATIONLIST[@]}"
do

#DUTLIST=(0 1 2 3 5 6)
DUTLIST=(5)

for DUT in "${DUTLIST[@]}"
do

if [ $DUT = 0 ]; then
RUNLIST=(114 120 124 128 132 149 150 153 154 155 156 157 669 671 672)
GEARFILE="gearfiles/gear-tb5-telescope"
fi

if [ $DUT = 1 ]; then
RUNLIST=(1274 1010 1014 1016 1020 1024 1028 1273 1041 1049)
GEARFILE="gearfiles/gear-tb5-telescope"
fi

if [ $DUT = 2 ]; then
RUNLIST=(2006 2011 2012 2019 2020 2027 2075 2301 2305 2310 2314 2348) 
GEARFILE="gearfiles/gear-tb5-telescope"
fi

if [ $DUT = 3 ]; then
RUNLIST=(3004 3008 3009 3040 3036 3032 3028 3024 3020 3016) 
GEARFILE="gearfiles/gear-tb5-telescope"
fi

if [ $DUT = 4 ]; then
#RUNLIST=(4197)
RUNLIST=()
fi


if [ $DUT = 5 ]; then
#RUNLIST=(5024)
RUNLIST=(5064 5057 5056 5048 5024 5031 5032 5039 5040 5047)
GEARFILE="gearfiles/gear-tb6-telescope"
fi

if [ $DUT = 6 ]; then
RUNLIST=(6004 6005 6012 6013 6020 6021 6028 6029 6036 6037)
GEARFILE="gearfiles/gear-tb6-telescope"
fi

if [ $PRINT = 1 ]; then
alignment_textfile="alignment_parameters/alignment_parameters_dut${DUT}_it${iteration}.txt"
rm $alignment_textfile
fi



for runnum in $RUNLIST
do

if [ $iteration != 1 ]; then
prev_it=`python -c "print ${iteration}-1"`
GEARFILE="output/run${(l:6::0:)runnum}/gear_${runnum}_it${prev_it}"
fi

if [ $PRINT = 0 ]; then
source myscripts/gbl_qsub.sh ${runnum} ${GEARFILE} $iteration
else 
source myscripts/print_alignment_parameters_cce.sh ${runnum} ${iteration} >> $alignment_textfile
echo " " >> ${alignment_textfile}
fi

done # RUNLIST

done # DUTLIST

if [ $PRINT = 0 ]; then
source ../myscripts/continue_when_all_runs_finished.sh
fi


done #ITERATIONLIST
done # PRINT
