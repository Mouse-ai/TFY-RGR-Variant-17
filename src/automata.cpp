#include "tfy/automata.hpp"
#include <algorithm>
#include <queue>
#include <sstream>
#include <stdexcept>

namespace tfy {
static bool inAlphabet(const std::string& abc, char c) {
    return abc.find(c)!=std::string::npos;
}
bool DFA::accepts(const std::string& word) const {
    std::string state = initial;
    for (char c:word) {
        if (!inAlphabet(alphabet,c)) return false;
        auto it = delta.find({state,c});
        if (it==delta.end()) return false;
        state = it->second;
    }
    return finals.count(state)>0;
}
bool NFA::accepts(const std::string& word) const {
    std::set<std::string> active={initial};
    for (char c:word) {
        if (!inAlphabet(alphabet,c)) return false;
        std::set<std::string> next;
        for (const auto& s:active) {
            auto it=delta.find({s,c});
            if (it!=delta.end()) next.insert(it->second.begin(),it->second.end());
        }
        active=std::move(next);
        if (active.empty()) return false;
    }
    for (const auto& s:active) if (finals.count(s)) return true;
    return false;
}
DFA makeTask5DFA() {
    DFA d;
    d.states={"k1","k2","k3","k4","k5"};
    d.alphabet="abc";d.initial="k1";d.finals={"k4","k5"};
    d.delta={{{"k1",'a'},"k2"},{{"k1",'b'},"k3"},
      {{"k2",'b'},"k2"},{{"k2",'c'},"k4"},
      {{"k3",'a'},"k5"},{{"k3",'c'},"k3"},
      {{"k4",'a'},"k4"},{{"k5",'b'},"k5"}};
    return d;
}
NFA makeTask6NFA() {
    NFA n;
    n.states={"k1","k2","k3"};n.alphabet="0x";n.initial="k1";n.finals={"k2"};
    n.delta={{{"k1",'0'},{"k2","k3"}},{{"k1",'x'},{"k3"}},
      {{"k2",'x'},{"k2"}},{{"k3",'0'},{"k2"}}};
    return n;
}
NFA embedAsNFA(const DFA& dfa, bool withTrap) {
    NFA n;
    n.states=dfa.states;n.alphabet=dfa.alphabet;n.initial=dfa.initial;n.finals=dfa.finals;
    for (const auto& [key,target]:dfa.delta) n.delta[key].insert(target);
    if (withTrap) {
        const std::string trap="kTrap";
        if (std::find(n.states.begin(),n.states.end(),trap)!=n.states.end())
            throw std::invalid_argument("Trap name collides with a state");
        n.states.push_back(trap);
        for (const auto& state:n.states) for (char c:n.alphabet)
            if (!n.delta.count({state,c})) n.delta[{state,c}]={trap};
        for (char c:n.alphabet) n.delta[{trap,c}]={trap};
    }
    return n;
}
static std::string nameSet(const std::set<std::string>& states) {
    if (states.empty()) return "EMPTY";
    std::string out="{";
    bool first=true;
    for (const auto& s:states) { if(!first)out+=",";first=false;out+=s; }
    return out+"}";
}
DFA determinize(const NFA& n) {
    DFA d;
    d.alphabet=n.alphabet;
    std::set<std::string> initial={n.initial};
    d.initial=nameSet(initial);
    std::queue<std::set<std::string>> queue;
    std::set<std::string> seen;
    queue.push(initial);seen.insert(d.initial);
    while (!queue.empty()) {
        const auto curr=queue.front();queue.pop();
        auto from=nameSet(curr);
        d.states.push_back(from);
        for (const auto& s:curr) if (n.finals.count(s)) d.finals.insert(from);
        for (char c:n.alphabet) {
            std::set<std::string> next;
            for (const auto& s:curr) {
                auto it=n.delta.find({s,c});
                if (it!=n.delta.end()) next.insert(it->second.begin(),it->second.end());
            }
            std::string to=nameSet(next);
            d.delta[{from,c}]=to;
            if (seen.insert(to).second)queue.push(next);
        }
    }
    return d;
}
static std::string quote(const std::string& v) {
    std::string s="\"";
    for (char c:v) {if(c=='"'||c=='\\')s+='\\';s+=c;}
    return s+'"';
}
static std::string dotHeader(const std::string& initial, const std::set<std::string>& finals,
                             const std::vector<std::string>& states) {
    std::ostringstream os;
    os << "digraph Automaton {\n rankdir=LR; node [shape=circle];\n start [shape=point];\n";
    for (const auto& s:states)os << " " << quote(s) << " [shape=" << (finals.count(s)?"doublecircle":"circle") << "];\n";
    os << " start -> " << quote(initial) << ";\n";
    return os.str();
}
std::string dot(const DFA& d) {
    std::ostringstream os;os << dotHeader(d.initial,d.finals,d.states);
    for (const auto& [key,to]:d.delta)
        os << " " << quote(key.first) << " -> " << quote(to) << " [label=" << quote(std::string(1,key.second)) << "];\n";
    os << "}\n";return os.str();
}
std::string dot(const NFA& n) {
    std::ostringstream os;os << dotHeader(n.initial,n.finals,n.states);
    for (const auto& [key,to]:n.delta)
      for (const auto& target:to)
        os << " " << quote(key.first) << " -> " << quote(target) << " [label=" << quote(std::string(1,key.second)) << "];\n";
    os << "}\n";return os.str();
}
std::string transitions(const DFA& d) {
    std::ostringstream os;os << "state | symbol | next\n";
    for (const auto& s:d.states) for (char c:d.alphabet) {
        auto it=d.delta.find({s,c});
        os << s << " | " << c << " | " << (it==d.delta.end()?"-":it->second) << '\n';
    }
    return os.str();
}
std::string transitions(const NFA& n) {
    std::ostringstream os;os << "state | symbol | next set\n";
    for (const auto& s:n.states) for (char c:n.alphabet) {
        auto it=n.delta.find({s,c});
        os << s << " | " << c << " | " << (it==n.delta.end()?"EMPTY":nameSet(it->second)) << '\n';
    }
    return os.str();
}
}
