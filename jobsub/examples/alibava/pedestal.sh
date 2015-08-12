#!/bin/zsh

################################
#                              #
# ALiBaVa Analysis - Pedestal  #
#                              #
################################

# usage: source pedestal.sh runnumber(s)

#To calculate Pedestals
# you will need to run these templates
TEMPLATELIST=('alibava-convert-ped' 'alibava-pedestal' 'alibava-commonmode' 'alibava-pedestal2')

for TEMPLATE in $TEMPLATELIST; do
	jobsub -c config.cfg -csv runlist.csv $TEMPLATE $@
done
