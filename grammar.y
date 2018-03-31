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
extern char* yytext;
extern int yylineno;

void yyerror(const char *msg) {
	errors++;
	fprintf(stderr, "%s\n", msg);
	fprintf(stderr, "Ln %d Input is: %s\n", yylineno, yytext);
}

using namespace std;

//BlockStatement *statement;

#define YYERROR_VERBOSE 1
#define YYDEBUG 1
#define YYTRACE(str) fprintf(stderr, " <Yacc>%s", str);

static void yyprint (FILE* file, int type, YYSTYPE value)
{
  if (type == TK_ID)
    fprintf (file, " %s", value.string_t);
  else if (type == TK_NUMBER)
    fprintf (file, " %d", value.int_t);
}


#define YYPRINT(file, type, value) yyprint (file, type, value)

%}

%union {
	int int_t;
  char *string_t;
}

%token TK_COLONS
%token OP_GEQUAL OP_LEQUAL OP_EQUALS OP_NEQUAL OP_GTHAN OP_LTHAN
%token OP_LOGAND OP_LOGOR OP_POWER
%token OP_ASHIFTL OP_ASHIFTR
%token KW_IF KW_FOR KW_ELSE KW_ELSEIF KW_PRINT KW_WHILE KW_BOOL
%token KW_INT32 KW_FUNCTION KW_END KW_RETURN KW_BREAK KW_CONTINUE
%token KW_LOCAL KW_GLOBAL

%token<string_t> TK_NUMBER "number"
%token<string_t> TK_ID "identifier"

// %type<expr_t> expr term factor
// %type<statement_t> assign_statement
// %type<statement_t> if_statement
// %type<statement_t> opt_else
// %type<statement_t> while_statement
// %type<statement_t> print_statement
// %type<statement_t> statement
// %type<blk_statement_t> statement_list opt_statement_list
%start root
%error-verbose
%debug
%%

root: ':' { YYTRACE("Yeap, all good\n"); }
	| %empty { YYTRACE("empty file, yep\n"); }
;

%%
