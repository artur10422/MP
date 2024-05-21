#include <iostream>
#include <vector>
#include <pthread.h>
#include <windows.h>

const int num_threads = 2; 
const int num_iterations = 1000000; 

volatile bool flag = false; 


struct ThreadData {
    int start;
    int end;
    double sum;
};



HANDLE threads[num_threads];
ThreadData thread_data[num_threads];


DWORD WINAPI calculate_partial_sum(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;
    data->sum = 0.0;
    for (int i = data->start; i < data->end && !flag; ++i) {
        double term = 1.0 / (2 * i + 1);
        if (i % 2 == 0)
            data->sum += term;
        else
            data->sum -= term;
    }
    return 0;
}

BOOL WINAPI ConsoleHandler(DWORD ctrl_type) {
    if (ctrl_type == CTRL_C_EVENT) {
        flag = true;
        
        WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);
        
        double pi = 0.0;
        for (int i = 0; i < num_threads; ++i) {
            pi += thread_data[i].sum;
        }
        pi *= 4;
        std::cout << "Расчетное значение pi: " << pi << std::endl;
        
        exit(0); 
        return TRUE;
    }
    return FALSE;
}

int main() {
    setlocale(LC_ALL, "RU");
    int iterations_per_thread = num_iterations / num_threads;
    
    
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "Ошибка настройки консольного обработчика." << std::endl;
        return 1;
    }

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * iterations_per_thread;
        thread_data[i].end = (i + 1) * iterations_per_thread;
        threads[i] = CreateThread(NULL, 0, calculate_partial_sum, &thread_data[i], 0, NULL);
    }

    
    while (!flag) {
        Sleep(3000); 
    }

    return 0;
}