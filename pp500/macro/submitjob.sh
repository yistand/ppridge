#!/bin/bash -l
#PBS -N pp500dihadron
#PBS -l nodes=1:ppn=1,walltime=20:00:00
#PBS -r n
#PBS -V
#PBS -q hep
#PBS -j oe

cd $PBS_O_WORKDIR
echo $PWD

echo "Job Start at `date`"

./test.sh

echo "Job End at `date`"

