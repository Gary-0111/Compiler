#ifndef PARSER_H
#define PARSER_H

#include <cstdio>
#include <cstring>
#include <vector>
#include <set>
#include <queue>
#include <iostream>
#include <map>
#include <algorithm>
#include <stack>
#include <list>
#include <fstream>
#include "util.h"
#include "Token.h"

using namespace std;

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

class Parser {
    public:
        Parser();
        ~Parser();
        
        void printTable();
        void printFirstSet();
        void printFollowSet();
        void printItemSet();
        
        bool analyze(vector<Token> &line);

    private:
        static const vector<SyntaxItem> Syntax;
        //void analyzeRule(const string &rule);
        void generateFirstSet();
        void generateFollowSet();
        void generateTable();
        void printFirst(const SyntaxSymbol &A);
        void printFollow(const SyntaxSymbol &A);
        int findRule(const SyntaxSymbol &left, const vector<SyntaxSymbol> &right);
        set<pair<SyntaxItem, SyntaxSymbol> > go(int i, const SyntaxSymbol &x);
        set<pair<SyntaxItem, SyntaxSymbol> > closure(set<pair<SyntaxItem, SyntaxSymbol> > I);

        SyntaxSymbol startSymbol;
        set<SyntaxSymbol> Vn;
        set<SyntaxSymbol> Vt;

        int initState;
        map<SyntaxSymbol, set<SyntaxSymbol> > FIRST, FOLLOW;
        vector<set<pair<SyntaxItem, SyntaxSymbol> > > itemSet;
        map<set<pair<SyntaxItem, SyntaxSymbol> >, int> C;

        map<int, map<SyntaxSymbol, ActionItem> > ACTION;
        map<int, map<SyntaxSymbol, int> > GOTO;
};

#endif