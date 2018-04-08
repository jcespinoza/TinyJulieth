#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include <stdexcept>
#include "ast.h"
#include "tokens.h"

using namespace std;

extern FILE *yyin;
extern int yylineno;
extern int errors;

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
	errors = 0;
	yylineno = 1;
	//try{
		yyparse();
	// }catch(std::exception& e){
	// 	cerr << "ERROR: " << e.what() << endl;
	// 	return 3;
	// }
  return errors > 0;
}
