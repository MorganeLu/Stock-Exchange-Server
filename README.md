# Exchange Matching Engine

## Getting started

Since we provide docker to make the execution easier, you can directly use
```shell
sudo docker-compose up
```

If you see some problems, you can try this command before your next try.
```shell
sudo docker-compose build --no-cache
```

During your replicate testing, if you see this error "Error starting userland proxy: listen tcp4 0.0.0.0:5432: bind: address already in use", please try `systemctl status postgresql` to check tha status. If it is active, you can use `sudo systemctl start postgresql` to shut it down.

## Testing

In the testing part, we have provide some XML files for testing, and you can find them by `cd /testing/xml`.

### Single Client with Multiple Cores

To get results by running single client on multiple cores, please follow:
```shell
cd /testing/scaleTest
chmod +x test1.sh
./test1.sh
```
The results would be shown on the terminal.
By using docker to start the server, it runs on all cores by default. If you would like to limit it on a certain cores, please modify `command: bash -c "cd /code && ./server"` to `command: bash -c "cd /code && taskset -c 0,1 ./server"`.

### Multiple Client with Multiple Cores
To get results by running multiple client on multiple cores, please follow:
```shell
cd /testing/scaleTest
chmod +x test2.sh
./test2.sh
```
or
```shell
cd /testing/scaleTest
chmod +x test3.sh
./test3.sh
```
The results can be seen by:
```shell
cat output2/client0.txt
cat output2/client1.txt
cat output2/client2.txt
cat output2/client3.txt
cat output2/client4.txt
cat output2/client5.txt
cat output2/client6.txt
cat output2/client7.txt
```
or
```shell
cat output3/client0.txt
cat output3/client1.txt
cat output3/client2.txt
cat output3/client3.txt
cat output3/client4.txt
cat output3/client5.txt
cat output3/client6.txt
cat output3/client7.txt
```

## Contribution
**Developed by Xinyi Xie and Kaixin Lu**