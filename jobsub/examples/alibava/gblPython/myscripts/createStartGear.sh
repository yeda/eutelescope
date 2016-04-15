#!/bin/bash

RunNum=$1
OutputFolder=$2


DutCode=${RunNum:0:1}

if [ $DutCode = 0 ] || [ $DutCode = 1 ] || [ $DutCode = 2 ] || [ $DutCode = 3 ]; then
TB=5;
else
TB=6
fi

AlibavaFolder="/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava"
GblFolder="${AlibavaFolder}/gblPython"

InfoFile="${AlibavaFolder}/runlistfiles/lam_file_info.csv"
DutLine=$(grep ${RunNum} ${InfoFile} | tail -1)
TiltAngle=$(echo ${DutLine} | cut -f8 -d";")
BField=$(echo ${DutLine} | cut -f12 -d";")


AlignParam=$(source ${GblFolder}/myscripts/print_alignment_parameters_lam.sh ${RunNum} 0${TB})
Line_Dut1=$(echo ${AlignParam} | grep DUT1)
Line_Dut2=$(echo ${AlignParam} | grep DUT2)

PosX_Dut1=$(echo ${Line_Dut1} | cut -f2 -d"\"")
PosX_Dut2=$(echo ${Line_Dut2} | cut -f2 -d"\"")
# note that Z position of duts must be equal
PosZ_Dut=$(echo ${Line_Dut1} | cut -f4 -d"\"")

NewPosX_Dut1=`python -c "import math; print ${PosX_Dut1}*(math.cos(${TiltAngle}*math.pi/180.0))"`
NewPosX_Dut2=`python -c "import math; print ${PosX_Dut2}*(math.cos(${TiltAngle}*math.pi/180.0))"`
NewPosZ_Dut1=`python -c "import math; print ${PosZ_Dut}+${PosX_Dut1}*(math.sin(${TiltAngle}*math.pi/180.0))"`
NewPosZ_Dut2=`python -c "import math; print ${PosZ_Dut}+${PosX_Dut2}*(math.sin(${TiltAngle}*math.pi/180.0))"`


GearTemplate="${GblFolder}/gearfiles/template-gear-tb${TB}.xml"
mkdir -p "${OutputFolder}/run${(l:6::0:)RunNum}"
NewGear="${OutputFolder}/run${(l:6::0:)RunNum}/gear_${RunNum}_it0.xml"
cp ${GearTemplate} ${NewGear}

sed -i "s@B_FIELD@${BField}@g" ${NewGear}
sed -i "s@POSX_DUT1@${NewPosX_Dut1}@g" ${NewGear}
sed -i "s@POSZ_DUT1@${NewPosZ_Dut1}@g" ${NewGear}
sed -i "s@ROTATION_DUT@${TiltAngle}@g" ${NewGear}
sed -i "s@POSX_DUT2@${NewPosX_Dut2}@g" ${NewGear}
sed -i "s@POSZ_DUT2@${NewPosZ_Dut2}@g" ${NewGear}

ls ${NewGear}


