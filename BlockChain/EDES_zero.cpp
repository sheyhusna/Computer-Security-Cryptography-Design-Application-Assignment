// des_demo.cpp
// Educational DES implementation with verbose printing of steps.
// Compile with: g++ des_demo.cpp -std=c++17 -O2 -o des_demo

#include <bits/stdc++.h>
using namespace std;

// --- DES tables (standard) ---

// Initial Permutation (IP)
static const int IP[64] = {
  58,50,42,34,26,18,10,2,
  60,52,44,36,28,20,12,4,
  62,54,46,38,30,22,14,6,
  64,56,48,40,32,24,16,8,
  57,49,41,33,25,17,9,1,
  59,51,43,35,27,19,11,3,
  61,53,45,37,29,21,13,5,
  63,55,47,39,31,23,15,7
};

// Final Permutation (IP inverse)
static const int IP_INV[64] = {
  40,8,48,16,56,24,64,32,
  39,7,47,15,55,23,63,31,
  38,6,46,14,54,22,62,30,
  37,5,45,13,53,21,61,29,
  36,4,44,12,52,20,60,28,
  35,3,43,11,51,19,59,27,
  34,2,42,10,50,18,58,26,
  33,1,41,9,49,17,57,25
};

// Expansion E (32 -> 48)
static const int E[48] = {
  32,1,2,3,4,5,
  4,5,6,7,8,9,
  8,9,10,11,12,13,
  12,13,14,15,16,17,
  16,17,18,19,20,21,
  20,21,22,23,24,25,
  24,25,26,27,28,29,
  28,29,30,31,32,1
};

// Permutation P (32 -> 32)
static const int P[32] = {
  16,7,20,21,29,12,28,17,
  1,15,23,26,5,18,31,10,
  2,8,24,14,32,27,3,9,
  19,13,30,6,22,11,4,25
};

// PC-1 (64 -> 56)
static const int PC1[56] = {
  57,49,41,33,25,17,9,
  1,58,50,42,34,26,18,
  10,2,59,51,43,35,27,
  19,11,3,60,52,44,36,
  63,55,47,39,31,23,15,
  7,62,54,46,38,30,22,
  14,6,61,53,45,37,29,
  21,13,5,28,20,12,4
};

// PC-2 (56 -> 48)
static const int PC2[48] = {
  14,17,11,24,1,5,
  3,28,15,6,21,10,
  23,19,12,4,26,8,
  16,7,27,20,13,2,
  41,52,31,37,47,55,
  30,40,51,45,33,48,
  44,49,39,56,34,53,
  46,42,50,36,29,32
};

// left shift schedule for key halves
static const int SHIFTS[16] = {
  1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

// S-boxes: 8 boxes each 4x16 mapping. We'll flatten each as 64 entries.
static const int SBOX[8][64] = {
  // S1
  {
    14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
    0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
    4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
    15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
  },
  // S2
  {
    15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
    3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
    0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
    13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
  },
  // S3
  {
    10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
    13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
    13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
    1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
  },
  // S4
  {
    7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
    13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
    10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
    3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
  },
  // S5
  {
    2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
    14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
    4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
    11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
  },
  // S6
  {
    12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
    10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
    9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
    4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
  },
  // S7
  {
    4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
    13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
    1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
    6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
  },
  // S8
  {
    13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
    1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
    7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
    2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
  }
};

// --- Utility functions ---

// Convert string of bytes to a binary string of '0'/'1' (no spaces)
string bytesToBinary(const string &in) {
  string out;
  for (unsigned char c : in) {
    bitset<8> b(c);
    out += b.to_string();
  }
  return out;
}

// Convert binary string (length multiple of 8) to bytes
string binaryToBytes(const string &bin) {
  string out;
  for (size_t i = 0; i < bin.size(); i += 8) {
    bitset<8> b(bin.substr(i, 8));
    out.push_back(char(b.to_ulong()));
  }
  return out;
}

// Permute input bits (as string of '0'/'1') per a table (1-based indices)
string permute(const string &in, const int table[], int tableLen) {
  string out;
  out.resize(tableLen);
  for (int i = 0; i < tableLen; ++i) {
    out[i] = in[table[i] - 1];
  }
  return out;
}

// Left shift a bitstring (as string) by n bits (circular)
string leftShift(const string &in, int n) {
  int L = in.size();
  string out = in.substr(n) + in.substr(0, n);
  return out;
}

// XOR of two binary strings of equal length
string xorStr(const string &a, const string &b) {
  string out;
  out.resize(a.size());
  for (size_t i = 0; i < a.size(); ++i) out[i] = (a[i] == b[i]) ? '0' : '1';
  return out;
}

// Split binary string to vector of blocks of blockSize bits
vector<string> splitBlocks(const string &bin, int blockSize) {
  vector<string> blocks;
  for (size_t i = 0; i < bin.size(); i += blockSize) {
    blocks.push_back(bin.substr(i, blockSize));
  }
  return blocks;
}

// Print a binary string grouped by bytes for readability
string groupedBinary(const string &bin) {
  string s;
  for (size_t i = 0; i < bin.size(); ++i) {
    s.push_back(bin[i]);
    if ((i+1) % 8 == 0 && i+1 != bin.size()) s.push_back(' ');
  }
  return s;
}

// --- DES core functions ---

// Generate 16 subkeys (each 48 bits) from 64-bit key (binary string)
vector<string> generateSubkeys(const string &key64) {
  // Apply PC-1 -> 56 bits
  string key56 = permute(key64, PC1, 56);

  // Split to C and D each 28 bits
  string C = key56.substr(0, 28);
  string D = key56.substr(28, 28);

  vector<string> subkeys;
  for (int round = 0; round < 16; ++round) {
    int shift = SHIFTS[round];
    C = leftShift(C, shift);
    D = leftShift(D, shift);
    string CD = C + D;
    string K = permute(CD, PC2, 48);
    subkeys.push_back(K);
  }
  return subkeys;
}

// DES F function: takes 32-bit R and 48-bit subkey, returns 32-bit result
string fFunction(const string &R32, const string &subkey48) {
  // Expand to 48 bits
  string expanded = permute(R32, E, 48);
  // XOR with subkey
  string x = xorStr(expanded, subkey48);

  // S-box substitution: 8 groups of 6 bits -> 4 bits each (total 32 bits)
  string s_out;
  s_out.reserve(32);
  for (int i = 0; i < 8; ++i) {
    string six = x.substr(i*6, 6);
    int row = (six[0]-'0')*2 + (six[5]-'0');
    int col = (six[1]-'0')*8 + (six[2]-'0')*4 + (six[3]-'0')*2 + (six[4]-'0');
    int sval = SBOX[i][row*16 + col];
    bitset<4> b4(sval);
    s_out += b4.to_string();
  }

  // P permutation
  string p_out = permute(s_out, P, 32);
  return p_out;
}

// Encrypt one 64-bit block (binary string of length 64), with verbose printing
string desEncryptBlockVerbose(const string &block64, const vector<string> &subkeys, bool verbose=true) {
  string ip = permute(block64, IP, 64);
  if (verbose) {
    cout << "  After IP: " << groupedBinary(ip) << "\n";
  }

  string L = ip.substr(0,32);
  string R = ip.substr(32,32);

  if (verbose) {
    cout << "  L0: " << groupedBinary(L) << "\n";
    cout << "  R0: " << groupedBinary(R) << "\n";
  }

  for (int round = 0; round < 16; ++round) {
    string K = subkeys[round];
    string f = fFunction(R, K);
    string newR = xorStr(L, f);
    if (verbose) {
      cout << "  --- Round " << (round+1) << " ---\n";
      cout << "    Subkey K" << (round+1) << ": " << groupedBinary(K) << "\n";
      cout << "    f(R" << round << ", K" << (round+1) << "): " << groupedBinary(f) << "\n";
      cout << "    L" << (round+1) << ": " << groupedBinary(R) << "\n";
      cout << "    R" << (round+1) << ": " << groupedBinary(newR) << "\n";
    }
    L = R;
    R = newR;
  }

  // Final swap (R16,L16) then IP_INV
  string preout = R + L;
  string cipher = permute(preout, IP_INV, 64);
  if (verbose) {
    cout << "  pre-output (R16+L16): " << groupedBinary(preout) << "\n";
    cout << "  After IP_INV (cipher block): " << groupedBinary(cipher) << "\n\n";
  }
  return cipher;
}

// Decrypt one 64-bit block (binary string of length 64), with verbose printing
string desDecryptBlockVerbose(const string &block64, const vector<string> &subkeys, bool verbose=true) {
  // For decryption we run same but subkeys in reverse order
  string ip = permute(block64, IP, 64);
  if (verbose) cout << "  After IP: " << groupedBinary(ip) << "\n";
  string L = ip.substr(0,32);
  string R = ip.substr(32,32);
  if (verbose) {
    cout << "  L0: " << groupedBinary(L) << "\n";
    cout << "  R0: " << groupedBinary(R) << "\n";
  }
  for (int round = 0; round < 16; ++round) {
    string K = subkeys[15 - round];
    string f = fFunction(R, K);
    string newR = xorStr(L, f);
    if (verbose) {
      cout << "  --- Round " << (round+1) << " (decrypt uses K" << (16 - round) << ") ---\n";
      cout << "    Subkey K" << (16 - round) << ": " << groupedBinary(K) << "\n";
      cout << "    f(R" << round << ", K" << (16 - round) << "): " << groupedBinary(f) << "\n";
      cout << "    L" << (round+1) << ": " << groupedBinary(R) << "\n";
      cout << "    R" << (round+1) << ": " << groupedBinary(newR) << "\n";
    }
    L = R;
    R = newR;
  }
  string preout = R + L;
  string plain = permute(preout, IP_INV, 64);
  if (verbose) {
    cout << "  pre-output (R16+L16): " << groupedBinary(preout) << "\n";
    cout << "  After IP_INV (plain block): " << groupedBinary(plain) << "\n\n";
  }
  return plain;
}

// --- Padding (PKCS#5-like: pad with number of padding bytes) ---
string padPKCS5(const string &in) {
  int pad = 8 - (in.size() % 8);
  if (pad == 0) pad = 8;
  string out = in;
  out.append(pad, char(pad));
  return out;
}

string unpadPKCS5(const string &in) {
    if (in.empty()) return in;
    unsigned char last = static_cast<unsigned char>(in.back());
    int pad = static_cast<int>(last);
    if (pad < 1 || pad > 8) return in; // invalid padding -> return as-is

    // check that last N bytes all equal pad
    for (int i = 0; i < pad; ++i) {
        if (static_cast<unsigned char>(in[in.size() - 1 - i]) != pad)
            return in; // invalid -> return as-is
    }

    return in.substr(0, in.size() - pad);
}


// Convert 8-char ASCII key to 64-bit binary string
string keyTo64bits(const string &key8) {
  string k = key8;
  if (k.size() < 8) k.append(8 - k.size(), '\0');
  return bytesToBinary(k);
}

// --- High level encrypt/decrypt for arbitrary text (ECB mode) ---

string desEncrypt(const string &plaintext, const string &key8, bool verbose=true) {
  string padded = padPKCS5(plaintext);
  cout << "Padded plaintext (len=" << padded.size() << "): ";
  for (unsigned char c : padded) {
    if (isprint(c)) cout << c;
    else cout << "\\x" << hex << setw(2) << setfill('0') << (int)c << dec;
  }
  cout << "\nBinary: " << groupedBinary(bytesToBinary(padded)) << "\n\n";

  string key64 = keyTo64bits(key8);
  cout << "Key (64-bit binary): " << groupedBinary(key64) << "\n";
  // Generate subkeys and show them
  auto subkeys = generateSubkeys(key64);
  if (verbose) {
    for (int i = 0; i < 16; ++i) {
      cout << "K" << (i+1) << ": " << groupedBinary(subkeys[i]) << "\n";
    }
    cout << "\n";
  }

  string bin = bytesToBinary(padded);
  vector<string> blocks = splitBlocks(bin, 64);
  string cipherBin;
  for (size_t idx = 0; idx < blocks.size(); ++idx) {
    cout << "=== Encrypting block " << idx+1 << " ===\n";
    string cblk = desEncryptBlockVerbose(blocks[idx], subkeys, verbose);
    cipherBin += cblk;
  }
  return binaryToBytes(cipherBin);
}

string desDecrypt(const string &ciphertext, const string &key8, bool verbose=true) {
  string key64 = keyTo64bits(key8);
  auto subkeys = generateSubkeys(key64);
  string bin = bytesToBinary(ciphertext);
  vector<string> blocks = splitBlocks(bin, 64);
  string plainBin;
  for (size_t idx = 0; idx < blocks.size(); ++idx) {
    // cout << "=== Decrypting block " << idx+1 << " ===\n";
    string pblk = desDecryptBlockVerbose(blocks[idx], subkeys, verbose);
    plainBin += pblk;
  }
  string plain = binaryToBytes(plainBin);
  return unpadPKCS5(plain);
}

// --- Main demo ---

int main() {
  cout << "EDES_zero. Input plaintext and 8-byte key.\n";
  cout << "Enter plaintext: ";
  string plaintext;
  getline(cin, plaintext);

  cout << "Enter 8-character key (will be used as 64-bit key): ";
  string key;
  getline(cin, key);
  if (key.size() < 8) {
    cout << "Key shorter than 8 characters — it will be padded with zeros.\n";
    key.append(8 - key.size(), '\0');
  } else if (key.size() > 8) {
    key = key.substr(0, 8);
    cout << "Key longer than 8 characters — truncated to 8 chars.\n";
  }

  cout << "\n--- ENCRYPTION ---\n";
  string cipher = desEncrypt(plaintext, key, true);

  cout << "\nCiphertext (hex): ";
  for (unsigned char c : cipher) {
    cout << hex << setw(2) << setfill('0') << (int)c;
  }
  cout << dec << "\n";

  string plain = desDecrypt(cipher, key, false);

  cout << "\nRecovered plaintext: ";
  for (unsigned char c : plain) {
    if (isprint(c)) cout << c;
    else cout << "\\x" << hex << setw(2) << setfill('0') << (int)c << dec;
  }
  cout << "\n";

  return 0;
}
