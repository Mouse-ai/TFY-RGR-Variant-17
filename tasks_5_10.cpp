#include "tasks.h"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

// ДКА: по паре (состояние, символ) получаем ОДНО состояние.
struct DFA {
    int count = 0;
    int start = 1;
    string alphabet;
    set<int> finals;
    map<pair<int, char>, int> moves;
};

// НКА: по той же паре можем получить НЕСКОЛЬКО состояний.
struct NFA {
    int count = 0;
    int start = 1;
    string alphabet;
    set<int> finals;
    map<pair<int, char>, set<int>> moves;
};

void printStates(const set<int>& states) {
    if (states.empty()) { cout << "∅"; return; }
    cout << "{";
    bool first = true;
    for (int k : states) {
        if (!first) cout << ",";
        cout << "k" << k;
        first = false;
    }
    cout << "}";
}

void printDFA(const DFA& d) {
    cout << "ДКА: K={k1..k" << d.count << "}, начальное k" << d.start << ", F=";
    printStates(d.finals);
    cout << "\nk | символ | t(k,символ)\n";
    for (int k = 1; k <= d.count; k++) {
        for (char c : d.alphabet) {
            cout << "k" << k << " | " << c << " | ";
            auto it = d.moves.find({k, c});
            if (it == d.moves.end()) cout << "-";
            else cout << "k" << it->second;
            cout << "\n";
        }
    }
}

void printNFA(const NFA& n) {
    cout << "НКА: K={k1..k" << n.count << "}, начальное k" << n.start << ", F=";
    printStates(n.finals);
    cout << "\nk | символ | t(k,символ)\n";
    for (int k = 1; k <= n.count; k++) {
        for (char c : n.alphabet) {
            cout << "k" << k << " | " << c << " | ";
            auto it = n.moves.find({k, c});
            if (it == n.moves.end()) cout << "-";
            else printStates(it->second);
            cout << "\n";
        }
    }
}

bool checkDFA(const DFA& d, const string& word) {
    int state = d.start;
    for (char c : word) {
        auto it = d.moves.find({state, c});
        if (it == d.moves.end()) return false;
        state = it->second;
    }
    return d.finals.count(state) > 0;
}

bool checkNFA(const NFA& n, const string& word) {
    set<int> current = {n.start};
    for (char c : word) {
        set<int> next;
        for (int state : current) {
            auto it = n.moves.find({state, c});
            if (it != n.moves.end())
                next.insert(it->second.begin(), it->second.end());
        }
        current = next;
    }
    for (int state : current)
        if (n.finals.count(state)) return true;
    return false;
}

// Задание 5: ab*ca* ИЛИ bc*ab* (частичный ДКА на 5 состояниях).
DFA makeDFA5() {
    DFA d;
    d.count = 5;
    d.alphabet = "abc";
    d.finals = {4, 5};
    d.moves[{1,'a'}] = 2;
    d.moves[{1,'b'}] = 3;
    d.moves[{2,'b'}] = 2;
    d.moves[{2,'c'}] = 4;
    d.moves[{3,'c'}] = 3;
    d.moves[{3,'a'}] = 5;
    d.moves[{4,'a'}] = 4;
    d.moves[{5,'b'}] = 5;
    return d;
}

void task5() {
    cout << "Задание 5: L = ab*ca* U bc*ab*\n";
    DFA d = makeDFA5();
    printDFA(d);
    cout << "abca: " << (checkDFA(d, "abca") ? "принято" : "отклонено") << "\n";
    cout << "acb: " << (checkDFA(d, "acb") ? "принято" : "отклонено") << "\n";
    cout << "При 5 состояниях ДКА частичный. Для полного нужна ловушка.\n";
}

// Задание 6: 0x* ИЛИ 00x* ИЛИ x0x* (НКА на 3 состояниях).
NFA makeNFA6() {
    NFA n;
    n.count = 3;
    n.alphabet = "0x";
    n.finals = {2};
    n.moves[{1,'0'}] = {2, 3};
    n.moves[{1,'x'}] = {3};
    n.moves[{2,'x'}] = {2};
    n.moves[{3,'0'}] = {2};
    return n;
}

void task6() {
    cout << "Задание 6: L = 0x* U 00x* U x0x*\n";
    NFA n = makeNFA6();
    printNFA(n);
    cout << "00x: " << (checkNFA(n, "00x") ? "принято" : "отклонено") << "\n";
    cout << "xxx: " << (checkNFA(n, "xxx") ? "принято" : "отклонено") << "\n";
}

// Задание 7: превращаем переходы ДКА в одноэлементные множества НКА.
NFA makeNFA7() {
    DFA d = makeDFA5();
    NFA n;
    n.count = 6;               // k6 — ловушка
    n.alphabet = d.alphabet;
    n.finals = d.finals;
    for (int state = 1; state <= 5; state++) {
        for (char c : d.alphabet) {
            auto it = d.moves.find({state, c});
            if (it == d.moves.end()) n.moves[{state,c}] = {6};
            else n.moves[{state,c}] = {it->second};
        }
    }
    for (char c : d.alphabet) n.moves[{6,c}] = {6};
    return n;
}

void task7() {
    cout << "Задание 7: ДКА из №5 -> эквивалентный НКА\n";
    cout << "Обычный переход k2 -> k4 становится множеством {k4}.\n";
    NFA n = makeNFA7();
    printNFA(n);
    cout << "k6 — ловушка для отсутствующих переходов.\n";
    cout << "abca: " << (checkNFA(n, "abca") ? "принято" : "отклонено") << "\n";
}

// Задание 8: каждое множество состояний НКА становится
// ОДНИМ состоянием ДКА. Обрабатываем найденные множества по очереди.
void task8() {
    cout << "Задание 8: НКА из №6 -> ДКА методом подмножеств\n";
    NFA n = makeNFA6();
    vector<set<int>> groups = {{n.start}}; // новое k1 = {старое k1}
    DFA d;
    d.alphabet = n.alphabet;

    for (int i = 0; i < (int)groups.size(); i++) {
        set<int> group = groups[i];
        for (int oldState : group)
            if (n.finals.count(oldState)) d.finals.insert(i + 1);

        for (char c : n.alphabet) {
            set<int> next;
            for (int oldState : group) {
                auto it = n.moves.find({oldState, c});
                if (it != n.moves.end())
                    next.insert(it->second.begin(), it->second.end());
            }

            int nextNumber = 0;
            for (int j = 0; j < (int)groups.size(); j++) {
                if (groups[j] == next) nextNumber = j + 1;
            }
            if (nextNumber == 0) {
                groups.push_back(next);
                nextNumber = groups.size();
            }
            d.moves[{i + 1, c}] = nextNumber;
        }
    }
    d.count = groups.size();

    cout << "Соответствие состояний:\n";
    for (int i = 0; i < (int)groups.size(); i++) {
        cout << "новое k" << i + 1 << " = ";
        printStates(groups[i]);
        cout << "\n";
    }
    cout << "Пустое множество — ловушка ДКА.\n";
    printDFA(d);
    cout << "x0x: " << (checkDFA(d, "x0x") ? "принято" : "отклонено") << "\n";
}

// Задание 9: переход k1 -a-> k2 превращаем в правило S -> aA.
void task9() {
    cout << "Задание 9: ДКА из №5 -> регулярная грамматика\n";
    DFA d = makeDFA5();
    char names[] = {'?', 'S', 'A', 'B', 'C', 'D'};
    cout << "k1=S, k2=A, k3=B, k4=C, k5=D\n";
    cout << "P:\n";
    for (int state = 1; state <= d.count; state++) {
        for (char c : d.alphabet) {
            auto it = d.moves.find({state,c});
            if (it != d.moves.end())
                cout << names[state] << " -> " << c << names[it->second] << "\n";
        }
        if (d.finals.count(state))
            cout << names[state] << " -> lambda\n";
    }
}

// Задание 10: для праволинейной грамматики
// X -> aY даёт переход X -a-> Y, X -> a ведёт в конечное k3.
void task10() {
    cout << "Задание 10: грамматика -> НКА\n";
    vector<pair<char,string>> rules = {
        {'S',"a"}, {'S',"aS"}, {'S',"aA"},
        {'A',"a"}, {'A',"b"}, {'A',"c"}
    };
    // Нумеруем нетерминалы из самих правил, не вручную.
    map<char, int> numbers;
    numbers['S'] = 1;
    for (const auto& rule : rules) {
        if (!numbers.count(rule.first))
            numbers[rule.first] = (int)numbers.size() + 1;
        if (rule.second.size() == 2) {
            char target = rule.second[1];
            if (!numbers.count(target))
                numbers[target] = (int)numbers.size() + 1;
        }
    }
    NFA n;
    n.alphabet = "abc";
    n.count = (int)numbers.size() + 1;
    n.finals = {n.count}; // Дополнительное конечное состояние.
    cout << "Состояния нетерминалов: ";
    for (const auto& entry : numbers)
        cout << entry.first << "=k" << entry.second << " ";
    cout << "конечное=k" << n.count << "\n";
    for (const auto& rule : rules) {
        int from = numbers[rule.first];
        char letter = rule.second[0];
        int to = n.count;
        if (rule.second.size() == 2)
            to = numbers[rule.second[1]];
        n.moves[{from, letter}].insert(to);
        cout << rule.first << " -> " << rule.second << "\n";
    }
    printNFA(n);
    cout << "Принятые цепочки длиной до 4 (вычислены автоматом):\n";
    vector<string> current = {""};
    for (int length = 0; length <= 4; length++) {
        vector<string> next;
        for (const string& word : current) {
            if (checkNFA(n, word)) cout << word << " ";
            for (char c : n.alphabet) next.push_back(word + c);
        }
        current = next;
    }
    cout << "\n";
}
