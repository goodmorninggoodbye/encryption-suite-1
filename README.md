# encryption-suite-1
three progressively complex ciphers and tamper-detection mechanisms

three parts: implement three simple ciphers
1. Block-based columnar substitution cipher
* with no use of pads
2. Stream cipher using:
* A linear congruential pseudorandom keystream generator
* Simple password hashing for seed generation
3. Block cipher derived from the stream cipher
* Keystream-based byte-swapping
* Random initialization vector (IV)
* Cipher block chaining (CBC) for diffusion

Initialization vectors, cipher block chaining, padding
- IV Implementation: iv[i] = (unsigned char)rand() in bencrypt.c
- CBC Function: CBC(block, previous_block) with proper XOR chaining
- PKCS#7 Padding: pad_block() and remove_padding() functions

"Tamper-detection mechanisms":
- Hash chaining provides integrity verification
- CBC mode ensures block interdependence
