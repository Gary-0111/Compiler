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


#endif