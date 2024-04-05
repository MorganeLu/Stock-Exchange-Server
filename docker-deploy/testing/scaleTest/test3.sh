#!/bin/bash
# 多client单核
screen -ls | grep -oP '(?<=\t)\d+(?=\.)' | xargs -I {} screen -X -S {} quit

num_clients=$(nproc)

# 在 screen 会话中启动客户端程序
for ((i=0; i<num_clients; i++)); do
    screen -d -m -S "session_$i"

    screen -S "session_$i" -X stuff "./helper.sh $i > ./output3/client$((i)).txt
    "
done