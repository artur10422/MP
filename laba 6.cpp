#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <mutex>

using namespace std;
mutex mtx;

// Функция для сортировки строк по длине
bool compareLength(const string& a, const string& b) {
    return a.length() < b.length();
}

void output(const string& str)
{
    //Использование mutex для избавления от "спаек" строк путем блокирование потока
    mtx.lock();

    // Выводим строку в стандартный поток вывода
    cout << str << endl;

    mtx.unlock();
}

// Функция для сортировки строки по длине с помощью sleepsort
void sleepSort(const string& str) {
    // Вычисляем время ожидания пропорционально длине строки
    this_thread::sleep_for(chrono::milliseconds(str.length() * 100)); // Предполагаем, что 1 символ == 100 миллисекундам

    //Вывод строк с помощью вызова метода output
    output(str);
}

int main() {

    setlocale(LC_ALL, "RU");
    
    // Имя файла для чтения
    string filename = "text.txt";

    // Открываем файл для чтения
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return 1;
    }

    // Вектор для хранения считанных строк
    vector<string> strings;

    // Считываем строки из файла
    string line;
    while (getline(file, line)) {
        strings.push_back(line);
    }

    // Закрываем файл
    file.close();

    // Сортируем строки по длине
    sort(strings.begin(), strings.end(), compareLength);

    // Создаем поток для каждой строки
    vector<thread> threads;
    for (const string& str : strings) {
        threads.emplace_back(sleepSort, str);
    }

    // Ждем завершения всех потоков
    for (thread& t : threads) {
        t.join();
    }

    return 0;
}