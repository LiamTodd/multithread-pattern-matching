#include <string.h>

int kmp_search_serial(char *string, char *pattern, int *table){
    int string_position = 0;
    int pattern_position = 0;
    int count = 0;
    int string_length = strlen(string);
    int pattern_length = strlen(pattern);

    while ((string_length - string_position) >= (pattern_length - pattern_position)){
        if (pattern[pattern_position] == string[string_position]){
            pattern_position ++;
            string_position ++;
        }
        if (pattern_position == pattern_length){
            count ++;
            pattern_position = table[pattern_position - 1];
        }
        else if ((string_position < string_length) && (pattern[pattern_position] != string[string_position])){
            if (pattern_position != 0){
                pattern_position = table[pattern_position - 1];
            }
            else{
                string_position ++;
            }
        }
    }
    return count;
}