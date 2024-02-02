#!/bin/bash
i=100000000
rm -rf "./results"
mkdir "results"

while ((i<100000000 * 10**6)); do 
  # 5 runs per each
  x=2
  while ((x < 128)); do 
    psrssum=0
    p1sum=0
    p2sum=0
    p3sum=0
    p4sum=0
   
    for ((y=0; y<5; y+=1)); do
      output=$(./main $i $x)

      psrssum=$(($(echo "$output" | awk -F',' '{s+=$1} END {print s}')+$psrssum))
      p1sum=$(($(echo "$output" | awk -F',' '{s+=$2} END {print s}')+$p1sum))
      p2sum=$(($(echo "$output" | awk -F',' '{s+=$3} END {print s}')+$p2sum))
      p3sum=$(($(echo "$output" | awk -F',' '{s+=$4} END {print s}')+$p3sum))
      p4sum=$(($(echo "$output" | awk -F',' '{s+=$5} END {print s}')+$p4sum))
    done

    psrsavg=$(($psrssum/5))
    p1avg=$(($p1sum/5))
    p2avg=$(($p2sum/5))
    p3avg=$(($p3sum/5))
    p4avg=$(($p4sum/5))
    echo "Finished run. Outputting"
    echo "psrs: $psrsavg phase1 $p1avg phase2 $p2avg phase3 $p3avg phase4 $p4avg" >> "results/$x-processors-$i"
    ((x=x*2))
  done
  ((i=i*10))
done
