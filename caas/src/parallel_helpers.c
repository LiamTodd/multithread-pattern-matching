#include <string.h>
#include <omp.h>

int kmp_search_parallel(char *string, char *pattern, int *table){
    int count = 0;
    int string_length = strlen(string);
    int pattern_length = strlen(pattern);
    int overlap = pattern_length - 1;
    #pragma omp parallel shared(string, pattern, table, overlap, string_length) reduction(+:count)
    {
        int my_rank = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int chunk_size = string_length / num_threads;
        int string_position = chunk_size * my_rank;
        int end_search;
        if (my_rank == num_threads - 1){
            end_search = string_length;
        }
        else{
         end_search = string_position + chunk_size;
        }
        // account for matches in the section boundaries
        if (end_search + overlap < string_length-1){
            end_search = end_search + overlap;
        }

        int pattern_position = 0;
        while ((end_search - string_position) >= (pattern_length - pattern_position)){
            if (pattern[pattern_position] == string[string_position]){
                pattern_position ++;
                string_position ++;
            }
            if (pattern_position == pattern_length){
                count ++;
                pattern_position = table[pattern_position - 1];
            }
            else if ((string_position < end_search) && (pattern[pattern_position] != string[string_position])){
                if (pattern_position != 0){
                    pattern_position = table[pattern_position - 1];
                }
                else{
                    string_position ++;
                }
            }
        }
    }
    return count;
}