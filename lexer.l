%option noyywrap
%option nodefault

%{
#include <stdio.h>
#include "tokens.h"
%}

letter [_a-zA-Z]

%x comments

%%

[ \t]	{ /* Nada */ }
"#="         { BEGIN(comments); }
"#".*\n      { /* ignore */ }
\n    { return TK_NEWLINE; }
"++"  { return OP_INCREMENT; }
"--"  { return OP_DECREMENT; }
">="  { return OP_GEQUAL; }
"<="  { return OP_LEQUAL; }
"=="  { return OP_EQUALS; }
"!="  { return OP_NEQUAL; }
">"   { return OP_GTHAN; }
"<"   { return OP_LTHAN; }
"+"		{ return '+'; }
"-"		{ return '-'; }
"**"  { return OP_POWER; }
"*"		{ return '*'; }
"%"   { return '%'; }
"/"		{ return '/'; }
"("		{ return '('; }
")"		{ return ')'; }
";"		{ return ';'; }
"="		{ return '='; }
"{"		{ return '{'; }
"}"		{ return '}'; }
"if"	{ return KW_IF; }
"for" { return KW_FOR; }
"else"	{ return KW_ELSE; }
"print"	{ return KW_PRINT; }
"while"	{ return KW_WHILE; }
\"([^\"\\]|\\.)*\" {
        char* newText = new char[strlen(yytext)-1];
        strncpy(newText, &yytext[1], strlen(yytext)-2);

        //yylval.string_t = strdup(newText);

        return TK_STRING;
      }
[0-9]+	{ //yylval.string_t = strdup(yytext);
      return TK_NUMBER; }
{letter}({letter}|[0-9])* { //yylval.string_t = strdup(yytext);
      return TK_ID; }
.		{ return TK_ERROR; }

<comments>"=#" { BEGIN(INITIAL); }
<comments>.|\n { /* NADA */ }
<comments><<EOLF>> { printf("Line %d: Unmatched block comment\n", yylineno); return 0; }

%%
