# Encryption Suite
Cryptographic infrastructure implementing three progressively complex cipher systems with tamper-detection mechanisms. 

## Key Features
- **Block-Based Columnar Substitution**: Configurable transposition cipher with dynamic key scheduling
- **Stream Cipher Implementation**: Linear congruential pseudorandom keystream generator with password-based seeding
- **Advanced Block Cipher**: CBC mode with random initialization vectors and PKCS#7 padding

## Architecture Overview

### Core Components

**Columnar Transposition Engine (`ctencrypt`/`ctdecrypt`)**
- Variable block size support with optimized memory allocation
- Dynamic key sorting algorithm for column permutation
- 2D matrix operations with minimal padding

**Stream Cipher Framework (`scrypt`)**
- SDBM hash function for secure password-to-seed conversion
- Linear congruential generator with proven cryptographic parameters
- Byte-level XOR operations

**Block Cipher System (`bencrypt`/`bdecrypt`)**
- Random initialization vector generation for semantic security
- Cipher Block Chaining (CBC) mode with proper diffusion
- PKCS#7 padding implementation with automatic boundary handling
- Keystream-based byte-swapping for additional confusion


## Technical Specifications

- **Language**: C (C99 standard)
- **Cipher Types**: Transposition, Stream, Block (CBC mode)
- **Key Derivation**: SDBM hash function with 32-bit output
- **Random Generation**: Linear congruential generator (a=1103515245, c=12345, m=256)
- **Block Size**: 16 bytes for block cipher operations
- **Padding Scheme**: PKCS#7 with automatic boundary detection
- **Initialization Vectors**: 128-bit random IV per encryption session


## Build & Deployment

### Prerequisites
```bash
# Required: GCC with C99 support
gcc --version  # Verify GCC installation
```

### Compilation
```bash
make           # Builds all cipher components
make clean     # Clean build artifacts
```

### Individual Cipher Usage

**Columnar Transposition Cipher**
```bash
# Default is 8-byte blocks
./ctencrypt key plaintext.txt ciphertext.bin

# For custom block size
./ctencrypt -b 16 key plaintext.txt ciphertext.bin

# Decryption
./ctdecrypt key ciphertext.bin recovered.txt
```

**Stream Cipher**
```bash
# Encrypt with password-based keystream
./scrypt password plaintext.txt ciphertext.bin

# Decrypt (same command. XOR is symmetric)
./scrypt password ciphertext.bin recovered.txt
```

**Block Cipher with CBC**
```bash
# Encrypt with IV and padding
./bencrypt password plaintext.txt ciphertext.bin

# Decrypt with automatic padding removal
./bdecrypt password ciphertext.bin recovered.txt
```

## Getting Started

1. **Clone and build**:
   ```bash
   git clone <repository>
   cd encryption-suite-1
   make
   ```

2. **Test basic functionality**:
   ```bash
   echo "Hello world" > test.txt
   ./ctencrypt mykey test.txt test.ct
   ./ctdecrypt mykey test.ct recovered.txt
   cat recovered.txt  # Output: Hello world
   ```

3. **Multi-layer encryption**:
   ```bash
   # Apply multiple encryption layers
   ./ctencrypt key1 plaintext.txt layer1.bin
   ./scrypt password123 layer1.bin layer2.bin  
   ./bencrypt secretkey layer2.bin final.bin
   ```
