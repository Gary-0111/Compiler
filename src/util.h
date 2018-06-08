#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include "Token.h"
#include <iomanip>

enum Operator {
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_assign,
    op_output,
    op_j,
    op_jz
};

struct Quadruple {
    Operator op;
    int arg1;
    int arg2;
    int result;
};

extern std::vector<Identifier> IDlist;
extern std::vector<variable> varList;
extern std::vector<Quadruple> quadList;

int newtemp(expType type);
void printIDList();
void printVarList();
void printQuadList();

void run();
void add(int arg1, int arg2, int res);
void sub(int arg1, int arg2, int res);
void mul(int arg1, int arg2, int res);
void div(int arg1, int arg2, int res);
void assign(int arg1, int res);
void output(int arg1);
void jump(int res, int &pc);
void jump_zero(int arg1, int res, int &pc);

#endif