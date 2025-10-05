#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_INPUTS 20 // determined by white spaces from user query
#define RC_FILE_PATH ".zdrc"

typedef struct {
  char (*data)[BUFFER_SIZE];
  size_t len;

} TokenizedString;

int buffer_whitespace_count(char *buf);
void substring(char *src, char *dest, size_t start, size_t end);
TokenizedString *string_tokenizer(char *buf);
TokenizedString *string_tokenizer_delim(char delimiter, char *path_buf);
TokenizedString *tokenized_file(FILE *file_ptr);

void substring(char *src, char *dest, size_t start, size_t end) {

  if (end > strlen(src)) {
    printf("[ ERROR ]: character overflow from end=%lu, src length=%lu\n", end,
           strlen(src));
    exit(1);
  }
  int str_size = end - start;
  // this somehow works than previous iteration
  for (int i = 0; i <= strlen(src); i++) {
    if (i == start) {
      for (int j = 0; j <= end; j++) {
        // start = 3
        // if j = 0 : start = start + 0 = 3
        // if j = 1 : start = start + 1 = 4
        // if j = 2 : start = start + 2 = 5
        // if j = 3 : start = start + 3 = 6
        // until end
        dest[j] = src[start + j];
      }
      break;
    }
  }

  printf("[ TEST ]: DEST LEN =%lu\n", strlen(dest));
  printf("[ TEST ]: SRC LEN =%lu\n", strlen(src));
}

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

  TokenizedString *tokenized_file_buf = tokenized_file(zdrc);

  printf("[ TEST ]: LINE COUNT: %lu\n", tokenized_file_buf->len);
  for (int i = 0; i < tokenized_file_buf->len; i++) {
    printf("[ INFO ]: FILE CONTENTS [%d] \n%s", i, tokenized_file_buf->data[i]);
  }

  printf("[ TEST ]: lines in file =%lu\n", tokenized_file_buf->len);
  // TODO: create enum if want to extend the config file... idk

  for (int i = 0; i < tokenized_file_buf->len; i++) {

    char *cur_str = tokenized_file_buf->data[i];
    size_t line_name_len = 5;
    int cmp_result = strncmp(cur_str, "PATH=", line_name_len);

    if (cmp_result == 0) {
      printf("[ INFO ]: PATH exists\n");
      char path_buf[BUFFER_SIZE];

      // TODO: fix bug where first few characters are gobbly guk
      substring(cur_str, path_buf, line_name_len, strlen(cur_str));

      TokenizedString *tokenized_paths = string_tokenizer_delim(':', path_buf);
      printf("[ TEST ]: %s", tokenized_paths->data[0]);
      free(tokenized_paths->data);
      free(tokenized_paths);
    }
  }

  free(tokenized_file_buf->data);
  free(tokenized_file_buf);
  return 0;
  printf("[ ERROR ]: PATH does not exists");
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
TokenizedString *string_tokenizer_delim(char delimiter, char *buf) {

  printf("\nCALLED\n");
  TokenizedString *tokenized_str = malloc(sizeof(TokenizedString));
  if (tokenized_str == NULL) {
    perror("[ ERROR ]: Unable to allocate memory for tokenized_str");
    exit(1);
  }

  size_t current_str_pos = 0;
  size_t base = 0;
  size_t size_by = 1;
  char (*set_ptr)[BUFFER_SIZE] = malloc(sizeof(*set_ptr));

  printf("[ INFO ]: size of set_ptr=%lu\n", sizeof(*set_ptr));

  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] == delimiter || buf[i] == '\n' || buf[i] == '\0') {

      int cur_str_size = i - base;
      for (int j = 0; j < cur_str_size; j++) {
        set_ptr[current_str_pos][j] = buf[base + j];
      }

      size_by++;
      set_ptr[current_str_pos][i] = '\0';

      char (*tmp_set_ptr)[BUFFER_SIZE] =
          realloc(set_ptr, sizeof(*set_ptr) * size_by);

      if (tmp_set_ptr == NULL) {
        perror("[ ERROR ]: Unable to reallocate for string's size");
        exit(1);
      }

      set_ptr = tmp_set_ptr;
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

/*
 Parsing a file that is read line by line to extract config from .zdrc
 when reading the returned array of strings must offset by BUFFER_SIZE

- return: a pointer to a character which should be accessed by (BUFFER_SIZE *
index) to get each string in the array.

 */
TokenizedString *tokenized_file(FILE *file_ptr) {

  size_t line_capp = 0;
  int line_count = 0;
  int size_by = 1; // increment to multiply by current size
  char *file_buf = NULL;
  char (*ptr_str_tmp)[BUFFER_SIZE] = malloc(sizeof(*ptr_str_tmp));
  // dynamically reallocate for every new line
  while (getline(&file_buf, &line_capp, file_ptr) != EOF) {
    if (*file_buf == '\n')
      continue;
    int cur_str_size = strlen(file_buf);
    for (int i = 0; i < cur_str_size; i++) {
      ptr_str_tmp[line_count][i] = file_buf[i];
    }

    line_count++;
    size_by++;

    // add n times more memory size for every new line that has contents
    char (*tmp)[BUFFER_SIZE] =
        realloc(ptr_str_tmp,
                sizeof(*ptr_str_tmp) *
                    size_by); // need to add since line_count starts at 0

    if (tmp == NULL) {
      perror("[ ERROR ]: Unable to reallocate new memory for buffer");
      exit(1);
    }
    ptr_str_tmp = tmp;
  };

  TokenizedString *tokenized_file =
      malloc(sizeof(TokenizedString) + sizeof(*ptr_str_tmp));

  tokenized_file->data = ptr_str_tmp;
  tokenized_file->len = line_count;

  free(file_buf);
  return tokenized_file;
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
  size_t size_by = 1;
  char (*set_ptr)[BUFFER_SIZE] = malloc(sizeof(*set_ptr));

  printf("[ INFO ]: incoming string=%s\n", buf);

  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0') {

      int cur_str_size = i - base;
      for (int j = 0; j < cur_str_size; j++) {
        set_ptr[current_str_pos][j] = buf[base + j];
      }

      size_by++;
      set_ptr[current_str_pos][i] = '\0';
      char (*tmp_set_ptr)[BUFFER_SIZE] =
          realloc(set_ptr, sizeof(*set_ptr) * size_by);

      if (tmp_set_ptr == NULL) {
        perror("[ ERROR ]: Unable to reallocate for string's size");
        exit(1);
      }
      set_ptr = tmp_set_ptr;

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
