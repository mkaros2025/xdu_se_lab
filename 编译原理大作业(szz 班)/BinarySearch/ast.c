#include "ast.h"

// 创建新节点
ASTNode *new_node(const char *type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate memory for ASTNode");
        exit(1);
    }
    node->id = global_id++;
    node->type = strdup(type);
    node->value = value ? strdup(value) : NULL;
    node->child_count = 0;
    return node;
}

// 添加子节点
void add_child(ASTNode *parent, ASTNode *child) {
    if (parent->child_count >= MAX_CHILDREN) {
        fprintf(stderr, "Error: Node %d exceeds maximum children limit\n", parent->id);
        exit(1);
    }
    parent->children[parent->child_count++] = child;
}

// 释放 AST
void free_ast(ASTNode *node) {
    if (!node) return;

    // 释放子节点
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }

    // 释放当前节点
    free(node->type);
    if (node->value) free(node->value);
    free(node);
}

// 生成 Graphviz DOT 文件
void generate_dot(ASTNode *node, FILE *out) {
    if (!node) return;

    // 输出当前节点
    fprintf(out, "  node%d [label=\"%s", node->id, node->type);
    if (node->value) {
        fprintf(out, ": %s", node->value);
    }
    fprintf(out, "\"];\n");

    // 遍历所有子节点
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]) {
            generate_dot(node->children[i], out);
            fprintf(out, "  node%d -> node%d;\n", node->id, node->children[i]->id);
        }
    }
}
