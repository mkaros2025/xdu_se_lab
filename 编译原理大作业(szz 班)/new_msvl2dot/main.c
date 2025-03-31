#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yyparse();
extern ASTNode *root;
extern int node_count;

int main() {
    node_count = 0;

    FILE *out = fopen("ast.dot", "w");
    if (!out) {
        perror("无法创建 ast.dot 文件");
        return 1;
    }

    // fprintf(out, "digraph AST {\n");
    if (yyparse() == 0) {
        printf("解析成功，生成 ast.dot 文件\n");
    } else {
        printf("解析失败！\n");
    }
    // fprintf(out, "}\n");

    fclose(out);
    return 0;
}
