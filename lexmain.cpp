#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include "ast.h"

using namespace std;

extern FILE *yyin;
extern int yylineno;
extern char* yytext;

int yylex();

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	yyin = fopen(argv[1], "rb");
	if (yyin == NULL) {
		fprintf(stderr, "Cannot open input file '%s'\n", argv[1]);
		return 2;
	}
	yylineno = 1;

	int tokenCode;
  while((tokenCode = yylex())){
    printf("%s", yytext);
  }

  return 0;
}
