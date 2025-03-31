%{
#include "ast.h"
#include "parser.h"
%}

%%

"int"               { return INT; }
"and"               { return AND; }
"skip"              { return SKIP; }
"if"                { return IF; }
"then"              { return THEN; }
"else"              { return ELSE; }
"<=="               { return IMMEDIATE_ASSIGN; }
":="                { return NEXT_ASSIGN; }
">"                 { return GT; }
[0-9]+              { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.id = strdup(yytext); return IDENTIFIER; }
"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
";"                 { return SEMICOLON; }
[ \t\r\n]+          { /* 忽略空白字符 */ }
.                   { return yytext[0]; }
%%
