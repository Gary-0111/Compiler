./bin/compiler : ./bin/main.o ./bin/Lexer.o ./bin/Parser.o ./bin/util.o LR1_Table
	g++ -o ./bin/compiler ./bin/main.o ./bin/Lexer.o ./bin/Parser.o ./bin/util.o

./bin/main.o : ./src/main.cpp
	g++ -o ./bin/main.o -std=c++11 -c ./src/main.cpp

./bin/Lexer.o : ./src/Lexer.cpp
	g++ -o ./bin/Lexer.o -std=c++11 -c ./src/Lexer.cpp

./bin/Parser.o : ./src/Parser.cpp ./src/Syntax.h
	g++ -o ./bin/Parser.o -std=c++11 -c ./src/Parser.cpp

./bin/util.o : ./src/util.cpp
	g++ -o ./bin/util.o -std=c++11 -c ./src/util.cpp

LR1_Table : ./bin/getTable
	./bin/getTable

./bin/getTable : ./bin/getTable.o ./bin/util.o
	g++ -o ./bin/getTable ./bin/getTable.o ./bin/util.o

./bin/getTable.o : ./src/getTable.cpp ./src/Syntax.h
	g++ -o ./bin/getTable.o -std=c++11 -c ./src/getTable.cpp

clean : 
	rm ./bin/compiler ./bin/*.o ./bin/getTable