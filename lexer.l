%option noyywrap
%option nodefault

%{
#include <stdio.h>
#include "tokensl.h"
%}

letter [_a-zA-Z]

%x comments

%%

[ \t]                 { /* Nada */ }
#=                    { BEGIN(comments); }
#[^=][^\n]*\n         { /* ignore */ }
#\n                   { /* ignore */ }
\n                    { return TK_NEWLINE; }
">="                  { return OP_GEQUAL; }
"<="                  { return OP_LEQUAL; }
"=="                  { return OP_EQUALS; }
"!="                  { return OP_NEQUAL; }
">"                   { return OP_GTHAN; }
"<"                   { return OP_LTHAN; }
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
";"                   { return ';'; }
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
"if"                  { return KW_IF; }
"for"                 { return KW_FOR; }
"else"                { return KW_ELSE; }
"elseif"              { return KW_ELSEIF; }
"print"               { return KW_PRINT; }
"while"               { return KW_WHILE; }
"Bool"                { return KW_BOOL; }
"Int32"               { return KW_INT32; }
"function"            { return KW_FUNCTION; }
"end"                 { return KW_END; }
"return"              { return KW_RETURN; }
"break"               { return KW_BREAK; }
"continue"            { return KW_CONTINUE; }
"local"               { return KW_LOCAL; }
"global"              { return KW_GLOBAL; }

\"([^\"\\]|\\.)*\"    {
                        char* newText = new char[strlen(yytext)-1];
                        strncpy(newText, &yytext[1], strlen(yytext)-2);

                        //yylval.string_t = strdup(newText);

                        return TK_STRING;
                      }
[0-9]+                {
                        //yylval.string_t = strdup(yytext);
                        return TK_NUMBER;
                      }
{letter}({letter}|[0-9])* { //yylval.string_t = strdup(yytext);
                        return TK_ID;
                      }
.                     { return TK_ERROR; }

<comments>=#          { BEGIN(INITIAL); }
<comments>.|\n        { /* NADA */ }
<comments><<EOLF>>    { fprintf(stderr, "Line %d: Unmatched block comment\n", yylineno); return 0; }

%%
