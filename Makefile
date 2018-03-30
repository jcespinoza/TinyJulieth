TARGET=julieth
PARSER_SRC=grammar.cpp
LEXER_SRC=lexer.cpp
SRCFILES=$(PARSER_SRC) $(LEXER_SRC) ast.cpp main.cpp
OBJ_FILES=${SRCFILES:.cpp=.o}
.PHONY: clean
.SUFFIXES:

$(TARGET): $(OBJ_FILES)
	g++ -g -o $@ $(OBJ_FILES)

$(LEXER_SRC): lexer.l
	flex -o $@ $^

$(PARSER_SRC): grammar.y
	bison -rall --defines=tokens.h -o $@ $<

%.o:%.cpp
	g++ -std=c++11 -g -c -o $@ $<

run: $(TARGET)
	./$(TARGET) input1.txt > main.S

lexmain:  lexer.l
	flex -o lexer.cpp $^
	g++ -std=c++11 -g -c -o lexer.o lexer.cpp
	g++ -std=c++11 -g -c -o lexmain.o lexmain.cpp
	g++ -g -o lexmain lexmain.o lexer.o

main.S: $(TARGET)
	./$(TARGET) input1.txt > $@

gen.o: main.S
	nasm -felf -o $@ $<

compile: gen.o
	gcc -m32 -o genexe $<

exe: compile
	./genexe

clean:
	rm -f $(PARSER_SRC) $(LEXER_SRC)
	rm -f *.o
	rm -f tokens.h
	rm -f *.output
	rm -f $(TARGET)
	rm -f main.S
	rm -f genexe
