#include "stdlib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_INPUTS 20 // determined by white spaces from user query
#define RC_FILE_PATH ".zdrc"


typedef struct {

	char *data;
	char **str;
	size_t str_len;

} String;

int buffer_whitespace_count(char *buf);
char *string_tokenizer(char *buf);
char *read_bin_path(char delimiter, char *path_buf, size_t *size);
// index is used to keep track of the size of the allocated memory for the file
// buffer
char *read_file(FILE *file_ptr, size_t *size);

int main() {
  printf("starting git-tui\n");
  char input_buffer[BUFFER_SIZE];
  char bin_cmd[] = "gago\n";

  printf("RC FILE PATH %s\n", RC_FILE_PATH);
  FILE *zdrc = fopen(RC_FILE_PATH, "r");

  if (zdrc == NULL) {
    perror("[ ERROR ]: Unable to open file");
    return 1;
  }

  size_t file_buf_size = 0;
  char *file_buf = read_file(zdrc, &file_buf_size);

  size_t paths_buf_size = 0;
  char *paths_buf = read_bin_path(':', file_buf, &paths_buf_size);

  printf("[ TEST ]: size of file buffer=%lu\n", file_buf_size);

  // TODO: create enum if want to extend the config file... idk

  for (int i = 0; i < file_buf_size; i++) {
    char *current_str = (file_buf + BUFFER_SIZE * i);
    int cmp_result = strncmp(current_str, "PATH", 4);
    if (cmp_result == 0) {
      printf("[ INFO ]: PATH exists\n");
      return 0;
    }
  }
  printf("[ ERROR ]: PATH does not exists");
  return 1;
  printf("~>");

  while (fgets(input_buffer, BUFFER_SIZE, stdin) != NULL) {
    printf("[ TEST ]: Size of buffer = %lu\n", strlen(input_buffer));
    printf("[ TEST ]: Whitespace count=%d\n",
           buffer_whitespace_count(input_buffer));

    if (buffer_whitespace_count(input_buffer) > MAX_INPUTS) {
      printf("[ WARNING ]: Input size exceed max user inputs of %d",
             MAX_INPUTS);
      return 1;
    }
    char *str_arr = string_tokenizer(input_buffer);

    if (str_arr == NULL) {
      printf("[ ERROR ]: Pointer returned null");
      free(str_arr);
      exit(1);
    };

    int res = strcmp(input_buffer, bin_cmd);

    if (res == 0) {
      printf("[ INFO ]: Executed command\n");
    } else {
      printf("[ INFO ]: Command does not exist, check your $PATH\n");
    }
    printf("[ INFO ]: Current size of buffer=%lu\n", strlen(input_buffer));
    printf("~>");
    free(str_arr);
  };
}

// - return: a pointer to a character which should be accessed by (BUFFER_SIZE *
// index) to get each string in the array.
char *read_bin_path(char delimiter, char *path_buf, size_t *size) {

  char *paths = (char *)malloc(sizeof(char) * BUFFER_SIZE);
  if (paths == NULL) {
    perror("[ ERROR ]: Unable to allocate memory for paths ");
    exit(1);
  }
  int base = 0;
  int size_by = 1;
  int line_count = 0;
  for (int i = 5; i < BUFFER_SIZE; i++) {
    char *current_ch = (path_buf + sizeof(char) * i);
    if (*current_ch == delimiter || *current_ch == '\n') {

      char *tmp_realloc = (char *)realloc(paths, BUFFER_SIZE * size_by);

      if (tmp_realloc == NULL) {
        perror("[ ERROR ]: Unable to reallocate new memory for buffer");
        free(paths);
        exit(1);
      }

      paths = tmp_realloc;

      int index = sizeof(char) * i; // no need to miltiply over and over again
      for (int j = base; j < i; j++) {
        *(paths + index + j) = *(path_buf + (sizeof(char) * j));
      }

      *(paths + index + i) = '\0'; // replace ':' with '\0'
      base = i;
      line_count++;
    }
  }

  *size += line_count * BUFFER_SIZE * sizeof(char);
  return paths;
}

/*
 Parsing a file that is read line by line to extract config from .zdrc
 when reading the returned array of strings must offset by BUFFER_SIZE

- return: a pointer to a character which should be accessed by (BUFFER_SIZE *
index) to get each string in the array.

 */
char *read_file(FILE *file_ptr, size_t *size) {

  size_t line_capp = 0;
  int line_count = 0;
  int size_by = 1; // increment to multiply by current size
  char *file_buf = NULL;
  char *ptr_str_tmp = (char *)malloc(sizeof(char) * BUFFER_SIZE);
  // dynamically reallocate for every new line
  while (getline(&file_buf, &line_capp, file_ptr) != EOF) {
    if (*file_buf == '\n')
      continue;
    int cur_str_size = strlen(file_buf);
    for (int i = 0; i < cur_str_size; i++) {
      *(ptr_str_tmp + sizeof(char) * (BUFFER_SIZE * line_count + i)) =
          file_buf[i];
    }
    char *p =
        (ptr_str_tmp + sizeof(char) * (BUFFER_SIZE * line_count)); // access

    line_count++;
    size_by++;

    // add n times more memory size for every new line that has contents
    char *tmp = (char *)realloc(
        ptr_str_tmp,
        sizeof(char) * BUFFER_SIZE *
            size_by); // need to add since line_count starts at 0

    if (tmp == NULL) {
      perror("[ ERROR ]: Unable to reallocate new memory for buffer");
      exit(1);
    }
    ptr_str_tmp = tmp;
  };
  *size += line_count * BUFFER_SIZE * sizeof(char);
  printf("[ TEST ]: LINE COUNT: %d\n", line_count);
  for (int i = 0; i < line_count; i++) {
    printf("[ INFO ]: FILE CONTENTS [%i] \n%s", i,
           (ptr_str_tmp + i * BUFFER_SIZE));
  }
  free(file_buf);
  return ptr_str_tmp;
}
int buffer_whitespace_count(char *buf) {
  int counter = 0;

  for (int i = counter; i < BUFFER_SIZE; i++) {
    if (buf[i] == ' ') {
      counter++;
    }
    if (buf[i] == 0) {
      break;
    }
  }
  return counter;
}

// Reads the whole buffer input of the user
// accessing each input needs to be offset by BUFFER_SIZE
char *string_tokenizer(char *buf) {

  char *ptr = buf;
  char *p_tmp_buf = (char *)malloc((MAX_INPUTS * BUFFER_SIZE) * sizeof(char));
  if (p_tmp_buf == NULL) {
    return NULL;
  }
  // the outer array
  size_t current_str_pos = 0;
  // starting of new string_tokenizering separated by space is base
  size_t base = 0;

  printf("[ INFO ]: incoming string=%s\n", buf);
  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0') {

      // iterate up to i where i is a runner pointer of the buf string
      int cur_str_size = i - base;

      // extract from base to i
      for (int k = 0; k < cur_str_size; k++) {
        // dereferncing every k-th elementh within the allcoated heap
        *(p_tmp_buf + sizeof(char) * (BUFFER_SIZE * current_str_pos + k)) =
            *(ptr + base + k);
        // tmp_buf[current_str_pos][k] =
        // from buffer base + k offset
      }

      // append null terminator else it does not guarantee a 0 byte value
      // within the occupied space
      *(p_tmp_buf + sizeof(char) * (BUFFER_SIZE * cur_str_size + 1)) = '\0';

      base = i + 1;
      current_str_pos++;
    }
  }
  printf("[ TEST ]: Buffer Array\n");
  for (int i = 0; i < 10; i++) {
    printf("[ TEST ]: Buff[%d]='%s'\n", i,
           (p_tmp_buf + sizeof(char) * (BUFFER_SIZE * i)));
  }
  return p_tmp_buf;
}

// file tree recurision to look for the specific command given a path to a
// binary extract user input -> tokenize -> use first string as input for
// recursion within the path -> if there exists a bin name === <input> then we
// know that the it exists else just return an error after checking if it
// exists, fork process and excec with additional parameters from the tokenized
// user input string string: <bin> ...parameters
