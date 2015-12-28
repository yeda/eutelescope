
runnum=$1
iteration=$2


BaseFolder="/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/gblPython"
if [ $iteration = '05' ]; then
InputGear="${BaseFolder}/gearfiles/gear-tb5-telescope.xml"
elif [ $iteration = '06' ]; then
InputGear="${BaseFolder}/gearfiles/gear-tb6-telescope.xml"
else
InputGear="${BaseFolder}/output/run${(l:6::0:)runnum}/gear_${runnum}_it${iteration}.xml"
fi

# get the line for DUTs
a=`awk '/-- DUT/{nr[NR]; nr[NR+2]}; NR in nr' ${InputGear}`
a=${a:gs/DUT /DUT}
b=(${(s: :)a})

InfoFile="/nfs/dust/atlas/user/yeda/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/alibava/runlistfiles/lam_file_info.csv"
DutLine=$(grep ${runnum} ${InfoFile} | tail -1)
TiltAngle=$(echo ${DutLine} | cut -f8 -d";") 
BField=$(echo ${DutLine} | cut -f12 -d";") 

AlignRot=$(echo $b[11] | cut -f2 -d"\"")
DiffRot=$(python -c "print ${TiltAngle}-${AlignRot}")

echo "run${(l:6::0:)runnum}/gear_${runnum}_it${iteration}.xml      TiltAngle=${TiltAngle} B = ${BField}    DiffRot= ${DiffRot}"
echo $b[2] $b[6] $b[8] $b[10] $b[11]
echo $b[17] $b[21] $b[23] $b[25] $b[26]


