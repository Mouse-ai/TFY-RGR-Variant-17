#include "tfy/automata.hpp"
#include "tfy/grammar.hpp"
#include "tfy/language.hpp"
#include "tfy/tasks.hpp"
#ifdef NDEBUG
#undef NDEBUG // Keep test assertions active even in Release builds.
#endif
#include <cassert>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace tfy;
static void everyWord(const std::string& abc,int maxLen,const std::function<void(const std::string&)>& check) {
    check("");
    std::function<void(std::string,int)> dfs=[&](std::string prefix,int left){
        if(!left)return;
        for(char ch:abc) {
            auto s=prefix+ch;check(s);dfs(s,left-1);
        }
    };
    dfs("",maxLen);
}
int main() {
    const Language a={"ab","ac"},b={"ac","ab","acb"};
    assert(langUnion(a,b)==(Language{"ab","ac","acb"}));
    assert(langIntersection(a,b)==a);
    assert(langDifference(a,b).empty());
    assert(langDifference(b,a)==(Language{"acb"}));
    assert(concatenate(a,b).count("abacb"));
    assert(concatenate(b,a).count("acbac"));
    assert(complementBounded(a,"abc",2).count("aa"));
    assert(!complementBounded(a,"abc",2).count("ab"));
    assert(iterateBounded({"11"},3,false)==(Language{"11","1111","111111"}));
    assert(iterateBounded({"11"},2,true).count(""));
    assert(contextFree(makeTask3Grammar()));
    assert(!rightLinear(makeTask3Grammar()));
    assert(rightLinear(makeTask10Grammar()));
    assert(!reversible(makeTask10Grammar()));
    for(std::size_t n=0;n<10;++n){
        auto steps=task3Derivation(n);
        assert(steps.back()==std::string(2*n+3,'a'));
        assert(belongsTask3(steps.back()));
    }
    for(std::size_t k=1;k<=3;++k)for(std::size_t l=1;l<=3;++l)
        for(std::size_t m=1;m<=3;++m) {
            auto steps=task4Derivation(k,l,m);
            auto word=std::string(k,'a')+std::string(l,'b')+std::string(m,'c')+
                      std::string(l,'b')+std::string(k,'a');
            assert(steps.back()==word);
            assert(belongsTask4(word));
        }
    assert(!belongsTask4("abbcbbaaa"));
    auto d5=makeTask5DFA();auto n6=makeTask6NFA();
    auto n7=embedAsNFA(d5,true);auto d8=determinize(n6);
    auto g9=grammarFromDFA(d5);auto n9=nfaFromRightLinearGrammar(g9);
    auto n10=nfaFromRightLinearGrammar(makeTask10Grammar());
    assert(d5.states.size()==5&&d5.finals.size()==2);
    assert(n6.states.size()==3&&n6.finals.size()==1);
    assert(n7.states.size()==6);
    assert(d8.states.size()==5&&d8.finals.size()==2);
    assert(rightLinear(g9));
    everyWord("abc",8,[&](const std::string& w){
        auto expected=belongsTask5(w);
        assert(d5.accepts(w)==expected);
        assert(n7.accepts(w)==expected);
        assert(n9.accepts(w)==expected);
    });
    everyWord("0x",10,[&](const std::string& w){
        auto expected=belongsTask6(w);
        assert(n6.accepts(w)==expected);
        assert(d8.accepts(w)==expected);
    });
    everyWord("abc",7,[&](const std::string& w){
        const bool expected = !w.empty()&&w[0]=='a' &&
           (w.find_first_not_of('a')==std::string::npos ||
            (w.back()=='b'||w.back()=='c') &&
              w.find_first_not_of('a')==w.size()-1);
        assert(n10.accepts(w)==expected);
    });
    std::ostringstream out;
    for(int n=1;n<=12;++n)runTask(n,out,2);
    assert(out.str().find("VARIANT 17 / TASK 12")!=std::string::npos);
    assert(task11TreeDot(false).find("digraph")!=std::string::npos);
    assert(task11TreeDot(true).find("digraph")!=std::string::npos);
    std::cout << "All tests passed (including exhaustive words up to length 8/10).\n";
}
