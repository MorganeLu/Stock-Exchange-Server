#!/bin/bash
# 多client单核
screen -ls | grep -oP '(?<=\t)\d+(?=\.)' | xargs -I {} screen -X -S {} quit

num_clients=5
cpu_core=0

# 在 screen 会话中启动客户端程序
for ((i=0; i<num_clients; i++)); do
    screen -d -m -S "session_$i"
    
    screen -S "session_$i" -X stuff "./test1.sh > ./output2/client$((i)).txt
    "
done

#screen -ls | grep -oP '(?<=\t)\d+(?=\.)' | xargs -I {} screen -X -S {} quit
