#ifndef SYNTAX_H
#define SYNTAX_H

#include "util.h"
#include <vector>
#include <algorithm>

using namespace std;

#define OPERATION [](const vector<Tuple> &stk)->Tuple 

const string LR1_file = "LR1_Table";
extern vector<Identifier> IDlist;
extern vector<variable> varList;
extern vector<Quadruple> quadList;

enum Act_type {
    Act_error,
    Act_accept,
    Act_shift,
    Act_reduce,
};

struct ActionItem {
    ActionItem(Act_type _type = Act_error, int _num = -1): type(_type), num(_num) {}
    Act_type type;
    int num;
};

struct SyntaxItem {
    SyntaxItem() {}
    SyntaxItem(SyntaxSymbol _symbol, const vector<SyntaxSymbol>& vec) {
        rule.first = _symbol;
        rule.second.assign(vec.begin(), vec.end());
        operation = NULL;
    }
    SyntaxItem(SyntaxSymbol _symbol, const vector<SyntaxSymbol>& vec, Tuple (*p)(const vector<Tuple> &stk)) {
        rule.first = _symbol;
        rule.second.assign(vec.begin(), vec.end());
        operation = p;
    }

    bool operator < (const SyntaxItem &A) const {
        if(rule.first == A.rule.first) {
            for(int i = 0; i < rule.second.size() && i < A.rule.second.size(); ++i) {
                if(rule.second[i] == A.rule.second[i]) continue;
                return rule.second[i] < A.rule.second[i];
            }
            return rule.second.size() < A.rule.second.size();
        }
        return rule.first < A.rule.first;
    }

    bool operator == (const SyntaxItem &A) const {
        if(rule.first == A.rule.first) {
            if(rule.second.size() == A.rule.second.size()) {
                for(int i = 0; i < rule.second.size(); ++i) {
                    if(rule.second[i] != A.rule.second[i])
                        return false;
                }
                return true;
            } else {
                return false;
            }
        } else return false;
    }

    pair<SyntaxSymbol, vector<SyntaxSymbol> > rule;
    Tuple (*operation)(const vector<Tuple> &stk);
};

// 语法规则及对应的语义子程序
const vector<SyntaxItem> Syntax = {
    SyntaxItem(s_auxiliary, {s_lines}),             //

    SyntaxItem(s_lines, {s_line, s_lines}),         // lines -> line lines
    SyntaxItem(s_lines, {s_line}),                  // lines -> line
    SyntaxItem(s_line, {s_exp, l_semicolon}),       // line  -> expression;
    SyntaxItem(s_line, {s_declare, l_semicolon}),   // line  -> declare;
    SyntaxItem(s_line, {s_assignment, l_semicolon}),
    SyntaxItem(s_line, {s_output, l_semicolon}),
    SyntaxItem(s_line, {s_ifstatement}),
    SyntaxItem(s_line, {s_whilestatement}),
    
    // expression
    SyntaxItem(s_exp, {s_exp, l_add, s_times},      // expression -> epsression + times
        OPERATION {
            int top = stk.size() - 1;
            Tuple E;
            const Tuple &E1 = stk[top-2], &T = stk[top];
            if(E1.type == exp_float || T.type == exp_float) E.type = exp_float;
            else E.type = exp_int;
            E.place = newtemp(E.type);
            quadList.push_back({op_add, E1.place, T.place, E.place}); // emit
            return E;
        }
    ),
    
    SyntaxItem(s_exp, {s_exp, l_sub, s_times},      // expression -> epsression + times
        OPERATION {
            int top = stk.size() - 1;
            Tuple E;
            const Tuple &E1 = stk[top-2], &T = stk[top];
            if(E1.type == exp_float || T.type == exp_float) E.type = exp_float;
            else E.type = exp_int;
            E.place = newtemp(E.type);
            quadList.push_back({op_sub, E1.place, T.place, E.place}); // emit
            return E;
        }
    ),
    
    SyntaxItem(s_exp, {s_times},                    // expression -> times
        OPERATION {
            Tuple E;
            E.place = stk.back().place;
            E.type = stk.back().type;
            return E;
        }
    ),
    SyntaxItem(s_times, {s_times, l_mul, s_factor}, // times -> times * factor
        OPERATION {
            int top = stk.size() - 1;
            Tuple T;
            const Tuple &T1 = stk[top-2], &F = stk[top];
            if(T1.type == exp_float || F.type == exp_float) T.type = exp_float;
            else T.type = exp_int;
            T.place = newtemp(T.type);
            quadList.push_back({op_mul, T1.place, F.place, T.place}); // emit
            return T;
        }
    ),
    SyntaxItem(s_times, {s_times, l_div, s_factor}, // times -> times * factor
        OPERATION {
            int top = stk.size() - 1;
            Tuple T;
            const Tuple &T1 = stk[top-2], &F = stk[top];
            if(T1.type == exp_float || F.type == exp_float) T.type = exp_float;
            else T.type = exp_int;
            T.place = newtemp(T.type);
            quadList.push_back({op_div, T1.place, F.place, T.place}); // emit
            return T;
        }
    ),
    SyntaxItem(s_times, {s_factor},                 // times -> factor
        OPERATION {
            Tuple T;
            T.place = stk.back().place;
            T.type = stk.back().type;
            return T;
        }
    ),
    SyntaxItem(s_factor, {l_open_paren, s_exp, l_close_paren},  // factor -> ( expression )
        OPERATION {
            int top = stk.size() - 1;
            Tuple F;
            F.place = stk[top-1].place;
            F.type = stk[top-1].type;
            return F;
        }    
    ),
    SyntaxItem(s_factor, {l_id},                    // factor -> identifier
        OPERATION {
            Tuple F;
            int index = find(IDlist.begin(), IDlist.end(), Identifier(stk.back().name)) - IDlist.begin();
            if(index == IDlist.size()) {
                // error
                cout << "error!!!\n";
                exit(-1);
            }
            //cout << index << "   hahahahhahaha\n";
            F.place = IDlist[index].place;
            F.type = varList[IDlist[index].place].type;
            return F;
        }
    ),
    SyntaxItem(s_factor, {s_const},                 // factor -> constant
        OPERATION {
            Tuple F;
            F.place = stk.back().place;
            F.type = stk.back().type;
            return F;
        }
    ),
    SyntaxItem(s_const, {l_sub, l_integer},         // constant -> - integer
        OPERATION {
            Tuple C;
            C.type = exp_int;
            C.place = newtemp(C.type);
            quadList.push_back({op_sub, -1, stk.back().place, C.place});
            return C;
        }
    ),
    SyntaxItem(s_const, {l_integer},                // constant -> integer
        OPERATION {
            Tuple C;
            C.place = stk.back().place;
            C.type = exp_int;
            return C;
        }
    ),
    SyntaxItem(s_const, {l_sub, l_decimal},         // constant -> - decimal
        OPERATION {
            Tuple C;
            C.type = exp_float;
            C.place = newtemp(C.type);
            quadList.push_back({op_sub, -1, stk.back().place, C.place});
            return C;
        }
    ),
    SyntaxItem(s_const, {l_decimal},                // constant -> decimal
        OPERATION {
            Tuple C;
            C.place = stk.back().place;
            C.type = exp_float;
            return C;
        }
    ),

    // declare
    SyntaxItem(s_declare, {s_idtype, l_id},         // declare -> var_type id
        OPERATION {
            int top = stk.size() - 1;
            int index = newtemp(stk[top-1].type);
            // TODO:
            // check the identifier if only
            IDlist.push_back(Identifier(stk[top].name, index));
            Tuple D;
            D.type = stk[top-1].type;
            return D;
        }
    ),
    SyntaxItem(s_declare, {s_idtype, l_id, l_assign, s_exp}, // declare -> var_type id = expression
        OPERATION {
            int top = stk.size() - 1;
            if(stk[top-3].type == exp_float) {
                varList[stk[top].place].itof();
            } else {
                varList[stk[top].place].type = exp_int;
            }
            // TODO:
            // check the identifier if only
            IDlist.push_back(Identifier(stk[top-2].name, stk[top].place));
            Tuple D;
            D.type = stk[top-3].type;
            return D;
        }
    ),
    SyntaxItem(s_declare, {s_declare, l_comma, l_id},         // declare -> var_type id
        OPERATION {
            int top = stk.size() - 1;
            int index = newtemp(stk[top-2].type);
            // TODO:
            // check the identifier if only
            IDlist.push_back(Identifier(stk[top].name, index));
            Tuple D;
            D.type = stk[top-2].type;
            return D;
        }
    ),
    SyntaxItem(s_declare, {s_declare, l_comma, l_id, l_assign, s_exp}, // declare -> var_type id = expression
        OPERATION {
            int top = stk.size() - 1;
            if(stk[top-4].type == exp_float) {
                varList[stk[top].place].itof();
            } else {
                varList[stk[top].place].type = exp_int;
            }
            // TODO:
            // check the identifier if only
            IDlist.push_back(Identifier(stk[top-2].name, stk[top].place));
            Tuple D;
            D.type = stk[top-4].type;
            return D;
        }
    ),
    SyntaxItem(s_idtype, {l_int},                   // var_type -> int
        OPERATION {
            Tuple T;
            T.type = exp_int;
            return T;
        }
    ),
    SyntaxItem(s_idtype, {l_float},                 // var_type -> float
        OPERATION {
            Tuple T;
            T.type = exp_float;
            return T;
        }
    ),

    SyntaxItem(s_assignment, {l_id, l_assign, s_exp}, 
        OPERATION {
            int top = stk.size() - 1;
            int index = find(IDlist.begin(), IDlist.end(), Identifier(stk[top-2].name)) - IDlist.begin();
            if(index == IDlist.size()) {
                // error
                cout << "error!!!\n";
                exit(-1);
            }
            quadList.push_back({op_assign, stk[top].place, -1, IDlist[index].place});
            return Tuple();
        }
    ),

    SyntaxItem(s_output, {l_print, l_open_paren, s_exp, l_close_paren},
        OPERATION {
            int top = stk.size() - 1;
            quadList.push_back({op_output, stk[top-1].place, -1, -1});
            return Tuple();
        }
    ),

    // if statement
    SyntaxItem(s_ifstatement, {s_ifcondition, s_block}, 
        OPERATION {
            int top = stk.size() - 1;
            const Tuple &C = stk[top-1];
            quadList[C.addr].result = quadList.size();
            return Tuple();
        }
    ),
    SyntaxItem(s_ifstatement, {s_elsecondition, s_block}, 
        OPERATION {
            int top = stk.size() - 1;
            const Tuple &E = stk[top-1];
            quadList[E.addr].result = quadList.size();
            return Tuple();
        }
    ),
    SyntaxItem(s_ifcondition, {l_if, l_open_paren, s_exp, l_close_paren},
        OPERATION {
            int top = stk.size() - 1;
            Tuple C;
            C.addr = quadList.size();
            quadList.push_back({op_jz, stk[top - 1].place, -1, -1});
            return C;
        }
    ),
    SyntaxItem(s_elsecondition, {s_ifcondition, s_block, l_else}, 
        OPERATION {
            int top = stk.size() - 1;
            const Tuple &C = stk[top-2];
            Tuple E;
            E.addr = quadList.size();
            quadList.push_back({op_j, -1, -1, -1});
            quadList[C.addr].result = quadList.size();
            return E;
        }
    ),

    SyntaxItem(s_whilestatement, {s_whilecondition, s_block}, 
        OPERATION {
            int top = stk.size() - 1;
            const Tuple &C = stk[top-1];
            quadList.push_back({op_j, -1, -1, C.addr});
            quadList[C.addr].result = quadList.size();
            return Tuple();
        }
    ),
    SyntaxItem(s_whilecondition, {l_while, l_open_paren, s_exp, l_close_paren},
        OPERATION {
            int top = stk.size() - 1;
            Tuple C;
            C.addr = quadList.size();
            quadList.push_back({op_jz, stk[top - 1].place, -1, -1});
            return C;
        }
    ),

    SyntaxItem(s_block, {s_line}),
    SyntaxItem(s_block, {l_open_brace, s_lines, l_close_brace})
};

#endif