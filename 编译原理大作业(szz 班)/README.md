# 编译原理大作业

简单来说就是使用 flex 和 bison 完成词法分析和语法分析，绘制一颗抽象语法树
这个实验也是很有意思的，值得认真完成

实验给了一个 msvl 伪代码，要求绘制一颗这个伪代码的抽象语法树
然后还可以自己写一份伪代码，然后再画一个，我做了一个二分搜索算法的

## 📂 项目结构

项目包含两个子目录：

- **BinarySearch/**：实现了二分查找算法的伪代码抽象语法树绘制
- **new_msvl2dot/**：实验给出的伪代码的抽象语法树绘制

## 🚀 如何使用

### 编译项目

首先要安装环境，我是 fedora linux，装了 flex 2.6.4，bison 3.8.2，gcc 14.2.1
还要装一个 Graphviz，不过我实验报告里忘记写了，不知道是什么版本（

这里有从我的实验报告里拎出来的对每个文件的简要解释

- ast.c 是实现抽象语法树（AST）相关操作的源文件。
- ast.h 是其对应的头文件，定义了AST的结构和函数接口。
- ast.dot 是 input.msvl 经过 msvl2dot 处理后得到的可以通过Graphviz描述的AST图表文件。
- ast.png 是使用 graphviz 将 ast.dot 转换来的抽象语法树
- input.msvl输入的 msvl 代码，这里就是操作手册上的那段代码
- lexer.l 和 lexer.c
- lexer.l 是Lex工具生成词法分析器的源文件，定义了词法规则。
- lexer.c 是从 lexer.l 生成的C代码，负责将输入分解为词法单元（tokens）。
- main.c主程序，负责启动词法分析和语法分析
- msvl2dot最终生成的可执行文件，功能是将 .msvl 文件解析为 .dot 格式。
- parser.y 是Bison工具生成语法分析器的源文件，定义语法规则。
- parser.c 是从 parser.y 生成的C代码，负责将词法单元解析为抽象语法树。
- parser.h 是语法解析器的头文件，定义了接口。


```bash
# 进入BinarySearch目录
cd BinarySearch

# 编译项目
make
```

### 运行示例

```bash
# 处理示例输入文件，生成AST可视化
./msvl2dot < input.msvl

# 使用Graphviz将DOT文件转换为图像
dot -Tpng ast.dot -o ast.png
```