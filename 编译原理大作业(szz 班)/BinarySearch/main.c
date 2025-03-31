#include <stdio.h>
#include "ast.h"
#include "parser.h"

extern int yyparse();

int main() {
    printf("开始解析...\n");
    if (yyparse() == 0) {
        printf("解析完成！\n");
    } else {
        printf("解析失败！\n");
    }
    return 0;
}
