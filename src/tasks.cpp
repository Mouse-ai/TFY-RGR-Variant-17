#include "tfy/tasks.hpp"
#include "tfy/automata.hpp"
#include "tfy/grammar.hpp"
#include "tfy/language.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace tfy {
namespace {
void saveDot(const std::filesystem::path& dir,const std::string& name,
             const std::string& contents,std::ostream& out) {
    if(dir.empty()) return;
    std::filesystem::create_directories(dir);
    auto file=dir/(name+".dot");
    std::ofstream stream(file);
    if(!stream)throw std::runtime_error("Cannot write "+file.string());
    stream << contents;
    out << "Graphviz DOT: " << file.string() << '\n';
}
void printSteps(const std::vector<std::string>& steps,std::ostream& out) {
    for(std::size_t i=0;i<steps.size();++i)out << (i?" => ":"") << showWord(steps[i]);
    out<<'\n';
}
void task01(std::ostream& out,std::size_t limit) {
    const Language l1={"ab","ac"},l2={"ac","ab","acb"};
    out << "L1 = " << showLanguage(l1) << "\nL2 = " << showLanguage(l2) << '\n';
    out << "a) L1 union L2 = "<<showLanguage(langUnion(l1,l2))<<'\n';
    out << "b) L1 intersection L2 = "<<showLanguage(langIntersection(l1,l2))<<'\n';
    out << "c) L1 \\ L2 = "<<showLanguage(langDifference(l1,l2))<<'\n';
    out << "d) L2 \\ L1 = "<<showLanguage(langDifference(l2,l1))<<'\n';
    out << "e) complement(L1) = {a,b,c}* \\ {ab,ac}\n";
    out << "   bounded (len <= "<<limit<<"): "<<showLanguage(complementBounded(l1,"abc",limit))<<'\n';
    out << "f) complement(L2) = {a,b,c}* \\ {ac,ab,acb}\n";
    out << "   bounded (len <= "<<limit<<"): "<<showLanguage(complementBounded(l2,"abc",limit))<<'\n';
    out << "g) L1.L2 = "<<showLanguage(concatenate(l1,l2))<<'\n';
    out << "h) L2.L1 = "<<showLanguage(concatenate(l2,l1))<<'\n';
}
void task02(std::ostream& out,std::size_t limit) {
    Language l={"11"};
    out << "L = "<<showLanguage(l)<<'\n';
    out << "L+ = {1^(2n) | n >= 1}: "<<showLanguage(iterateBounded(l,limit,false))<<'\n';
    out << "L* = {1^(2n) | n >= 0}: "<<showLanguage(iterateBounded(l,limit,true))<<'\n';
    out << "Displayed only the first "<<limit<<" positive powers.\n";
}
void task03(std::ostream& out,std::size_t limit) {
    const auto g=makeTask3Grammar();
    out << showGrammar(g);
    out << "Grammar class: type 2 (context-free); rules S->aAa and A->aBa are not right-linear.\n";
    out << "L(G) = {a^(2n+3) | n >= 0} = aaa(aa)*\n";
    for(std::size_t n=0;n<limit;++n){out<<"n="<<n<<": ";printSteps(task3Derivation(n),out);}
}
void task04(std::ostream& out,std::size_t limit) {
    const auto g=makeTask4Grammar();
    out << showGrammar(g);
    out << "Class: type 2 (context-free).\n";
    out << "L(G) = {a^k b^l c^m b^l a^k | k,l,m >= 1}\n";
    out << "Smallest word k=l=m=1: ";printSteps(task4Derivation(1,1,1),out);
    if(limit>=2){out<<"Example k=2,l=1,m=2: ";printSteps(task4Derivation(2,1,2),out);}
}
void task05(std::ostream& out,const std::filesystem::path& dotDir) {
    const auto d=makeTask5DFA();
    out << "Partial DFA: K={k1,k2,k3,k4,k5}, alphabet={a,b,c}, start=k1, F={k4,k5}\n";
    out << "L = {ab^m c a^n | m,n >= 0} UNION {bc^k a b^l | k,l >= 0}\n";
    out << transitions(d);
    out << "Test abca: "<<(d.accepts("abca")?"accept":"reject")<<"; acb: "<<(d.accepts("acb")?"accept":"reject")<<"\n";
    out << "5-state solution is partial; a total DFA needs a 6th trap state.\n";
    saveDot(dotDir,"task05_DFA",dot(d),out);
}
void task06(std::ostream& out,const std::filesystem::path& dotDir) {
    const auto n=makeTask6NFA();
    out << "NFA: K={k1,k2,k3}, alphabet={0,x}, start=k1, F={k2}\n";
    out << "L={0x^k | k>=0} UNION {00x^m | m>=0} UNION {x0x^n | n>=0}\n";
    out << transitions(n);
    out << "Test 00x: "<<(n.accepts("00x")?"accept":"reject")<<"; x0x: "<<(n.accepts("x0x")?"accept":"reject")<<"\n";
    saveDot(dotDir,"task06_NFA",dot(n),out);
}
void task07(std::ostream& out,const std::filesystem::path& dotDir) {
    const auto n=embedAsNFA(makeTask5DFA(),true);
    out << "Convert DFA(task 5) to equivalent NFA by singleton transition sets.\n";
    out << "With explicit trap kTrap, missing transitions go to {kTrap}.\n";
    out << "K={k1,k2,k3,k4,k5,kTrap}, start=k1, F={k4,k5}\n";
    out << transitions(n);
    out << "A DFA is already a special case of an NFA; no extra nondeterministic branch is required.\n";
    saveDot(dotDir,"task07_NFA",dot(n),out);
}
void task08(std::ostream& out,const std::filesystem::path& dotDir) {
    const auto d=determinize(makeTask6NFA());
    out << "Subset construction: each DFA state is a subset of NFA states.\n";
    out << "Initial state: "<<d.initial<<"; accepting states: ";
    for(const auto& f:d.finals) out<<f<<" ";
    out<<"\n";
    out << "EMPTY is a trap state.\n";
    out << transitions(d);
    saveDot(dotDir,"task08_DFA",dot(d),out);
}
void task09(std::ostream& out) {
    const auto g=grammarFromDFA(makeTask5DFA());
    out << "Right-linear grammar obtained from task 5 DFA:\n"<<showGrammar(g);
    out << "Correspondence: k1=S,k2=A,k3=B,k4=C,k5=D.\n";
    out << "Accepting states yield epsilon rules C->lambda and D->lambda.\n";
    out << "L(G) = {ab^mca^n} UNION {bc^kab^l}, all exponents >=0.\n";
}
void task10(std::ostream& out,const std::filesystem::path& dotDir) {
    const auto g=makeTask10Grammar();
    auto n=nfaFromRightLinearGrammar(g);
    // Alias S->k1, A->k2, FIN->k3 to match handwritten convention.
    auto renamed=[](const std::string& s){return s=="S"?"k1":s=="A"?"k2":s=="FIN"?"k3":s;};
    NFA display;
    display.alphabet=n.alphabet;display.initial=renamed(n.initial);
    for(const auto& state:n.states)display.states.push_back(renamed(state));
    for(const auto& state:n.finals)display.finals.insert(renamed(state));
    for(const auto& [key,values]:n.delta)
        for(const auto& value:values)display.delta[{renamed(key.first),key.second}].insert(renamed(value));
    out << "Original regular grammar:\n"<<showGrammar(g);
    out << "NFA constructed from productions (S=k1, A=k2, final=k3):\n";
    out << "K={k1,k2,k3}, initial=k1, F={k3}\n"<<transitions(display);
    out << "L(G) = {a^n, a^nb, a^nc | n>=1} = a+(lambda|b|c)\n";
    saveDot(dotDir,"task10_NFA",dot(display),out);
}
void task11(std::ostream& out,const std::filesystem::path& dotDir) {
    const auto derivation=task4Derivation(1,1,1);
    out << "Task 11 uses the grammar of task 4.\n";
    out << "|w|=2k+2l+m, k,l,m>=1 -> smallest word abcba (length 5).\n";
    out << "Delta_1: ";printSteps(derivation,out);
    out << "Marked Delta'_1: (*S*, a*A*a, ab*B*ba, abc*C*ba, abcba)\n";
    out << "Unique sequence: S->aAa; A->bBb; B->cC; C->lambda.\n";
    out << "The grammar is unambiguous: k,l,m determine all rule counts uniquely.\n";
    saveDot(dotDir,"task11_parse_tree",task11TreeDot(false),out);
    saveDot(dotDir,"task11_stretched_tree",task11TreeDot(true),out);
}
void task12(std::ostream& out) {
    const auto g=makeTask10Grammar();
    out << "Task 12 checks the grammar of task 10.\n"<<showGrammar(g);
    out << "Condition 1 (reversibility): "<<(reversible(g)?"PASS":"FAIL")<<"\n";
    out << "S -> a and A -> a have identical right-hand sides.\n";
    out << "Precedence relations derived from adjacent RHS symbols and FIRST:\n";
    out << "a = S, a = A; a < a, a < b, a < c.\n";
    out << "Precedence table (rows/columns are grammar symbols):\n";
    out << "      S    A    a    b    c\n";
    out << " S    -    -    -    -    -\n";
    out << " A    -    -    -    -    -\n";
    out << " a    =    =    <    <    <\n";
    out << " b    -    -    -    -    -\n";
    out << " c    -    -    -    -    -\n";
    out << "Two different parse trees for aa: S=>aS=>aa and S=>aA=>aa.\n";
    out << "Conclusion: NOT a simple-precedence grammar.\n";
    out << "Part 12(b) is conditional on a positive result; it does not apply.\n";
}
}
void runTask(int n,std::ostream& out,std::size_t limit,const std::filesystem::path& dotDir) {
    if(n<1||n>12)throw std::invalid_argument("Task number must be from 1 to 12");
    if(limit<1||limit>5)throw std::invalid_argument("Limit must be 1..5");
    out << "\n========== VARIANT 17 / TASK "<<n<<" ==========\n";
    switch(n) {
        case 1:task01(out,limit);break;
        case 2:task02(out,limit);break;
        case 3:task03(out,limit);break;
        case 4:task04(out,limit);break;
        case 5:task05(out,dotDir);break;
        case 6:task06(out,dotDir);break;
        case 7:task07(out,dotDir);break;
        case 8:task08(out,dotDir);break;
        case 9:task09(out);break;
        case 10:task10(out,dotDir);break;
        case 11:task11(out,dotDir);break;
        case 12:task12(out);break;
    }
}
}
