%option noyywrap
%option yylineno
%option nodefault

%{
#include <stdio.h>
#include "tokens.h"
%}

letter [_a-zA-Z]

%x comments

%%

[ \t]                 { /* Nada */ }
#=                    { BEGIN(comments); }
#[^=][^\n]*[\r]?\n    { unput('\n'); }
\r\n                  {
                        return TK_NEWLINE;
                      }
\n                    {
                        return TK_NEWLINE;
                      }
">="                  { return OP_GEQUAL; }
"<="                  { return OP_LEQUAL; }
"=="                  { return OP_EQUALS; }
"!="                  { return OP_NEQUAL; }
">"                   { return '>'; }
"<"                   { return '<'; }
"<<"                  { return OP_ASHIFTL; }
">>"                  { return OP_ASHIFTR; }
"+"                   { return '+'; }
"-"                   { return '-'; }
"**"                  { return OP_POWER; }
"*"                   { return '*'; }
"%"                   { return '%'; }
"/"                   { return '/'; }
"("                   { return '('; }
")"                   { return ')'; }
";"                   { return TK_NEWLINE; }
"="                   { return '='; }
"{"                   { return '{'; }
"}"                   { return '}'; }
"::"                  { return TK_COLONS; }
":"                   { return ':'; }
"~"                   { return '~'; }
"^"                   { return '^'; }
"&&"                  { return OP_LOGAND; }
"||"                  { return OP_LOGOR; }
"&"                   { return '&'; }
"|"                   { return '|'; }
","                   { return ','; }
"["                   { return '['; }
"]"                   { return ']'; }
"?"                   { return '?'; }
"!"                   { return '!'; }
"$"                   { return '$'; }
"if"                  { return KW_IF; }
"for"                 { return KW_FOR; }
"else"                { return KW_ELSE; }
"elseif"              { return KW_ELSEIF; }
"print"               { return KW_PRINT; }
"println"             { return KW_PRINTLN; }
"while"               { return KW_WHILE; }
"Bool"                { return KW_BOOL; }
"Int"                 { return KW_INT; }
"Array"               { return KW_ARRAY; }
"function"            { return KW_FUNCTION; }
"end"                 { return KW_END; }
"return"              { return KW_RETURN; }
"break"               { return KW_BREAK; }
"continue"            { return KW_CONTINUE; }
"local"               { return KW_LOCAL; }
"global"              { return KW_GLOBAL; }
"true"                { return KW_TRUE; }
"false"               { return KW_FALSE;}

\"([^\"\\]|\\.)*\"    {
                        char* newText = new char[strlen(yytext)-1];
                        strncpy(newText, &yytext[1], strlen(yytext)-2);

                        yylval.string_t = strdup(newText);

                        return TK_STRING;
                      }
[0-9]+                {
                        yylval.int_t = atoi(strdup(yytext));
                        return TK_NUMBER;
                      }
{letter}({letter}|[0-9])* { yylval.string_t = strdup(yytext);
                        return TK_IDENTIFIER;
                      }
<INITIAL><<EOF>>      { unput('\n'); return 0; }
.                     { return TK_ERROR; }

<comments>=#          { BEGIN(INITIAL); }
<comments>.|\n        { /* NADA */ }
<comments><<EOF>>     { fprintf(stderr, "Line %d: Unmatched block comment\n", yylineno); return 0; }

%%
