%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

void yyerror(const char *s);
int yylex();
%}

%union {
    int num;
    char *id;
    ASTNode *node;
}

/* Token declarations */
%token <id> IDENTIFIER
%token <num> NUMBER
%token INT WHILE RETURN IF ELSE
%token ASSIGN LT PLUS DIVIDE COMMA
%token LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN SEMICOLON

/* Operator precedence and associativity */
%left PLUS DIVIDE
%nonassoc LT
%nonassoc IFX
%nonassoc ELSE

/* Specify types for non-terminals */
%type <node> program statement_list statement condition block expression array_initializer expression_list

%debug

%%

/* Grammar rules */
program:
    statement_list {
        printf("解析完成，生成 AST DOT 文件...\n");
        FILE *out = fopen("ast.dot", "w");
        if (!out) {
            perror("无法创建 ast.dot 文件");
            exit(1);
        }
        fprintf(out, "digraph AST {\n");
        generate_dot($1, out);
        fprintf(out, "}\n");
        fclose(out);
        free_ast($1);
    }
;

statement_list:
    statement {
        $$ = $1;
    }
    | statement_list SEMICOLON statement {
        $$ = new_node("STATEMENT_LIST", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    ;

statement:
    INT IDENTIFIER LBRACKET RBRACKET ASSIGN array_initializer {
        $$ = new_node("ARRAY_DECLARE", $2);
        add_child($$, $6);
    }
    | INT IDENTIFIER ASSIGN expression {
        $$ = new_node("DECLARE_ASSIGN", $2);
        add_child($$, $4);
    }
    | IDENTIFIER ASSIGN expression {
        $$ = new_node("ASSIGN", $1);
        add_child($$, $3);
    }
    | RETURN expression {
        $$ = new_node("RETURN", NULL);
        add_child($$, $2);
    }
    | WHILE LPAREN condition RPAREN block {
        $$ = new_node("WHILE", NULL);
        add_child($$, $3);
        add_child($$, $5);
    }
    | IF LPAREN condition RPAREN block ELSE block {
        $$ = new_node("IF_ELSE", NULL);
        add_child($$, $3);
        add_child($$, $5);
        add_child($$, $7);
    }
    | IF LPAREN condition RPAREN block %prec IFX {
        $$ = new_node("IF", NULL);
        add_child($$, $3);
        add_child($$, $5);
    }
    ;

condition:
    expression LT expression {
        $$ = new_node("CONDITION: <", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    ;

block:
    LBRACE statement_list RBRACE {
        $$ = $2;
    }
    | LBRACE RBRACE {
        $$ = new_node("EMPTY_BLOCK", NULL);
    }
    ;

expression:
    NUMBER {
        char num_str[20];
        sprintf(num_str, "%d", $1);
        $$ = new_node("NUMBER", num_str);
    }
    | IDENTIFIER {
        $$ = new_node("IDENTIFIER", $1);
    }
    | IDENTIFIER LBRACKET expression RBRACKET {
        $$ = new_node("ARRAY_ACCESS", $1);
        add_child($$, $3);
    }
    | expression PLUS expression {
        $$ = new_node("PLUS", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    | expression DIVIDE expression {
        $$ = new_node("DIVIDE", NULL);
        add_child($$, $1);
        add_child($$, $3);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

array_initializer:
    LBRACE expression_list RBRACE {
        $$ = $2;
    }
    ;

expression_list:
    expression {
        $$ = new_node("EXPRESSION_LIST", NULL);
        add_child($$, $1);
    }
    | expression_list COMMA expression {
        add_child($1, $3);
        $$ = $1;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
