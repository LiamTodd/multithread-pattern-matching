#define MAX_PATTERN_LENGTH 1024
#define MAX_FILE_NAME 256
#define MAX_PATTERNS 1000

int read_file_into_string(char **file_content, char *file_name);

int read_file_into_patterns_array(char patterns[MAX_PATTERNS][MAX_PATTERN_LENGTH], char *file_name);

void fill_kmp_table(char *pattern, int *table);