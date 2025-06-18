#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>

int compare(const void *x_void, const void *y_void){
    char x = *(char *) x_void;
    char y = *(char *) y_void;
    return x - y;
}

int main(int argc, char *argv[]){  
    int blocksize;
    int keyPos;
    int inputPos;
    int outputPos;

    if (argc == 6){
        blocksize = atoi(argv[2]);
        keyPos = 3;
        inputPos = 4;
        outputPos = 5;
        // printf("b : %s\n", argv[1]);
        

    } else if (argc == 4){
        // printf("using default ");

        blocksize = 8;
        keyPos = 1;
        inputPos = 2;
        outputPos = 3;
        
    } else {
        printf("Bad Usage: Wrong num of args\n");
        printf("./ctencrypt -b blocksize key plaintext ciphertext\n");
        printf("or\n./ctencrypt key plaintext ciphertext\n");

        exit(1);
    }

    if (blocksize < 1){
        printf("Blocksize less than one\n");
        exit(1);
    }

    char *key = argv[keyPos];
    char *plaintext = argv[inputPos];
    char *ciphertext = argv[outputPos];

    // printf("blocksize : %d\n", blocksize);
    // printf("key : %s\n", key);
    // printf("reading : %s\n", plaintext);
    // printf("writing : %s\n", ciphertext);

    int keyLen = strlen(key); // key defines the width of the table and the sequence in which columns are read from said table

    FILE *readFile = fopen(plaintext, "rb");
    if (readFile == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    int rows = blocksize / keyLen + ((blocksize % keyLen == 0) ? 0 : 1);

    int columns = keyLen;
    // printf("fileLen length: %ld\n", fileLen);
    // printf("key length (COLUMNS): %d\n", columns);
    // printf("calculated (ROWS): %d\n", rows);

    // printf("\nkey: %s\n", key);
    char *sortedKey = calloc(keyLen + 1, sizeof(char));
    strcpy(sortedKey, key);

    qsort(sortedKey, keyLen, sizeof(char), compare);

    int *sortedPositions = calloc(keyLen + 1, sizeof(int));
    for (int i = 0; i < keyLen; i++){
        sortedPositions[i] = INT_MAX;
    }

    int count = 0;
    for (int i = 0; i < keyLen; i++){
        for (int j = 0; j < keyLen; j++){
            if (sortedKey[i] == key[j] && sortedPositions[j] == INT_MAX){
                sortedPositions[j] = count;
                count++;
            }
        }
    }

    // printf("key sort = { ");
    // for (int i = 0; i < keyLen; i++){
    //     printf("%d, ", sortedPositions[i]);
    // }
    // printf("}\n");


    //////////
    // CREATE WRITE //
    //////////////////
    
    //Clears file so you can append :)
    FILE *writeFile = fopen(ciphertext, "wb");

    if (writeFile == NULL){
        printf("\nUnable to find file\n");
        fclose(readFile);
        exit(1);
    }
    // READING FILE //

    // buffer acts as container and array of charactesr taht contains length of text
    char *buffer = malloc(blocksize);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(readFile);
        fclose(writeFile);
        exit(1);
    }

    // buffer, max input size, pointer to file that are opening
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, blocksize, readFile)) > 0){

        int bufferLen = bytesRead;
        int actualRows = (bufferLen + columns - 1) / columns; // Calculate actual number of rows

        /////////////////////////
        // DEALING WITH CIPHER //
        /////////////////////////

        // Allocate memory for ciphered data
        char **ciphered = malloc(rows * sizeof(char *));
        for (int i = 0; i < rows; i++) {
            ciphered[i] = malloc(columns * sizeof(char));
        }

        // Fill ciphered array without padding
        int sum = 0;
        for (int i = 0; i < actualRows; i++) {
            for (int j = 0; j < columns && sum < bufferLen; j++) {
                ciphered[i][j] = buffer[sum++];
            }
        }


        // printf("----original----\n");
        // for (int i = 0; i < actualRows; i++) {
        //     for (int j = 0; j < columns && (i * columns + j) < bufferLen; j++) {
        //         printf("'%c' ", ciphered[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");


        // Writing from cipher

        // printf("-----output-----\n");
        // Write ciphered data
        for (int orderCounter = 0; orderCounter < columns; orderCounter++) {
            for (int i = 0; i < columns; i++) {
                if (sortedPositions[i] == orderCounter) {
                    for (int j = 0; j < actualRows; j++) {
                        int index = (j * columns) + i;
                        if (index < bufferLen) {
                            // printf("'%c' ", ciphered[j][i]);
                            fwrite(&ciphered[j][i], sizeof(char), 1, writeFile);
                        }
                    }
                    break;
                }
            }
        }
        // printf("\n\n");

        for (int i = 0; i < rows; i++) {
            free(ciphered[i]);
        }
        free(ciphered);

    }
    free(buffer);

    free(sortedPositions);
    // printf("\n");
    free(sortedKey);
    fclose(readFile);
    fclose(writeFile);

    return 0;
}
