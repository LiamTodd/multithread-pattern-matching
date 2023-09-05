#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "headerfiles/generic_helpers.h"

int read_file_into_string(char **file_content, char *file_name){

    int i;
    long length;
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL){
        printf("Error opening dataset file.\n");
        return -1;
    }

    if (fp){
        // get number of chars
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        // set back to 0
        fseek(fp, 0, SEEK_SET);
        size_t bytesRead = 0;
        *file_content = (char *)malloc((length+1) * sizeof(char)); // free'd in main
        // read file contents into buffer
        for (i=0; i<length; i++){
            (*file_content)[i] = fgetc(fp);
            bytesRead ++;
        }
        (*file_content)[length] = '\0';
        
        fclose (fp);
    }
    return 0;
}


int read_file_into_patterns_array(char patterns[MAX_PATTERNS][MAX_PATTERN_LENGTH], char *file_name){
    // returns the number of patterns
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL){
        printf("Error opening patterns file.\n");
        return -1;
    }
    int line=0;
    while (line < MAX_PATTERNS && fgets(patterns[line], MAX_PATTERN_LENGTH, fp) != NULL) {
        // Remove the newline character, if present, replace with null terminator
        if (patterns[line][strlen(patterns[line])-1] == '\n'){
            patterns[line][strlen(patterns[line])-2] = '\0';
        }
        line++;
    }
    fclose(fp);
    return line;
}


void fill_kmp_table(char *pattern, int *table){

    int position = 0;
    int p_s_length = 0; // length of previous longest prefix-suffix
    table[position] = 0;
    position ++;
    int pattern_length = strlen(pattern);
    while (position < pattern_length){
        if (pattern[position] == pattern[p_s_length]){
            p_s_length ++;
            table[position] = p_s_length;
            position ++;
        }
        else{
            if (p_s_length != 0){
                p_s_length = table[p_s_length - 1];
            }
            else{
                table[position] = 0;
                position ++;
            }
        }
    }
}