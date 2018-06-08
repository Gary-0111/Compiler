#include "Lexer.h"
#include "Parser.h"
#include <sys/time.h>

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage:\n\t%s <inputfile>\n", argv[0]);
		return -1;
	}

	Lexer lexer;
	std::vector<Token>& tokenList = lexer.analyze(argv[1]);
	lexer.printTokenList();

	struct timeval start, end;
	gettimeofday(&start, NULL);
	double ts = (double)start.tv_sec*1000 + start.tv_usec/1000;

	Parser parser;

	gettimeofday(&end, NULL);
	double te = (double)end.tv_sec*1000 + end.tv_usec/1000;	

	std::cout << "Initial the parser cost: " << (te-ts) << "ms\n";

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


