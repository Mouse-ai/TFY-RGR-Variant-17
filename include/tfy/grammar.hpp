#pragma once
#include "tfy/automata.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace tfy {
struct Rule { std::string lhs, rhs; }; // rhs="" means epsilon
struct Grammar {
    std::set<std::string> nonterminals;
    std::string terminals;
    std::string start;
    std::vector<Rule> rules;
};
Grammar makeTask3Grammar();
Grammar makeTask4Grammar();
Grammar makeTask10Grammar();
Grammar grammarFromDFA(const DFA& d);
NFA nfaFromRightLinearGrammar(const Grammar& g);
std::string showGrammar(const Grammar& g);
bool rightLinear(const Grammar& g);
bool contextFree(const Grammar& g);
bool reversible(const Grammar& g);
bool belongsTask3(const std::string& s);
bool belongsTask4(const std::string& s);
bool belongsTask5(const std::string& s);
bool belongsTask6(const std::string& s);
std::vector<std::string> task3Derivation(std::size_t n);
std::vector<std::string> task4Derivation(std::size_t k, std::size_t l, std::size_t m);
std::string task11TreeDot(bool stretched);
}
