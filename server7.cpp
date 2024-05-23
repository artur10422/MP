#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <string>

#pragma comment(lib, "ws2_32.lib")
//////////////////////////curl.exe -x http://localhost:8888 http://www.example.com


constexpr int MAX_THREADS = 10;

std::mutex mutex;
std::condition_variable condition;
std::vector<std::thread> threads;
std::atomic<bool> stop(false);


void handleClient(SOCKET clientSocket) {
    
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Ошибка при приеме данных: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        return;
    }

    
    std::string httpRequest(buffer, bytesReceived);

    
    SOCKET remoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in remoteAddr;
    remoteAddr.sin_family = AF_INET;
    if (InetPton(AF_INET, TEXT("93.184.216.34"), &remoteAddr.sin_addr) != 1) {
        std::cerr << "Неверный адрес\n";
        closesocket(clientSocket);
        return;
    }
    remoteAddr.sin_port = htons(80); 
    if (connect(remoteSocket, reinterpret_cast<const sockaddr*>(&remoteAddr), sizeof(remoteAddr)) == SOCKET_ERROR) {
        std::cerr << "Не удалось подключиться к удаленному серверу: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        closesocket(remoteSocket);
        return;
    }
    send(remoteSocket, httpRequest.c_str(), httpRequest.length(), 0);


    char remoteBuffer[4096];
    int remoteBytesReceived = recv(remoteSocket, remoteBuffer, sizeof(remoteBuffer), 0);
    if (remoteBytesReceived == SOCKET_ERROR) {
        std::cerr << "Ошибка при приеме данных от удаленного сервера: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        closesocket(remoteSocket);
        return;
    }

    
    send(clientSocket, remoteBuffer, remoteBytesReceived, 0);

   
    closesocket(remoteSocket);
    closesocket(clientSocket);
}

void worker(SOCKET proxySocket) {
    fd_set readSet;
    int maxFd = proxySocket + 1;

    while (!stop) {
        FD_ZERO(&readSet);
        FD_SET(proxySocket, &readSet);

        if (select(maxFd, &readSet, nullptr, nullptr, nullptr) == SOCKET_ERROR) {
            std::cerr << "Ошибка выбора\n";
            continue;
        }

        if (FD_ISSET(proxySocket, &readSet)) {
            SOCKET clientSocket = accept(proxySocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Ошибка при приеме: " << WSAGetLastError() << "\n";
                continue;
            }

            handleClient(clientSocket);
        }
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RU");
    
    if (argc != 2) {
        std::cerr << "Использование: " << argv[0] << " <thread_pool_size>\n";
        return 1;
    }

    
    int poolSize = std::stoi(argv[1]);
    if (poolSize <= 0 || poolSize > MAX_THREADS) {
        std::cerr << "Неверный размер пула потоков\n";
        return 1;
    }

    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка запуска WSA\n";
        return 1;
    }

    
    SOCKET proxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (proxySocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета:  " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    
    sockaddr_in proxyAddr;
    proxyAddr.sin_family = AF_INET;
    proxyAddr.sin_addr.s_addr = INADDR_ANY;
    proxyAddr.sin_port = htons(8888); // Прокси слушает на порту 8888
    if (bind(proxySocket, reinterpret_cast<const sockaddr*>(&proxyAddr), sizeof(proxyAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки: " << WSAGetLastError() << "\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    
    if (listen(proxySocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Ошибка прослушивания: " << WSAGetLastError() << "\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Прокси-сервер запущен...\n";

    
    for (int i = 0; i < poolSize; ++i) {
        threads.emplace_back(worker, proxySocket);
    }

    
    std::cin.get(); 

    
    stop = true;
    for (auto& thread : threads) {
        thread.join();
    }

    
    closesocket(proxySocket);
    WSACleanup();

    return 0;
}
