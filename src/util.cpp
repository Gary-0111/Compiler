#include "util.h"

void printIDList() {
	std::cout << "################ Identifier List ##################\n";
	int i = 0;
	for(const auto &id : IDlist) {
		printf("  [%2d] ", i++);
		std::cout << "[" << id.name << ", " << id.place << "]\n";
	}
}

void printVarList() {
	std::cout << "################ Variable List ###################\n";
	int i = 0;
	for(const auto &var : varList) {
		printf("  [%2d] ", i++);
		if(var.type == exp_int) std::cout << "[int, " << var.evalue.integer << "]\n";
        else if(var.type == exp_float) std::cout << "[float, " << std::setiosflags(std::ios::fixed) << var.evalue.decimal << "]\n";
		else std::cout << "[unknown, null]\n";
	}
}

void printQuadList() {
	std::cout << "################ Quadruple List ###################\n";
	int i = 0;
	for(const auto &quad : quadList) {
		printf("  [%2d] ", i++);
		std::cout << "(" << quad.op << ", " << quad.arg1 << ", " << quad.arg2 << ", " << quad.result << ")\n";
	}
}

int newtemp(expType type) {
	varList.push_back(variable());
	varList.back().type = type;
	return varList.size()-1;
}

void run() {
	int pc = 0;
	std::cout << "********* Start ***********\n";
	while(pc >= 0 && pc < quadList.size()) {
		const Quadruple &quad = quadList[pc];
		switch(quad.op) 
		{
			case op_add: add(quad.arg1, quad.arg2, quad.result); break;
			case op_sub: sub(quad.arg1, quad.arg2, quad.result); break;
			case op_mul: break;
			case op_j:   break;
			case op_jz:  break;
			default:     break;
		}
		++pc;
	}
	std::cout << "********** End ************\n";
}

void add(int arg1, int arg2, int res) {
	variable &a = varList[arg1], &b = varList[arg2], &result = varList[res];
	if(result.type == exp_float) {
		result.evalue.decimal = a.tofloat() + b.tofloat();
	} else {
		result.evalue.integer = a.evalue.integer + b.evalue.integer;
	}
}

void sub(int arg1, int arg2, int res) {
	if(arg1 == -1) {
		variable &b = varList[arg2], &result = varList[res];
		if(result.type == exp_float) {
			result.evalue.decimal = -b.tofloat();
		} else {
			result.evalue.integer = -b.evalue.integer;
		}
	} else {
		variable &a = varList[arg1], &b = varList[arg2], &result = varList[res];
		if(result.type == exp_float) {
			result.evalue.decimal = a.tofloat() - b.tofloat();
		} else {
			result.evalue.integer = a.evalue.integer - b.evalue.integer;
		}
	}
}
