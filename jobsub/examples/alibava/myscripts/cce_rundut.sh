#!/bin/bash


if [ $1 = 0 ]; then
PEDRUNLIST=(602 626 533 538 499 515 530 543 753 771 788 546 550 554 557 562 567 570 575 578 583 409 414 417 463 481 496 719 736 750 422 426 431 436 441 446 448 453 456 461 405 401 304 330 351 397 677 698 716 391 386 382 378 373 369 363 359 354 302 297 210 228 245 294 585 629 661 289 282 279 274 269 265 260 668 255 248 252 163 162 42 161 193 207 160 159 151 158 129 125 19 121 115 98 111 23 26 29 32 15 11 645)
RUNLIST=(114 120 124 128 132 149 150 153 154 155 156 157)
BONDS='$0:19-33$ $0:35-121$ $1:23-35$ $1:39-76$ $1:78-125$'
TIMECUTMIN=25.0
TIMECUTMAX=35.0
fi

if [ $1 = 10 ]; then
PEDRUNLIST=(602 626 533 538 499 515 530 543 753 771 788 546 550 554 557 562 567 570 575 578 583 409 414 417 463 481 496 719 736 750 422 426 431 436 441 446 448 453 456 461 405 401 304 330 351 397 677 698 716 391 386 382 378 373 369 363 359 354 302 297 210 228 245 294 585 629 661 289 282 279 274 269 265 260 668 255 248 252 163 162 42 161 193 207 160 159 151 158 129 125 19 121 115 98 111 23 26 29 32 15 11 645)
RUNLIST=(669 671 672)
BONDS='$0:19-33$ $0:35-121$ $1:23-35$ $1:39-76$ $1:78-125$'
TIMECUTMIN=7.0
TIMECUTMAX=17.0
fi

if [ $1 = 1 ]; then
PEDRUNLIST=(1001 1006 1011 1015 1017 1021 1025 1029 1033 1042 1050 1054 1058 1072 1093 1099 1112 1125 1140 1144 1162 1177 1181 1201 1217 1220 1236 1251 1256 1269 1275)
RUNLIST=(1004 1274 1010 1014 1016 1020 1024 1028 1273 1041 1049)
BONDS='$0:20-22$ $0:24-29$ $0:31-50$ $0:56-122$ $1:24-112$ $1:114-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $1 = 2 ]; then
PEDRUNLIST=(2003 2007 2013 2018 2021 2026 2030 2047 2068 2072 2079 2097 2112 2115 2132 2148 2153 2170 2184 2187 2211 2226 2230 2252 2269 2278 2287 2291 2302 2306 2311 2315 2325 2339 2349) 
RUNLIST=(2006 2011 2012 2019 2020 2027 2075 2301 2305 2310 2314 2348) 
BONDS='$0:24-126$ $1:24-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $1 = 3 ]; then
PEDRUNLIST=(3001 3005 3010 3015 3017 3021 3025 3029 3033 3037 3041 3057 3073 3076 3095 3112 3116 3132 3150 3153 3166 3188 3191 3210 3228 3235)
RUNLIST=(3004 3008 3009 3040 3036 3032 3028 3024 3020 3016) 
BONDS='$0:20-22$ $0:24-29$ $0:31-50$ $0:56-122$ $1:24-112$ $1:114-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $1 = 4 ]; then
PEDRUNLIST=(4000 4016 4032 4035 4057 4072 4088 4105 4108 4130 4145 4148 4164 4178 4182 4198 4213) 
#PEDRUNLIST=(4198) 
RUNLIST=(4197)
BONDS='$0:19-33$ $0:35-121$ $1:23-35$ $1:39-76$ $1:78-125$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi


if [ $1 = 5 ]; then
PEDRUNLIST=(5001 5025 5030 5033 5038 5041 5046 5049 5054 5058 5063 5067 5076 5094 5107 5124 5127 5151 5161 5189 5217 5236 5246 5266 5281 5285 5299 5315 5319 5338) 
RUNLIST=(5064 5057 5056 5048 5024 5031 5032 5039 5040 5047)
BONDS='$0:20-122$ $1:24-127$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $1 = 6 ]; then
PEDRUNLIST=(6001 6006 6011 6014 6019 6022 6027 6030 6035 6038 6042 6058 6074 6077 6094 6110 6113 6127 6135)	
RUNLIST=(6004 6005 6012 6013 6020 6021 6028 6029 6036 6037)
BONDS='$0:20-122$ $1:24-127$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $2 = 'nocut' ]; then
CONFIG=config/noCuts_config.cfg
else
CONFIG=config/dut$1_cce_config.cfg
cp config/noCuts_config.cfg $CONFIG
sed -i "/Bonds/ c\Bonds=$BONDS" $CONFIG 
sed -i "/TimeCutMin/ c\TimeCutMin=$TIMECUTMIN" $CONFIG 
sed -i "/TimeCutMax/ c\TimeCutMax=$TIMECUTMAX" $CONFIG 
sed -i "/EventIDDifference/ c\EventIDDifference=@EventIDDiff@" $CONFIG 
fi 
echo Config file: $CONFIG

CSV=runlistfiles/cce_runlist_all.csv
PEDCSV=runlistfiles/pedestal_runlist_all.csv
ROOTFILEPED=\'plots_pedestal.C($1)\'
ROOTFILECLU=\'cce_plots_cluster.C($1,0)\'
ROOTFILEHIT=\'cce_plots_hit.C($1,0)\'

####### Pedestal
cp myscripts/template_pedestal.sh myscripts/dut$1_pedestal.sh

sed -i "/RUNLIST/ c\RUNLIST=($PEDRUNLIST)" myscripts/dut$1_pedestal.sh
sed -i "/CONFIG/ c\CONFIG=$CONFIG" myscripts/dut$1_pedestal.sh
sed -i "/CSV=/ c\CSV=$PEDCSV" myscripts/dut$1_pedestal.sh
sed -i '/jobsub/ c\jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $TEMPLATE $RUNLIST' myscripts/dut$1_pedestal.sh
sed -i "/root/ c\root -q -b $ROOTFILEPED" myscripts/dut$1_pedestal.sh

#######  Reco 
cp myscripts/template_reco.sh myscripts/dut$1_cce_reco.sh

sed -i "/RUNLIST/ c\RUNLIST=($RUNLIST)" myscripts/dut$1_cce_reco.sh
sed -i "/CONFIG/ c\CONFIG=$CONFIG" myscripts/dut$1_cce_reco.sh
sed -i "/CSV=/ c\CSV=$CSV" myscripts/dut$1_cce_reco.sh
sed -i '/jobsub/ c\jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $TEMPLATE $RUNLIST' myscripts/dut$1_cce_reco.sh
sed -i "/root/ c\root -q -b $ROOTFILECLU" myscripts/dut$1_cce_reco.sh


if [ $2 != 'nocut' ] 
then
####### Telescope 
cp myscripts/template_telescope.sh myscripts/dut$1_cce_telescope.sh

sed -i "/RUNLIST/ c\RUNLIST=($RUNLIST)" myscripts/dut$1_cce_telescope.sh
sed -i "/CONFIG/ c\CONFIG=$CONFIG" myscripts/dut$1_cce_telescope.sh
sed -i "/CSV=/ c\CSV=$CSV" myscripts/dut$1_cce_telescope.sh
sed -i '/jobsub/ c\jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $TEMPLATE $RUNLIST' myscripts/dut$1_cce_telescope.sh



####### Merge 
cp myscripts/template_merge.sh myscripts/dut$1_cce_merge.sh

sed -i "/RUNLIST/ c\RUNLIST=($RUNLIST)" myscripts/dut$1_cce_merge.sh
sed -i "/CONFIG/ c\CONFIG=$CONFIG" myscripts/dut$1_cce_merge.sh
sed -i "/CSV=/ c\CSV=$CSV" myscripts/dut$1_cce_merge.sh
sed -i '/jobsub/ c\jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $TEMPLATE $RUNLIST' myscripts/dut$1_cce_merge.sh
sed -i "/root/ c\root -q -b $ROOTFILEHIT" myscripts/dut$1_cce_merge.sh

fi

##### RUN

#source myscripts/dut$1_pedestal.sh
#source myscripts/dut$1_cce_reco.sh

if [ $2 != 'nocut' ] 
then
#source myscripts/dut$1_cce_telescope.sh
#source myscripts/dut$1_cce_merge.sh
fi

#cp -R run00* $DUST/birdfolders
#rm -r run00* 


