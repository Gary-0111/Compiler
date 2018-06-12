#include "Lexer.h"

const std::map<std::string, SyntaxSymbol> Lexer::keyword = {
	{"main",   l_main   },
	{"int",    l_int    },
	{"float",  l_float  },
	{"if",     l_if     },
	{"else",   l_else   },
	{"for",    l_for    },
	{"while",  l_while  },
	{"print",  l_print  },
	{"return", l_return }
};

int atoi_oct(const char *str) {
	if(str == NULL) return 0;
	int ret = 0;
	for(int i = 1; str[i]; ++i) {
		ret = ret * 8 + (str[i] - '0');
	}
	return ret;
}

int atoi_hex(const char *str) {
	if(str == NULL) return 0;
	int ret = 0;
	for(int i = 2; str[i]; ++i) {
		if(isdigit(str[i])) ret = ret * 16 + (str[i] - '0');
		else ret = ret * 16 + (str[i] - 'a' + 10);
	}
	return ret;
}

Lexer::Lexer() {

}

Lexer::~Lexer() {
	if(this->stream) {
		fclose(this->stream);
	}
}

// TODO:
// 		recongnize float numbers
// 		recongnize negetive numbers
std::vector<Token>& Lexer::analyze(const char *file) {
	init(file);
	int ch = nextchar();
	state = 0;

	while(1) {
		if(ch == '\n') {
			++line;
			col = 0;
		}
		concat(ch);
		//printf("state = %d    ch = %c\n", state, ch);
		switch(state) 
		{
			
			case 0:
				if(isalpha(ch))      state = 1;
				else if(ch == '0')   state = 2;
				else if(isdigit(ch)) state = 3;
				else if(isspace(ch)) strLen--;
				else {
					switch(ch) 
					{
						case '+': addToken(Token(l_add,           getStr())); break;
						case '-': addToken(Token(l_sub,           getStr())); break;
						case '*': addToken(Token(l_mul,           getStr())); break;
						case '/': addToken(Token(l_div,           getStr())); break;
						case '%': addToken(Token(l_mod,  		  getStr())); break;
						case '(': addToken(Token(l_open_paren,    getStr())); break;
						case ')': addToken(Token(l_close_paren,   getStr())); break;
						case '[': addToken(Token(l_open_bracket,  getStr())); break;
						case ']': addToken(Token(l_close_bracket, getStr())); break;
						case '{': addToken(Token(l_open_brace,    getStr())); break;
						case '}': addToken(Token(l_close_brace,   getStr())); break;
						case ',': addToken(Token(l_comma,         getStr())); break;
						case ';': addToken(Token(l_semicolon,     getStr())); break;
						case '=': state = 4; break;
						case '>': state = 5; break;
						case '<': state = 6; break;
						case '!': state = 7; break;
						case EOF: return tokenList;
						default:  error();
					}
				}
				ch = nextchar();
				break;
			case 1: 
				if(!(isalpha(ch) || isdigit(ch))) {
					strLen--;
					const char *tmpstr = getStr();
					if(keyword.find(tmpstr) == keyword.end()) {
						//addIdentifier(tmpstr, ID_unknown);
						addToken(Token(l_id, tmpstr));
					} else {
						addToken(Token(keyword.find(tmpstr)->second, tmpstr));
					}
					break;
				}
				ch = nextchar();
				break;
			case 2:
				if(ch == 'x') {
					state = 21;
				} else if(isdigit(ch)) {
					if(ch < '8') state = 20;
					else error();
				} else if(ch == '.') {
					state = 10; 
				} else {
					addToken(Token(l_integer, atoi(getStr())));
					break;
				}
				ch = nextchar();
				break;
			case 20:
				if(!isdigit(ch)) {
					strLen--;
					addToken(Token(l_integer, atoi_oct(getStr())));
					break;
				} else if(ch >= '8') {
					error();
				}
				ch = nextchar();
				break;
			case 21:
				if(!isdigit(ch) && !isalpha(ch)) {
					strLen--;
					addToken(Token(l_integer, atoi_hex(getStr())));
					break;
				} else if((ch > 'f' && ch <= 'z') || (ch > 'F' && ch <= 'Z')) {
					error();
				}
				ch = nextchar();
				break;
			case 3: 
				if(!isdigit(ch)) {
					if(ch == '.') state = 10;
					else if(ch == 'e' || ch == 'E') {
						state = 12;
					}
					else {
						strLen--;
						addToken(Token(l_integer, atoi(getStr())));
						break;
					}
				}
				ch = nextchar();
				break;
			case 10:
				if(!isdigit(ch)) {
					error();
				}
				state = 11;
				ch = nextchar();
				break;
			case 11:
				if(!isdigit(ch)) {
					if(ch == 'e' || ch == 'E') {
						state = 12;
					} else {
						strLen--;
						addToken(Token(l_decimal, (float)atof(getStr())));
						break;
					}
				}
				ch = nextchar();
				break;
			case 12:
				if(ch == '-' || ch == '+'){ 
					state = 13;
				} else if(isdigit(ch) && ch != '0') {
					state = 14;
				} else {
					error();
				}
				ch = nextchar();
				break;
			case 13:
				if(isdigit(ch) && ch != '0') {
					state = 14;
				} else {
					error();
				}
				ch = nextchar();
				break;
			case 14:
				if(!isdigit(ch)) {
					strLen--;
					addToken(Token(l_decimal, (float)atof(getStr())));
					break;
				}
				ch = nextchar();
				break;
			case 4:
				if(ch != '=') {
					strLen--;
					addToken(Token(l_assign, getStr()));
					break;
				}
				addToken(Token(l_equal, getStr()));
				ch = nextchar();
				break;
			case 5:
				if(ch != '=') {
					strLen--;
					addToken(Token(l_greater, getStr()));
					break;
				}
				addToken(Token(l_greater_equal, getStr()));
				ch = nextchar();
				break;
			case 6:
				if(ch != '=') {
					strLen--;
					addToken(Token(l_less, getStr()));
					break;
				}
				addToken(Token(l_less_equal, getStr()));
				ch = nextchar();
				break;
			case 7:
				if(ch != '=') {
					error();
				}
				addToken(Token(l_not_equal, getStr()));
				ch = nextchar();
				break;
			default: break; 
		}
	}
	return tokenList;
}

void Lexer::printTokenList() {
	printf("Token List: \n");
	for(int i = 0; i < tokenList.size(); i++) {
		printf("  [%2d] ", i);
		if(tokenList[i].symbol == l_integer) {
			printf("(%d, %d)\n", tokenList[i].symbol, tokenList[i].value.var.evalue.integer);
		} else if(tokenList[i].symbol == l_decimal) {
			printf("(%d, %f)\n", tokenList[i].symbol, tokenList[i].value.var.evalue.decimal);
		} else {
			printf("(%d, \"%s\")\n", tokenList[i].symbol, tokenList[i].value.str);
		}
	}
}

void Lexer::init(const char *file) {
	strcpy(filename, file);
	this->stream = fopen(file, "r");
	if(this->stream == NULL) {
		printf("No such file: %s\n", file);
		exit(-1);
	}
	strLen = 0;
	line = 1;
	col = 0;
	fgets(linebuf, BUFFERSIZE, stream);
}

int Lexer::nextchar() {
	static int pos = 0;
	if(linebuf[pos] == '\0') {
		if(fgets(linebuf, BUFFERSIZE, stream) == NULL)
			return EOF;
		pos = 0;
	}
	++col;
	return linebuf[pos++];
}

void Lexer::concat(int ch) {
	str[strLen++] = ch;
}

void Lexer::addToken(Token t) {
	this->state = 0;
	tokenList.push_back(t);
	strLen = 0;
}

char* Lexer::getStr() {
	str[strLen] = '\0';
	return str;
}

void Lexer::error() {
	int buflen = strlen(linebuf);
	if(linebuf[buflen - 1] == '\n') {
		linebuf[buflen - 1] = '\0';
	}
	replace(linebuf, '\t', ' ');
	printf("\033[1;037m%s:%d:%d:\033[0m \033[1;031merror:\033[0m can not recognize charector \033[1;037m\'%c\'\033[0m \n %s\n", filename, line, col, linebuf[col-1], linebuf);
	printf(" \033[1;032m%*c\033[0m\n", col%(BUFFERSIZE-1), '^');
	exit(-1);
}

void replace(char *str, char src, char rep) {
	for(int i = 0; str[i]; i++) {
		if(str[i] == src) str[i] = rep;
	}
}
