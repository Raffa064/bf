// Brain Fu** interpreter

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LEX_TOKEN_NAME_OFFSET 3
#include "./lex.h"

#define MEMORY_SIZE 128
#define STACK_SIZE  64

enum {
  BF_WS,                // 
  BF_COMMENT,          // ;  
  BF_MOVE,            // <>
  BF_OPERATOR,       // +-
  BF_IO,            // .,
  BF_LOOP,         // []
  BF_EXTENSION,   // !#@?
  BF_COUNT,      //
};

size_t bf_rule_move(LexCursor cursor)      { return lex_match_chars(cursor, "<>"); }
size_t bf_rule_operator(LexCursor cursor)  { return lex_match_chars(cursor, "+-"); }
size_t bf_rule_io(LexCursor cursor)        { return lex_match_chars(cursor, ".,"); }
size_t bf_rule_loop(LexCursor cursor)      { return lex_match_chars(cursor, "[]"); }
size_t bf_rule_extension(LexCursor cursor) { return lex_match_chars(cursor, "#@?!"); }

LexTokenType bf_tokens[BF_COUNT] = {
  LEX_TOKENTYPE(BF_WS,         lex_builtin_rule_ws,              .skip = true),
  LEX_TOKENTYPE(BF_COMMENT,    lex_builtin_rule_asmlike_comment, .skip = true),
  LEX_TOKENTYPE(BF_MOVE,       bf_rule_move),
  LEX_TOKENTYPE(BF_OPERATOR,   bf_rule_operator),
  LEX_TOKENTYPE(BF_IO,         bf_rule_io),
  LEX_TOKENTYPE(BF_LOOP,       bf_rule_loop),
  LEX_TOKENTYPE(BF_EXTENSION,  bf_rule_extension),
};

char* read_file(const char* path) {
  FILE* file = fopen(path, "r");
  char *content = NULL;

  if (file) {
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    content = malloc(size);

    fread(content, size, 1, file);
  } else {
    printf("Can't open %s\n", path);
  }

  fclose(file);

  return content;
}

void print_dbg(char* mem, int memsize, unsigned mp, unsigned sp) {
  printf("\n\e[32mmp:%d \e[31msp:%d\e[34m\n%50s\n", mp, sp, "[ mem ]");
  for (int  i = 0; i < memsize; i++)
    printf("%02x%s", mem[i], i % 32 == 31? "\n" : " ");
  printf("\e[0m\n");
}

int main(int argc, char** argv) {
  char *path = "./main.bf";

  if (argc >= 3)
    path = argv[argc - 1];

  bool print_lex = false;
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0) {
      print_lex = true;
    }
  }

  char* input = read_file(path);
  Lex l = lex_init(LEX_TOKENMAP(bf_tokens), input);

  if (print_lex) {
    lex_print_hl(l, true);
  }

  char      mem[128]  = {0};
  LexCursor stack[64] = {0};
  unsigned  mp        = 0;
  unsigned  sp        = 0;

  printf("mem: %lub stack: %lu\n", sizeof(mem), sizeof(stack)/sizeof(stack[0]));
  LexResult result;
  bool running = true;
  while (running && lex_current(&l, &result)) {
    char ch = lex_tkstr(l.tk)[0];

    bool jump = false;
    switch (l.tk.id) {
      case BF_MOVE: {
        if (ch == '<') mp -= 1;
        if (ch == '>') mp += 1;

        if (mp > MEMORY_SIZE) {
          printf("RuntimeError: Memory Overflow at line %d:%d\n", 
            (int) lex_curline(l.cursor), 
            (int) lex_curcol(l.cursor) 
          );

          running = false;
        }
      } break;
      case BF_OPERATOR: {
        if (ch == '-') mem[mp]--;
        if (ch == '+') mem[mp]++;
      } break;
      case BF_IO: {
        if (ch == '.') printf("\e[7;37m%c\e[0m", mem[mp]);
        if (ch == ',') mem[mp] = getchar();
      } break;
      case BF_LOOP: {
        if (ch == '[') {
          if (sp < STACK_SIZE)
            stack[sp++] = l.cursor;
          else {
            printf("RuntimeError: Stack Overflow at line %d:%d\n", 
              (int) lex_curline(l.cursor), 
              (int) lex_curcol(l.cursor) 
            );
            
            running = false;
          }
        }

        if (ch == ']' && mem[mp] != 0) jump = true;
      } break;
      case BF_EXTENSION: {
        if (ch == '!') printf("\ec");
        if (ch == '#') getchar(); 
        if (ch == '?') print_dbg(mem, sizeof(mem), mp, sp); 
        if (ch == '@') usleep(10000); 
      }
    }
    
    if (jump)
      lex_move_to(&l, stack[--sp]);
    else
      lex_move(&l);
  }

  if (result == LEX_INVALID_TOKEN) {
    printf(
      "Sintax error, at line %d:%d near: '%s'\n", 
      (int) lex_curline(l.cursor), 
      (int) lex_curcol(l.cursor), 
      lex_curstr(l.cursor)
    );
  }

  printf("\n");

  free(input);

  return 0;
}
