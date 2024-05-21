#include <pthread.h>
#include <iostream>
#include <thread>


using namespace std;

//функция, выводящая сообщение о прерывании дочернего потока
void cleanup(void* arg)
{
    cout << "The child thread canceled" << endl;
}

//определение функции для дочернего потока
void* ChildThread(void* arg)
{
    

    while (1)
    {
        for (int i = 0; i < 6; i++)
        {
            this_thread::sleep_for(chrono::milliseconds(500));
            cout << "Text from child tread №" << i + 1 << endl;
        }
    }
    

    return nullptr;
}

void* pushClean(void* arg)
{
    

    //регистрируем функцию clean
    pthread_cleanup_push(cleanup, nullptr);

    //удаляем cleanup-функцию
    pthread_cleanup_pop(1);

    return nullptr;
}




int main()
{
    setlocale(LC_ALL, "Russian");

    //создание переменной для идентификации дочернего потока
    //(ChildThread);
    pthread_t mythread;
    //(pushClean)
    pthread_t push;

    int p = 0;
    

    //Создаем поток, если поток не получилось создать - вывод ошибки
    if (p = pthread_create(&mythread, nullptr, ChildThread, nullptr) != 0)
    {
        cerr << "Error creating thread ChildThread \n";
        return 1;
    }
    
    //Родительский поток ждет 2 секунды и после прерываем дочерний поток
    this_thread::sleep_for(chrono::milliseconds(2000));        
    
    if (p = pthread_cancel(mythread) != 0)
        {
            cerr << "Error cancel" << endl;
        }

    //Вывод сообщения о прерывании дочернего потока
    if (pthread_create(&push, nullptr, pushClean, nullptr) != 0)
    {
            cerr << "Error creating thread push \n"; 
            return 1;
    }
    
    if (pthread_join(push, NULL) != 0)
    {
        cerr << "Error joining thread 'push' \n";
        return 3;
    }
    
    //конец
    cout << "END" << endl;
   
    return 0;
}