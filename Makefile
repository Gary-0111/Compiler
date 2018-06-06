./bin/compiler : ./bin/main.o ./bin/Lexer.o ./bin/Parser.o ./bin/util.o
	g++ -o ./bin/compiler ./bin/main.o ./bin/Lexer.o ./bin/Parser.o ./bin/util.o

./bin/main.o : ./src/main.cpp
	g++ -o ./bin/main.o -std=c++11 -c ./src/main.cpp

./bin/Lexer.o : ./src/Lexer.cpp ./src/Lexer.h ./src/Token.h
	g++ -o ./bin/Lexer.o -std=c++11 -c ./src/Lexer.cpp

./bin/Parser.o : ./src/Parser.cpp ./src/Parser.h ./src/Token.h
	g++ -o ./bin/Parser.o -std=c++11 -c ./src/Parser.cpp

./bin/util.o : ./src/util.cpp ./src/util.h
	g++ -o ./bin/util.o -std=c++11 -c ./src/util.cpp

clean : 
	rm compiler *.o