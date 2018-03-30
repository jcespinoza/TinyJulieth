%code requires {
	#include "ast.h"
}
%{
#include <stdio.h>
#include <iostream>
#include <string>
#include "tokens.h"

int yylex();
int errors;

void yyerror(const char *msg) {
	errors++;
	fprintf(stderr, "%s\n", msg);
}

using namespace std;

//BlockStatement *statement;

#define YYERROR_VERBOSE 1
%}

%union {
  Expr *expr_t;
	Statement *statement_t;
	BlockStatement *blk_statement_t;
  char *string_t;
}

// %token OP_ADD OP_SUB OP_MUL OP_DIV TK_LEFT_PAR TK_RIGHT_PAR
// %token<string_t> TK_NUMBER "number"
// %token<string_t> TK_ID "identifier"
// %token KW_IF KW_ELSE KW_WHILE KW_PRINT
// %token TK_ERROR
// %type<expr_t> expr term factor
// %type<statement_t> assign_statement
// %type<statement_t> if_statement
// %type<statement_t> opt_else
// %type<statement_t> while_statement
// %type<statement_t> print_statement
// %type<statement_t> statement
// %type<blk_statement_t> statement_list opt_statement_list

%%



%%
