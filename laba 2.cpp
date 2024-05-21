#include <iostream>
#include <pthread.h>

using namespace std;

// Дочерний поток hello
void* hello(void* args)
{
    cout << "\nFirst line of the second thread\n" << "Second line of the second thread\n" << "Third line of the second thread\n" << "Fourth line of the second thread\n" << "Fifth line of the second thread\n" << "Sixth line of the second thread\n" << "Seventh line of the second thread\n" << "Eighth line of the second thread\n" << "Ninth line of the second thread\n" << "Tenth line of the second thread \n\n";
    return nullptr;
}

// Родительский поток
int main()
{
    
    pthread_t t;

    int status;

    status = pthread_create(&t, nullptr, hello, nullptr);

    if (status != 0)
    {
        cerr << "error while create thread!" << endl;
        return 1;
    }

    if (pthread_join(t, NULL) != 0) {
        cerr << "error while waiting end of thread" << endl;
        return 2;
    }

    for (int i = 0; i < 10; i++)
    {
        cout << "Line of the first thread - " << i+1 << "\n" << endl;
    }
   
   


   
    return 0;
}