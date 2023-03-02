#!/bin/bash
# Time to generate 100 generations of Conway's Game of Life
printf "Comparing old algorithm with new assembly algorithm with the same parameters\n\n"
printf "Old Version:\n"
time ./conway 1000 10 1 100
printf "New Version:\n"
time ./cmake-build-release/conway 1000 10 1 100
