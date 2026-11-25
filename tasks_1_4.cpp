#include "tasks.h"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <utility>
using namespace std;

// Язык — множество строк. set автоматически убирает повторы.
void printLanguage(const string& name, const set<string>& words) {
    cout << name << " = {";
    bool first = true;
    for (const string& word : words) {
        if (!first) cout << ", ";
        cout << (word.empty() ? "lambda" : word);
        first = false;
    }
    cout << "}\n";
}

// Задание 1: объединение, пересечение, разность, дополнение,
// конкатенации двух языков.
void task1() {
    set<string> L1 = {"ab", "ac"};
    set<string> L2 = {"ac", "ab", "acb"};
    printLanguage("L1", L1);
    printLanguage("L2", L2);

    set<string> united = L1;
    for (const string& word : L2) united.insert(word);
    printLanguage("Объединение", united);

    set<string> common;
    for (const string& word : L1)
        if (L2.count(word)) common.insert(word);
    printLanguage("Пересечение", common);

    set<string> leftDifference, rightDifference;
    for (const string& word : L1)
        if (!L2.count(word)) leftDifference.insert(word);
    for (const string& word : L2)
        if (!L1.count(word)) rightDifference.insert(word);
    printLanguage("L1 - L2", leftDifference);
    printLanguage("L2 - L1", rightDifference);

    // Дополнение бесконечно. Выведем только слова длины 0..3.
    const string alphabet = "abc";
    const int maxLength = 3;
    set<string> all = {""};
    set<string> current = {""};
    for (int length = 1; length <= maxLength; length++) {
        set<string> next;
        for (const string& word : current)
            for (char letter : alphabet) next.insert(word + letter);
        all.insert(next.begin(), next.end());
        current = next;
    }
    set<string> complement1, complement2;
    for (const string& word : all) {
        if (!L1.count(word)) complement1.insert(word);
        if (!L2.count(word)) complement2.insert(word);
    }
    cout << "Дополнение L1 = {a,b,c}* - L1\n";
    printLanguage("Примеры (длина до 3)", complement1);
    cout << "Дополнение L2 = {a,b,c}* - L2\n";
    printLanguage("Примеры (длина до 3)", complement2);

    set<string> concat12, concat21;
    for (const string& x : L1)
        for (const string& y : L2) concat12.insert(x + y);
    for (const string& x : L2)
        for (const string& y : L1) concat21.insert(x + y);
    printLanguage("L1 * L2", concat12);
    printLanguage("L2 * L1", concat21);
}

// Задание 2: L = {11}, положительная итерация и звезда Клини.
void task2() {
    set<string> plus, star = {""}; // lambda входит только в звезду Клини
    string word;
    for (int n = 1; n <= 5; n++) {
        word += "11";
        plus.insert(word);
        star.insert(word);
    }
    printLanguage("Первые 5 элементов L+", plus);
    printLanguage("Первые 6 элементов L*", star);
    cout << "В общем виде: L+ = {1^(2n), n>=1}, L* = {1^(2n), n>=0}\n";
}

// Заменяем первый найденный нетерминал на правую часть продукции.
void replaceSymbol(string& word, char oldSymbol, const string& replacement) {
    size_t position = word.find(oldSymbol);
    if (position != string::npos) word.replace(position, 1, replacement);
}

// Проверяем форму правил, а не печатаем готовый тип.
// Для нашего варианта достаточно различать типы 2 и 3.
int grammarType(const vector<pair<char, string>>& rules) {
    bool contextFree = true;
    bool rightLinear = true;
    for (const auto& rule : rules) {
        char left = rule.first;
        const string& right = rule.second;
        if (left < 'A' || left > 'Z') contextFree = false;
        bool form1 = right.size() == 1 &&
                     right[0] >= 'a' && right[0] <= 'z';
        bool form2 = right.size() == 2 &&
                     right[0] >= 'a' && right[0] <= 'z' &&
                     right[1] >= 'A' && right[1] <= 'Z';
        if (!right.empty() && !form1 && !form2)
            rightLinear = false;
    }
    if (contextFree && rightLinear) return 3;
    if (contextFree) return 2;
    return 0; // Упрощённая проверка: типы 0 и 1 отдельно не различаем.
}

// Задание 3: S->aAa, A->aBa|a, B->A.
void task3() {
    cout << "Грамматика: S->aAa, A->aBa|a, B->A\n";
    vector<pair<char, string>> rules = {
        {'S', "aAa"}, {'A', "aBa"}, {'B', "A"}, {'A', "a"}
    };
    cout << "Вычисленный тип грамматики: " << grammarType(rules) << "\n";
    cout << "Сгенерированные по правилам цепочки (n=0..3):\n";
    for (int n = 0; n < 4; n++) {
        string word = "S";
        cout << "n=" << n << ": " << word;
        word = "aAa";
        cout << " => " << word;
        for (int i = 0; i < n; i++) {
            replaceSymbol(word, 'A', "aBa");
            cout << " => " << word;
            replaceSymbol(word, 'B', "A");
            cout << " => " << word;
        }
        replaceSymbol(word, 'A', "a");
        cout << " => " << word << "\n";
    }
}

// Задание 4: S->aSa|aAa, A->bAb|bBb, B->cC, C->cC|lambda.
void showTask4Derivation(int k, int l, int m) {
    string word = "S";
    cout << "k=" << k << ", l=" << l << ", m=" << m << ": " << word;
    for (int i = 1; i < k; i++) {
        replaceSymbol(word, 'S', "aSa"); cout << " => " << word;
    }
    replaceSymbol(word, 'S', "aAa"); cout << " => " << word;
    for (int i = 1; i < l; i++) {
        replaceSymbol(word, 'A', "bAb"); cout << " => " << word;
    }
    replaceSymbol(word, 'A', "bBb"); cout << " => " << word;
    replaceSymbol(word, 'B', "cC"); cout << " => " << word;
    for (int i = 1; i < m; i++) {
        replaceSymbol(word, 'C', "cC"); cout << " => " << word;
    }
    replaceSymbol(word, 'C', ""); cout << " => " << word << "\n";
}

void task4() {
    cout << "Грамматика для a^k b^l c^m b^l a^k, k,l,m>=1:\n";
    cout << "S -> aSa | aAa\nA -> bAb | bBb\n";
    cout << "B -> cC\nC -> cC | lambda\n";
    vector<pair<char, string>> rules = {
        {'S', "aSa"}, {'S', "aAa"}, {'A', "bAb"},
        {'A', "bBb"}, {'B', "cC"}, {'C', "cC"}, {'C', ""}
    };
    cout << "Вычисленный тип грамматики: " << grammarType(rules) << "\n";
    showTask4Derivation(1, 1, 1);
    showTask4Derivation(2, 2, 3);
}
