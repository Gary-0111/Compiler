#include <cstdio>
#include <cstring>
#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <map>
#include <algorithm>
#include "util.h"
#include "Syntax.h"

using namespace std;

extern const string LR1_file;

class Parser {
    public:
        Parser();
        ~Parser();
        /*
        void printTable();
        void printFirstSet();
        void printFollowSet();
        void printItemSet();
        
        bool analyze(vector<Token> &line);
        static const string LR1_file;
        */

    private:
        //static const vector<SyntaxItem> Syntax;
        //void analyzeRule(const string &rule);
        void generateFirstSet();
        void generateFollowSet();
        void generateTable();
        int findRule(const SyntaxSymbol &left, const vector<SyntaxSymbol> &right);
        set<pair<SyntaxItem, SyntaxSymbol> > go(int i, const SyntaxSymbol &x);
        set<pair<SyntaxItem, SyntaxSymbol> > closure(set<pair<SyntaxItem, SyntaxSymbol> > I);

        void printFirst(const SyntaxSymbol &A);
        void printFollow(const SyntaxSymbol &A);

        set<SyntaxSymbol> Vn;
        set<SyntaxSymbol> Vt;

        map<SyntaxSymbol, set<SyntaxSymbol> > FIRST, FOLLOW;
        vector<set<pair<SyntaxItem, SyntaxSymbol> > > itemSet;
        map<set<pair<SyntaxItem, SyntaxSymbol> >, int> C;

        map<int, map<SyntaxSymbol, ActionItem> > ACTION;
        map<int, map<SyntaxSymbol, int> > GOTO;
};

Parser::Parser() {
    generateTable();
    for(const auto &r : Syntax) {
        Vn.insert(r.rule.first);
        for(const auto &symbol : r.rule.second) {
            if(symbol < terminate_end) Vt.insert(symbol);
            else Vn.insert(symbol);
        }
    }
}

Parser::~Parser() {

}

void Parser::generateTable() {
    generateFirstSet();
    generateFollowSet();
    vector<SyntaxSymbol> startright = {s_dot, Syntax[0].rule.second[0]}, endright = {Syntax[0].rule.second[0], s_dot};
    const pair<SyntaxItem, SyntaxSymbol> endItem = {SyntaxItem(s_auxiliary, endright), l_eof};
    const pair<SyntaxItem, SyntaxSymbol> startItem = {SyntaxItem(s_auxiliary, startright), l_eof};
    auto I0 = closure({startItem});
    C[I0] = itemSet.size();
    itemSet.push_back(I0);
    queue<int> Q;
    Q.push(0);
    int step = 1;
    while(!Q.empty()) {
        cout << "[" << step++ << "] Generating the analyze table...\n";
        int i = Q.front();
        Q.pop();
        int count = 0;
        for(auto it = itemSet[i].begin(); it != itemSet[i].end(); ++it) {
            const auto &item = *it;
            const auto &left = item.first.rule.first;
            const auto &right = item.first.rule.second;
            auto pos = find(right.begin(), right.end(), s_dot);
            auto pre = pos++;
            if(pos == right.end()) {
                if(item == endItem) {
                    ACTION[i][l_eof] = ActionItem(Act_accept, -1);
                } else {
                    vector<SyntaxSymbol> vec;
                    vec.insert(vec.begin(), right.begin(), pre);
                    ACTION[i][item.second] = ActionItem(Act_reduce, findRule(left, vec));
                }
            } else {
                const auto &x = *pos;
                auto D = go(i, x);
                //printItemSet();
                if(C.find(D) == C.end()) {
                    C[D] = itemSet.size();
                    Q.push(itemSet.size());
                    itemSet.push_back(D);
                }
                if(x < terminate_end) {
                    ACTION[i][x] = ActionItem(Act_shift, C[D]);
                } else {
                    GOTO[i][x] = C[D];
                }
            }
        }

    }
    cout << "Done!\n";
    
    int actionsize = 0, gotosize = 0;
    for(const auto &s : ACTION) {
        actionsize += s.second.size();
    }
    for(const auto &s : GOTO) {
        gotosize += s.second.size();
    }

    FILE *fd = fopen(LR1_file.c_str(), "w");
    fprintf(fd, "%d\n", actionsize);
    for(const auto &s : ACTION) {
        for(const auto &item : s.second) {
            fprintf(fd, "%d %d %d %d\n", s.first, item.first, (int)item.second.type, item.second.num);
        }
    }
    fprintf(fd, "%d\n", gotosize);
    for(const auto &s : GOTO) {
        for(const auto &alpha : s.second) {
            fprintf(fd, "%d %d %d\n", s.first, (int)alpha.first, alpha.second);
        }
    }
    fclose(fd);
}

set<pair<SyntaxItem, SyntaxSymbol> > Parser::go(int i, const SyntaxSymbol &x) {
    set<pair<SyntaxItem, SyntaxSymbol> > tmp;
    for(const auto &item : itemSet[i]) {
        const auto &right = item.first.rule.second;
        auto pos = find(right.begin(), right.end(), s_dot);
        auto pre = pos++;
        if(pos == right.end()) continue;
        SyntaxSymbol B = *pos;
        if(B == x) {
            vector<SyntaxSymbol> vec;
            vec.insert(vec.end(), right.begin(), pre);
            vec.push_back(B);
            vec.push_back(s_dot);
            vec.insert(vec.end(), ++pos, right.end());
            tmp.insert({{item.first.rule.first, vec}, item.second});
        }
    }
    return closure(tmp);
}

set<pair<SyntaxItem, SyntaxSymbol> > Parser::closure(set<pair<SyntaxItem, SyntaxSymbol> > I) {
    while(1) {
        int presize = I.size();
        set<pair<SyntaxItem, SyntaxSymbol> > Itmp;
        for(const auto &item : I) {
            const auto &right = item.first.rule.second;
            auto pos = find(right.begin(), right.end(), s_dot);
            auto pre = pos++;
            if(pos == right.end()) continue;
            SyntaxSymbol B = *pos++;
            if(B > terminate_end) {
                set<SyntaxSymbol> f;
                if(pos == right.end()) {
                    f.insert(item.second);
                } else {
                    for(const auto &a : FIRST[*pos]) {
                        if(a == l_epsilon) f.insert(item.second);
                        else f.insert(a);
                    }
                }

                for(const auto &r : Syntax) {
                    if(r.rule.first == B) {
                        for(const auto &b : f) {
                            auto vec = r.rule.second;
                            vec.insert(vec.begin(), s_dot);
                            Itmp.insert({{r.rule.first, vec}, b});
                        }
                    }
                }
            }
        }
        for(const auto &item : Itmp) {
            I.insert(item);
        }
        if(I.size() == presize) break;
    }
    return I;
}

int Parser::findRule(const SyntaxSymbol &left, const vector<SyntaxSymbol> &right) {
    for(int i = 0; i < Syntax.size(); ++i) {
        if(Syntax[i].rule.first == left && Syntax[i].rule.second == right)
            return i;
    }
    return -1;
}

void Parser::generateFirstSet()
{
    cout << "Generating FIRST set...\n";
	for (int a = 0; a < terminate_end; ++a)
		FIRST[(SyntaxSymbol)a].insert((SyntaxSymbol)a);

	while(1) {
		bool flag = true;
        for(const auto &r : Syntax) {

            const SyntaxSymbol &A = r.rule.first;
            const auto &alpha = r.rule.second;

            int presize = FIRST[A].size();
            if(alpha[0] == l_epsilon) {
                FIRST[A].insert(l_epsilon);
            } else {
                bool haveEpsilon = true;
                for(int i = 0; i < alpha.size(); ++i) {
                    if(alpha[i] < terminate_end) {
                        FIRST[A].insert(alpha[i]);
                        haveEpsilon = false;
                        break;
                    }
                    for(auto a : FIRST[alpha[i]]) {
                        if(a == l_epsilon) continue;
                        FIRST[A].insert(a);
                    }
                    if(FIRST[alpha[i]].find(l_epsilon) == FIRST[alpha[i]].end()) {
                        haveEpsilon = false;
                        break;
                    }
                }
                if(haveEpsilon) FIRST[A].insert(l_epsilon);
            }
            if(FIRST[A].size() > presize) flag = false;
        }
		if(flag) break;
	}
}

void Parser::generateFollowSet()
{
    cout << "Generating FOLLOW set...\n";
    Vt.insert(l_eof);
	FOLLOW[s_auxiliary].insert(l_eof);
	while (1)
	{
		bool flag = true;
		for(const auto &r : Syntax) {

            const SyntaxSymbol &A = r.rule.first;
            const auto &alpha = r.rule.second;

            int i = 1;
            SyntaxSymbol symbol = alpha[0], nxt;
            if(i >= alpha.size()) nxt = l_epsilon;
			else nxt = alpha[i];
            while (symbol != l_epsilon) {
				if (symbol > terminate_end) {
					int presize = FOLLOW[symbol].size();
					if(nxt != l_epsilon) {
                        for(const auto &a : FIRST[nxt]) {
							if (a == l_epsilon) continue;
						    FOLLOW[symbol].insert(a);
						}
					}
					if (nxt == l_epsilon || FIRST[nxt].find(l_epsilon) != FIRST[nxt].end()) {
                        for(const auto &a : FOLLOW[A]) {
							FOLLOW[symbol].insert(a);
						}
					}
					if (FOLLOW[symbol].size() > presize)
						flag = false;
				}
				symbol = nxt;
                ++i;
                if(i >= alpha.size()) nxt = l_epsilon;
			    else nxt = alpha[i];
			}
        }
		if (flag) break;
	}
}

/*
// TODO:
// print the analyze process
bool Parser::analyze(vector<Token> &line) {
    bool ret;
    line.push_back(Token(l_eof, l_eof));
    vector<Tuple> stk;
    stk.push_back(Tuple(l_eof, 0));

    Token &token = line[0];
    SyntaxSymbol &symbol = token.symbol;
    int index = 0;
    while(1) {
        int state = stk.back().state;
        const ActionItem &act = ACTION[state][symbol];
        if(act.type == Act_shift) {
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


void Parser::printFirst(const SyntaxSymbol &A)
{
	cout << "\tFIRST( " << A << " ) = {";
    bool flag = false;
    for(auto &a : FIRST[A]) {
        if (flag) cout << ",";
		cout << " " << a;
        flag = true;
    }
    cout << " }\n";
}

void Parser::printFirstSet()
{
	for (int a = 0; a < terminate_end; ++a)
	{
		if((SyntaxSymbol)a == l_eof) continue;
		printFirst((SyntaxSymbol)a);
	}
	for (int A = terminate_end + 1; A < s_dot; ++A)
		printFirst((SyntaxSymbol)A);
	cout << "\n";
}

void Parser::printFollow(const SyntaxSymbol &A)
{
	cout << "\tFOLLOW( " << A << " ) = {";
    bool flag = false;
    for(auto &a : FOLLOW[A]) {
        if (flag) cout << ",";
		cout << " " << a;
        flag = true;
    }
    cout << " }\n";
}

void Parser::printFollowSet()
{
	for (int A = terminate_end + 1; A < s_dot; ++A)
		printFollow((SyntaxSymbol)A);
	cout << "\n";
}

void Parser::printItemSet() {
    cout << "###################################################\n";
    for(int i = 0; i < itemSet.size(); ++i) {
        cout << "I" << i << ": \n";
        for(const auto &item : itemSet[i]) {
            cout << "\t[" << item.first.rule.first << "->";
            for(const auto &symbol : item.first.rule.second)
                cout << " " << symbol;
            cout << ", " << item.second << "]\n"; 
        }
    }
}


void Parser::printTable() {
    cout << "ACTION: \n";
    cout << "state\t";
    for(const auto &a : Vt) cout << a << "\t";
    cout << "\n";
    for(int i = 0; i < itemSet.size(); ++i) {
        cout << i << "\t";
        for(const auto &a : Vt) {
            //SyntaxSymbol a = (SyntaxSymbol)j;
            if(ACTION.find(i) != ACTION.end() && ACTION[i].find(a) != ACTION[i].end()) {
                if(ACTION[i][a].type == Act_shift) cout << "s" << ACTION[i][a].num;
                else if(ACTION[i][a].type == Act_reduce) cout << "r" << ACTION[i][a].num;
                else if(ACTION[i][a].type == Act_accept) cout << "acc";
            }
            cout << "\t";
        }
        cout << "\n";
    }
    cout << "GOTO: \n";
    cout << "state\t";
    for(const auto &A : Vn) cout << A << "\t";
    cout << "\n";
    for(int i = 0; i < itemSet.size(); ++i) {
        cout << i << "\t";
        for(const auto &A : Vn) {
            //SyntaxSymbol A = (SyntaxSymbol)j;
            if(GOTO.find(i) != GOTO.end() && GOTO[i].find(A) != GOTO[i].end())
                cout << GOTO[i][A];
            cout << "\t";
        }
        cout << "\n";
    }
}
*/

int main()
{
    Parser parser;
    return 0;
}