#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

void processFile(char *password, char *inputFile, char *outputFile){

    //////////////
    // Get Seed //
    //////////////
    unsigned long seed = sdbm(password); // Convert the password to bytes

    ////////////////////
    // Read and Write //
    ////////////////////

    FILE *readFile = fopen(inputFile, "rb");
    if (readFile == NULL) {
        perror("Error opening input file");
        fclose(readFile);
        exit(1);
    }

    FILE *writeFile = fopen(outputFile, "wb");
    if (writeFile == NULL){
        perror("Error opening output file");
        fclose(writeFile);
        exit(1);
    }

    unsigned long byte;

    while ((byte = fgetc(readFile)) != EOF){
        // XOR byte with next value from PRNG
        uint32_t prng_value = pseudoRandomNumber(seed); 

        // XOR
        uint8_t processed_byte = byte ^ (prng_value & 0xFF);

        // Write processed byte to output file
        fputc(processed_byte, writeFile);

        // Update seed (to simulate next step in PRNG stream)
        seed = pseudoRandomNumber(seed);
    }
    fclose(readFile);
    fclose(writeFile);

}

int main(int argc, char *argv[]){

    if (argc != 4){
        printf("\nError: FOUR ARGS REQUIRED TO RUN FILE! : ./scrypt password plaintext ciphertext\n");
        exit(1);
    }

    char *password = argv[1];
    char *inputFile = argv[2];
    char *outputFile = argv[3];

    processFile(password, inputFile, outputFile);

    return 0;
}