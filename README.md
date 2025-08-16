# Ethical Hacking — Practical Labs

This repository contains coursework and solutions for the **Ethical Hacking** course at  
**Brandenburg University of Technology Cottbus-Senftenberg (BTU)**, Summer Term 2025.  

Each lab demonstrates offensive security techniques with hands-on tasks, exploits, and analysis.

---

## 🧪 Labs Overview

### 🔹 Lab 1 — MD5 Collision Attacks
- **Topic**: Cryptographic hash functions & collision resistance  
- **Tasks**:  
  1. Generate colliding files with `md5_fastcoll`  
  2. Show MD5 append property  
  3. Chosen-prefix collision on PNG files with HashClash  
- **Results**: Different files/images sharing the same MD5 hash.  
- **Property Broken**: Collision resistance.  
- **Key Insight**: MD5 is cryptographically broken and should not be used.  

---

### 🔹 Lab 2 — ICMP Redirect & Man-in-the-Middle Attacks
- **Topic**: Exploiting ICMP redirect to hijack network traffic  
- **Tasks**:  
  1. Network setup with victim, routers, attacker  
  2. ICMP redirect injection with Scapy  
  3. Passive sniffing with `tcpdump`  
  4. Live packet modification with `iptables` + Scapy  
- **Results**:  
  - Confidentiality breach: attacker reads victim’s messages.  
  - Integrity breach: attacker changes “I failed!” → “I passed”.  
- **Key Insight**: ICMP redirects enable MITM unless disabled; routing security is critical.  

---

### 🔹 Lab 3 — Buffer Overflow Exploits
- **Topic**: Memory safety vulnerabilities in C programs  
- **Setup**: Custom student management system (BTU), protections disabled (StackGuard, NX, ASLR, etc.) or selectively enabled for testing.  
- **Tasks**:
  1. **Analyze vulnerabilities**  
     - `check_password()` → stack-based overflow via `strcpy()`  
     - `add_student()` → heap corruption via unsafe pointer copying  
  2. **Basic Buffer Overflow**  
     - Injected shellcode (`exit(5)`) into stack buffer.  
     - Overwrote return address with buffer location → executed payload successfully.  
  3. **Impact of Protections**  
     - NX → blocked shellcode execution (segfault).  
     - StackGuard → detected canary corruption, aborted execution.  
     - ASLR → randomized addresses, exploit unreliable.  
  4. **Advanced Attacks**  
     - *ret2func*: Redirected execution to hidden function `exmatriculate()` to unenroll student without password.  
     - *ret2libc*: Spawned a shell using `system("/bin/sh")` despite NX.  
  5. **Bypassing StackGuard via Heap Overflow**  
     - Exploited `add_student()` to overwrite GOT entry for `write_log()`.  
     - Redirected calls to `exmatriculate()` → student removed without password.  
  6. **Secure Fixes**  
     - Replaced `strcpy()` with `strncpy()`, enforced null-termination.  
     - Suggested full RELRO, PIE, and ASLR for modern defense.  
- **Results**:  
  - Demonstrated both stack and heap exploitation.  
  - Showed layered defenses are needed; no single mitigation suffices.  
- **Key Insight**: Secure coding (avoid `strcpy`), compiler defenses (StackGuard, RELRO), and system protections (ASLR, NX) together are necessary.  

---

### 🔹 Lab 4 — SQL Injection & Cross-Site Scripting (XSS)
- **Topic**: Web application vulnerabilities in authentication and input handling  
- **Tasks**:  
  1. Extracted all user credentials (including `admin`) via SQL Injection in `login.php`.  
  2. Enumerated hidden pages using SQL Injection in `search.php`.  
  3. Found and confirmed a reflected XSS vector in `search.php`.  
  4. Hijacked user sessions by stealing cookies with a crafted malicious URL.  
  5. Built a persistent self-replicating XSS worm in `comments.php` that bypassed filters and posted malicious code automatically.  
  6. Patched vulnerabilities with secure coding fixes: prepared SQL statements, CSRF validation, and output sanitization.  
- **Results**:  
  - Retrieved sensitive data, hidden pages, and compromised user sessions.  
  - Successfully demonstrated both reflected and persistent XSS attacks.  
  - Proposed and implemented mitigations to secure the application.  
- **Key Insight**: SQLi and XSS remain critical threats — secure coding (prepared statements, input validation, output escaping) is essential to prevent full system compromise.  

---

### 🔹 Lab 5 — Android Repackaging & Mobile Malware
- **Topic**: Reverse engineering and modifying Android applications  
- **Tasks**:  
  1. **App Installation**  
     - Installed `Unpwnable.apk` onto an Android emulator with ADB.  
     - Observed challenge: app required a secret string.  
  2. **Extracting the Secret**  
     - Decompiled APK with `apktool` and analyzed smali code.  
     - Found AES/ECB-encrypted Base64 string in `MainActivity.smali`.  
     - Decrypted with Python script → secret = `"I want to believe"`.  
  3. **Bypassing Root Detection**  
     - App crashed on rooted devices due to root checks in smali code.  
     - Patched `onCreate` to skip detection logic.  
     - Rebuilt, signed, and aligned APK → app worked on rooted emulator.  
  4. **Injecting Malicious Functionality**  
     - Added new `BroadcastReceiver` listening for `BOOT_COMPLETED`.  
     - Implemented smali code to wipe all user contacts automatically on reboot.  
     - Modified `AndroidManifest.xml` to register receiver + request permissions.  
     - Rebuilt and signed APK (`Unpwnable-DeleteAll.apk`).  
     - Verified that rebooting emulator triggered automatic contact deletion.  
- **Results**:  
  - Successfully extracted secrets, bypassed security checks, and repackaged app.  
  - Demonstrated a **repackaging attack** with malicious payload (contact deletion).  
- **Key Insight**:  
  - Mobile apps are vulnerable to static analysis and tampering.  
  - Without strong protections (code obfuscation, integrity checks, Play Store vetting), apps can be modified into malware.  

---

### 🔹 Lab 6 — Spectre Vulnerability & Side-Channel Attacks
- **Topic**: Exploiting speculative execution and CPU cache timing side-channels  
- **Tasks**:  
  1. **Cache Timing (Hit vs. Miss)**  
     - Measured access times using `__rdtscp` and `_mm_clflush`.  
     - Established threshold (~406 cycles) to distinguish cache hits from misses.  
  2. **FLUSH+RELOAD Side Channel**  
     - Implemented cache flush, victim access, and reload cycle measurements.  
     - Leaked a one-byte secret by detecting which cache line was accessed.  
  3. **Speculative Execution Effects**  
     - Analyzed how branch prediction speculatively executes code.  
     - Showed that speculative loads leave measurable cache traces even if rolled back.  
  4. **Spectre Proof-of-Concept**  
     - Built attack using training loops to mis-train the branch predictor.  
     - Flushed bounds check from cache → induced misprediction.  
     - Successfully leaked secret array indices through speculative execution.  
  5. **Spectre Attack (Single Byte Leak)**  
     - Exploited speculative out-of-bounds access in `restrictedAccess()`.  
     - Used FLUSH+RELOAD to recover first secret byte (“S”).  
  6. **Improving Reliability**  
     - Ran attack multiple times with busy-wait delays.  
     - Used majority voting to filter noise and increase success rate.  
  7. **Stealing the Entire Secret String**  
     - Extended attack loop to leak all bytes of secret `"Some Secret Value"`.  
     - Successfully reconstructed full secret string via cache timing.  
- **Results**:  
  - Implemented a working **Spectre exploit** in C using cache side channels.  
  - Demonstrated both single-byte and multi-byte secret exfiltration.  
  - Showed statistical methods (retries + voting) greatly improve attack reliability.  
- **Key Insight**:  
  - Spectre abuses **speculative execution + cache timing** to bypass memory isolation.  
  - Even sandboxed secrets can be exfiltrated at the hardware level.  
  - Fixing Spectre requires **hardware redesigns**; software mitigations only reduce risk at performance cost.  

---

## ⚙️ Tools & Techniques
- **HashClash** (MD5 collisions)  
- **Scapy** (packet crafting & spoofing)  
- **GDB** (debugging, exploit development)  
- **Python scripting** (payload generation, automation)  
- **tcpdump / netcat** (traffic analysis, testing)  
- **gcc with hardened/disabled flags** (to test protections)  
- **Burp Suite / OWASP ZAP** (web exploitation)  
- **curl & browser DevTools** (HTTP request manipulation, debugging)  
- **apktool, JADX, Androwarn** (APK reverse engineering & modification)  

---

## 📌 Notes
- All labs are for **educational purposes only**.  
- Exploits were developed in a controlled environment (VM, Docker, Android Emulator, Remote Lab VM).  
- Each solution folder includes **writeups, code, and proof-of-concepts**.  

---

## 📜 License
This repository is for **academic use only** within the Ethical Hacking course at BTU.  
Unauthorized or malicious use of these materials is strictly prohibited.
