#!/bin/bash

#SBATCH --partition=interactive
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output=1RWS1K.out
#SBATCH --wait-all-nodes=1

hostname

srun ./myram 1 RWS 1
