#include "tasks.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

void task11() {
    cout << "Задание 11. Используется грамматика из №4.\n";
    cout << "Длина слова: 2*k + 2*l + m. При k=l=m=1 длина 5.\n";
    cout << "Минимальное слово: abcba\n";
    cout << "Вывод Delta1:\n";
    cout << "S => aAa => abBba => abcCba => abcba\n";
    cout << "Размеченный вывод Delta1':\n";
    cout << "(*S*, a*A*a, ab*B*ba, abc*C*ba, abcba)\n";

    // Дерево именно для кратчайшего слова abcba.
    cout << "Обычное дерево вывода:\n";
    cout << R"(           S
         / | \
        a  A  a
          /|\
         b B b
          / \
         c   C
             |
           lambda
)";
    cout << "Растянутое дерево: уровни вывода\n";
    cout << "S\na A a\na b B b a\na b c C b a\na b c lambda b a\n";
    cout << "Грамматика однозначна: для каждого k,l,m набор правил единственный.\n";
}

void task12() {
    cout << "Задание 12. Грамматика из №10.\n";
    vector<pair<char,string>> rules = {
        {'S',"a"}, {'S',"aS"}, {'S',"aA"},
        {'A',"a"}, {'A',"b"}, {'A',"c"}
    };
    for (const auto& r : rules)
        cout << r.first << " -> " << r.second << "\n";

    // Для обратимости не должно быть разных левых частей
    // с одинаковой правой частью.
    bool reversible = true;
    for (int i = 0; i < (int)rules.size(); i++) {
        for (int j = i + 1; j < (int)rules.size(); j++) {
            if (rules[i].second == rules[j].second &&
                rules[i].first != rules[j].first) {
                cout << "Конфликт: " << rules[i].first << " -> " << rules[i].second
                     << " и " << rules[j].first << " -> " << rules[j].second << "\n";
                reversible = false;
            }
        }
    }

    cout << "Обратимость: " << (reversible ? "да" : "нет") << "\n";
    // Таблица отношений получена вручную для конкретной грамматики.
    cout << "Отношения: a=S, a=A; a<a, a<b, a<c.\n";
    cout << "Грамматика не является грамматикой простого предшествования.\n";
    cout << "Пункт 12б по условию выполняется только при положительной проверке.\n";
}
