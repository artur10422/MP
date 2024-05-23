#include <iostream>
#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#define BUFFER_SIZE 1024
using namespace std;
/////////////////////////////////////UML адрес в проекте


::mutex mtx;
::condition_variable cv;
bool ready = false;


size_t writeCallback(void* ptr, size_t size, size_t nmemb, ::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}


void networkThread(const ::string& url, ::string& response) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        ::cerr << "Не удалось инициализировать CURL" << ::endl;
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        ::cerr << "Не удалось выполнить HTTP-запрос: " << curl_easy_strerror(res) << ::endl;
    }

    curl_easy_cleanup(curl);

    {
        ::lock_guard<::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one();
}


void userInteractionThread(::string& response) {
    {
        ::unique_lock<::mutex> lock(mtx);
        cv.wait(lock, [] { return ready; });
    }

    // Вывод тела ответа
    ::cout << response << ::endl;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        ::cerr << "Usage: " << argv[0] << " <URL>" << ::endl;
        return 1;
    }

    ::string url = argv[1];
    ::string response;

    ::thread network(networkThread, url, ::ref(response));
    ::thread interaction(userInteractionThread, ::ref(response));

    network.join();
    interaction.join();

    return 0;
}
