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
#define YYTRACE(str) fprintf(stderr, " ___%s", str);

static void yyprint (FILE* file, int type, YYSTYPE value)
{
  if (type == TK_IDENTIFIER)
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
%token '+' '-' '*' '%' '/' '(' ')' ';' '=' '{' '}' ':'
%token '~' '^' '&' '|' ',' '[' ']' '?'
%token OP_GEQUAL OP_LEQUAL OP_EQUALS OP_NEQUAL
%token OP_LOGAND OP_LOGOR OP_POWER
%token OP_ASHIFTL OP_ASHIFTR
%token KW_IF KW_FOR KW_ELSE KW_ELSEIF KW_PRINT KW_PRINTLN KW_WHILE KW_BOOL
%token KW_INT KW_FUNCTION KW_END KW_RETURN KW_BREAK KW_CONTINUE
%token KW_LOCAL KW_GLOBAL
%token TK_ERROR TK_NEWLINE

%token<string_t> TK_NUMBER "number"
%token<string_t> TK_STRING "str_literal"
%token<string_t> TK_IDENTIFIER "identifier"

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

root: opt_newlines gstatement_list { YYTRACE("PASSED: File with Statements\n"); }
	| opt_newlines { YYTRACE("PASSED: Empty File\n"); }
;

opt_newlines: newlines { }
	| %empty
;

newlines: TK_NEWLINE newlines { }
	| TK_NEWLINE { }
;

gstatement_list: gstatement_nl gstatement_list { }
	| gstatement_nl { };
;

gstatement_nl: g_statement newlines { }
;

g_statement: statement { }
	| func_declaration { }
;

func_declaration: KW_FUNCTION TK_IDENTIFIER '(' opt_func_params ')' TK_COLONS assert_type
											opt_newlines statement_list KW_END
;

opt_func_params: func_params { }
	| %empty { }
;

func_params: param_decl ',' func_params { }
	| param_decl { }
;

param_decl: TK_IDENTIFIER TK_COLONS assert_type { }
;

statement_list: statement_nl statement_list { }
	| statement_nl { }
;

statement_nl: statement newlines { }
;

statement: assign_statement { }
	| decl_statement { }
	| print_statement { }
	| return_statement { }
	| while_statement { }
	| for_statement { }
;

for_statement: KW_FOR TK_IDENTIFIER '=' expression ':' expression
	opt_newlines statement_list KW_END { }
;

while_statement: KW_WHILE expression opt_newlines statement_list KW_END { }

return_statement: KW_RETURN expression { }
;

decl_statement: TK_IDENTIFIER TK_COLONS assert_type '=' expression { }
;

assign_statement: TK_IDENTIFIER '=' expression { }
;

assert_type: KW_INT { }
	| KW_BOOL { }
;

print_statement: KW_PRINT '(' print_args ')' { }
	| KW_PRINTLN '(' print_args ')' { }
;

print_args: print_arg ',' print_args { }
	| print_arg { }
;

print_arg: expression { }
	| TK_STRING { }
;

expression: add_expression '<' expression { /*$$ = new LthanExpression($1, $3); */ }
| add_expression '>' expression { /*$$ = new GthanExpression($1, $3); */ }
| add_expression OP_EQUALS expression { /*$$ = new EquExpression($1, $3); */ }
| add_expression OP_NEQUAL expression { /*$$ = new NequExpression($1, $3); */ }
| add_expression OP_LEQUAL expression { /*$$ = new LeqExpression($1, $3); */ }
| add_expression OP_GEQUAL expression { /*$$ = new GeqExpression($1, $3); */ }
| add_expression { /* $$ = $1; */ }
;

add_expression: term '+' add_expression { /* $$ = new AddExpression($1, $3); */ }
| term '-' add_expression { /* $$ = new SubExpression($1, $3); */ }
| term { /* $$ = $1; */ }
;

term: factor '/' term { /* $$ = new DivExpression($1, $3); */ }
| factor '%' term { /* $$ = new ModExpression($1, $3); */ }
| factor '*' term { /* $$ = new MulExpression($1, $3); */ }
| factor '^' term { /* $$ = new PowExpression($1, $3); */ }
| factor { /* $$ = $1; */ }
;

factor: TK_NUMBER { /* $$ = new NumExpression($1); */ }
| '-' factor { }
| id_expressions { }
| '(' expression ')' { /* $$ = $2; */ }
;

id_expressions: TK_IDENTIFIER { /*$$ = new IdExpression($1); */}
	| TK_IDENTIFIER '(' opt_call_args ')'
;

opt_call_args: call_args { }
	| %empty { }
;

call_args: expression ',' call_args { }
	| expression { }
;

%%
