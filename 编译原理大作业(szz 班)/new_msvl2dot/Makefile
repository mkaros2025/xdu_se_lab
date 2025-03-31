all:  msvl2dot

parser.c parser.h: parser.y ast.h
	bison -d -o parser.c parser.y

lexer.c: lexer.l parser.h ast.h
	flex -o lexer.c lexer.l

msvl2dot: parser.c lexer.c main.c ast.c
	gcc -o msvl2dot parser.c lexer.c main.c ast.c -lfl

clean:
	rm -f msvl2dot parser.c parser.h lexer.c ast.dot

# 将所有文件转换为 Unix 换行符
unix_format:
	find ./ -type f -exec dos2unix {} +
