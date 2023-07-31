# Computer_Networks

## Libpcap

- Packet sniffer which can sniff packets to and from a paricular ip address and port.
- Used Packet Capture(Pcap) Library to capture network packets.
- Works on all Operating Systems.
- Future Scope : Add more functionality and can be converted into a system command using alias system call, Can be used as offline network analysis Tool

How to run :

```
Install libpcap: sudo apt-get install libpcap-dev
Compile :        gcc libcapv0.c -o libcapv0 -lpcap
Rum :            sudo ./libcapv0 <ip address> <port>
```

