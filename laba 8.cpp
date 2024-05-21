#include <iostream>
#include <pthread.h>
#include <vector>


const int num_threads = 2; // Указываем количество потоков

const int num_iterations = 1000000;//Указываем кол-во итераций

//информация о диапазоне итераций для каждого потока, а также переменная для хранения частичной суммы
struct ThreadData {
    int start;
    int end;
    double sum;
};

//Вычисление частичной суммы ряда Лейбница для заданного диапазона итераций
void* calculate_partial_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->sum = 0.0;
    for (int i = data->start; i < data->end; ++i) {
        double term = 1.0 / (2 * i + 1);
        if (i % 2 == 0)
            data->sum += term;
        else
            data->sum -= term;
    }
    return nullptr;
}

int main() {
    setlocale(LC_ALL, "RU");
    //Создаем два потока, первый для идентификаторов, второй для данных по каждому потоку
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadData> thread_data(num_threads);

    //Количество итераций для каждого потока
    int iterations_per_thread = num_iterations / num_threads;

    //Цикл, устанавливающий начальное и конечное значение итераций в структуре ThreadData
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * iterations_per_thread;
        thread_data[i].end = (i + 1) * iterations_per_thread;
        //Создание потока с помощью calculate_partial_sum
        pthread_create(&threads[i], NULL, calculate_partial_sum, (void*)&thread_data[i]);
    }

    double pi = 0.0;

    //Создаем pthread_join для каждого потока, чтобы дождаться их завершения
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        //Суммируем частичные суммы, вычисленные каждым потоком, в переменную pi
        pi += thread_data[i].sum;
    }
    //Оценка значения π умножается на 4, так как используется алгоритм Лейбница для вычисления четверти значения π
    pi *= 4;

    std::cout << "Расчетное значение pi: " << pi << std::endl;

    return 0;
}