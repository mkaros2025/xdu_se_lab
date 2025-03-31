%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// 函数声明
void yyerror(const char *s);
int yylex();

extern int node_count;

%}

%union {
    int num;
    char *id;
    ASTNode *node;
}

/* 关联到符号 */
/* 通过 %token 或 %type 将某个终结符或非终结符的语义值类型指定为 %union 中的某个字段： */
%token <id> IDENTIFIER
%token <num> NUMBER
%token INT AND SKIP IF THEN ELSE
%token IMMEDIATE_ASSIGN NEXT_ASSIGN GT
%token LBRACE RBRACE SEMICOLON

%type <node> program statement_list statement condition block

%%

// program 是语法的开始符号，代表整个程序
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

// statement_list 可以是一个 statement 或多个 statement 以分号分隔
statement_list:
    // $1 是 statement 的值，$$ 是 statement_list 的值
    statement { $$ = $1; }
    | statement_list SEMICOLON statement { $$ = new_node("STATEMENT_LIST", $1, $3); }
;

statement:
    // 声明语句
    // int x and skip;
    INT IDENTIFIER AND SKIP {
        $$ = new_node("DECLARE", new_leaf_node("IDENTIFIER", $2), NULL);
    }
    // 立即赋值语句
    // x <== 1 and skip;
    | IDENTIFIER IMMEDIATE_ASSIGN NUMBER AND SKIP {
        char num_str[20];
        sprintf(num_str, "%d", $3);  // 转换数字为字符串
        $$ = new_node("IMMEDIATE_ASSIGN", new_leaf_node("IDENTIFIER", $1), new_leaf_node("NUM", num_str));
    }
    // 条件语句
    // if ( condition ) then block
    | IF '(' condition ')' THEN block ELSE block {
        $$ = new_node("IF", $3, new_node("THEN_ELSE", $6, $8));
    }
    // 下一步赋值语句
    // x := 2 等等
    | IDENTIFIER NEXT_ASSIGN NUMBER {
        char num_str[20];
        sprintf(num_str, "%d", $3);  // 转换数字为字符串
        $$ = new_node("NEXT_ASSIGN", new_leaf_node("IDENTIFIER", $1), new_leaf_node("NUM", num_str));
    }
;

condition:
    // x > 1
    IDENTIFIER GT NUMBER {
        char num_str[20];
        sprintf(num_str, "%d", $3);  // 转换数字为字符串
        $$ = new_node("CONDITION: >", new_leaf_node("IDENTIFIER", $1), new_leaf_node("NUM", num_str));
    }
;

block:
    // { statement_list }
    LBRACE statement_list RBRACE { $$ = $2; }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
