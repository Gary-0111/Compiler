#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <string.h>

using namespace std;

enum SyntaxSymbol {
    l_epsilon,
    // key word
    l_main,
    l_int,
    l_float,
    l_if,
    l_else,
    l_for,
    l_while,
    l_print,
    l_return,
    // identifier
    l_id,
    // constant number
    l_integer,
    l_decimal,
    // operator
    l_assign,           // =
    l_add,              // +
    l_sub,              // -
    l_mul,              // *
    l_div,              // /
    l_mod,              // %
    l_equal,            // ==
    l_not_equal,        // !=
    l_less,             // <
    l_less_equal,       // <=
    l_greater,          // >
    l_greater_equal,    // >=
    l_open_paren,       // (
    l_close_paren,      // )
    l_open_bracket,     // [
    l_close_bracket,    // ]
    l_open_brace,       // {
    l_close_brace,      // }
    l_comma,            // ,
    l_semicolon,        // ;
    l_eof,              // #

    terminate_end,

    s_auxiliary,        // auxiliary start symbol
    s_lines,
    s_line,
    s_const,
    s_exp,
    s_times,
    s_factor,
    s_declare,
    s_assignment,
    s_output,
    s_idtype,
    s_idList,
    s_ifstatement,
    s_ifcondition,
    s_elsecondition,
    s_whilestatement,
    s_whilecondition,
    s_while,
    s_rop,
    s_block,

    s_dot               // 
};

enum expType {
	exp_unknown,
	exp_int,
	exp_float,
	exp_func
};

enum Operator {
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_mod,
    op_assign,
    op_output,
    op_j,
    op_jz,
    op_eq,
    op_ne,
    op_g,
    op_ge,
    op_l,
    op_le
};

union expValue {
    /*
    bool operator == (const expValue &a) const {
        return integer == a.integer;
    }
    bool operator != (const expValue &a) const {
        return integer != a.integer;
    }
    */
    int integer;
    float decimal;
};

struct variable {
    void itof() {
        if(type == exp_int) {
            evalue.decimal = (float)evalue.integer;
        }
        type = exp_float;
    }
    float tofloat() {
        if(type == exp_int) return (float)evalue.integer;
        return evalue.decimal;
    }
    expValue evalue;
    expType type;
};

struct Identifier {
	Identifier(const char* _name, int _place = -1) {
		strcpy(name, _name);
		place = _place;
	}
    bool operator == (const Identifier &A) const {
        return (strcmp(name, A.name) == 0);
    }

	char name[32];
    int place;
};

struct TokenValue {
    char str[32];
	variable var;
};

struct Token {
	Token(SyntaxSymbol _symbol, const char *_str):symbol(_symbol) {
		strcpy(value.str, _str);
	}
	Token(SyntaxSymbol _symbol, int _integer):symbol(_symbol) {
		value.var.evalue.integer = _integer;
        value.var.type = exp_int;
	}
    Token(SyntaxSymbol _symbol, float _decimal):symbol(_symbol) {
        value.var.evalue.decimal = _decimal;
        value.var.type = exp_float;
    }

	SyntaxSymbol symbol;
	TokenValue value;
};

struct Tuple {
    Tuple() {}
    Tuple(SyntaxSymbol _symbol, int _state, int _place=-1):symbol(_symbol), state(_state), place(_place) {

    }
    Tuple(SyntaxSymbol _symbol, int _state, const char *_name):symbol(_symbol), state(_state) {
        place = -1;
        strcpy(name, _name);
    }

    SyntaxSymbol symbol;
    expType type;
    Operator op;
    char name[32];
    int state;
    int place;
    int addr;
    int quad;  
};

struct Quadruple {
    Operator op;
    int arg1;
    int arg2;
    int result;
};



int newtemp(expType type);
void printIDList();
void printVarList();
void printQuadList();

void run();
void add(int arg1, int arg2, int res);
void sub(int arg1, int arg2, int res);
void mul(int arg1, int arg2, int res);
void div(int arg1, int arg2, int res);
void mod(int arg1, int arg2, int res);

void assign(int arg1, int res);
void output(int arg1);
void jump(int res, int &pc);
void jump_z(int arg1, int res, int &pc);

void eq(int arg1, int arg2, int res);
void neq(int arg1, int arg2, int res);
void gt(int arg1, int arg2, int res);
void gteq(int arg1, int arg2, int res);
void ls(int arg1, int arg2, int res);
void lseq(int arg1, int arg2, int res);

#endif