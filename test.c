

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DY_SIZE 3
#define CHAR_SIZE 1024

typedef struct {
  char (*data)[CHAR_SIZE];
  size_t len;
} Token;

Token *tokenizer(char *buf);
int main() {
  // char set[DY_SIZE][CHAR_SIZE];

  char input[CHAR_SIZE] = "go build main.go";

  Token *tokenized_input = tokenizer(input);

  printf("[ TEST ]: length of input tokenized %lu\n", tokenized_input->len);
  for (int i = 0; i < tokenized_input->len; i++) {
    printf("[ TEST ]: String[%d]=%s\n", i, tokenized_input->data[i]);
  }


		free(tokenized_input->data);
  free(tokenized_input);

  return 0;
}

Token *tokenizer(char *buf) {

  Token *token = malloc(sizeof(Token));

  if (token == NULL) {
    perror("[ ERROR ]: Unable to allocate token for return");
    exit(1);
  }
  char (*set)[CHAR_SIZE] = malloc(DY_SIZE * sizeof *set);
  int base = 0;
  size_t word_count = 0;
  size_t size_by = 2;
  printf("STRLEN = %lu\n", strlen(buf));

  for (int i = 0; i <= strlen(buf); i++) {
    if (buf[i] == ' ' || buf[i] == '\0' || buf[i] == EOF || buf[i] == '\n') {

      for (int j = 0; j < i - base; j++) {
        set[word_count][j] = buf[base + j];
      }

      set[word_count][i] = '\0';
      base = i + 1;
      word_count += 1;

      char (*tmp)[CHAR_SIZE] = realloc(set, size_by * sizeof *set);
      if (tmp == NULL) {
        perror("[ ERROR ]: Unable to reallocate buffer for set");
        exit(1);
      }
      set = tmp;
    }
  }
  for (int i = 0; i < DY_SIZE; i++) {
    printf("STRING[%d]=%s\n", i, set[i]);
  }
  Token *tmp_token = realloc(token, sizeof(Token) + sizeof(*set));

  if (tmp_token == NULL) {
    perror("[ ERROR ]: Unable to reallocate token for set's size");
    exit(1);
  }
  token->data = set;
  token->len = word_count;

  return token;
}
