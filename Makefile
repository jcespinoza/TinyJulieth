TARGET=julieth
PARSER_SRC=grammar.cpp
LEXER_SRC=lexer.cpp
TESTS=Arithmetic Arrays Bitwise BubbleSort comments Factorial functions helloworld if printFormat QuickSort recursion Relational several while_break while
SRCFILES=$(PARSER_SRC) $(LEXER_SRC) ast.cpp statements.cpp expressions.cpp main.cpp
OBJ_FILES=${SRCFILES:.cpp=.o}
.PHONY: clean
.SUFFIXES:

$(TARGET): $(OBJ_FILES)
	g++ -g -o $@ $(OBJ_FILES)

$(LEXER_SRC): lexer.l
	flex -o $@ $^

$(PARSER_SRC): grammar.y ast.h expressions.h statements.h
	bison -rall -t --defines=tokens.h -o $@ $<

%.o:%.cpp
	g++ -std=c++11 -g -c -o $@ $<

run: $(TARGET)
	./$(TARGET) ./own_samples/hello.jl > ./own_samples/test.S
	nasm -felf -o ./own_samples/test.o ./own_samples/test.S
	gcc -m32 -o ./own_samples/test ./own_samples/test.o
	./own_samples/test

lexmain:  lexer.l
	flex -o lexer.cpp $^
	g++ -std=c++11 -g -c -o lexer.o lexer.cpp
	g++ -std=c++11 -g -c -o lexmain.o lexmain.cpp
	g++ -g -o lexmain lexmain.o lexer.o

main.S: $(TARGET)
	./$(TARGET) input1.txt > $@

test: $(TARGET)
	./$(TARGET) ./own_samples/empty_function.jl
	./$(TARGET) ./own_samples/for_loop.jl
	./$(TARGET) ./own_samples/array_declaration.jl
	./$(TARGET) ./own_samples/array_access.jl
	./$(TARGET) ./samples/Arithmetic.jl
	./$(TARGET) ./samples/Arrays.jl
	./$(TARGET) ./samples/Bitwise.jl
	./$(TARGET) ./samples/BubbleSort.jl
	./$(TARGET) ./samples/comments.jl
	./$(TARGET) ./samples/Factorial.jl
	./$(TARGET) ./samples/functions.jl
	./$(TARGET) ./samples/helloworld.jl
	./$(TARGET) ./samples/if.jl
	./$(TARGET) ./samples/printFormat.jl
	./$(TARGET) ./samples/QuickSort.jl
	./$(TARGET) ./samples/Relational.jl
	./$(TARGET) ./samples/several.jl
	./$(TARGET) ./samples/while_break.jl
	./$(TARGET) ./samples/while.jl
	./$(TARGET) ./own_samples/raim_test.jl

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
	rm -f lexmain
	rm -f ./own_samples/test.S ./own_samples/test.o ./own_samples/test

comparer:
	g++ -std=c++11 -g -c -o ./UnitTesting/compare.o ./UnitTesting/compare.cpp
	g++ -g -o ./UnitTesting/compare ./UnitTesting/compare.o

check: comparer  $(TARGET) $(TESTS)

$(TESTS):
	@./$(TARGET) UnitTesting/tests/$@.jl > run.asm
	@nasm -felf run.asm
	@gcc -m32 -o run run.o
	@./run > UnitTesting/generated/$@
	@./UnitTesting/compare UnitTesting/results/$@ UnitTesting/generated/$@
	@rm -f run.asm run.o run
	@rm -f UnitTesting/generated/$@
