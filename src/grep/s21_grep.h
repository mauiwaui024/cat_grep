#define _GNU_SOURCE
#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  char* eopt[4000];
  int eindex;
  int files_count;
} opt;

void parser(int argc, char* argv[], opt* options);
void reader(char* file_name, opt* options, char** argv);
void output(int argc, char** argv, opt* options);
void free_regexes(opt* options);