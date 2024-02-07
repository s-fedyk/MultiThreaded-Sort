#!/bin/bash
m=4096000000
i=$m
rm -rf "./qsresults"
mkdir "qsresults"

while ((i<$m*2**6)); do 
  # 5 runs per each

    #warmup
    for ((y=0; y<2; y+=1)); do
        ./qs $i
    done

    qssum=0
    for ((y=0; y<5; y+=1)); do
      output=$(./qs $i)
      echo $output
      qssum=$((output + qssum))
    done

    avg=$(($qssum/5))
    echo "Finished run. Outputting"
    echo "total: $avg" >> "qsresults/$i"
  ((i=$i*2))
done
