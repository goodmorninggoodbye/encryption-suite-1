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
    char *ciphertext = argv[inputPos];
    char *plaintext = argv[outputPos];

    // printf("blocksize : %d\n", blocksize);
    // printf("key : %s\n", key);
    // printf("reading : %s\n", plaintext);
    // printf("writing : %s\n", ciphertext);

    int keyLen = strlen(key); // key defines the width of the table and the sequence in which columns are read from said table

    FILE *readFile = fopen(ciphertext, "rb");
    if (readFile == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    int rows = blocksize / keyLen + ((blocksize % keyLen == 0) ? 0 : 1);

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
    FILE *writeFile = fopen(plaintext, "wb");

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
        int columns = keyLen;
        int actualRows = (bytesRead + columns - 1) / columns;
        int lastRowColumns = bytesRead % columns;
        if (lastRowColumns == 0){
            lastRowColumns = columns;
        }

        /////////////////////////
        // DEALING WITH CIPHER //
        /////////////////////////

        char **table = malloc(actualRows * sizeof(char *));
        for (int i = 0; i < actualRows; i++) {
            table[i] = calloc(columns, sizeof(char));
        }



        // Allocate memory for ciphered data
        int bufferIndex = 0;
        for (int col = 0; col < columns; col++) {
            int colIndex = sortedPositions[col];
            int rowsInThisColumn = (colIndex < lastRowColumns) ? actualRows : (actualRows - 1);
            for (int row = 0; row < rowsInThisColumn && bufferIndex < bytesRead; row++) {
                table[row][colIndex] = buffer[bufferIndex++];
            }
        }

        // Fill ciphered array without padding
        for (int i = 0; i < actualRows; i++) {
            int colsToWrite = (i == actualRows - 1) ? lastRowColumns : columns;
            fwrite(table[i], sizeof(char), colsToWrite, writeFile);
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
        for (int i = 0; i < actualRows; i++) {
            free(table[i]);
        }
        free(table);
        // printf("\n\n");


    }
    free(buffer);

    free(sortedPositions);
    // printf("\n");
    free(sortedKey);
    fclose(readFile);
    fclose(writeFile);

    return 0;
}
