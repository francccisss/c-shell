#include "stdlib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_INPUTS 20 // determined by white spaces from user query

int current_buff_sz(char *buf);
int buffer_whitespace_count(char *buf);
char *string_tokenizer(char *buf);

int main() {
  printf("starting git-tui\n");
  char buffer[BUFFER_SIZE];
  char bin_cmd[] = "gago\n";

  // fgets includes newline '\n'
  printf("~>");

  while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
    printf("[ TEST ]: Size of buffer = %d\n", current_buff_sz(buffer));
    printf("[ TEST ]: Whitespace count=%d\n", buffer_whitespace_count(buffer));
    if (buffer_whitespace_count(buffer) > MAX_INPUTS) {
      printf("[ WARNING ]: Input size exceed max user inputs of %d",
             MAX_INPUTS);
      return 1;
    }
    char *str_arr = string_tokenizer(buffer);
    if (str_arr == NULL) {
      printf("[ ERROR ]: Pointer returned null");
      exit(1);
    };
    int res = strcmp(buffer, bin_cmd);
    if (res == 0) {
      printf("[ INFO ]: Executed command\n");
    } else {
      printf("[ INFO ]: Command does not exist, check your $PATH\n");
    }
    printf("[ INFO ]: Current size of buffer=%lu\n", strlen(buffer));
    printf("~>");
  };
}

int current_buff_sz(char *buf) {

  int counter = 0;
  for (int i = counter; i < BUFFER_SIZE; i++) {
    if (buf[i] == 0) {
      break;
    }
    counter++;
  }
  return counter;
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

  printf("[ TEST ]: incoming string=%s", buf);
  for (int i = 0; i < current_buff_sz(buf); i++) {
    if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0') {

      // iterate up to i where i is a runner pointer of the buf string
      int cur_str_size = i - base;
      printf("[ TEST ]: base=%lu\n", base);
      printf("[ TEST ]: i=%d\n", i);
      printf("[ TEST ]: cur_str_size=%d\n", cur_str_size);

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

      printf("[ TEST ]: new string_tokenizering %s\n",
             p_tmp_buf + i * BUFFER_SIZE);
      printf("[ TEST ]: prev base=%s\n", (ptr + base));
      base = i + 1;
      current_str_pos++;
      printf("[ TEST ]: new base=%s\n", (ptr + base));
    }
  }
  printf("[ TEST ]: Buffer Array\n");
  for (int i = 0; i < 10; i++) {
    printf("[ TEST ]: Buff[%d]='%s'\n", i,
           (p_tmp_buf + sizeof(char) * (BUFFER_SIZE * i)));
  }
  return p_tmp_buf;
}
