# Computer_Networks

#### Output Screenshots in the respective folders.

## Libpcap

* Packet sniffer which can sniff packets to and from a particular ip address and port.
* Used Packet Capture(Pcap) Library to capture network packets.
* Works on all Operating Systems (Mac, Linux, Windows).
* Logs the packet headers into log.txt file.
* Can be used as offline network analysis Tool.

Install libpcap:
```
  sudo apt-get install libpcap-dev
```
Compile :
```
  gcc libcapv0.c -o libcapv0 -lpcap
```
Run :
```
  sudo ./libcapv0 <ip address> <port>
```
You need to change the value of devname(Line 58) to your device name.

To find the device name of your system Run:
```
  ifconfig
```

## Distributed File System

* Uses unix domain sockets for remote servers and tcp for other servers.
* All the severs continuosly run and handle clients continuously without the need for rerunning.
* Using pthreads to create seperate sockets to handle each client concurrently.
* Using make to simplify the compilation process.

Files contents are distributed across different server, this information with a main super server "dfs". some portion of the file is stored local servers. Client requests for any file and super server "dfs" will fetch the data from local servers and send the data along with the information (ip address and ports) of servers, to whom client then requests for the remaining data.

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

## Ingenerate Super Server

* Uses tcp,raw, unix domain sockets
* Uses Libpcap to sniff out the network traffic in the subnet


![35998788-63cb-4303-8e22-437419f5f069](https://github.com/Vijayadityaraj-source/Computer_Networks/assets/103420230/ae04fa4a-ee00-4fb1-b3be-246c6f7a31a4)

* A service server "si" serves the client,the tracer process "T" observes the "si" and sends the service port number to the Ingenerate super server "Iss"
* The "Iss" doesn't let the "si" accept the client directly. The "Iss" accepts the clients using the "si" port and sends the accepted clients to the corresponding "si"
* Client "ci" also gets service port numbers from Tracer "T" and can get connected for service
* Whenever "T" observes a new server, it informs all clients and "Iss" about the server

Compile :
```
make
```

Running servers : 
#### Order is important.

```
sudo ./ISS
sudo ./t
sudo ./s
sudo ./c
```
