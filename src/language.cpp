#include "tfy/language.hpp"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace tfy {
Language langUnion(const Language& a, const Language& b) {
    Language res;
    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
}
Language langIntersection(const Language& a, const Language& b) {
    Language res;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
}
Language langDifference(const Language& a, const Language& b) {
    Language res;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
}
Language concatenate(const Language& a, const Language& b) {
    Language res;
    for (const auto& x : a) for (const auto& y : b) res.insert(x + y);
    return res;
}
Language power(const Language& a, std::size_t n) {
    Language res = {""};
    for (std::size_t i=0; i<n; ++i) res = concatenate(res, a);
    return res;
}
Language allWords(const std::string& alphabet, std::size_t maxLength) {
    if (maxLength > 8) throw std::invalid_argument("maxLength > 8: unsafe output size");
    Language res = {""};
    std::vector<std::string> level = {""};
    for (std::size_t len=1;len<=maxLength;++len) {
        std::vector<std::string> next;
        for (const auto& w : level) for (char ch : alphabet) {
            auto word = w + ch;
            res.insert(word);
            next.push_back(word);
        }
        level = std::move(next);
    }
    return res;
}
Language complementBounded(const Language& a, const std::string& alphabet, std::size_t maxLength) {
    return langDifference(allWords(alphabet,maxLength),a);
}
Language iterateBounded(const Language& a, std::size_t maxPowers, bool includeEpsilon) {
    Language res = includeEpsilon ? Language{""} : Language{};
    Language term = {""};
    for (std::size_t i=1;i<=maxPowers;++i) {
        term = concatenate(term,a);
        res = langUnion(res,term);
    }
    return res;
}
std::string showWord(const std::string& s) { return s.empty() ? "lambda" : s; }
std::string showLanguage(const Language& l) {
    std::ostringstream os;
    os << "{";
    bool first=true;
    for (const auto& w:l) {
        if (!first) os << ", ";
        first=false;
        os << showWord(w);
    }
    os << "}";
    return os.str();
}
}
