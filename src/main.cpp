#include "Lexer.h"
#include "Parser.h"


int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage:\n\t%s <inputfile>\n", argv[0]);
		return -1;
	}

	Lexer lexer;
	std::vector<Token>& tokenList = lexer.analyze(argv[1]);
	lexer.printTokenList();

	Parser parser;

/*
	parser.printFirstSet();
	parser.printFollowSet();
	parser.printTable();
	//parser.printItemSet();
*/
	
	if(parser.analyze(tokenList)) {
		cout << "Perfect!!\n";

		printVarList();
		printIDList();
		printQuadList();

		run();

		printVarList();
		printIDList();
		printQuadList();

	} else {
		cout << "QAQ!!\n";
	}

	return 0;
}


