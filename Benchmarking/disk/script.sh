#!/bin/bash

#SBATCH --partition=interactive
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=1RS1MB.out
#SBATCH --wait-all-nodes=1

hostname
srun ./mydisk 1 RS 1
