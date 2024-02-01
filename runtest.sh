#!/bin/bash
i=1000000
while ((i<10000000000)); do 

  # 5 runs per each
  psrsSum=0
  p1Sum=0;
  p2Sum=0
  p3Sum=0
  p4Sum=0
  for ((x=0 ; x<5 ; x++)); do 
    output=$(./main $i 4)

    psrsSum=$(($(echo "$output" | awk -F',' '{s+=$1} END {print s}')+$psrsSum))
    p1Sum=$(($(echo "$output" | awk -F',' '{s+=$2} END {print s}')+$p1Sum))
    p2Sum=$(($(echo "$output" | awk -F',' '{s+=$3} END {print s}')+$p2Sum))
    p3Sum=$(($(echo "$output" | awk -F',' '{s+=$4} END {print s}')+$p3Sum))
    p4Sum=$(($(echo "$output" | awk -F',' '{s+=$5} END {print s}')+$p4Sum))
  done
  echo $psrsSum
  echo $p1Sum
  echo $p2Sum
  echo $p3Sum
  echo $p4Sum

  ((i*=10))
done
