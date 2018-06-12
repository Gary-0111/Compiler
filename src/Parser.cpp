#include "Parser.h"


Parser::Parser() {
    //generateTable();
    FILE *fd = fopen(LR1_file.c_str(), "r");
    int actionsize, gotosize;
    fscanf(fd, "%d", &actionsize);
    while(actionsize--) {
        int a, b, c, d;
        fscanf(fd, "%d %d %d %d", &a, &b, &c, &d);
        ACTION[a][SyntaxSymbol(b)] = ActionItem(Act_type(c), d);
    }
    fscanf(fd, "%d", &gotosize);
    while(gotosize--) {
        int a, b, c;
        fscanf(fd, "%d %d %d", &a, &b, &c);
        GOTO[a][SyntaxSymbol(b)] = c;
    }
}

Parser::~Parser() {

}


// TODO:
// print the analyze process
bool Parser::analyze(vector<Token> &line) {
    bool ret;
    line.push_back(Token(l_eof, -1));
    static vector<Tuple> stk;
    stk.push_back(Tuple(l_eof, 0));

    Token &token = line[0];
    SyntaxSymbol &symbol = token.symbol;
    int index = 0;
    while(1) {
        int state = stk.back().state;
        const ActionItem &act = ACTION[state][symbol];
        //cout << "ACTION[" << state << "][" << symbol << "] = {" << act.type << ", " << act.num << "}\n";
        if(act.type == Act_shift) {
            //cout << "ACTION[" << state << "][" << symbol << "] = {shift, " << act.num << "}\n";
            if(symbol == l_integer || symbol == l_decimal) {
                int place = newtemp(symbol == l_integer? exp_int : exp_float);
                varList[place] = token.value.var;
                stk.push_back(Tuple(symbol, act.num, place));
            } else if (token.symbol == l_id) {
                stk.push_back(Tuple(symbol, act.num, token.value.str));
            } else {
                stk.push_back(Tuple(symbol, act.num));
            }
            token = line[++index];
            symbol = token.symbol;

        } else if(act.type == Act_reduce) {
            //cout << "ACTION[" << state << "][" << symbol << "] = {reduce, " << act.num << "}\n";
            int i = act.num;
            const auto &left = Syntax[i].rule.first;
            const auto &right = Syntax[i].rule.second;
            // TODO:
            Tuple tp;
            if(Syntax[i].operation != NULL) {
                tp = Syntax[i].operation(stk);
            }
            for(int j = 0; j < right.size(); ++j) {
                stk.pop_back();
            }
            tp.symbol = left;
            tp.state = GOTO[stk.back().state][left];
            stk.push_back(tp);
        } else if(act.type == Act_accept) {
            ret = true;
            break;
        } else {
            cout << "error: ";
            if(symbol == l_integer)
                cout << token.value.var.evalue.integer << "\n";
            else if(symbol == l_decimal)
                cout << token.value.var.evalue.decimal << "\n";
            else 
                cout << token.value.str << "\n";
            ret = false;
            break; 
        }
    }
    return ret;
}

