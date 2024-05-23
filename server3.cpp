#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>

#define BUFFER_SIZE 1024

struct AsyncData {
    OVERLAPPED overlapped;
    HANDLE fileHandle;
    std::vector<char> buffer;
};

void CALLBACK readCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    AsyncData* asyncData = reinterpret_cast<AsyncData*>(lpOverlapped);
    if (dwErrorCode == ERROR_SUCCESS) {
        std::cout << "Read " << dwNumberOfBytesTransfered << " bytes: " << std::string(asyncData->buffer.begin(), asyncData->buffer.begin() + dwNumberOfBytesTransfered) << std::endl;
    }
    else {
        std::cerr << "ReadFileEx failed with error code " << dwErrorCode << std::endl;
    }
    delete asyncData;
}


void CALLBACK writeCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    AsyncData* asyncData = reinterpret_cast<AsyncData*>(lpOverlapped);
    if (dwErrorCode == ERROR_SUCCESS) {
        std::cout << "Write " << dwNumberOfBytesTransfered << " bytes" << std::endl;
    }
    else {
        std::cerr << "WriteFileEx failed with error code " << dwErrorCode << std::endl;
    }
    delete asyncData;
}

int main(int argc, char* argv[]) {
    system("chcp 1251");

    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    
    const char* filePath = argv[1];

    
    std::wstring wideFilePath(filePath, filePath + strlen(filePath));

    
    HANDLE fileHandle = CreateFile(wideFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось открыть файл" << std::endl;
        return 1;
    }

    
    if (!SetEndOfFile(fileHandle)) {
        std::cerr << "Не удалось очистить содержимое файла" << std::endl;
        CloseHandle(fileHandle);
        return 1;
    }

    
    std::vector<char> readBuffer(BUFFER_SIZE, 0);
    AsyncData* readData = new AsyncData();
    readData->overlapped = { 0 };
    readData->fileHandle = fileHandle;
    readData->buffer = readBuffer;

    if (!ReadFileEx(fileHandle, readBuffer.data(), BUFFER_SIZE, reinterpret_cast<LPOVERLAPPED>(readData), readCompletionRoutine)) {
        std::cerr << "Не удалось прочитать файл" << std::endl;
        delete readData;
        CloseHandle(fileHandle);
        return 1;
    }

    std::string inputData;
    std::cout << "Введите данные для записи в файл: ";
    std::getline(std::cin, inputData);

    
    std::vector<char> writeBuffer(inputData.begin(), inputData.end());
    AsyncData* writeData = new AsyncData();
    writeData->overlapped = { 0 };
    writeData->fileHandle = fileHandle;
    writeData->buffer = writeBuffer;

    if (!WriteFileEx(fileHandle, writeBuffer.data(), writeBuffer.size(), reinterpret_cast<LPOVERLAPPED>(writeData), writeCompletionRoutine)) {
        std::cerr << "Не удалось записать в файл" << std::endl;
        delete writeData;
        CloseHandle(fileHandle);
        return 1;
    }

    
    std::cout << "Файл обновлен" << std::endl;
    

    CloseHandle(fileHandle);
    return 0;
}
