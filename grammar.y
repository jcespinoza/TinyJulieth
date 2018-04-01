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

JuliaDocument* document;

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
	Statement* statement_t;
	StatementList* statementlist_t;
	Expression* expression_t;
	ParamList* paramlist_t;
	FuncParam* param_t;
	ObjectType* type_t;
}

%token TK_COLONS
%token '+' '-' '*' '%' '/' '(' ')' ';' '=' '{' '}' ':'
%token '~' '^' '&' '|' ',' '[' ']' '?' '!' '$'
%token OP_GEQUAL OP_LEQUAL OP_EQUALS OP_NEQUAL
%token OP_LOGAND OP_LOGOR OP_POWER
%token OP_ASHIFTL OP_ASHIFTR
%token KW_IF KW_FOR KW_ELSE KW_ELSEIF KW_PRINT KW_PRINTLN KW_WHILE KW_BOOL
%token KW_INT KW_FUNCTION KW_END KW_RETURN KW_BREAK KW_CONTINUE
%token KW_LOCAL KW_GLOBAL KW_ARRAY KW_TRUE KW_FALSE
%token TK_ERROR TK_NEWLINE

%token<string_t> TK_NUMBER "number"
%token<string_t> TK_STRING "str_literal"
%token<string_t> TK_IDENTIFIER "identifier"

//%type<expression_t> expression term factor
%type<statementlist_t> gstatement_list statement_list
%type<statement_t> gstatement_nl g_statement
%type<statement_t> statement statement_nl
%type<statement_t> print_statement func_declaration
%type<paramlist_t> opt_func_params func_params
%type<param_t> param_decl
%type<type_t> assert_type
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

root: opt_newlines gstatement_list { document = new JuliaDocument(); document->statements = $2; document->Print(); YYTRACE("PASSED: File with Statements\n"); }
	| opt_newlines { document = new JuliaDocument(); document->statements = new StatementList();  YYTRACE("PASSED: Empty File\n"); }
;

opt_newlines: newlines { }
	| %empty
;

newlines: TK_NEWLINE newlines { }
	| TK_NEWLINE { }
;

gstatement_list: gstatement_nl gstatement_list {$$ = $2; $$->AddNew($1);  }
	| gstatement_nl { $$ = new StatementList(); $$->AddNew($1); };
;

gstatement_nl: g_statement newlines { $$ = $1; }
;

g_statement: statement { $$ = $1; }
	| func_declaration { $$ = $1; }
;

func_declaration: KW_FUNCTION TK_IDENTIFIER '(' opt_func_params ')' TK_COLONS assert_type
											opt_newlines statement_list KW_END { $$ = new PassStatement(); }
;

opt_func_params: func_params { }
	| %empty { }
;

func_params: param_decl ',' func_params { }
	| param_decl { }
;

param_decl: TK_IDENTIFIER TK_COLONS assert_type { }
;

statement_list: statement_nl statement_list { $$ = new StatementList(); }
	| statement_nl { $$ = new StatementList();   }
;

statement_nl: statement newlines { $$ = $1; }
;

statement: assign_statement { $$ = new PassStatement(); }
	| decl_statement  { $$ = new PassStatement(); }
	| print_statement  { $$ = new PassStatement(); }
	| return_statement  { $$ = new PassStatement(); }
	| while_statement  { $$ = new PassStatement(); }
	| for_statement  { $$ = new PassStatement(); }
	| if_statement  { $$ = new PassStatement(); }
	;

if_statement: KW_IF expression opt_newlines statement_list
	 							opt_otherwise KW_END { }
;

opt_otherwise: elseif { }
	| else_clause { }
	| %empty { }
;

elseif: KW_ELSEIF expression opt_newlines statement_list opt_otherwise { }
;

else_clause: KW_ELSE opt_newlines statement_list { }
;

for_statement: KW_FOR TK_IDENTIFIER '=' expression ':' expression
	opt_newlines statement_list KW_END { }
;

while_statement: KW_WHILE expression opt_newlines statement_list KW_END { }

return_statement: KW_RETURN expression { }
;

decl_statement: TK_IDENTIFIER TK_COLONS type_and_value { }
;
type_and_value: assert_type '=' expression { }
	| KW_ARRAY '{' assert_type '}' '=' '[' expression_list ']' { }
;

expression_list: expression ',' expression_list { }
	| expression { }
;

assign_statement: TK_IDENTIFIER '=' expression { }
	| TK_IDENTIFIER '[' expression ']' '=' expression { }
;

assert_type: KW_INT { $$ = new ObjectType(IntType); }
	| KW_BOOL { $$ = new ObjectType(BoolType); }
;

print_statement: KW_PRINT '(' print_args ')' { $$ = new PassStatement(); }
	| KW_PRINTLN '(' print_args ')' { }
;

print_args: print_arg ',' print_args { }
	| print_arg { }
;

print_arg: expression { }
	| TK_STRING { }
;

expression: log_expression OP_LOGAND expression { }
	| log_expression OP_LOGOR expression { }
	| log_expression { }
;

log_expression: add_expression '<' log_expression { /*$$ = new LthanExpression($1, $3); */ }
| add_expression '>' log_expression { /*$$ = new GthanExpression($1, $3); */ }
| add_expression OP_EQUALS log_expression { /*$$ = new EquExpression($1, $3); */ }
| add_expression OP_NEQUAL log_expression { /*$$ = new NequExpression($1, $3); */ }
| add_expression OP_LEQUAL log_expression { /*$$ = new LeqExpression($1, $3); */ }
| add_expression OP_GEQUAL log_expression { /*$$ = new GeqExpression($1, $3); */ }
| add_expression { /* $$ = $1; */ }
;

add_expression: shift_expression '+' add_expression { /* $$ = new AddExpression($1, $3); */ }
| shift_expression '-' add_expression { /* $$ = new SubExpression($1, $3); */ }
| shift_expression '$' add_expression { } //XOR
| shift_expression '|' add_expression { }
| shift_expression { /* $$ = $1; */ }
;

shift_expression: term OP_ASHIFTL shift_expression { }
	| term OP_ASHIFTR shift_expression { }
	| term { }
;

term: factor '/' term { /* $$ = new DivExpression($1, $3); */ }
| factor '%' term { /* $$ = new ModExpression($1, $3); */ }
| factor '*' term { /* $$ = new MulExpression($1, $3); */ }
| factor '&' term { }
| factor { /* $$ = $1; */ }
;

factor: expofactor '^' factor { }
	| expofactor { }
;

expofactor: TK_NUMBER { /* $$ = new NumExpression($1); */ }
| bool_literal { }
| '-' expofactor { }
| '~' expofactor { }
| '!' expofactor { }
| id_expressions { }
| '(' expression ')' { /* $$ = $2; */ }
;

bool_literal: KW_TRUE { }
	| KW_FALSE { }
;

id_expressions: TK_IDENTIFIER { /*$$ = new IdExpression($1); */}
	| TK_IDENTIFIER '(' opt_call_args ')' { }
	| TK_IDENTIFIER '[' expression ']' { }
;

opt_call_args: expression_list { }
	| %empty { }
;

%%
