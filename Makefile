build:
	g++ tester.cpp barrier.cpp BinarySearch.cpp main.cpp -g -o main
	g++ tester.cpp qs.cpp -g -o qs

#!/bin/bash
i=1000000
while ((i<100000000)); do 
  # 5 runs per each
  x=0
  for ((x < 128)); do 
        psrssum=0
    p1sum=0
    p2sum=0
    p3sum=0
    p4sum=0
   
    for ((y=0; y<5; y+=1)); do
      output=$(./main $i $x)

      psrssum=$(($(echo "$output" | awk -f',' '{s+=$1} end {print s}')+$psrssum))
      p1sum=$(($(echo "$output" | awk -f',' '{s+=$2} end {print s}')+$p1sum))
      p2sum=$(($(echo "$output" | awk -f',' '{s+=$3} end {print s}')+$p2sum))
      p3sum=$(($(echo "$output" | awk -f',' '{s+=$4} end {print s}')+$p3sum))
      p4sum=$(($(echo "$output" | awk -f',' '{s+=$5} end {print s}')+$p4sum))
    done

    psrsavg=$(($psrssum/5))
    p1avg=$(($p1sum/5))
    p2avg=$(($p2sum/5))
    p3avg=$(($p3sum/5))
    p4avg=$(($p4sum/5))

  echo "psrs: $psrsavg phase1 $p1avg phase2 $p2avg phase3 $p3avg phase4 $p4avg" >> "$x-processors-$i"
  ((x*=2))
  done

  ((i*=2))
done
