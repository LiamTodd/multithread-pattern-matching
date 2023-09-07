#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headerfiles/generic_helpers.h"
#include "headerfiles/serial_helpers.h"

int main(){

    char data_file_name[MAX_FILE_NAME];
    printf("Enter a filename to load a dataset from: ");
    scanf("%s", data_file_name);

    char patterns_file_name[MAX_FILE_NAME];
    printf("Enter a filename to load search terms from: ");
    // skip the newline char from previous read
    scanf("\n%s", patterns_file_name);

    // set up clock
    struct timespec start, end, start_read_patterns, end_read_patterns, start_read_string, end_read_string, start_kmp_table, end_kmp_table, start_match, end_match, start_print, end_print; 
	double time_taken, time_taken_read_patterns, time_taken_read_string, time_taken_kmp_table, time_taken_match, time_taken_print;
	clock_gettime(CLOCK_MONOTONIC, &start); 

    // read file content
    clock_gettime(CLOCK_MONOTONIC, &start_read_string);
    char *file_content;
    int read_file_into_string_success = read_file_into_string(&file_content, data_file_name);
    if (read_file_into_string_success == -1){
        // error message printed in function
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_read_string); 
	time_taken_read_string = (end_read_string.tv_sec - start_read_string.tv_sec) * 1e9; 
    time_taken_read_string = (time_taken_read_string + (end_read_string.tv_nsec - start_read_string.tv_nsec)) * 1e-9; 

    clock_gettime(CLOCK_MONOTONIC, &start_read_patterns);
    char patterns[MAX_PATTERNS][MAX_PATTERN_LENGTH];
    int num_patterns = read_file_into_patterns_array(patterns, patterns_file_name);
    if (num_patterns == -1){
        // error message printed in function
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_read_patterns); 
	time_taken_read_patterns = (end_read_patterns.tv_sec - start_read_patterns.tv_sec) * 1e9; 
    time_taken_read_patterns = (time_taken_read_patterns + (end_read_patterns.tv_nsec - start_read_patterns.tv_nsec)) * 1e-9; 



    // construct kmp table for each pattern
	clock_gettime(CLOCK_MONOTONIC, &start_kmp_table);
    int **lps_tables = (int **)malloc(num_patterns * sizeof(int *));
    int i;
    for (i=0; i<num_patterns; i++){        
        // Allocate memory for the kmp table
        lps_tables[i] = (int *)malloc((strlen(patterns[i])) * sizeof(int));
        fill_kmp_table(patterns[i], lps_tables[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_kmp_table); 
	time_taken_kmp_table = (end_kmp_table.tv_sec - start_kmp_table.tv_sec) * 1e9; 
    time_taken_kmp_table = (time_taken_kmp_table + (end_kmp_table.tv_nsec - start_kmp_table.tv_nsec)) * 1e-9; 


    // search for substrings
	clock_gettime(CLOCK_MONOTONIC, &start_match);
    int *match_counts = (int *)malloc(num_patterns * sizeof(int));
    for (i=0; i<num_patterns; i++){
        match_counts[i] = kmp_search_serial(file_content, patterns[i], lps_tables[i]);
        free(lps_tables[i]);
    }
    free(lps_tables);
    free(file_content);
    clock_gettime(CLOCK_MONOTONIC, &end_match); 
	time_taken_match = (end_match.tv_sec - start_match.tv_sec) * 1e9; 
    time_taken_match = (time_taken_match + (end_match.tv_nsec - start_match.tv_nsec)) * 1e-9; 

    // display counts
	clock_gettime(CLOCK_MONOTONIC, &start_print);
    printf("Matches found for each pattern:\n");
    for (i=0; i<num_patterns; i++){
        printf("\nPattern: '%s'\nMatches found: %d\n", patterns[i], match_counts[i]);
    }
    free(match_counts);
    clock_gettime(CLOCK_MONOTONIC, &end_print); 
    time_taken_print = (end_print.tv_sec - start_print.tv_sec) * 1e9; 
    time_taken_print = (time_taken_print + (end_print.tv_nsec - start_print.tv_nsec)) * 1e-9; 

    clock_gettime(CLOCK_MONOTONIC, &end); 
    time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 

    // display results
    printf("\nTimes:\n");
	printf("String read time: %lf\n", time_taken_read_string);
	printf("Pattern read time: %lf\n", time_taken_read_patterns);
	printf("Construct partial match tables time: %lf\n", time_taken_kmp_table);
	printf("Match finding time: %lf\n", time_taken_match);
	printf("Printing time: %lf\n", time_taken_print);
	printf("Total time: %lf\n", time_taken);

    return 0;
}
