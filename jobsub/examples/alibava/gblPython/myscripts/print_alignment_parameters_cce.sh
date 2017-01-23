
runnum=$1
iteration=$2

# get the line for DUTs
a=`awk '/-- DUT/{nr[NR]; nr[NR+2]}; NR in nr' output/run${(l:6::0:)runnum}/gear_${runnum}_it${iteration}.xml`
a=${a:gs/DUT /DUT}
b=(${(s: :)a})
echo "run${(l:6::0:)runnum}/gear_${runnum}_it${iteration}.xml"
echo $b[2] $b[6] $b[8] $b[10] $b[11]
echo $b[17] $b[21] $b[23] $b[25] $b[26]


