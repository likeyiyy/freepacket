#!/bin/bash
generator=(1,2,3,4,5,6,7,8)
manager=(1,3,5,7,9,11,13,15)
for g in {1..8}
do
    for m in {3,5,7,9,11,13,15}
    do
        for pktlen in {64,128,256,512,1024,1280,1500}
        do
        command="./simulation -g $g -p $g -m $m --pktlen $pktlen"
        echo "####################### start test simulator #########################" >> ./log/simulation.log
        $command >> ./log/simulation.log &
        pid=`ps aux | grep simulation | grep pktlen | awk '{print $2}'`
        echo "simulator PID is: "$pid
        echo "####################### start test simulator pid:$pid#########################" | tee -a log/ttop.log
        ./bin/ttop -p $pid -t 60 -i 5000 | tee -a log/ttop.log
        gener_speed=`tail -n 40 log/simulation.log | awk 'BEGIN{total=0}{total+=$2}END{print total/40}'`
        cpu_usage=`tail -n 9 log/ttop.log | awk 'BEGIN{total=0}{total+=$4}END{print total/9}'`
        mem_usage=`tail -n 9 log/ttop.log | awk 'BEGIN{total=0}{total+=$3}END{print total/9}'`
        performance=`echo "$gener_speed $cpu_usage $mem_usage)" | awk '{print $1 / ($2 * $3)}'`
        echo "G: $g, P: $g, M: $m, PKTLEN: $pktlen, GENER_SPEED: $gener_speed, CPU_USAGE: $cpu_usage, MEM_USAGE: $mem_usage, Perfor: $performance" | tee -a log/performance.log
        kill -9 $pid
        done
    done
done
exit
