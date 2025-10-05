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
  char (*data)[BUFFER_SIZE];
  size_t len;

} TokenizedString;

int buffer_whitespace_count(char *buf);
TokenizedString *string_tokenizer(char *buf);
char *read_bin_path(char delimiter, char *path_buf, size_t *size);
// index is used to keep track of the size of the allocated memory for the file
// buffer
char *read_file(FILE *file_ptr, size_t *size);

int main() {
  printf("starting git-tui\n");
  char input_buffer[BUFFER_SIZE];
  char bin_cmd[] = "gago\n";

  // printf("RC FILE PATH %s\n", RC_FILE_PATH);
  // FILE *zdrc = fopen(RC_FILE_PATH, "r");
  //
  // if (zdrc == NULL) {
  //   perror("[ ERROR ]: Unable to open file");
  //   return 1;
  // }
  //
  // size_t file_buf_size = 0;
  // char *file_buf = read_file(zdrc, &file_buf_size);
  //
  // size_t paths_buf_size = 0;
  // char *paths_buf = read_bin_path(':', file_buf, &paths_buf_size);
  //
  // printf("[ TEST ]: size of file buffer=%lu\n", file_buf_size);
  //
  // // TODO: create enum if want to extend the config file... idk
  //
  // for (int i = 0; i < file_buf_size; i++) {
  //   char *current_str = (file_buf + BUFFER_SIZE * i);
  //   int cmp_result = strncmp(current_str, "PATH", 4);
  //   if (cmp_result == 0) {
  //     printf("[ INFO ]: PATH exists\n");
  //     return 0;
  //   }
  // }
  // printf("[ ERROR ]: PATH does not exists");
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

    // used for executing a new process
    TokenizedString *tokened_input = string_tokenizer(input_buffer);

    printf("[ TEST ]: Buffer Array\n");
    for (int i = 0; i < tokened_input->len; i++) {
      printf("[ TEST ]: Buff[%d]='%s'\n", i, tokened_input->data[i]);
    }
    printf("[ TEST ]: Length of tokens: %lu ", tokened_input->len);

    int res = strcmp(input_buffer, bin_cmd);

    if (res == 0) {
      printf("[ INFO ]: Executed command\n");
    } else {
      printf("[ INFO ]: Command does not exist, check your $PATH\n");
    }
    printf("[ INFO ]: Current size of buffer=%lu\n", strlen(input_buffer));
    printf("~>");
    free(tokened_input->data);
    free(tokened_input);
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
TokenizedString *string_tokenizer(char *buf) {

  TokenizedString *tokenized_str = malloc(sizeof(TokenizedString));
  if (tokenized_str == NULL) {
    return NULL;
  }

  size_t current_str_pos = 0;
  size_t base = 0;
  size_t size_by = 2;
  char (*set_ptr)[BUFFER_SIZE] = malloc(sizeof(*set_ptr));

  printf("[ INFO ]: incoming string=%s\n", buf);

  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0') {

      int cur_str_size = i - base;
      for (int j = 0; j < cur_str_size; j++) {
        set_ptr[current_str_pos][j] = buf[base + j];
      }

      set_ptr[current_str_pos][i] = '\0';
      char (*tmp_set_ptr)[BUFFER_SIZE] =
          realloc(set_ptr, sizeof(*set_ptr) * size_by);

      if (tmp_set_ptr == NULL) {
        perror("[ ERROR ]: Unable to reallocate for string's size");
        exit(1);
      }
      tmp_set_ptr = set_ptr;

			size_by++;
      base = i + 1;
      current_str_pos++;
    }
  }

  TokenizedString *tmp_token =
      realloc(tokenized_str, sizeof(TokenizedString) + sizeof(*set_ptr));
  tokenized_str = tmp_token;
  tokenized_str->data = set_ptr;
  tokenized_str->len = current_str_pos;

  if (tmp_token == NULL) {
    perror("[ ERROR ]: Unable to reallocate for string's size");
    exit(1);
  }

  return tokenized_str;
}
