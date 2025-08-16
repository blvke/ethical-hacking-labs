Cyber Security Lab – MD5 Collision Attacks

Summer Term 2025 – BTU Cottbus

Author: Safia Bakr

# Task 1: Generating Two Different Files with the Same MD5 Hash

## Goal

Demonstrate that MD5 collision resistance is broken by generating two different binary files with the same MD5 hash using md5_fastcoll.

## Setup

Operating System: Ubuntu 24.04 (VirtualBox)

Tool: HashClash (md5_fastcoll)

Environment: Built with Boost 1.65.1

Prefix: CyberSecLab_MD5CollisionPrefix_2025_Testing__HASH__BLOCK (exactly 64 bytes)

## Steps

### Prefix Creation:

echo -n "CyberSecLab_MD5CollisionPrefix_2025_Testing__HASH__BLOCK" > prefix.txt
printf '%-64s' "$(cat prefix.txt)" > prefix.txt

### Generate Colliding Files:

./bin/md5_fastcoll -p prefix.txt -o msg1.bin msg2.bin

### Verify MD5:

md5sum msg1.bin msg2.bin

### Result:

154b11ab9e2ebfd8b4e278c42aefa64f  msg1.bin
154b11ab9e2ebfd8b4e278c42aefa64f  msg2.bin

## Task 1 Answers

Q1: What happens if the prefix is not 64 bytes?
The tool may fail or produce invalid collisions because MD5 processes 64-byte blocks. Misalignment affects padding and internal states.

Q2: What happens with 64-byte prefix?
The tool works correctly; collision happens cleanly in the next MD5 block.

Q3: Are the generated 128 bytes completely different?
No, only specific, controlled bit flips exist between the two collision blocks to synchronize final MD5 states.

# Task 2: Understanding MD5’s Append Property

## Goal

Prove that if two files collide under MD5, appending identical data preserves the collision.

## Setup

### Inputs: msg1.bin and msg2.bin

### Suffix: ThisIsTheSuffixForTestingTheMD5AppendProperty

## Steps

### Suffix Creation:

echo "ThisIsTheSuffixForTestingTheMD5AppendProperty" > suffix.txt

### Concatenate Suffix:

cat msg1.bin suffix.txt > extended1.bin
cat msg2.bin suffix.txt > extended2.bin

### Verify MD5:

md5sum extended1.bin extended2.bin

### Result:

8713b45bc6c00adaa99077727df4bfeb  extended1.bin
8713b45bc6c00adaa99077727df4bfeb  extended2.bin

## Theoretical Explanation

Since MD5 is iterative and compresses in 64-byte blocks, if the first part collides, the same suffix processed from the same Intermediate Hash Value (IHV) results in identical final hashes.

## Security Implication

This property allows an attacker to take two colliding files, append malicious content, and still maintain identical MD5 hashes, creating serious risks for file integrity and security systems.

# Task 3: Generating Two Files with the Same MD5 Hash

## 1. Cryptographic Property Broken

The fundamental cryptographic property broken in this attack is **collision resistance**. Collision resistance ensures that it is computationally infeasible to find two distinct messages m_1 = m_2 such that:

```
MD5(m1) = MD5(m2)
```

### Scenario 1: Modify One File Only

The attacker must find a second input `m2` such that `MD5(m2) = MD5(m1)` for a known `m1`. This is a **second preimage attack**, which is generally harder than finding any two colliding inputs.

### Scenario 2: Modify Both Files

The attacker controls both `m1` and `m2` and can make them collide under MD5. This is a straightforward **collision attack**, which is practically feasible for MD5 using tools like HashClash.

---

## 2. Is MD5 Vulnerable?

Yes. MD5 is no longer secure. Collision attacks are practical and have been demonstrated repeatedly.

- **Scenario 1**: Still difficult (second preimage not broken).
- **Scenario 2**: Fully broken (collisions can be created efficiently).

---

## 3. Attack Implementation (Scenario 2)

This attack uses the HashClash toolkit to construct two different PNG files that share the same MD5 hash but display different content.

### A. Understanding the Python Script (`collision.py`)

#### Step 1: Constructing the Prefix (80 bytes)

We prepare a prefix of exactly 80 bytes consisting of:

- 33 bytes: Standard PNG header including IHDR
- 38 bytes: A valid `tEXT` chunk for alignment
- 9 bytes: Beginning of the `mARC` chunk (length = 0x75, type = 'mARC', data = '!')

Why 80 bytes?

- MD5 processes 64 bytes and buffers 16.
- The collision block begins where MD5 left off, completing the next block with 48 bytes and then adding two full 64-byte blocks (192 bytes total).

#### Step 2: Generating the Collision

The script runs:

```bash
~/hashclash/scripts/generic_ipc.sh prefix
```

This generates:

- `collision1.bin` and `collision2.bin` (each 192 bytes)
- These represent 3 MD5 blocks crafted to produce the same MD5 hash despite differing content

#### Step 3: Building the Suffix and Divergence

- Complete the `mARC` chunk with 8 bytes (`RealHash`) so that it reaches 117 bytes (0x75)
- Add a `jUMP` chunk:
  - 4 bytes: length
  - 4 bytes: type = 'jUMP'
  - 244 bytes: filler (e.g., b"e" * 244)
  - 4 bytes: fake CRC
- Then append the payload from `d2`, followed by `d1` to facilitate visual divergence

### Divergent Paths

- In File A: mARC length = 0x75 → parses through `jUMP`
- In File B: mARC length = 0x175 (via bit-flip) → skips over `jUMP` (256 bytes) into `d2`

---

### B. How `generic_ipc.sh` Works Internally

#### Step 1: Prefix Handling

- Takes in a prefix file (80 bytes in our case)
- HashClash computes the internal MD5 state after this prefix
- Starts collision search from this state

#### Step 2: Collision Pipeline

1. `md5_diffpathhelper`: Initializes path from prefix
2. `md5_diffpathforward` + `md5_diffpathbackward`: Explore possible collisions
3. `md5_diffpathconnect`: Finds a valid connection between paths
4. `md5_diffpathhelper --findcollision`: Final collision discovery

#### Step 3: Collision Block and Alignment

- Always outputs 192-byte collision block (3 × 64 bytes)
- Starts from MD5 buffer (in our case, 16 bytes already filled)
- Completes next 48 + 64 + 64 = 192 bytes across 3 full MD5 blocks

---

## Key Design Insights

### Why 3-Block (192-Byte) Collision?

- MD5 is block-based (64 bytes)
- Starting from custom state requires 3 blocks to fully diverge
- 192 bytes = perfect size for reliable chosen-prefix collision

### Why Prefix is 80 Bytes?

- Allows 64 bytes to be fully processed
- Leaves 16 bytes in buffer → next 48 bytes complete the second block
- Aligns the collision block perfectly for MD5 and PNG chunk parsing

### Why Use 0x75 vs. 0x175?

- Difference is 256 bytes
- One-bit difference between 0x75 and 0x175 in length field triggers divergent parsing
- Enables hiding/jumping over malicious payload cleanly

### Why Not Use 0xC9/0x1C9?

- Would require flipping multiple bits
- Breaks MD5 differential path rules (which only tolerate certain 1-bit flips)

### Why Declare the mARC Chunk as 117 Bytes and Not 192?

We declare the mARC chunk as **117 bytes (0x75)** even though the actual MD5 collision block is **192 bytes** long.

- In File A: The PNG parser reads only 117 bytes of the chunk as declared. It then moves to the next chunk (`jUMP`).
- In File B: Due to a bit-flip, the declared length becomes 0x175, so the parser reads the full 192-byte collision block plus additional data.

**Important:**  
Even if the PNG parser in File A ignores the extra bytes, the **MD5 hashing engine processes all 192 bytes** linearly because MD5 doesn't understand file formats — it just hashes the raw file bytes sequentially.

Thus:
- **All 192 bytes are crucial for creating the hash collision**.
- The declared length is a parsing trick; the physical bytes are there for MD5 to hash.

---

## Output Files

- Two PNG files (`abcd1234-1.png`, `abcd1234-2.png`)
- Valid, viewable images
- Different content shown
- Same MD5 hash

---

## Conclusion

This project demonstrates a real-world, practical chosen-prefix MD5 collision attack using PNG files. We successfully:

- Crafted a shared 80-byte prefix
- Computed a 192-byte, 3-block MD5 collision
- Embedded controlled divergence via PNG chunk lengths and jump logic

It proves once again that **MD5 is unsafe for integrity or security-critical applications** and should be fully deprecated.

---

## Appendix: Chunk Structures, Prefix Design, and Collision Layout

### PNG Chunks Overview

Each PNG chunk follows this structure:

```
[4-byte length][4-byte type][chunk data][4-byte CRC]
```

This structure is essential for both proper image parsing and stealthily embedding the collision payload.

---

### A. Image Header and Prefix Generation

We extracted the first 33 bytes from both PNG files, which includes:

- The PNG signature: 8 bytes
- The IHDR chunk: 25 bytes (4 length + 4 type + 13 data + 4 CRC)

This forms the initial part of our 80-byte prefix.

---

### B. Alignment Chunk

Next, we add a `tEXT` chunk to pad the structure:

- `\x00\x00\x00\x1a`: 4-byte length = 26
- `'tEXT'`: 4-byte type
- `'a'*26`: 26-byte payload
- `'AAAA'`: 4-byte fake CRC

Total = **38 bytes**, chosen to align the full prefix to exactly 80 bytes.

---

### C. Collision Chunk Start

- `\x00\x00\x00\x75`: length = 117
- `'mARC'`: type
- `'!'`: first byte of payload

After this, we insert the 192-byte MD5 collision block.

---

### D. Why Use 0x75 and Not 0xC9?

- A 1-bit flip turns `0x75` into `0x175` (256-byte difference).
- 0xC9/0x1C9 would require multi-bit flips — incompatible with MD5 chosen-prefix attacks.

---

### E. Suffix Design

We add a `jUMP` chunk that is exactly 256 bytes:

- 4 bytes: length
- 4 bytes: type
- 244 bytes: payload
- 4 bytes: CRC

File A parses it, File B skips it due to bit-flipped length in `mARC`.

---

### Final File Structure Recap

```
[Header (33)]
+ [tEXT chunk (38)]
+ [mARC start (9)]
= 80 bytes prefix

+ [Collision block (192)]
+ [RealHash (8)]
+ [jUMP chunk (256)]
+ [d2 data]
+ [d1 data]
```

This structure maintains PNG validity, preserves MD5 collision alignment, and delivers diverging behavior while producing identical MD5 checksums.

## 📏 Prefix Size vs. MD5 Block Alignment

### Why We Used 80 Bytes Instead of a Multiple of 64

MD5 processes data in **64-byte blocks** (512 bits). When constructing a chosen-prefix collision, precise control over the internal MD5 state is necessary.

We used **80 bytes** as the prefix length for the following reasons:

- **64 bytes** are processed as one full MD5 block.
- **16 bytes** are left in the MD5 buffer.
- The collision block then fills up:
  - **48 bytes** to complete the second block,
  - **64 + 64 bytes** for the next two full blocks.
  - Total collision size = **192 bytes**

### Why Not Use Exactly 64 Bytes?

If we made the prefix exactly 64 bytes:

- The collision block would begin on a clean MD5 block boundary (block 2).
- This is valid for MD5, but would shift all subsequent data **16 bytes earlier**.
- That shift would **misalign the bit-flip** targeting the PNG chunk length (e.g., `0x75 → 0x175`).
- Result: **The collision would not create the correct divergence.**

### Why the 80-Byte Prefix is Ideal

- Maintains perfect alignment between the PNG chunk layout and MD5 collision structure.
- Keeps the collision block starting mid-block for better control.
- Ensures the length field (`00 00 00 75`) is positioned so a single bit flip changes it to `00 01 00 75`.
- Allows `xxd` to show clean separation between header, collision, and suffix for human verification.

Thus, **80 bytes** is not arbitrary — it’s a carefully chosen length to ensure the collision functions correctly and aligns with both MD5 internals and PNG chunk parsing.
