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
	ExpressionList* expressionlist_t;
	VarDeclStatement* vardecl_t;
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
%type<statementlist_t> gstatement_list statement_list opt_otherwise  elseif else_clause
%type<statement_t> gstatement_nl g_statement
%type<statement_t> statement statement_nl while_statement for_statement
%type<statement_t> print_statement func_declaration return_statement if_statement
%type<statement_t> assign_statement
%type<vardecl_t> decl_statement type_and_value
%type<paramlist_t> opt_func_params func_params
%type<param_t> param_decl
%type<type_t> assert_type
%type<expressionlist_t> print_args expression_list opt_call_args
%type<expression_t> expression print_arg
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

statement_list: statement_nl statement_list { $$ = $2; $$->AddNew($1);  }
	| statement_nl { $$ = new StatementList(); $$->AddNew($1);  }
;

statement_nl: statement newlines { $$ = $1; }
;

statement: assign_statement { $$ = $1; }
	| decl_statement  { $$ = $1; }
	| print_statement  { $$ = $1; }
	| return_statement  { $$ = $1; }
	| while_statement  { $$ = $1; }
	| for_statement  { $$ = $1; }
	| if_statement  { $$ = $1; }
	;

if_statement: KW_IF expression opt_newlines statement_list
	 							opt_otherwise KW_END { $$ = new IfStatement($2, $4, $5); }
;

opt_otherwise: elseif { $$ = $1; }
	| else_clause { $$ = $1; }
	| %empty { $$ = new StatementList(); }
;

elseif: KW_ELSEIF expression opt_newlines statement_list
										opt_otherwise {
											$$ = new StatementList();
											IfStatement* ifSt = new IfStatement($2, $4, $5);
											$$->AddNew(ifSt);
										}
;

else_clause: KW_ELSE opt_newlines statement_list { $$ = $3; }
;

for_statement: KW_FOR TK_IDENTIFIER '=' expression ':' expression
	opt_newlines statement_list KW_END { $$ = new ForStatement($2, $4, $6, $8); }
;

while_statement: KW_WHILE expression opt_newlines statement_list KW_END { $$ = new WhileStatement($2, $4); }

return_statement: KW_RETURN expression { $$ = new ReturnStatement($2); }
;

decl_statement: TK_IDENTIFIER TK_COLONS type_and_value { $$ = $3; $$->SetVarName($1); }
;

type_and_value: assert_type '=' expression { $$ = new ScalarVarDeclStatement((char*)"n", $1, $3); }
	| KW_ARRAY '{' assert_type '}' '=' '[' expression_list ']' {
																	ObjectType* arrType = new ObjectType($3->typeCode + 2);
																	$$ = new ArrayVarDeclStatement((char*)"n", arrType, $7);
																}
;

expression_list: expression ',' expression_list { $$ = $3; $3->AddNew($1); }
	| expression { $$ = new ExpressionList(); $$->AddNew($1); }
;

assign_statement: TK_IDENTIFIER '=' expression {  $$ = new AssignStatement($1, $3); }
	| TK_IDENTIFIER '[' expression ']' '=' expression { $$ = new ArrayItemAssignStatement($1, $3, $6); }
;

assert_type: KW_INT { $$ = new ObjectType(IntType); }
	| KW_BOOL { $$ = new ObjectType(BoolType); }
;

print_statement: KW_PRINT '(' print_args ')' { $$ = new PrintStatement($3, false); }
	| KW_PRINTLN '(' print_args ')' { $$ = new PrintStatement($3, true); }
;

print_args: print_arg ',' print_args { $$ = $3; $3->AddNew($1); }
	| print_arg { $$ = new ExpressionList(); $$->AddNew($1);  }
;

print_arg: expression { $$ = $1; }
	| TK_STRING { $$ = new StrExpression($1); }
;

expression: log_expression OP_LOGAND expression { $$ = new NumExpression(0); }
	| log_expression OP_LOGOR expression { $$ = new NumExpression(0); }
	| log_expression { $$ = new NumExpression(0); }
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
