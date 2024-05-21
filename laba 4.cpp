#include <pthread.h>
#include <iostream>
#include <thread>

using namespace std;

//определение функции для дочернего потока
void* ChildThread(void* args)
{
    for (int i = 0; i < 5; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << "Text from child tread №" << i + 1 << endl;
    }
    return nullptr;
}


int main()
{
    setlocale(LC_ALL, "Russian");

    //создание переменной для идентификации дочернего потока
    pthread_t mythread;

    //Создаем поток, если поток не получилось создать - вывод ошибки
    if (pthread_create(&mythread, nullptr, ChildThread, nullptr) != 0)
    {
        cerr << "Error creating thread \n";
        return 1;
    }

    //Родительский поток ждет 2 секунды и после прерываем дочерний поток
    this_thread::sleep_for(chrono::seconds(2));

    //если поток не получилось закрыть - вывод ошибки
    if (pthread_cancel(mythread) != 0)
    {
        cerr << "Error canceling thread \n";
        return 2;
    }

    cout << "END" << endl;

}
