#!/bin/bash
#单client多核

fileNum=30
loopNum=100
client=$1

start_time=$(date +%s.%N)
echo "Running client No.$1 on core No.$1"
total_requests=0

for((i=0; i<loopNum; i++)); do
    for((j=0; j<fileNum; j++)); do
        if ((j<5)); then
            filename="../xml/create$(($j+1)).xml"
        fi
        if((j>=5)); then
            filename="../xml/test$(($j-4)).xml"
        fi
        if [ -f "$filename" ]; then
            # ../client "$filename" &
            # client_pid=$!
            # taskset -cpa $(($client)) $client_pid > /dev/null
            taskset -c 0-$(($client)) ../client "$filename" &
            client_pid=$!
            wait $client_pid
            ((total_requests++))
        fi
    done
done
end_time=$(date +%s.%N)
execution_time=$(echo "$end_time - $start_time" | bc)

throughput=$(echo "scale=2; $total_requests / $execution_time" | bc)

echo "Execution time: $execution_time seconds"
echo "Total Requests: $total_requests"
echo "Throughput: $throughput requests/second"
echo "================================================"
