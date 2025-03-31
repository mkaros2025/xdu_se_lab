#include "ast.h"

// 创建新 AST 节点
ASTNode* new_node(const char *type, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->id = node_count++;
    node->type = strdup(type);
    node->value = NULL;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* new_leaf_node(const char *type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->id = node_count++;
    node->type = strdup(type);
    node->value = strdup(value);
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 释放 AST
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node->type);
    if (node->value) free(node->value);
    free(node);
}

// 生成 DOT 格式输出
// dot -Tpng ast.dot -o ast.png
void generate_dot(ASTNode *node, FILE *out) {
    if (!node) return;

    int current_id = node->id;

    // 输出当前节点的信息
    if (node->value) {
        // 如果节点有具体值（如数字或标识符）
        fprintf(out, "  node%d [label=\"%s: %s\"];\n", current_id, node->type, node->value);
    } else {
        // 如果节点没有具体值（如运算符或语句类型）
        fprintf(out, "  node%d [label=\"%s\"];\n", current_id, node->type);
    }

    // 递归处理左子节点
    if (node->left) {
        int left_id = node->left->id;  // 左子节点的 ID
        generate_dot(node->left, out);  // 递归生成左子节点
        fprintf(out, "  node%d -> node%d;\n", current_id, left_id);  // 输出边
    }

    // 递归处理右子节点
    if (node->right) {
        int right_id = node->right->id;  // 右子节点的 ID
        generate_dot(node->right, out);  // 递归生成右子节点
        fprintf(out, "  node%d -> node%d;\n", current_id, right_id);  // 输出边
    }
}

