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
#include "Syntax.h"

using namespace std;

extern const string LR1_file;
extern vector<Identifier> IDlist;
extern vector<variable> varList;
extern vector<Quadruple> quadList;

class Parser {
    public:
        Parser();
        ~Parser();
        /*
        void printTable();
        void printFirstSet();
        void printFollowSet();
        void printItemSet();
        */        
        bool analyze(vector<Token> &line);

    private:
    /*
        static const vector<SyntaxItem> Syntax;
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
    */
        map<int, map<SyntaxSymbol, ActionItem> > ACTION;
        map<int, map<SyntaxSymbol, int> > GOTO;
};

#endif