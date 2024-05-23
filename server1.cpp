#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <string>


using namespace std;

#define MAX_CLIENTS 510
#define BUFFER_SIZE 1024


string generateHttpResponse() {
    string responseBody = "Server received your request";
    
    string contentLength = to_string(responseBody.length());

    
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + contentLength + "\r\n\r\n" + responseBody;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RU");

    
    if (argc != 4) {
        ::cerr << "Ошибка: Неверное количество аргументов командной строки." << ::endl;
        ::cerr << "Использование: " << argv[0] << " <port> <server_address> <server_port>" << ::endl;
        return 1;
    }

    
    int port = atoi(argv[1]);
    const char* server_address = argv[2];
    int server_port = atoi(argv[3]);

    ::cout << "Порт: " << port << ::endl;
    ::cout << "Адрес сервера: " << server_address << ::endl;
    ::cout << "Порт сервера: " << server_port << ::endl;

    
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        ::cerr << "Ошибка: WSAStartup не удался" << ::endl;
        return 1;
    }

    
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) {
        ::cerr << "Ошибка: не удалось создать сокет" << ::endl;
        WSACleanup();
        return 1;
    }

    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        ::cerr << "Ошибка: не удалось привязать сокет к адресу" << ::endl;
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    
    if (listen(listener, MAX_CLIENTS) == SOCKET_ERROR) {
        ::cerr << "Ошибка: не удалось прослушать порт" << ::endl;
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    
    fd_set master_fds;
    FD_ZERO(&master_fds);
    FD_SET(listener, &master_fds);
    int max_fd = listener;

    
    while (true) {
        fd_set read_fds = master_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == SOCKET_ERROR) {
            ::cerr << "Ошибка: select не удался" << ::endl;
            break;
        }

        
        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == listener) {
                    
                    SOCKET client_socket = accept(listener, NULL, NULL);
                    if (client_socket == INVALID_SOCKET) {
                        ::cerr << "Ошибка: не удалось принять соединение" << ::endl;
                        continue;
                    }
                    FD_SET(client_socket, &master_fds);
                    if (client_socket > max_fd) {
                        max_fd = client_socket;
                    }
                }
                else {
                    
                    char buffer[BUFFER_SIZE] = { 0 };
                    int bytes_recv = recv(fd, buffer, BUFFER_SIZE, 0);
                    if (bytes_recv <= 0) {
                        
                        if (bytes_recv == 0) {
                            ::cout << "Соединение закрыто клиентом" << ::endl;
                        }
                        else {
                            ::cerr << "Ошибка: recv не удался" << ::endl;
                        }
                        closesocket(fd);
                        FD_CLR(fd, &master_fds);
                    }
                    else {
                        
                        ::cout << "Получен запрос от клиента" << ::endl;
                        ::cout << "Данные запроса: " << buffer << ::endl;

                        
                        ::string response = generateHttpResponse();
                        
                        ::cout << "Отправка ответа клиенту: " << response << ::endl;
                        
                        send(fd, response.c_str(), response.length(), 0);

                        
                        closesocket(fd);
                        FD_CLR(fd, &master_fds);
                    }
                }
            }
        }
    }

    // Закрытие сокета прослушивания
    closesocket(listener);

    // Очистка Winsock
    WSACleanup();

    return 0;
}
