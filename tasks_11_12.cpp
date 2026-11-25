#include "tasks.h"
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
using namespace std;

// Правило грамматики, например S -> aAa.
struct Rule {
    char left;
    string right;
};

// История вывода: цепочки, применённые правила и позиции замен.
struct Derivation {
    vector<string> words = {"S"};
    vector<int> positions;
    vector<int> rules;
};

bool isNonterminal(char c, const set<char>& nonterminals) {
    return nonterminals.count(c) > 0;
}

int terminalCount(const string& word, const set<char>& nonterminals) {
    int result = 0;
    for (char c : word)
        if (!isNonterminal(c, nonterminals)) result++;
    return result;
}

// Перебираем возможные выводы указанной длины.
// Ограничение числа шагов защищает от бесконечных циклов.
vector<Derivation> derivationsOfLength(
    const vector<Rule>& grammar, char start, int targetLength, int maxSteps = 22
) {
    set<char> nonterminals;
    for (const Rule& r : grammar) nonterminals.insert(r.left);
    Derivation initial;
    initial.words = {string(1, start)};
    vector<Derivation> stack = {initial};
    vector<Derivation> found;
    int explored = 0;

    while (!stack.empty()) {
        Derivation current = stack.back();
        stack.pop_back();
        if (++explored > 100000) {
            cout << "Поиск остановлен: слишком много вариантов.\n";
            return {};
        }
        string word = current.words.back();
        if (terminalCount(word, nonterminals) > targetLength) continue;
        if ((int)current.rules.size() > maxSteps) continue;
        if ((int)word.size() > targetLength + 4) continue;

        bool hasNonterminal = false;
        for (char c : word)
            if (isNonterminal(c, nonterminals)) hasNonterminal = true;
        if (!hasNonterminal) {
            if ((int)word.size() == targetLength) found.push_back(current);
            continue;
        }
        if ((int)current.rules.size() == maxSteps) continue;

        // Пробуем каждое место замены и каждое допустимое правило.
        for (int position = 0; position < (int)word.size(); position++) {
            if (!isNonterminal(word[position], nonterminals)) continue;
            for (int i = 0; i < (int)grammar.size(); i++) {
                if (grammar[i].left != word[position]) continue;
                Derivation next = current;
                string changed = word;
                changed.replace(position, 1, grammar[i].right);
                next.words.push_back(changed);
                next.positions.push_back(position);
                next.rules.push_back(i);
                stack.push_back(next);
            }
        }
    }
    return found;
}

vector<Derivation> shortestDerivations(
    const vector<Rule>& grammar, char start,
    int maxWordLength = 12, int maxSteps = 22
) {
    for (int length = 0; length <= maxWordLength; length++) {
        auto found = derivationsOfLength(grammar, start, length, maxSteps);
        if (!found.empty()) return found;
    }
    return {};
}

// Дерево строится из истории замен, а не из заранее записанного текста.
struct TreeNode {
    string symbol;
    vector<int> children;
};

vector<TreeNode> buildTree(const Derivation& derivation, const vector<Rule>& grammar) {
    vector<TreeNode> tree = {{derivation.words.front(), {}}};
    vector<int> leaves = {0};

    for (int step = 0; step < (int)derivation.rules.size(); step++) {
        int position = derivation.positions[step];
        int parent = leaves[position];
        const string& right = grammar[derivation.rules[step]].right;
        vector<int> newLeaves;
        if (right.empty()) {
            int child = tree.size();
            tree.push_back({"lambda", {}});
            tree[parent].children.push_back(child);
        } else {
            for (char c : right) {
                int child = tree.size();
                tree.push_back({string(1, c), {}});
                tree[parent].children.push_back(child);
                newLeaves.push_back(child);
            }
        }
        leaves.erase(leaves.begin() + position);
        leaves.insert(leaves.begin() + position, newLeaves.begin(), newLeaves.end());
    }
    return tree;
}

void printTree(const vector<TreeNode>& tree, int node = 0, int level = 0) {
    for (int i = 0; i < level; i++) cout << "    ";
    cout << tree[node].symbol << "\n";
    for (int child : tree[node].children)
        printTree(tree, child, level + 1);
}

// Одинаковое дерево может получиться при разном порядке раскрытия.
string treeSignature(const vector<TreeNode>& tree, int node = 0) {
    string result = tree[node].symbol + "(";
    for (int child : tree[node].children)
        result += treeSignature(tree, child);
    return result + ")";
}

void printDerivation(const Derivation& d) {
    for (int i = 0; i < (int)d.words.size(); i++) {
        if (i) cout << " => ";
        cout << (d.words[i].empty() ? "lambda" : d.words[i]);
    }
    cout << "\nРазмеченный вывод: (";
    for (int i = 0; i < (int)d.words.size(); i++) {
        if (i) cout << ", ";
        string word = d.words[i];
        if (i < (int)d.positions.size()) {
            int position = d.positions[i];
            word.insert(position + 1, "*");
            word.insert(position, "*");
        }
        cout << (word.empty() ? "lambda" : word);
    }
    cout << ")\n";
    cout << "Растянутое дерево (уровни замен):\n";
    for (const string& word : d.words)
        cout << (word.empty() ? "lambda" : word) << "\n";
}

void task11() {
    // Заданы ПРАВИЛА грамматики задания 4, но не ответ.
    vector<Rule> grammar = {
        {'S', "aSa"}, {'S', "aAa"},
        {'A', "bAb"}, {'A', "bBb"},
        {'B', "cC"}, {'C', "cC"}, {'C', ""}
    };
    cout << "Задание 11: ищем кратчайшие цепочки по правилам.\n";
    auto found = shortestDerivations(grammar, 'S');
    if (found.empty()) {
        cout << "В пределах поиска цепочки не найдены.\n";
        return;
    }
    int minLength = found[0].words.back().size();
    cout << "Минимальная найденная длина: " << minLength << "\n";
    cout << "Количество выводов: " << found.size() << "\n";

    map<string, set<string>> treesForWord;
    for (int i = 0; i < (int)found.size(); i++) {
        cout << "\nВывод №" << i + 1 << ":\n";
        printDerivation(found[i]);
        auto tree = buildTree(found[i], grammar);
        cout << "Дерево вывода:\n";
        printTree(tree);
        treesForWord[found[i].words.back()].insert(treeSignature(tree));
    }

    // Дополнительная ограниченная проверка неоднозначности до длины 9.
    for (int length = minLength + 1; length <= 9; length++) {
        auto longer = derivationsOfLength(grammar, 'S', length);
        for (const Derivation& d : longer) {
            auto tree = buildTree(d, grammar);
            treesForWord[d.words.back()].insert(treeSignature(tree));
        }
    }

    bool ambiguityFound = false;
    cout << "Проверка деревьев для слов длиной до 9:\n";
    for (const auto& entry : treesForWord) {
        cout << "  '" << entry.first << "': "
             << entry.second.size() << " деревьев\n";
        if (entry.second.size() > 1) ambiguityFound = true;
    }
    if (ambiguityFound)
        cout << "Обнаружена неоднозначность: два дерева для одного слова.\n";
    else
        cout << "Неоднозначности в пределах проверки нет. "
                "Это НЕ доказательство однозначности всего языка.\n";
}

// FIRST(X) и LAST(X) вычисляем итеративно по правилам.
map<char, set<char>> calculateEnds(const vector<Rule>& grammar, bool first) {
    set<char> nonterminals;
    for (const Rule& r : grammar) nonterminals.insert(r.left);
    map<char, set<char>> result;
    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule& r : grammar) {
            if (r.right.empty()) continue;
            char edge = first ? r.right.front() : r.right.back();
            int before = result[r.left].size();
            result[r.left].insert(edge);
            if (nonterminals.count(edge)) {
                const set<char>& addition = result[edge];
                result[r.left].insert(addition.begin(), addition.end());
            }
            if ((int)result[r.left].size() != before) changed = true;
        }
    }
    return result;
}

void task12() {
    // Исходные правила задания 10, не готовый ответ проверки.
    vector<Rule> grammar = {
        {'S', "a"}, {'S', "aS"}, {'S', "aA"},
        {'A', "a"}, {'A', "b"}, {'A', "c"}
    };
    set<char> nonterminals, symbols;
    for (const Rule& r : grammar) {
        nonterminals.insert(r.left);
        symbols.insert(r.left);
        for (char c : r.right) symbols.insert(c);
    }
    cout << "Задание 12: проверяем простое предшествование.\n";
    for (const Rule& r : grammar)
        cout << r.left << " -> " << (r.right.empty() ? "lambda" : r.right) << "\n";

    bool reversible = true;
    for (int i = 0; i < (int)grammar.size(); i++) {
        for (int j = i + 1; j < (int)grammar.size(); j++) {
            if (grammar[i].right == grammar[j].right &&
                grammar[i].left != grammar[j].left) {
                cout << "Одинаковая правая часть: " << grammar[i].left
                     << " -> " << grammar[i].right << " и " << grammar[j].left
                     << " -> " << grammar[j].right << "\n";
                reversible = false;
            }
        }
    }

    auto first = calculateEnds(grammar, true);
    auto last = calculateEnds(grammar, false);
    // '=' — соседство, '<' — слева, '>' — справа.
    map<pair<char, char>, set<char>> relations;
    for (const Rule& r : grammar) {
        const string& s = r.right;
        for (int i = 0; i + 1 < (int)s.size(); i++) {
            char x = s[i], y = s[i + 1];
            relations[{x, y}].insert('=');
            if (nonterminals.count(y))
                for (char z : first[y]) relations[{x, z}].insert('<');
            if (nonterminals.count(x))
                for (char z : last[x]) relations[{z, y}].insert('>');
        }
    }

    cout << "Таблица отношений (прочерк = отношения нет):\n    ";
    for (char c : symbols) cout << c << "   ";
    cout << "\n";
    bool hasConflict = false;
    for (char x : symbols) {
        cout << x << " | ";
        for (char y : symbols) {
            const set<char>& rel = relations[{x, y}];
            if (rel.empty()) cout << "-";
            else {
                for (char r : rel) cout << r;
                if (rel.size() > 1) hasConflict = true;
            }
            cout << "   ";
        }
        cout << "\n";
    }

    cout << "Обратимость: " << (reversible ? "да" : "нет") << "\n";
    cout << "Конфликты отношений: " << (hasConflict ? "есть" : "нет") << "\n";
    bool simplePrecedence = reversible && !hasConflict;
    cout << "Грамматика " << (simplePrecedence ? "является" : "не является")
         << " грамматикой простого предшествования.\n";
    if (!simplePrecedence)
        cout << "Пункт 12(б) по условию не выполняется.\n";
    else
        cout << "При положительном результате нужен анализ снизу вверх.\n";
}
