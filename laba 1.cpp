#include <iostream>
#include <thread>

using namespace std;

void hello()
{
    cout << "\nFirst line of the second thread\n" << "Second line of the second thread\n" << "Third line of the second thread\n" << "Fourth line of the second thread\n" << "Fifth line of the second thread\n" << "Sixth line of the second thread\n" << "Seventh line of the second thread\n" << "Eighth line of the second thread\n" << "Ninth line of the second thread\n" << "Tenth line of the second thread\n";

}

int main()
{
    cout << "First line of the first thread\n" << "Second line of the first thread\n" << "Third line of the first thread\n" << "Fourth line of the first thread\n" << "Fifth line of the first thread\n" << "Sixth line of the first thread\n" << "Seventh line of the first thread\n" << "Eighth line of the first thread\n" << "Ninth line of the first thread\n" << "Tenth line of the first thread\n";
    //создание потока
    thread t(hello);
    t.join();
    return 0;
    
}
