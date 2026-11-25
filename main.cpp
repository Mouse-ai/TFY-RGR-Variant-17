#include <iostream>
#include "tasks.h"
using namespace std;

int main(int argc, char* argv[]) {
    int number;

    // Можно передать номер: ./rgr 8
    // Или просто запустить программу и ввести номер вручную.
    if (argc > 1) {
        try {
            number = stoi(argv[1]);
        } catch (...) {
            cout << "Номер задания должен быть числом от 1 до 12.\n";
            return 1;
        }
    } else {
        cout << "Вариант 17. Введите номер задания (1-12): ";
        if (!(cin >> number)) return 1;
    }

    switch (number) {
        case 1: task1(); break;
        case 2: task2(); break;
        case 3: task3(); break;
        case 4: task4(); break;
        case 5: task5(); break;
        case 6: task6(); break;
        case 7: task7(); break;
        case 8: task8(); break;
        case 9: task9(); break;
        case 10: task10(); break;
        case 11: task11(); break;
        case 12: task12(); break;
        default: cout << "Такого задания нет.\n"; return 1;
    }
}
