#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 16

static unsigned long sdbm(str)
unsigned char *str;
{
    unsigned long hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

unsigned long a = 1103515245;
unsigned long c = 12345;
unsigned long m = 256;

unsigned long pseudoRandomNumber(unsigned long seed){
    seed = ((a * seed) + c) % m;
    return seed;
}

void printBlock(unsigned char *block){
    for (int i = 0; i < BLOCK_SIZE; i++){
        printf("%x ", block[i]);
    }
}

void pad_block(unsigned char *block, int block_len) {
    int padding_len = BLOCK_SIZE - block_len;
    for (int i = block_len; i < BLOCK_SIZE; i++) {
        block[i] = (unsigned char)padding_len; // Add padding value (PKCS#7)
    }
    // printf("padding from byte %d: value = %d\n", block_len, padding_len);
}


void swap(unsigned char *x, unsigned char *y) {
    unsigned char temp = *x;
    *x = *y;
    *y = temp;
}

void swap_bytes_based_on_key(unsigned char *block, unsigned char *previous_block) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        unsigned char first = previous_block[i] & 0x0F; // Extract lower 4 bits (0-15)
        unsigned char second = (previous_block[i] >> 4) & 0x0F; // Extract upper 4 bits (0-15)
        // printf("%d: swapping (%d, %d) = [ ", i, first, second);

        // Swap the bytes at positions 'first' and 'second'
        swap(&block[first], &block[second]);
        // printf("%x <> %x ]\n", block[second], block[first]);
    }
}

void CBC(unsigned char *block, unsigned char *prev_block) {

// Generate a 16-byte key from the prng
// apply xor with previous block
// shuffle block w/ 16-byte key
// apply xor with 16-byte key

    unsigned char temp_block[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++) {
        temp_block[i] = block[i] ^ prev_block[i];
    }

    for (int i = 0; i < BLOCK_SIZE; i++) {
        block[i] = temp_block[i];   // Get current pseudo-random byte
    }
}

void processFile(char *password, char *plaintext, char *ciphertext){

    //////////////
    // Get Seed //
    //////////////
    unsigned long seed = sdbm(password); // Convert the password to bytes
    printf("using seed=%ld from password=\"%s\"\n", seed, password);
    unsigned long prng = seed; 

    ////////////////////
    // Read and Write //
    ////////////////////

    FILE *readFile = fopen(plaintext, "rb");
    FILE *writeFile = fopen(ciphertext, "wb");

    if (!readFile || !writeFile) {
        perror("Error opening file");
        fclose(readFile);
        exit(1);
    
    }

    //////////////////////////////////////////
    //// RANDOM INITIALIZATION VECTOR!!!! ////
    //////////////////////////////////////////

    unsigned char iv[BLOCK_SIZE] = {0}; //generate a 16-byte IV from the PRNG

       ///////////////////////////////////////////////////////////
      //// THE MAIN DIFFERENCE BETWEEN DECRYPT AND ENCRYPT.  ////
     //// CREATES INITIALIZATION VECTOR AND WRITES TO FILE. ////
    ///////////////////////////////////////////////////////////

    for (int i = 0; i < BLOCK_SIZE; i++) {
        iv[i] = (unsigned char)rand();
    }

    fwrite(iv, sizeof(unsigned char), BLOCK_SIZE, writeFile);
    
    unsigned char previous_block[BLOCK_SIZE];
    memcpy(previous_block, iv, BLOCK_SIZE); 

    unsigned char block[BLOCK_SIZE];
    size_t bytes_read;



    // For first block 
    // The initialization vector will be the first 16 bytes written to the file

    // XOR with first plaintext block; then encrypt the block
    // Each block of plaintext is XORed with the previous block of ciphertext

    unsigned char keystream[BLOCK_SIZE]; //generate a 16-byte IV from the PRNG
    int lastBlock = 0;
    while ((bytes_read = fread(block, sizeof(unsigned char), BLOCK_SIZE, readFile)) > 0) {


        if (bytes_read < BLOCK_SIZE) {
            lastBlock = 1; //this is the last block
            pad_block(block, bytes_read);
        }


        for (int i = 0; i < BLOCK_SIZE; i++) {
            keystream[i] = (unsigned char)pseudoRandomNumber(prng);
            prng = pseudoRandomNumber(prng);
        }

        CBC(block, previous_block);
        
        // printf("\nbefore shuffle: [");
        // printBlock(block);
        // printf("]\n");


        // printf("keystream: [ ");
        // printBlock(keystream);
        // printf("]\n");


        swap_bytes_based_on_key(block, keystream);
        // printf("after shuffle: [ ");
        // printBlock(block);
        // printf("]\n");

        CBC(block, keystream);

        // printf("after xor with keystream: [ ");
        // printBlock(block);
        // printf("]\n");

        fwrite(block, sizeof(unsigned char), BLOCK_SIZE, writeFile);
        
        memcpy(previous_block, block, BLOCK_SIZE);

        if (bytes_read < BLOCK_SIZE){
            break;
        }
    }

    // If there's no padding needed to be done, still need to add it.
    // Creation of a last block

    if (lastBlock == 0){
        lastBlock = 1; //this is the last block
        pad_block(block, 0);


        for (int i = 0; i < BLOCK_SIZE; i++) {
            keystream[i] = (unsigned char)pseudoRandomNumber(prng);
            prng = pseudoRandomNumber(prng);
        }

        CBC(block, previous_block);

        // printf("\nbefore shuffle: [");
        // printBlock(block);
        // printf("]\n");

        // printf("keystream: [ ");
        // printBlock(keystream);
        // printf("]\n");

        swap_bytes_based_on_key(block, keystream);
        // printf("after shuffle: [ ");
        // printBlock(block);
        // printf("]\n");

        CBC(block, keystream);

        // printf("after xor with keystream: [ ");
        // printBlock(block);
        // printf("]\n");

        fwrite(block, sizeof(unsigned char), BLOCK_SIZE, writeFile);
    }



    fclose(readFile);
    fclose(writeFile);

}

int main(int argc, char *argv[]){

    if (argc != 4){
        printf("Error: FOUR ARGS REQUIRED TO RUN FILE! : ./bencrypt password plaintext ciphertext\n");
        exit(1);
    }

    char *password = argv[1];
    char *inputFile = argv[2];
    char *outputFile = argv[3];
    printf("plaintextfile=\"%s\" ciphertextfile=\"%s\" password=\"%s\"\n", inputFile, outputFile, password);

    processFile(password, inputFile, outputFile);

    return 0;
}
