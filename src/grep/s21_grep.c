#include "s21_grep.h"

int main(int argc, char* argv[]) {
  opt options = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {NULL}, 0, 0};

  parser(argc, argv, &options);

  output(argc, argv, &options);
  free_regexes(&options);

  return 0;
}

void parser(int argc, char* argv[], opt* options) {
  char* pattern_file = NULL;
  int check_opt;

  while ((check_opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (check_opt) {
      case 'e':
        options->e = 1;
        options->eopt[options->eindex++] = optarg;
        break;

      case 'i':
        options->i = 1;
        break;

      case 'v':
        options->v = 1;
        break;

      case 'c':
        options->c = 1;
        break;

      case 'l':
        options->l = 1;
        break;

      case 'n':
        options->n = 1;
        break;
      case 'h':
        options->h = 1;
        break;
      case 's':
        options->s = 1;
        break;

      case 'f':
        options->f = 1;
        pattern_file = optarg;
        FILE* ptf;
        if ((ptf = fopen(pattern_file, "r")) == NULL) {
          printf("unable to read from pattern file");
        } else {
          char* line = NULL;
          size_t len = 0;
          ssize_t read;

          while ((read = getline(&line, &len, ptf)) != -1) {
            // Trim newline character from line
            line[strcspn(line, "\n")] = '\0';
            // Compile regular expression and add to eopt array
            options->eopt[options->eindex++] = strdup(line);
            // free(line);
            // line = NULL;
            // len = 0;
          }
          if (line) {
            free(line);
          }
          fclose(ptf);
        }

        break;
      case 'o':
        options->o = 1;
        break;
      default:
        fprintf(stderr, "Invalid option");
        exit(1);
    }
  }

  //считаем количество файлов
  if (options->f || options->e) {
    for (int i = optind; i < argc; i++) {
      options->files_count++;
    }
  } else {
    for (int i = optind + 1; i < argc; i++) {
      options->files_count++;
    }
  }
}

void reader(char* file_name, opt* options, char** argv) {
  FILE* ftr;

  if ((ftr = fopen(file_name, "r")) == NULL) {
    if (!options->s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", file_name);
    }

  } else {
    int matched_line_count = 0;
    //для l options we use this flag
    int file_is_printed = 0;
    int line_count = 0;
    // for -o flag
    regmatch_t matches[1];

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    //для е and f опции
    regex_t regex[options->eindex];
    //без опций
    regex_t reegex;
    if (options->e || options->f) {
      for (int i = 0; i < options->eindex; i++) {
        regcomp(&regex[i], options->eopt[i], REG_EXTENDED);
      }
    } else if (options->i) {
      char* pattern = argv[optind];
      int i = 0;
      while (pattern[i]) {
        pattern[i] = tolower(pattern[i]);
        i++;
      }
      regcomp(&reegex, pattern, REG_EXTENDED);
    } else {
      char* pattern = argv[optind];
      regcomp(&reegex, pattern, REG_EXTENDED);
    }

    // Compile regular expression for each element in eopt array
    while ((read = getline(&line, &len, ftr)) != -1) {
      int match_found = 0;
      line_count++;
      // Match line against each regular expression in eopt array
      if (options->e || options->f) {
        for (int i = 0; i < options->eindex; i++) {
          if (regexec(&regex[i], line, 0, NULL, 0) == 0) {
            match_found = 1;
            break;
          }
        }
        if (match_found) {
          if (options->files_count > 1) {
            int has_newline = (line[strlen(line) - 1] == '\n');
            if (!has_newline) {
              printf("%s:%s\n", file_name, line);
            } else {
              printf("%s:%s", file_name, line);
            }
          } else {
            int has_newline = (line[strlen(line) - 1] == '\n');
            if (!has_newline) {
              printf("%s\n", line);
            } else {
              printf("%s", line);
            }
          }
        }

      } else if (options->c && options->v) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 1) {
          matched_line_count++;
        }
      } else if (options->v && options->l) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 1) {
          if (!file_is_printed) {
            printf("%s\n", file_name);
            file_is_printed = 1;
          }
        }
      } else if (options->v && options->n) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 1) {
          if (options->files_count > 1) {
            int has_newline = (line[strlen(line) - 1] == '\n');

            if (!has_newline) {
              printf("%s:%d:%s\n", file_name, line_count, line);
            } else {
              printf("%s:%d:%s", file_name, line_count, line);
            }
          } else {
            int has_newline = (line[strlen(line) - 1] == '\n');

            if (!has_newline) {
              printf("%d:%s\n", line_count, line);
            } else {
              printf("%d:%s", line_count, line);
            }
          }
        }
      } else if (options->v && options->i) {
        char* line_copy = strdup(line);

        // Convert line to lowercase
        for (int i = 0; line_copy[i]; i++) {
          line_copy[i] = tolower(line_copy[i]);
        }
        int search_result = regexec(&reegex, line_copy, 0, NULL, 0);
        // Do something with the lowercase line
        if (search_result == 1) {
          if (options->files_count > 1) {
            int has_newline = (line[strlen(line) - 1] == '\n');
            if (!has_newline) {
              if (options->n) {
                printf("%s:%d:%s\n", file_name, line_count, line);
              } else {
                printf("%s:%s\n", file_name, line);
              }

            } else {
              if (options->n) {
                printf("%d:%s", line_count, line);
              } else {
                printf("%s:%s", file_name, line);
              }
            }
          } else {
            int has_newline = (line[strlen(line) - 1] == '\n');
            if (!has_newline) {
              if (options->n) {
                printf("%d:%s\n", line_count, line);
              } else {
                printf("%s\n", line);
              }

            } else {
              if (options->n) {
                printf("%d:%s", line_count, line);
              } else {
                printf("%s", line);
              }
            }
          }
          if (line_copy) {
            free(line_copy);
          }
        }
      } else if (options->i) {
        char* line_copy = strdup(line);
        // Convert line to lowercase
        for (int i = 0; line_copy[i]; i++) {
          line_copy[i] = tolower(line_copy[i]);
        }

        int search_result = regexec(&reegex, line_copy, 0, NULL, 0);

        if (options->o) {
          search_result = regexec(&reegex, line_copy, 1, matches, 0);
        }
        // Do something with the lowercase line
        if (!options->o) {
          if (search_result == 0) {
            if (!options->c) {
              if (options->files_count > 1) {
                int has_newline = (line[strlen(line) - 1] == '\n');
                if (!has_newline) {
                  if (options->n) {
                    printf("%s:%d:%s\n", file_name, line_count, line);
                  } else {
                    printf("%s:%s\n", file_name, line);
                  }

                } else {
                  if (options->n) {
                    printf("%d:%s", line_count, line);
                  } else {
                    printf("%s:%s", file_name, line);
                  }
                }
              } else {
                int has_newline = (line[strlen(line) - 1] == '\n');
                if (!has_newline) {
                  if (options->n) {
                    if (!options->l) {
                      printf("%d:%s\n", line_count, line);
                    }
                  } else {
                    printf("%s\n", line);
                  }

                } else {
                  if (options->n) {
                    if (!options->l) {
                      printf("%d:%s", line_count, line);
                    }
                  } else {
                    printf("%s", line);
                  }
                }
              }
            } else {
              matched_line_count++;
            }
          }
        } else {
          if (search_result == 0) {
            if (options->files_count > 1) {
              for (int i = 0; i < 1; i++) {
                int start = matches[i].rm_so;
                int end = matches[i].rm_eo;
                if (start != -1 && end != -1) {
                  printf("%s:%.*s\n", file_name, end - start, line + start);
                }
              }
            } else {
              for (int i = 0; i < 1; i++) {
                int start = matches[i].rm_so;
                int end = matches[i].rm_eo;
                if (start != -1 && end != -1) {
                  printf("%.*s\n", end - start, line + start);
                }
              }
            }
          }
        }

        if (line_copy) {
          free(line_copy);
        }
      } else if (options->v) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 1) {
          if (options->files_count > 1) {
            int has_newline = (line[strlen(line) - 1] == '\n');
            if (!has_newline) {
              printf("%s:%s\n", file_name, line);
            } else {
              printf("%s:%s", file_name, line);
            }
          } else {
            int has_newline = (line[strlen(line) - 1] == '\n');
            if (!has_newline) {
              printf("%s\n", line);
            } else {
              printf("%s", line);
            }
          }
        }
      } else if (options->c) {
        //нужно создать переменную матч_лайн_каунт
        //если файлов больше чем 1, то выводим еще и название файла
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 0) {
          matched_line_count++;
        }

      } else if (options->l) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 0) {
          if (!file_is_printed) {
            printf("%s\n", file_name);
            file_is_printed = 1;
          }
        }
        ///
      } else if (options->n) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 0) {
          if (options->files_count > 1) {
            int has_newline = (line[strlen(line) - 1] == '\n');

            if (!has_newline) {
              printf("%s:%d:%s\n", file_name, line_count, line);
            } else {
              printf("%s:%d:%s", file_name, line_count, line);
            }
          } else {
            int has_newline = (line[strlen(line) - 1] == '\n');

            if (!has_newline) {
              printf("%d:%s\n", line_count, line);
            } else {
              printf("%d:%s", line_count, line);
            }
          }
        }
      } else if (options->h) {
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 0) {
          int has_newline = (line[strlen(line) - 1] == '\n');

          if (!has_newline) {
            printf("%s\n", line);
          } else {
            printf("%s", line);
          }
        }
      } else if (options->o) {
        int search_result = regexec(&reegex, line, 1, matches, 0);
        if (search_result == 0) {
          if (options->files_count > 1) {
            for (int i = 0; i < 1; i++) {
              int start = matches[i].rm_so;
              int end = matches[i].rm_eo;
              if (start != -1 && end != -1) {
                printf("%s:%.*s\n", file_name, end - start, line + start);
              }
            }
          } else {
            for (int i = 0; i < 1; i++) {
              int start = matches[i].rm_so;
              int end = matches[i].rm_eo;
              if (start != -1 && end != -1) {
                printf("%.*s\n", end - start, line + start);
              }
            }
          }
        }

      } else {
        //логика без флагов
        int search_result = regexec(&reegex, line, 0, NULL, 0);
        if (search_result == 0) {
          if (options->files_count > 1) {
            int has_newline = (line[strlen(line) - 1] == '\n');

            if (!has_newline) {
              printf("%s:%s\n", file_name, line);
            } else {
              printf("%s:%s", file_name, line);
            }
          } else {
            int has_newline = (line[strlen(line) - 1] == '\n');

            if (!has_newline) {
              printf("%s\n", line);
            } else {
              printf("%s", line);
            }
          }
        }
      }
    }

    if (options->c) {
      // if (options->i) {
      //   printf("%s\n", file_name);
      // } else

      if (options->n) {
        if (options->v) {
          if (options->l) {
            printf("%s\n", file_name);
          } else {
            printf("%d\n", matched_line_count);
          }

        } else if (!options->l && !options->i) {
          printf("%d\n", matched_line_count);
        } else {
          printf("%s\n", file_name);
        }

      } else if (options->l) {
        printf("%s\n", file_name);
      } else if (options->files_count > 1) {
        printf("%s:%d\n", file_name, matched_line_count);

      } else {
        printf("%d\n", matched_line_count);
      }
    }
    //////////////////////////ЧИСТКА ПАМЯТИ!!!!!!!!!!//////////////////////////

    if (options->e || options->f) {
      for (int i = 0; i < options->eindex; i++) {
        regfree(&regex[i]);
      }
    } else {
      regfree(&reegex);
    }

    if (line) {
      free(line);
    }

    fclose(ftr);
  }
}

void output(int argc, char** argv, opt* options) {
  if (options->e || options->f) {
    for (int i = optind; i < argc; i++) {
      reader(argv[i], options, argv);
    }

  } else {
    for (int i = optind + 1; i < argc; i++) {
      reader(argv[i], options, argv);
    }
  }
}

void free_regexes(opt* options) {
  if (options->f && !options->e) {
    for (int i = 0; i < options->eindex; i++) {
      if (options->eopt) free(options->eopt[i]);
    }
  }
}