#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>

using namespace std;

mutex my_mutex;

void printSequence(const vector<string>& sequence)
{

    lock_guard<mutex> lock(my_mutex);

    for (const auto& line : sequence)
    {
        cout << line << endl;
    }
}

int main()
{
    // Определение последовательностей строк для каждого потока
    vector<string> sequence1 = { "Thread 1 - Line 1 \n", "Thread 1 - Line 2\n", "Thread 1 - Line 3\n" };
    vector<string> sequence2 = { "Thread 2 - Line 1\n", "Thread 2 - Line 2\n", "Thread 2 - Line 3\n" };
    vector<string> sequence3 = { "Thread 3 - Line 1\n", "Thread 3 - Line 2\n", "Thread 3 - Line 3\n" };
    vector<string> sequence4 = { "Thread 4 - Line 1\n", "Thread 4 - Line 2\n", "Thread 4 - Line 3\n" };

    // Создание четырех потоков
    thread t1(printSequence, sequence1);
    thread t2(printSequence, sequence2);
    thread t3(printSequence, sequence3);
    thread t4(printSequence, sequence4);

    // Ожидание завершения каждого потока
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}