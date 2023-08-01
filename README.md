# Computer_Networks

#### Output Screenshots in the respective folders.

## Libpcap

- Packet sniffer which can sniff packets to and from a particular ip address and port.
- Used Packet Capture(Pcap) Library to capture network packets.
- Works on all Operating Systems (Mac, Linux, Windows).
- Logs the packet headers into log.txt file.
#### Future Scope : Can be converted into a system command using alias system call, Can be used as offline network analysis Tool.

How to run :

```
Install libpcap: sudo apt-get install libpcap-dev
Compile :        gcc libcapv0.c -o libcapv0 -lpcap
Rum :            sudo ./libcapv0 <ip address> <port>
```
You need to change the value of devname(Line 58) to your device name.

To find the device name of your system Run:
```
ifconfig
```

## Distributed File System

Files contents are distributed across different server, this information with a main super server "dfs". some portion of the file is stored local servers. Client requests for any file and super server "dfs" will fetch the data from local servers and send the data along with the information (ip address and ports) of servers, to whom client then requests for the remaining data.

- Uses unix domain sockets for remote servers and tcp for other servers.
- All the severs continuosly run and handle clients continuously without the need for rerunning.
- Using pthreads to create seperate sockets to handle each client concurrently.
- Using make to simplify the compilation process.

How to run : 

Compile :
```
 make
```
Running servers : 
#### Order is important.
```
sudo ./dfs
sudo ./locals4
sudo ./locals5
sudo ./s1
sudo ./s2
sudo ./s3
```
Running client :
```
sudo ./c
```

