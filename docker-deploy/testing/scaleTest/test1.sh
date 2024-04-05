#!/bin/bash
#单client多核

fileNum=30
loopNum=10
num_clients=1
total_cores=$(nproc)

for((coreNum=1; coreNum<=total_cores; coreNum=coreNum*2)); do
    start_time=$(date +%s.%N)
    echo "Running on $coreNum cores"
    total_requests=0

    for((i=0; i<loopNum; i++)); do
        for((j=0; j<fileNum; j++)); do
            if ((j<5)); then
                filename="../xml/create$((i+1)).xml"
            fi
            if((j>=5)); then
                filename="../xml/test$((i-4)).xml"
            fi
            if [ -f "$filename" ]; then
                # taskset -c 0-$(($coreNum-1)) ../client "$filename" &
                ../client "$filename" &
                client_pid=$!
                taskset -cpa 0-$(($coreNum-1)) $client_pid > /dev/null
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
done
