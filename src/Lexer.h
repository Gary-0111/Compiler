#ifndef LEXER_H
#define LEXER_H

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <map>
#include <string>
#include "Token.h"

class Lexer {
	public:
		Lexer();
		~Lexer();
		std::vector<Token>& analyze(const char *file);
		static const std::map<std::string, SyntaxSymbol> keyword;
        
		// ----------- Debug ------------
		void printTokenList();
		void printIDList();
        // --------- Debug end ----------

	private:
		void init(const char *file);
		int nextchar();
		void concat(int ch);
		void addToken(Token t);
		char *getStr();

		void error();

		static const int BUFFERSIZE = 64;
		int state;
		char str[32];
		int strLen;
		int line, col;
		char filename[128];
		char linebuf[BUFFERSIZE];
		FILE *stream;
		std::vector<Token> tokenList;
};

// utilities
void replace(char *str, char src, char rep);

#endif