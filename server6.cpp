#include <iostream>
#include <thread>
#include <vector>
#include <WinSock2.h>
#include <Ws2tcpip.h>

//////////////////////////////////////////////curl.exe -x http://localhost:8888 http://www.example.com


#pragma comment(lib, "ws2_32.lib")
using namespace std;


void handleClient(SOCKET clientSocket) {
    
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        ::cerr << "Ошибка при приеме данных: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        return;
    }

    
    ::string httpRequest(buffer, bytesReceived);

    
    SOCKET remoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in remoteAddr;
    remoteAddr.sin_family = AF_INET;
    if (InetPton(AF_INET, TEXT("93.184.216.34"), &remoteAddr.sin_addr) != 1) {
        ::cerr << "Неверный адрес\n";
        closesocket(clientSocket);
        return;
    }
    remoteAddr.sin_port = htons(80); 
    if (connect(remoteSocket, reinterpret_cast<const sockaddr*>(&remoteAddr), sizeof(remoteAddr)) == SOCKET_ERROR) {
        ::cerr << "Не удалось подключиться к удаленному серверу: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        closesocket(remoteSocket);
        return;
    }
    send(remoteSocket, httpRequest.c_str(), httpRequest.length(), 0);

    
    char remoteBuffer[4096];
    int remoteBytesReceived = recv(remoteSocket, remoteBuffer, sizeof(remoteBuffer), 0);
    if (remoteBytesReceived == SOCKET_ERROR) {
        ::cerr << "Ошибка при приеме данных от удаленного сервера: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        closesocket(remoteSocket);
        return;
    }

    
    send(clientSocket, remoteBuffer, remoteBytesReceived, 0);

    
    closesocket(remoteSocket);
    closesocket(clientSocket);
}

int main() {
    setlocale(LC_ALL, "RU");
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ::cerr << "Ошибка при инициализации Winsock\n";
        return 1;
    }

    
    SOCKET proxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (proxySocket == INVALID_SOCKET) {
        ::cerr << "Ошибка при создании сокета: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    
    sockaddr_in proxyAddr;
    proxyAddr.sin_family = AF_INET;
    proxyAddr.sin_addr.s_addr = INADDR_ANY;
    proxyAddr.sin_port = htons(8888); 
    if (bind(proxySocket, reinterpret_cast<const sockaddr*>(&proxyAddr), sizeof(proxyAddr)) == SOCKET_ERROR) {
        ::cerr << "Ошибка при привязке сокета: " << WSAGetLastError() << "\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }

    
    if (listen(proxySocket, SOMAXCONN) == SOCKET_ERROR) {
        ::cerr << "Ошибка при ожидании входящих соединений: " << WSAGetLastError() << "\n";
        closesocket(proxySocket);
        WSACleanup();
        return 1;
    }


    ::cout << "Прокси-сервер запущен...\n";

    
    ::vector<::thread> threads;

    
    while (true) {
        SOCKET clientSocket = accept(proxySocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            ::cerr << "Ошибка при принятии соединения: " << WSAGetLastError() << "\n";
            closesocket(proxySocket);
            WSACleanup();
            return 1;
        }

        
        ::cout << "Создание нового потока для обработки запроса клиента...\n";

        
        threads.emplace_back(handleClient, clientSocket);
    }

    
    for (auto& thread : threads) {
        thread.join();
    }

    
    closesocket(proxySocket);
    WSACleanup();

    return 0;
}
