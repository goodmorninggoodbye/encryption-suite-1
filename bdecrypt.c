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

int remove_padding(unsigned char *block, int block_len) {
    unsigned char paddingChar = block[block_len - 1];
    int paddingVal = paddingChar;
    int offset = block_len - paddingVal;
    // printf("removing padding, start offset =%d: value=%d\n", offset, paddingVal);

    return block_len - paddingVal;
}


void swap(unsigned char *x, unsigned char *y) {
    unsigned char temp = *x;
    *x = *y;
    *y = temp;
}

void swap_bytes_based_on_key(unsigned char *block, unsigned char *previous_block) {
    for (int i = BLOCK_SIZE - 1; i >= 0; i--){
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

void processFile(char *password, char *ciphertext, char *plaintext){

    //////////////
    // Get Seed //
    //////////////
    unsigned long seed = sdbm(password); // Convert the password to bytes
    printf("using seed=%ld from password=\"%s\"\n", seed, password);
    unsigned long prng = seed; 

    ////////////////////
    // Read and Write //
    ////////////////////

    FILE *readFile = fopen(ciphertext, "rb");
    FILE *writeFile = fopen(plaintext, "wb");

    if (!readFile || !writeFile) {
        perror("Error opening file");
        fclose(readFile);
        exit(1);
    
    }

    //////////////////////////////////////////
    //// RANDOM INITIALIZATION VECTOR!!!! ////
    //////////////////////////////////////////

    unsigned char iv[BLOCK_SIZE] = {0}; //generate a 16-byte IV from the PRNG

    //////////////////////////////////////////////////////////
    //// THE MAIN DIFFERENCE BETWEEN DECRYPT AND ENCRYPT. ////
    //////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //// READS INITIALIZATION VECTOR AND ADDS TO ARRAY. ////
    ////////////////////////////////////////////////////////

    fread(iv, sizeof(unsigned char), BLOCK_SIZE, readFile);
    // printf("IV = [\n");
    // printBlock(iv);

    unsigned char previous_block[BLOCK_SIZE];
    memcpy(previous_block, iv, BLOCK_SIZE); 

    unsigned char block[BLOCK_SIZE];
    size_t bytes_read;



    // completely correct the first line written, for some reason.

    // for first block 
    // The initialization vector will be the first 16 bytes written to the file

    // XOR with first plaintext block; then encrypt the block
    // each block of plaintext is XORed with the previous block of ciphertext

    unsigned char keystream[BLOCK_SIZE]; //generate a 16-byte IV from the PRNG
    int lastBlock = 0;
    int firstRound = 0;
    unsigned char tempBlock[BLOCK_SIZE];
    unsigned char plaintextBlock[BLOCK_SIZE];

    while (1) {
        bytes_read = fread(block, sizeof(unsigned char), BLOCK_SIZE, readFile);
        if (feof(readFile)) {
            lastBlock = 1; //this is the last block
            // printf("\nthis is the last block\n");
            break;
        }
        if (firstRound == 1){
            // printf("writing\n");
            fwrite(plaintextBlock, sizeof(unsigned char), BLOCK_SIZE, writeFile);
            memcpy(previous_block, tempBlock, BLOCK_SIZE);
        } else {
            firstRound = 1;
        }

        for (int i = 0; i < BLOCK_SIZE; i++) {
            keystream[i] = (unsigned char)pseudoRandomNumber(prng);
            prng = pseudoRandomNumber(prng);
        }
       
        // printf("\nencrypted block before shuffle: [");
        // printBlock(block);
        // printf("]\n");

        memcpy(tempBlock, block, BLOCK_SIZE);


        // printf("keystream: [ ");
        // printBlock(keystream);
        // printf("]\n");

        CBC(block, keystream);

        // printf("after xor with keystream: [ ");
        // printBlock(block);
        // printf("] - scrambled\n");

        swap_bytes_based_on_key(block, keystream);

        // printf("plaintext before xor with CBC: [ ");
        // printBlock(block);
        // printf("]\n");


        CBC(block, previous_block);

        // printf("after plaintext xor CBC: [ ");
        // printBlock(block);
        // printf("]\n");

        memcpy(plaintextBlock, block, BLOCK_SIZE);
    }

    // Remove padding and then write

    if (lastBlock == 1){
        int pos = remove_padding(block, BLOCK_SIZE);
        unsigned char padding[pos];
        // printf("after padding removed: [ ");
        for (int i = 0; i < pos; i++){
            padding[i] = block[i];
            // printf("%x ", padding[i]);
        }
        // printf("]\n");

        fwrite(padding, sizeof(unsigned char), pos, writeFile);
    }

    fclose(readFile);
    fclose(writeFile);

}

int main(int argc, char *argv[]){

    if (argc != 4){
        printf("\nError: FOUR ARGS REQUIRED TO RUN FILE! : ./bdecrypt password ciphertext plaintext\n");
        exit(1);
    }

    char *password = argv[1];
    char *inputFile = argv[2];
    char *outputFile = argv[3];
    printf("plaintextfile=\"%s\" ciphertextfile=\"%s\" password=\"%s\"\n", inputFile, outputFile, password);

    processFile(password, inputFile, outputFile);

    return 0;
}