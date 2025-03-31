%{
#include "ast.h"
#include "parser.h"
%}

%%

"if"                { printf("TOKEN: IF\n"); return IF; }
"else"              { printf("TOKEN: ELSE\n"); return ELSE; }
"int"               { printf("TOKEN: INT\n"); return INT; }
"while"             { printf("TOKEN: WHILE\n"); return WHILE; }
"return"            { printf("TOKEN: RETURN\n"); return RETURN; }
"("                 { printf("TOKEN: LPAREN\n"); return LPAREN; }
")"                 { printf("TOKEN: RPAREN\n"); return RPAREN; }
"{"                 { printf("TOKEN: LBRACE\n"); return LBRACE; }
"}"                 { printf("TOKEN: RBRACE\n"); return RBRACE; }
"["                 { printf("TOKEN: LBRACKET\n"); return LBRACKET; }
"]"                 { printf("TOKEN: RBRACKET\n"); return RBRACKET; }
"="                 { printf("TOKEN: ASSIGN\n"); return ASSIGN; }
","                 { printf("TOKEN: COMMA\n"); return COMMA; }
"<"                 { printf("TOKEN: LT\n"); return LT; }
"+"                 { printf("TOKEN: PLUS\n"); return PLUS; }
"/"                 { printf("TOKEN: DIVIDE\n"); return DIVIDE; }
";"                 { printf("TOKEN: SEMICOLON\n"); return SEMICOLON; }
[0-9]+              { printf("TOKEN: NUMBER (%s)\n", yytext); yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { printf("TOKEN: IDENTIFIER (%s)\n", yytext); yylval.id = strdup(yytext); return IDENTIFIER; }
[ \t\r\n]+          { printf("TOKEN: WHITESPACE\n"); /* 忽略空白字符 */ }
.                   { printf("Unknown token: %s\n", yytext); return yytext[0]; }

%%
