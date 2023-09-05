#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "headerfiles/generic_helpers.h"
#include "headerfiles/parallel_helpers.h"

int main(){

    char data_file_name[MAX_FILE_NAME];
    printf("Enter a filename to load a dataset from: ");
    scanf("%s", data_file_name);

    char patterns_file_name[MAX_FILE_NAME];
    printf("Enter a filename to load search terms from: ");
    // skip the newline char from previous read
    scanf("\n%s", patterns_file_name);

    // set up clock
    struct timespec start, end, start_read, end_read, start_kmp_table, end_kmp_table, start_match, end_match, start_print, end_print; 
	double time_taken, time_taken_read, time_taken_kmp_table, time_taken_match, time_taken_print;
	clock_gettime(CLOCK_MONOTONIC, &start); 

    // read file content
    clock_gettime(CLOCK_MONOTONIC, &start_read);
    char *file_content;
    char patterns[MAX_PATTERNS][MAX_PATTERN_LENGTH];
    int num_patterns = -1;
    int read_file_into_string_success = -1;
    // read dataset and query list concurrently
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            read_file_into_string_success = read_file_into_string(&file_content, data_file_name);
        }

        #pragma omp section
        {
            num_patterns = read_file_into_patterns_array(patterns, patterns_file_name);
        }
    }
    if (num_patterns == -1 || read_file_into_string_success == -1){
        // error message printed in function
        return 1;
    }
	clock_gettime(CLOCK_MONOTONIC, &end_read); 
	time_taken_read = (end_read.tv_sec - start_read.tv_sec) * 1e9; 
    time_taken_read = (time_taken_read + (end_read.tv_nsec - start_read.tv_nsec)) * 1e-9; 

    // construct kmp table for each pattern
	clock_gettime(CLOCK_MONOTONIC, &start_kmp_table);
    int **lps_tables = (int **)malloc(num_patterns * sizeof(int *));
    int i;
    # pragma omp parallel for private(i) shared(lps_tables, patterns)
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
    // allow nested parallelism due to parallelism inside kmp_search
    omp_set_dynamic(0);
    omp_set_nested(1);
    # pragma omp parallel for private(i), shared(match_counts, file_content, patterns, lps_tables)
    for (i=0; i<num_patterns; i++){
        match_counts[i] = kmp_search_parallel(file_content, patterns[i], lps_tables[i]);
        free(lps_tables[i]);
    }
    omp_set_nested(0);
    omp_set_dynamic(1);
    free(lps_tables);
    free(file_content);
    clock_gettime(CLOCK_MONOTONIC, &end_match); 
	time_taken_match = (end_match.tv_sec - start_match.tv_sec) * 1e9; 
    time_taken_match = (time_taken_match + (end_match.tv_nsec - start_match.tv_nsec)) * 1e-9; 

    // display counts
	clock_gettime(CLOCK_MONOTONIC, &start_print);
    printf("Matches found for each pattern:\n");
    // not parallelised as the operation is purely i/o
    int sum = 0;
    for (i=0; i<num_patterns; i++){
        sum += match_counts[i];
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
	printf("File read time: %lf\n", time_taken_read);
	printf("Construct partial match tables: %lf\n", time_taken_kmp_table);
	printf("Match finding time: %lf\n", time_taken_match);
	printf("Print result time: %lf\n", time_taken_print);
	printf("Total time: %lf\n", time_taken);

    return 0;
}
