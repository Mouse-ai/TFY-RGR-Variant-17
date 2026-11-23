#pragma once
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace tfy {
using Key=std::pair<std::string,char>;
struct DFA {
    std::vector<std::string> states;
    std::string alphabet;
    std::string initial;
    std::set<std::string> finals;
    std::map<Key,std::string> delta; // partial DFA is supported
    bool accepts(const std::string& word) const;
};
struct NFA {
    std::vector<std::string> states;
    std::string alphabet;
    std::string initial;
    std::set<std::string> finals;
    std::map<Key,std::set<std::string>> delta; // no epsilon transitions needed for variant 17
    bool accepts(const std::string& word) const;
};
DFA makeTask5DFA();
NFA makeTask6NFA();
NFA embedAsNFA(const DFA& dfa, bool withTrap=false);
DFA determinize(const NFA& nfa);
std::string dot(const DFA& dfa);
std::string dot(const NFA& nfa);
std::string transitions(const DFA& dfa);
std::string transitions(const NFA& nfa);
}
