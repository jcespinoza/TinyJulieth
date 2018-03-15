TARGET=julieth
EXPR_PARSER_SRC=expr_parser.cpp
EXPR_LEXER_SRC=expr_lexer.cpp
SRCFILES=$(EXPR_PARSER_SRC) $(EXPR_LEXER_SRC) ast.cpp main.cpp
OBJ_FILES=${SRCFILES:.cpp=.o}
.PHONY: clean
.SUFFIXES:

$(TARGET): $(OBJ_FILES)
	g++ -g -o $@ $(OBJ_FILES)

$(EXPR_LEXER_SRC): lexer.l
	flex -o $@ $^

$(EXPR_PARSER_SRC): parser.y
	bison -rall --defines=tokens.h -o $@ $<

%.o:%.cpp
	g++ -std=c++11 -g -c -o $@ $<

run: $(TARGET)
	./$(TARGET) input1.txt > main.S

main.S: $(TARGET)
	./$(TARGET) input1.txt > $@

gen.o: main.S
	nasm -felf -o $@ $<

compile: gen.o
	gcc -m32 -o genexe $<

exe: compile
	./genexe

clean:
	rm -f $(EXPR_PARSER_SRC) $(EXPR_LEXER_SRC)
	rm -f *.o
	rm -f tokens.h
	rm -f *.output
	rm -f $(TARGET)
	rm -f main.S
	rm -f genexe
