#include "util.h"

vector<Identifier> IDlist;
vector<variable> varList;
vector<Quadruple> quadList;

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
			case op_mul: mul(quad.arg1, quad.arg2, quad.result); break;
			case op_div: div(quad.arg1, quad.arg2, quad.result); break;
			case op_assign: assign(quad.arg1, quad.result); break;
			case op_output: output(quad.arg1); break;
			case op_j:   jump(quad.result, pc); break;
			case op_jz:  jump_zero(quad.arg1, quad.result, pc); break;
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

void mul(int arg1, int arg2, int res) {
	variable &a = varList[arg1], &b = varList[arg2], &result = varList[res];
	if(result.type == exp_float) {
		result.evalue.decimal = a.tofloat() * b.tofloat();
	} else {
		result.evalue.integer = a.evalue.integer * b.evalue.integer;
	}
}

void div(int arg1, int arg2, int res) {
	variable &a = varList[arg1], &b = varList[arg2], &result = varList[res];
	if(result.type == exp_float) {
		result.evalue.decimal = a.tofloat() / b.tofloat();
	} else {
		result.evalue.integer = a.evalue.integer / b.evalue.integer;
	}
}

void assign(int arg1, int res) {
	variable &a = varList[arg1], &result = varList[res];
	if(result.type == exp_float) {
		result.evalue.decimal = a.tofloat();
	} else {
		result.evalue.integer = a.evalue.integer;
	}
}

void output(int arg1) {
	variable &a = varList[arg1];
	if(a.type == exp_float) {
		std::cout << a.evalue.decimal << '\n';
	} else {
		std::cout << a.evalue.integer << '\n';
	}
}

void jump(int res, int &pc) {
	pc = res - 1;
}

void jump_zero(int arg1, int res, int &pc) {
	variable &a = varList[arg1];
	if(a.evalue.integer == 0) {
		pc = res - 1;
	}
}