#include "tfy/grammar.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace tfy {
Grammar makeTask3Grammar() {
    return {{"S","A","B"},"a","S",{{"S","aAa"},{"A","aBa"},{"B","A"},{"A","a"}}};
}
Grammar makeTask4Grammar() {
    return {{"S","A","B","C"},"abc","S",{{"S","aSa"},{"S","aAa"},{"A","bAb"},
        {"A","bBb"},{"B","cC"},{"C","cC"},{"C",""}}};
}
Grammar makeTask10Grammar() {
    return {{"S","A"},"abc","S",{{"S","a"},{"S","aS"},{"S","aA"},
         {"A","a"},{"A","b"},{"A","c"}}};
}
static void replaceOne(std::string& text, char symbol, const std::string& replacement) {
    auto pos=text.find(symbol);
    if (pos==std::string::npos) throw std::logic_error("Nonterminal not found");
    text.replace(pos,1,replacement);
}
std::vector<std::string> task3Derivation(std::size_t n) {
    std::vector<std::string> steps={"S","aAa"};
    std::string w="aAa";
    for (std::size_t i=0;i<n;++i) {
        replaceOne(w,'A',"aBa");steps.push_back(w);
        replaceOne(w,'B',"A");steps.push_back(w);
    }
    replaceOne(w,'A',"a");steps.push_back(w);
    return steps;
}
std::vector<std::string> task4Derivation(std::size_t k, std::size_t l, std::size_t m) {
    if (k==0 || l==0 || m==0) throw std::invalid_argument("k,l,m >= 1");
    std::vector<std::string> steps={"S"};
    std::string w="S";
    for (std::size_t i=1;i<k;++i) {replaceOne(w,'S',"aSa");steps.push_back(w);}
    replaceOne(w,'S',"aAa");steps.push_back(w);
    for (std::size_t i=1;i<l;++i) {replaceOne(w,'A',"bAb");steps.push_back(w);}
    replaceOne(w,'A',"bBb");steps.push_back(w);
    replaceOne(w,'B',"cC");steps.push_back(w);
    for (std::size_t i=1;i<m;++i) {replaceOne(w,'C',"cC");steps.push_back(w);}
    replaceOne(w,'C',"");steps.push_back(w);
    return steps;
}
static bool allEqual(const std::string& s, char ch) {
    return std::all_of(s.begin(),s.end(),[ch](char x){return x==ch;});
}
bool belongsTask3(const std::string& s) {
    return s.size()>=3 && s.size()%2==1 && allEqual(s,'a');
}
bool belongsTask4(const std::string& s) {
    // Match a^k b^l c^m b^l a^k with k,l,m >= 1.
    std::size_t p=0,k=0,l=0,m=0;
    while(p<s.size()&&s[p]=='a'){++p;++k;}
    while(p<s.size()&&s[p]=='b'){++p;++l;}
    while(p<s.size()&&s[p]=='c'){++p;++m;}
    if(k==0||l==0||m==0||p+l+k!=s.size())return false;
    for(std::size_t i=0;i<l;++i)if(s[p+i]!='b')return false;
    p+=l;
    for(std::size_t i=0;i<k;++i)if(s[p+i]!='a')return false;
    return true;
}
bool belongsTask5(const std::string& s) {
    if(s.size()>=2&&s[0]=='a') {
        std::size_t p=1;
        while(p<s.size()&&s[p]=='b')++p;
        if(p<s.size()&&s[p]=='c') {
            ++p;
            while(p<s.size()&&s[p]=='a')++p;
            if(p==s.size())return true;
        }
    }
    if(s.size()>=2&&s[0]=='b') {
        std::size_t p=1;
        while(p<s.size()&&s[p]=='c')++p;
        if(p<s.size()&&s[p]=='a') {
            ++p;
            while(p<s.size()&&s[p]=='b')++p;
            if(p==s.size())return true;
        }
    }
    return false;
}
bool belongsTask6(const std::string& s) {
    if(s.empty())return false;
    std::size_t p=0;
    if(s[0]=='0') {
        p=1;
        if(std::all_of(s.begin()+1,s.end(),[](char c){return c=='x';}))return true;
        if(s.size()>=2 && s[1]=='0')p=2;
        else return false;
    } else if (s.size()>=2&&s[0]=='x'&&s[1]=='0')p=2;
    else return false;
    return std::all_of(s.begin()+static_cast<std::ptrdiff_t>(p),s.end(),[](char c){return c=='x';});
}
bool contextFree(const Grammar& g) {
    for(const auto& r:g.rules) if(!g.nonterminals.count(r.lhs))return false;
    return true;
}
bool rightLinear(const Grammar& g) {
    if(!contextFree(g))return false;
    for(const auto& r:g.rules) {
        if(r.rhs.empty())continue;
        if(r.rhs.size()==1) {
            if(g.terminals.find(r.rhs[0])==std::string::npos)return false;
        } else if(r.rhs.size()==2) {
            if(g.terminals.find(r.rhs[0])==std::string::npos ||
               !g.nonterminals.count(std::string(1,r.rhs[1])))return false;
        } else return false;
    }
    return true;
}
bool reversible(const Grammar& g) {
    std::map<std::string,std::string> seen;
    for(const auto& rule:g.rules) {
        auto it=seen.find(rule.rhs);
        if(it!=seen.end() && it->second!=rule.lhs)return false;
        seen[rule.rhs]=rule.lhs;
    }
    return true;
}
Grammar grammarFromDFA(const DFA& d) {
    if(d.states.size()>26)throw std::invalid_argument("too many states");
    std::map<std::string,std::string> vars;
    const std::string names="SABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(std::size_t i=0;i<d.states.size();++i)
        vars[d.states[i]]=std::string(1,names[i]);
    Grammar g;g.terminals=d.alphabet;g.start=vars.at(d.initial);
    for(auto& kv:vars)g.nonterminals.insert(kv.second);
    for(const auto& [key,dest]:d.delta) {
        if(!vars.count(key.first)||!vars.count(dest))throw std::invalid_argument("unknown DFA state");
        g.rules.push_back({vars.at(key.first),std::string(1,key.second)+vars.at(dest)});
    }
    for(const auto& fin:d.finals)g.rules.push_back({vars.at(fin),""});
    return g;
}
NFA nfaFromRightLinearGrammar(const Grammar& g) {
    if(!rightLinear(g))throw std::invalid_argument("Grammar is not right-linear");
    NFA n;n.alphabet=g.terminals;
    for(const auto& nt:g.nonterminals)n.states.push_back(nt);
    n.initial=g.start;
    // A -> a: introduce final state FIN, A -> epsilon: A is accepting
    bool needExtra=false;
    for(const auto& rule:g.rules) if(rule.rhs.size()==1)needExtra=true;
    if(needExtra)n.states.push_back("FIN");
    if(needExtra)n.finals.insert("FIN");
    for(const auto& r:g.rules) {
        if(r.rhs.empty()) n.finals.insert(r.lhs);
        else if(r.rhs.size()==1) n.delta[{r.lhs,r.rhs[0]}].insert("FIN");
        else n.delta[{r.lhs,r.rhs[0]}].insert(std::string(1,r.rhs[1]));
    }
    return n;
}
std::string showGrammar(const Grammar& g) {
    std::ostringstream os;
    os << "G=(VN,VT,P,"<<g.start<<")\nVN={";
    bool first=true;
    for(auto& n:g.nonterminals){if(!first)os<<",";first=false;os<<n;}
    os<<"} VT={";
    for(std::size_t i=0;i<g.terminals.size();++i){if(i)os<<",";os<<g.terminals[i];}
    os<<"}\nP:\n";
    for(const auto& r:g.rules)os<<"  "<<r.lhs<<" -> "<<(r.rhs.empty()?"lambda":r.rhs)<<"\n";
    return os.str();
}
std::string task11TreeDot(bool stretched) {
    std::ostringstream os;
    os << "digraph ParseTree {\n graph [ordering=out]; node [shape=circle];\n";
    if(!stretched) {
        os << "S -> {a1 A a2};\n A -> {b1 B b2};\n B -> {c1 C};\n C -> eps;\n";
        for(const char* s:{"a1","a2","b1","b2","c1"})os << s << " [label=\""<<s[0]<<"\",shape=plain];\n";
        os << "eps [label=\"lambda\",shape=plain];\n";
        os << "{rank=same;a1;A;a2;}\n{rank=same;b1;B;b2;}\n";
    } else {
        const std::vector<std::string> rows={"S","aAa","abBba","abcCba","abcba"};
        for(std::size_t i=0;i<rows.size();++i){
            os << "subgraph cluster"<<i<<" { style=invis; ";
            for(std::size_t j=0;j<rows[i].size();++j)
                os << " n"<<i<<"_"<<j<<" [label=\""<<rows[i][j]<<"\"];";
            os << " {rank=same;";
            for(std::size_t j=0;j<rows[i].size();++j)os<<"n"<<i<<"_"<<j<<";";
            os << "}}\n";
        }
        os << "n0_0 -> n1_0; n0_0 -> n1_1; n0_0 -> n1_2;\n";
        os << "n1_0 -> n2_0; n1_1 -> n2_1; n1_1 -> n2_2; n1_1 -> n2_3; n1_2 -> n2_4;\n";
        os << "n2_0 -> n3_0; n2_1 -> n3_1; n2_2 -> n3_2; n2_2 -> n3_3; n2_3 -> n3_4; n2_4 -> n3_5;\n";
        os << "n3_0 -> n4_0; n3_1 -> n4_1; n3_2 -> n4_2; n3_4 -> n4_3; n3_5 -> n4_4;\n";
        os << "// n3_3 is C -> lambda (no terminal child)\n";
    }
    os << "}\n";return os.str();
}
}
