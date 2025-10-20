# Computer-Security-Cryptography-Design-Application-Assignment
This project includes the design and implementation of custom stream and block ciphers with encryption/decryption demos, naming, workflow diagrams, and usage justification. It also proposes a hybrid cryptography application combining symmetric and asymmetric methods.

# Block Cipher
**EDES_zero** is a **fully commented**, **step-by-step educational implementation** of the **Data Encryption Standard (DES)** algorithm, written in C++.  
It demonstrates how DES operates internally, including key generation, expansion, substitution, and permutation processes.

| File Name | Description |
|------------|-------------|
| `EDES_zero.cpp` | Main source file containing the complete DES algorithm and demo program |

---

## Compilation & Execution

To compile the program, use the following command:

```bash
g++ EDES_zero.cpp -std=c++17 -O2 -o EDES_zero

./EDES_zero
 ```

 ## Example Output

```bash
EDES_zero. Input plaintext and 8-byte key.
Enter plaintext: HelloDES
Enter 8-character key (will be used as 64-bit key): mysecret

--- ENCRYPTION ---
Padded plaintext (len=8): HelloDES
Binary: 01001000 01100101 01101100 01101100 01101111 01000100 01000101 01010011

Key (64-bit binary): 01101101 01111001 01110011 01100101 01100011 01110010 01100101 01110100
K1: 00011011 00000000 11110000 11101110 11111100 10101010
...
=== Encrypting block 1 ===
  After IP: 11001010 11100001 01010111 ...
  --- Round 1 ---
  Subkey K1: 00011011 ...
  ...
Ciphertext (hex): 8f1a2bce21d4e765

Recovered plaintext: HelloDES
```
