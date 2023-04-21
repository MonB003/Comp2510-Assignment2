#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define MAX_WORD_LENGTH 100
#define VALID_NUM_ARGS 2
#define ONE 1
#define ZERO 0
#define TEMP_FILE "temp.txt"


// Deletes the original file that was passed as an argument.
void deleteOriginalFile(char *fileName) {
    int canDelete;
    canDelete = remove(fileName);

    // If file can't be deleted
    if (canDelete == ONE) {
        printf("Unable to delete the file.\n");
    }
}


// Deletes temp file if old word wasn't found.
void deleteTempFile() {
    int canDelete;
    canDelete = remove(TEMP_FILE);

    // If file can't be deleted
    if (canDelete == ONE) {
        printf("Unable to delete the file.\n");
    }
}


// Renames the temp file to the name of the original file.
void renameNewFile(char *fileName) {
    // Store original file's name
    char *originalFile = fileName;
    int canRename;
    char *tempFile = TEMP_FILE;

    // First rename the original file to delete.txt so there aren't any name conflicts
    rename(fileName, "delete.txt");

    // Rename the temp file to the original file's name
    canRename = rename(tempFile, originalFile);

    // If file can be renamed, delete original file
    if (canRename == ZERO) {
        deleteOriginalFile("delete.txt");
    } else {
        printf("Unable to rename the file.\n");
    }
}


/*
 * Called in readEachLine() method.
 * Checks edge case where the first letter of the old word is repeated.
 * Prints the letter, resets store word variable, and adds the letter at the new position to store word.
 */
void repeatOfFirstLetter(FILE *outputFilePtr, char *storeWord, int *index, const char *line, int letter) {
    fprintf(outputFilePtr, "%s", storeWord);
    *index = ZERO;
    storeWord[*index] = line[letter];
    storeWord[*index + ONE] = '\0';
}


/*
 * If letters being read match the positions of the old word's letters, they are stored in a char array.
 * If an occurrence of the old word is found, print the new word to the file.
 */
void storeLetters(char *storeWord, char *line, int letter, int *index, const size_t *origWordLen, FILE *outputFilePtr, char *replaceWord, int *oldWordFound) {
    size_t len = strlen(line);

    // If the last letter matches the last letter of the old word, the words match and the old word was found
    if (*index + ONE == *origWordLen) {
        fprintf(outputFilePtr, "%s", replaceWord);
        *index = ZERO;
        *oldWordFound = ONE;   // Word was found, 1 means true
    } else if (letter + ONE == len) {
        // If the letters only match part of the old word, print the letters
        fprintf(outputFilePtr, "%s", storeWord);
    } else {
        // Increment index
        *index = *index + ONE;
    }
}


// If current letter doesn't match any letter in the original word.
void letterNotMatching(int *index, FILE *outputFilePtr, char *storeWord, char *line, int letter) {
    // If letters from part of the original word were stored but the rest didn't match, write those letters to the file
    if (*index > ZERO) {
        fprintf(outputFilePtr, "%s", storeWord);
    }

    // Write the current letter to the file
    fprintf(outputFilePtr, "%c", line[letter]);
    *index = ZERO;
}


/*
 * Reads each line in the text file and checks for occurrences of the word.
 * Reads through each line letter by letter and checks if a letter matches the first letter in the old word.
 * If a set of consecutive letters match the old word's letters, they are stored in a temporary char array until the
 * length of the old word is reached. If all the letters match the old word, the new word is printed to the file.
 */
void readEachLine(const size_t *origWordLen, FILE *filePtr, FILE *outputFilePtr, char *replaceWord, int *oldWordFound, const char *origWord) {
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), filePtr)) {
        size_t len = strlen(line);
        char storeWord[*origWordLen+ONE];
        int index = ZERO;

        for (int letter = ZERO; letter < len; letter++) {
            if (line[letter] == origWord[index] && index < *origWordLen) {
                storeWord[index] = line[letter];
                storeWord[index+ONE] = '\0';
                storeLetters(storeWord, line, letter, &index, origWordLen, outputFilePtr, replaceWord, oldWordFound);
            } else if (line[letter] == origWord[ZERO]) {
                repeatOfFirstLetter(outputFilePtr, storeWord, &index, line, letter);
                storeLetters(storeWord, line, letter, &index, origWordLen, outputFilePtr, replaceWord, oldWordFound);
            } else {
                letterNotMatching(&index, outputFilePtr, storeWord, line, letter);
            }
        }
    }
}


/*
 * Creates a temporary text file for the file passed as a parameter.
 * Calls a method to print the content of the parameter file to the temp file.
 * Returns 0 if the old word was not found, or 1 if it was found.
 */
int replaceOldWordWithNewWord(char *fileName, char *oldWord, char *newWord) {
    int oldWordFound = ZERO;
    char *origWord = oldWord;
    size_t origWordLen = strlen(origWord);
    char *replaceWord = newWord;
    FILE *filePtr = fopen(fileName, "r");
    FILE *outputFilePtr = fopen(TEMP_FILE, "w");

    readEachLine(&origWordLen, filePtr, outputFilePtr, replaceWord, &oldWordFound, origWord);

    fclose(filePtr);
    fclose(outputFilePtr);

    return oldWordFound;
}


/*
 * Checks if old and new words are the same.
 * Checks if one of the words entered is null or empty.
 * If input is valid, store users values for old word and new word.
 */
void checkValidInput(char *oldWord, char *newWord, char *fileName) {
    if (strcmp(oldWord, newWord) == ZERO) {
        printf("Old word is the same as the new word.");

    } else if (strlen(oldWord) == ZERO || strlen(newWord) == ZERO) {
        printf("One of the words you entered was empty.");

    } else {
        // Call method to replace old word with new word in the file
        int findWord = replaceOldWordWithNewWord(fileName, oldWord, newWord);
        if (findWord == ZERO) {
            printf("'%s' was not found in the file.", oldWord);
            deleteTempFile();
        } else {
            renameNewFile(fileName);
            printf("\nSuccessfully replaced all occurrences of '%s' with '%s'.", oldWord, newWord);
        }
    }
}


// Prompts user for old and new words, then stores them.
void promptUserForWord(char *fileName) {
    char oldWord[MAX_WORD_LENGTH];
    char newWord[MAX_WORD_LENGTH];

    printf("Enter a word to replace: ");
    gets(oldWord);
    printf("Replace '%s' with: ", oldWord);
    gets(newWord);

    // Check users input words
    checkValidInput(oldWord, newWord, fileName);
}


// Checks if file passed as an argument is valid.
int checkValidFile(char *fileName) {
    int isValid = ONE;
    FILE *filePtr = fopen(fileName, "r");

    if (filePtr == NULL) {
        isValid = ZERO;
    }
    fclose(filePtr);

    return isValid;
}


/*
 * Checks the number of arguments passed.
 * If the number is valid, check that the file is valid (not empty).
 * If file is valid, call method to prompt user for words.
 */
void callMethods(int argc, char* argv[]) {
    // If number of arguments is 2, it's valid (1 for the current directory and 1 for the file being read)
    if (argc == VALID_NUM_ARGS) {
        char *fileName = argv[ONE];

        int validFile = checkValidFile(fileName);
        if (validFile == ZERO) {
            printf("Unable to open the file: %s", fileName);
        } else {
            promptUserForWord(fileName);
        }
    } else {
        printf("Number of arguments passed must be 2. Number of arguments you passed: %d.", argc);
    }
}


int main(int argc, char* argv[]) {
    callMethods(argc, argv);

    return 0;
}
