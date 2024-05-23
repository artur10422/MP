#include <iostream>
#include <curl/curl.h>


#define BUFFER_SIZE 1024
using namespace std;




size_t writeCallback(void* ptr, size_t size, size_t nmemb, ::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        ::cerr << "Использование: " << argv[0] << " <URL>" << ::endl;
        return 1;
    }

    
    ::string url = argv[1];

    
    CURL* curl = curl_easy_init();
    if (!curl) {
        ::cerr << "Не удалось инициализировать CURL" << ::endl;
        return 1;
    }

    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    ::string response;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        ::cerr << "Не удалось выполнить HTTP-запрос: " << curl_easy_strerror(res) << ::endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_easy_cleanup(curl);

    // Вывод тела ответа
    ::cout << response << ::endl;

    return 0;
}
