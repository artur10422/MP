#include <filesystem>
#include <iostream>
#include <thread>

namespace fs = std::filesystem;

// Функция для копирования регулярных файлов в каталог "Files"
void CopyRegularFiles(const fs::path& RootDir, const fs::path& FilesDir) {
    for (const auto& entry : fs::directory_iterator(RootDir)) {
        if (fs::is_regular_file(entry)) {
            fs::copy_file(entry.path(), FilesDir / entry.path().filename(), fs::copy_options::overwrite_existing);

            // Создаем новый поток для копирования файла
            std::thread t([&entry, &FilesDir]() {
                fs::copy_file(entry.path(), FilesDir / entry.path().filename(), fs::copy_options::overwrite_existing);
                });
            t.detach(); // Отделяем поток от основного потока исполнения
        }
    }
}

// Функция для поиска и копирования каталогов в каталог "Catalogs"
void CopyDirectoriesToCatalogs(const fs::path& RootDir, const fs::path& CatalogsDir) {
    for (const auto& entry : fs::directory_iterator(RootDir)) {
        const auto& entryPath = entry.path();
        if (fs::is_directory(entryPath) && entryPath != CatalogsDir) { // Проверяем, что текущий каталог не является целевым каталогом
            fs::copy(entryPath, CatalogsDir / entryPath.filename(), fs::copy_options::recursive);

            // Создаем новый поток для обработки текущего каталога
            std::thread t(CopyDirectoriesToCatalogs, entryPath, CatalogsDir / entryPath.filename());
            t.detach(); // Отделяем поток от основного потока исполнения
        }
    }
}

void check(const fs::path& RootDir, const fs::path& FilesDir, const fs::path& CatalogsDir) {

    if (fs::exists(FilesDir) && fs::is_directory(FilesDir)) {
        // Если каталог существует, очищаем его
        fs::remove_all(FilesDir);
    }

    if (fs::exists(CatalogsDir) && fs::is_directory(CatalogsDir)) {
        // Если каталог существует, очищаем его
        fs::remove_all(CatalogsDir);
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    // Путь к корневому каталогу
    fs::path RootDir = "C:\\Users\\admin\\source\\repos\\laba 2.2\\laba 2.2";
    

    // Пути к каталогам "Files" и "Catalogs"
    fs::path FilesDir = RootDir / "Files";
    fs::path CatalogsDir = RootDir / "Catalogs";

    check(RootDir, FilesDir, CatalogsDir);
    // Создание каталога "Files"
    if (!fs::exists(FilesDir)) {
        fs::create_directory(FilesDir);
    }

    // Создание каталога "Catalogs"
    if (!fs::exists(CatalogsDir)) {
        fs::create_directory(CatalogsDir);
    }

    // Шаг 2: Копирование регулярных файлов в каталог "Files"
    CopyRegularFiles(RootDir, FilesDir);

    // Шаг 3: Поиск и копирование каталогов в каталог "Catalogs"
    CopyDirectoriesToCatalogs(RootDir, CatalogsDir);

    std::cout << "Операции завершены успешно" << std::endl;
    return 0;
}