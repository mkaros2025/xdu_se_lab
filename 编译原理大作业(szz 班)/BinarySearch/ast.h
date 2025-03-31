#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 最大子节点数 (可根据需要调整)
#define MAX_CHILDREN 10

static int global_id = 0;

// AST 节点结构
typedef struct ASTNode {
    int id;                    // 节点唯一标识
    char *type;                // 节点类型
    char *value;               // 节点值（可选）
    struct ASTNode *children[MAX_CHILDREN]; // 子节点数组
    int child_count;           // 子节点数量
} ASTNode;

// 创建新节点
ASTNode *new_node(const char *type, const char *value);

// 添加子节点
void add_child(ASTNode *parent, ASTNode *child);

// 释放 AST
void free_ast(ASTNode *node);

// 生成 Graphviz DOT 文件
void generate_dot(ASTNode *node, FILE *out);

#endif // AST_H
