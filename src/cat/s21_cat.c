#include "s21_cat.h"

void parser(int argc, char *argv[], opt *options);
void reader(char *file_name, opt *options);

void output(int argc, char **argv, opt *options, int optind);

int main(int argc, char *argv[]) {
  opt options = {0};
  parser(argc, argv, &options);
  // reader(argv,  &options);
  if (options.b == 1 && options.n == 1) {
    options.n = 0;
  }
  output(argc, argv, &options, optind);
  return 0;
}

void parser(int argc, char *argv[], opt *options) {
  int check_opt;
  int option_index;

  static struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                         {"number", 0, 0, 'n'},
                                         {"squeeze-blank", 0, 0, 's'},
                                         {0, 0, 0, 0}};

  while ((check_opt = getopt_long(argc, argv, "+benstvTE", long_options,
                                  &option_index)) != -1) {
    switch (check_opt) {
      case 'b':
        options->b = 1;
        break;

      case 'e':
        options->e = 1;
        options->v = 1;
        break;

      case 'E':
        options->e = 1;
        break;

      case 'n':
        options->n = 1;
        break;

      case 's':
        options->s = 1;
        break;

      case 't':
        options->t = 1;
        options->v = 1;
        break;

      case 'T':
        options->t = 1;
        break;

      case 'v':
        options->v = 1;
        break;

      default:
        fprintf(stderr, "cat usage: +benstvTE [file.txt]");
        exit(1);
    }
  }
}

void reader(char *file_name, opt *options) {
  FILE *ftr;

  if ((ftr = fopen(file_name, "r")) == NULL) {
    fprintf(stderr, "cat: No such file or directory\n");
  } else {
    char cur_symbol = fgetc(ftr);
    int line_count = 0, blank_line_fl, prev_blank_line_fl = 0;
    char prev_symbol = '\n';

    while (cur_symbol != EOF) {
      // check empty lines
      if (cur_symbol == '\n' && prev_symbol == '\n') {
        blank_line_fl = 1;
      } else {
        blank_line_fl = 0;
      }

      if (!(options->s && prev_blank_line_fl && blank_line_fl)) {
        if (prev_symbol == '\n') {
          if (options->n || (options->b && blank_line_fl == 0)) {
            printf("%6d\t", ++line_count);
          }
        }
        if (options->e && cur_symbol == '\n') {
          printf("$\n");
        } else if (options->t && cur_symbol == '\t') {
          printf("^I");
        } else if (options->v && cur_symbol < 32 && cur_symbol != '\n' &&
                   cur_symbol != '\t') {
          printf("^%c", cur_symbol + 64);
        } else if (options->v && cur_symbol == 127) {
          printf("^?");
        } else {
          printf("%c", cur_symbol);
        }
      }

      prev_symbol = cur_symbol;
      prev_blank_line_fl = blank_line_fl;
      cur_symbol = fgetc(ftr);
    }
    fclose(ftr);
  }
}

void output(int argc, char **argv, opt *options, int optind) {
  for (int i = optind; i < argc; i++) {
    reader(argv[i], options);
  }
}
