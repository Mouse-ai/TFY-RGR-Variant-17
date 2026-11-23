#pragma once
#include <set>
#include <string>
#include <cstddef>

namespace tfy {
using Language = std::set<std::string>;
Language langUnion(const Language& a, const Language& b);
Language langIntersection(const Language& a, const Language& b);
Language langDifference(const Language& a, const Language& b);
Language concatenate(const Language& a, const Language& b);
Language power(const Language& a, std::size_t n);
Language allWords(const std::string& alphabet, std::size_t maxLength);
Language complementBounded(const Language& a, const std::string& alphabet, std::size_t maxLength);
Language iterateBounded(const Language& a, std::size_t maxPowers, bool includeEpsilon);
std::string showWord(const std::string& s);
std::string showLanguage(const Language& l);
}
