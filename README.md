# Computer_Networks

#### Output Screenshots in the respective folders.

## Libpcap

- Packet sniffer which can sniff packets to and from a particular ip address and port.
- Used Packet Capture(Pcap) Library to capture network packets.
- Works on all Operating Systems (Mac, Linux, Windows).
- Logs the packet headers into log.txt file.
- Can be used as offline network analysis Tool.

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

