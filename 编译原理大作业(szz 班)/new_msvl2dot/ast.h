#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int node_count = 0;

typedef struct ASTNode {
    int id;                  // 节点唯一标识符
    char *type;              // 节点类型（如 "NUM", "CONDITION" 等）
    char *value;             // 节点的值（如具体数字或运算符）
    struct ASTNode *left;    // 左子节点
    struct ASTNode *right;   // 右子节点
} ASTNode;

// 创建新节点
ASTNode* new_node(const char *type, ASTNode *left, ASTNode *right);

// 创建叶子节点
ASTNode* new_leaf_node(const char *type, const char *value);

void free_ast(ASTNode *node);
void generate_dot(ASTNode *node, FILE *out);

#endif // AST_H
